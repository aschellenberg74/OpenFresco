function varargout = EEZeroLength2d(action,el_no,ndm,ElemData,xyz,ElemState,varargin)
%EEZEROLENGTH2D 2d experimental zero-length element
% varargout = EEZeroLength2d(action,el_no,ndm,ElemData,xyz,ElemState,varargin)
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
% contributed by Andreas Schellenberg 10/2006
%
% Element Properties
% ElemData.kInit  : initial stiffness in basic system
%         .ipPort : ip port
%         .ipAddr : ip address
%         .dir    : direction ID
%         .x      : x-axis vector
%         .yp     : y-axis vector
%         .m      : mass
%
%   See also TCPSOCKET, GENERICCLIENT2D, EETRUSS, EEFRAME2D, EEFRAME3D

% GLOBAL VARIABLES
global IOW;       % output file number
global HEAD_PR;   % header print indicator
global MAGF;      % magnification factor for deformed shape

% persistent (static) variables
persistent T;           % transformation matrix
persistent socketID;    % tcp/ip socket identifier

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
      if (~isfield(ElemData,'kInit'));  disp('Element');disp(el_no);error('initial stiffness missing'); end
      if (~isfield(ElemData,'ipPort')); disp('Element');disp(el_no);error('ip port missing'); end
      if (~isfield(ElemData,'ipAddr')); ElemData.ipAddr = '127.0.0.1'; end
      if (~isfield(ElemData,'dir'));    disp('Element');disp(el_no); error('direction ID missing'); end
      if (~isfield(ElemData,'x'));      ElemData.x = [1;0;0]; end
      if (~isfield(ElemData,'yp'));     ElemData.yp = [0;1;0]; end
      if (~isfield(ElemData,'m'));      ElemData.m = 0; end
      varargout = {ElemData};
   % =========================================================================
   otherwise
      % extract element properties
      kInit  = ElemData.kInit;
      ipPort = ElemData.ipPort;
      ipAddr = ElemData.ipAddr;
      dir    = ElemData.dir;
      x      = ElemData.x;
      yp     = ElemData.yp;
      m      = ElemData.m;
      numDir = length(dir);
      
      % setup connection to experimental site
      dataSize = 1+3*numDir;
      sData = zeros(1,dataSize);
      if isempty(socketID)
         socketID = TCPSocket('openConnection',ipPort,ipAddr);
         % set the data size for the experimental site
         dataSizes = int32([numDir numDir numDir 0 0, numDir 0 0 numDir 0, dataSize]);
         TCPSocket('sendData',socketID,dataSizes,11);
      end
   % =========================================================================
end

% element actions
switch action
   % =========================================================================
   case 'data'
      if (HEAD_PR)
         fprintf (IOW,strcat('\n\n Properties for EEZeroLength2d Element'));
         fprintf (IOW,'\n Elem     kInit     ipPort    ipAddr      dir         x          yp            m');
      end
      fprintf (IOW,'\n%4d  %11.3e  %7d  %s  [%s]  [%s]  [%s]  %11.3e', el_no,kInit,ipPort,ipAddr,num2str(dir),num2str(x'),num2str(yp'),m);
   % =========================================================================
   case 'init'
      % get transformation matrix
      T = getTranGlobalBasic(ndm,ndf,dir,x,yp);

      % history information
      ElemState.Pres.v = zeros(numDir,1);
      ElemState.Pres.q = zeros(numDir,1);
      varargout = {ElemState};
   % =========================================================================
   case 'stif'
      % transform end displacements from global reference to basic system
      vh = ElemState.vh;       % extract end displacements from ElemState
      v = T*vh;

      % send trial response to experimental element
      sData(1) = 3;
      sData(2:1+3*numDir) = v(:,[1,4,5]);
      TCPSocket('sendData',socketID,sData,dataSize);

      % obtain resisting forces from experimental site
      sData(1) = 10;
      TCPSocket('sendData',socketID,sData,dataSize);
      rData = TCPSocket('recvData',socketID,dataSize);
      q = rData(numDir+1:2*numDir)';

      % store history variables
      ElemState.Pres.v = v;
      ElemState.Pres.q = q;
      
      % tranform stiffness and forces from basic to global reference system
      qh = T'*q;
      kh = T'*kInit*T;
      ElemState.kh  = kh;
      ElemState.qh  = qh;
      varargout = {ElemState};
   % =========================================================================
   case 'forc'
      % transform end displacements from global reference to basic system
      vh = ElemState.vh;       % extract end displacements from ElemState
      v = T*vh;

      % send trial response to experimental element
      sData(1) = 3;
      sData(2:1+3*numDir) = v(:,[1,4,5]);
      TCPSocket('sendData',socketID,sData,dataSize);

      % obtain resisting forces from experimental site
      sData(1) = 10;
      TCPSocket('sendData',socketID,sData,dataSize);
      rData = TCPSocket('recvData',socketID,dataSize);
      q = rData(numDir+1:2*numDir)';

      % store history variables
      ElemState.Pres.v = v;
      ElemState.Pres.q = q;

      % tranform forces from basic to global reference system
      qh = T'*q;
      ElemState.qh  = qh;
      varargout = {ElemState};
   % =========================================================================
   case 'post'
      Post.v = ElemState.Pres.v;
      Post.q = ElemState.Pres.q;
      varargout = {Post};
   % =========================================================================
   case 'stre'
      q = ElemState.Pres.q;
      if (HEAD_PR)
         fprintf(IOW,'\n End Forces for EEZeroLength2d Element');
         fprintf(IOW,'\n Elem      Ni           Vi           Mi           Nj           Vj           Mj');
      end
      fprintf(IOW,'\n%4d  %11.3e  %11.3e  %11.3e  %11.3e  %11.3e  %11.3e', el_no,-q,q);
   % =========================================================================
   case 'mass'
      % lumped mass matrix
      lm = zeros(2*ndf,1);
      for i=1:ndm
         lm(i)     = 0.5*m;
         lm(i+ndf) = 0.5*m;
      end
      cm = zeros(2*ndf,2*ndf);
      for i=1:ndm
         cm(i,i)         = 0.5*m;
         cm(i+ndf,i+ndf) = 0.5*m;
      end
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

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function T = getTranGlobalBasic(ndm,ndf,dir,x,yp)
z = cross(x,yp);
y = cross(z,x);
xn = norm(x);
yn = norm(y);
zn = norm(z);

transf = zeros(3,3);
for i=1:3
   transf(1,i) = x(i)/xn;
   transf(2,i) = y(i)/yn;
   transf(3,i) = z(i)/zn;
end

T = zeros(length(dir),2*ndf);
for i=1:length(dir)
   axisID = mod(dir(i),3);
   if (ndm==2 && ndf==2)
      if (dir(i)<=3)
         T(i,3) = transf(axisID,1);
         T(i,4) = transf(axisID,2);
      end
   elseif (ndm==2 && ndf==3)
      if (dir(i)<=3)
         T(i,4) = transf(axisID,1);
         T(i,5) = transf(axisID,2);
         T(i,6) = 0.0;
      else
         T(i,4) = 0.0;
         T(i,5) = 0.0;
         T(i,6) = transf(axisID,3);
      end
   elseif (ndm==3 && ndf==3)
      if (dir(i)<=3)
         T(i,4) = transf(axisID,1);
         T(i,5) = transf(axisID,2);
         T(i,6) = transf(axisID,3);
      end
   elseif (ndm==3 && ndf==6)
      if (dir(i)<=3)
         T(i,7)  = transf(axisID,1);
         T(i,8)  = transf(axisID,2);
         T(i,9)  = transf(axisID,3);
         T(i,10) = 0.0;
         T(i,11) = 0.0;
         T(i,12) = 0.0;
      else
         T(i,7)  = 0.0;
         T(i,8)  = 0.0;
         T(i,9)  = 0.0;
         T(i,10) = transf(axisID,1);
         T(i,11) = transf(axisID,2);
         T(i,12) = transf(axisID,3);
      end
   end
   for j=1:ndf
      T(i,j) = -T(i,j+ndf);
   end
end
return