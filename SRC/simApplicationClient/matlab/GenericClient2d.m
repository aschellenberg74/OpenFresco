function varargout = GenericClient2d(action,el_no,ndm,ElemData,xyz,ElemState,varargin)
%GENERICCLIENT2D 2d generic client element
% varargout = GenericClient2d(action,el_no,ndm,ElemData,xyz,ElemState,varargin)
%
% varargout : variable return argument list
%  varargout = arsz       for action 'size'
%  varargout = ElemData   for action 'chec'
%  varargout = ElemState  for action 'init' with field  Pres
%  varargout = ElemState  for action 'stif' with fields kh and qh
%  varargout = ElemState  for action 'forc' with field qh
%  varargout = [lm cm]    for action 'mass'
%  varargout = Post       for action 'post'
%           where arsz = size of element arrays
%                   kh = element stiffness matrix in global coordinates
%                   qh = element resisting forces in global coordinates
%                   lm = lumped mass vector
%                   cm = consistent mass matrix
%                 Pres = current values of element history variables
%                 Post = element post-processing information
% action    : switch with following possible values
%                  'size' element reports size of element arrays
%                  'chec' element checks data for omissions
%                  'data' element prints properties
%                  'init' element returns history variables
%                  'stif' element returns stiffness matrix and end forces
%                  'forc' element returns only end forces
%                  'post' element stores information for post-processing
%                  'stre' element prints internal forces
%                  'mass' element returns lumped and consistent mass matrix
%                  'defo' element displays its deformed configuration
% el_no     : element number
% ndm       : problem dimension
% ElemData  : data structure of element properties
% xyz       : element node coordinates
% ElemState : current element state; data structure with fields vh, Past and Pres
%      .vh(:,1) : total displacements of element dof's
%      .vh(:,2) : displacement increments from last convergence
%      .vh(:,3) : displacement increments from last iteration
%      .vh(:,4) : velocities of element dof's
%      .vh(:,5) : accelerations of element dof's
%      .Past    : history variables at last convergence
%      .Pres    : history variables at last iteration
% varargin  : optional arguments
%
% =========================================================================
% FEDEAS Lab - Release 2.3, March 2001
% Matlab Finite Elements for Design, Evaluation and Analysis of Structures
%
% Copyright (c) 1998, Professor Filip C. Filippou, filippou@ce.berkeley.edu
% Department of Civil and Environmental Engineering, UC Berkeley
% =========================================================================
% contributed by Andreas Schellenberg 11/2006
%
% Element Properties
% ElemData.numNodes : number of nodes
%         .ipPort   : ip port
%         .ipAddr   : ip address
%
%   See also TCPSOCKET, EETRUSS, EEFRAME2D, EEFRAME3D, EETWONODELINK2D

% GLOBAL VARIABLES
global IOW;       % output file number
global HEAD_PR;   % header print indicator
global MAGF;      % magnification factor for deformed shape

% persistent (static) variables
persistent socketID;  % tcp/ip socket identifier
persistent Time;      % current analysis time

% report size of element arrays, check element data or retrieve data
% =========================================================================
ndf = 3;          % no of dof's per node
switch action
   % =========================================================================
   case 'size'
      arsz      = 2*ndf;
      varargout = {arsz};  % return size of element arrays
   % =========================================================================
   case 'chec'
      if (~isfield(ElemData,'numNodes'));  disp('Element');disp(el_no);error('number of nodes missing'); end
      if (~isfield(ElemData,'ipPort'));    disp('Element');disp(el_no);error('ip port missing'); end
      if (~isfield(ElemData,'ipAddr'));    ElemData.ipAddr = '127.0.0.1'; end
      varargout = {ElemData};
   % =========================================================================
   otherwise
      % extract element properties
      numNodes = ElemData.numNodes;
      ipPort   = ElemData.ipPort;
      ipAddr   = ElemData.ipAddr;
      numDOF   = numNodes*ndf;
      
      % setup connection to experimental element
      dataSize = max(1+3*numDOF,numDOF^2);
      sData = zeros(1,dataSize);
      if isempty(socketID)
         socketID = TCPSocket('openConnection',ipAddr,ipPort);
         if (socketID<0)
            error('TCPSocket:openConnection',['Unable to setup connection to ',...
               ipAddr,' : ',num2str(ipPort)]);
         end
         % set the data size for experimental element
         dataSizes = int32([numDOF numDOF numDOF 0 1, 0 0 0 numDOF 0, dataSize]);
         TCPSocket('sendData',socketID,dataSizes,11);
      end
   % =========================================================================
end

% element actions
switch action
   % =========================================================================
   case 'data'
      if (HEAD_PR)
         fprintf (IOW,strcat('\n\n Properties for GenericClient2d Element'));
         fprintf (IOW,'\n Elem    numNodes    ipPort    ipAddr');
      end
      fprintf (IOW,'\n%4d  %7d      %7d    %s', el_no,numNodes,ipPort,ipAddr);
   % =========================================================================
   case 'init'
      % set initial time
      Time = 0;
      % history information
      ElemState.Pres.vh = zeros(numDOF,1);
      ElemState.Pres.qh = zeros(numDOF,1);
      varargout = {ElemState};
   % =========================================================================
   case 'stif'
      if (ElemState.Time > Time)
         % commit state
         sData(1) = 5;
         TCPSocket('sendData',socketID,sData,dataSize);
         % save current time
         Time = ElemState.Time;
      end

      % transform end displacements from global reference to basic system
      vh = ElemState.vh;       % extract end displacements from ElemState

      % send trial response to experimental element
      sData(1) = 3;
      sData(2:1+3*numDOF) = vh(:,[1,4,5]);
      sData(2+3*numDOF) = ElemState.Time;
      TCPSocket('sendData',socketID,sData,dataSize);

      % obtain stiffness matrix from experimental element
      sData(1) = 13;
      TCPSocket('sendData',socketID,sData,dataSize);
      rData = TCPSocket('recvData',socketID,dataSize);
      kh = reshape(rData(1:numDOF^2)',numDOF,numDOF);

      % obtain resisting forces from experimental element
      sData(1) = 10;
      TCPSocket('sendData',socketID,sData,dataSize);
      rData = TCPSocket('recvData',socketID,dataSize);
      qh = rData(1:numDOF)';

      % store history variables
      ElemState.Pres.vh = vh;
      ElemState.Pres.qh = qh;
      
      % save stiffness and forces in global reference system
      ElemState.kh = kh;
      ElemState.qh = qh;
      varargout = {ElemState};
   % =========================================================================
   case 'forc'
      if (ElemState.Time > Time)
         % commit state
         sData(1) = 5;
         TCPSocket('sendData',socketID,sData,dataSize);
         % save current time
         Time = ElemState.Time;
      end

      % transform end displacements from global reference to basic system
      vh = ElemState.vh;       % extract end displacements from ElemState

      % send trial response to experimental element
      sData(1) = 3;
      sData(2:1+3*numDOF) = vh(:,[1,4,5]);
      sData(2+3*numDOF) = ElemState.Time;
      TCPSocket('sendData',socketID,sData,dataSize);

      % obtain resisting forces from experimental element
      sData(1) = 10;
      TCPSocket('sendData',socketID,sData,dataSize);
      rData = TCPSocket('recvData',socketID,dataSize);
      qh = rData(1:numDOF)';

      % store history variables
      ElemState.Pres.vh = vh;
      ElemState.Pres.qh = qh;
      
      % save forces in global reference system
      ElemState.qh = qh;
      varargout = {ElemState};
   % =========================================================================
   case 'post'
      Post.vh = ElemState.Pres.vh;
      Post.qh = ElemState.Pres.qh;
      varargout = {Post};
   % =========================================================================
   case 'stre'
      qh = ElemState.Pres.qh;
      if (HEAD_PR)
         fprintf(IOW,'\n End Forces for GenericClient2d Element');
         fprintf(IOW,'\n Elem      P1           P2           P3           P4           P5           P6');
      end
      fprintf(IOW,'\n%4d  %11.3e  %11.3e  %11.3e  %11.3e  %11.3e  %11.3e', el_no,qh);
   % =========================================================================
   case 'mass'
      % obtain consistent mass matrix from experimental element
      sData(1) = 15;
      TCPSocket('sendData',socketID,sData,dataSize);
      rData = TCPSocket('recvData',socketID,dataSize);
      cm = reshape(rData(1:numDOF^2)',numDOF,numDOF);
      % lumped mass matrix
      lm = diag(cm);
      varargout = {lm cm};
   % =========================================================================
   case 'defo'
      % draw member in deformed configuration
      xyzd = xyz + MAGF .* reshape(ElemState.vh,ndm,2);
      switch ndm
         case 2
            H=line(xyzd(1,:)',xyzd(2,:)');
            set(H,'color',[1,1,0]);
            set(H,'LineWidth',1);
         case 3
            H=line(xyzd(1,:)',xyzd(2,:)',xyzd(3,:)');
            set(H,'color',[1,1,0]);
            set(H,'LineWidth',1);
      end
   % =========================================================================
   otherwise
      % add further actions
   % =========================================================================
end
