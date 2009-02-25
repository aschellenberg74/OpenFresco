function varargout = EETruss(action,el_no,ndm,ElemData,xyz,ElemState,varargin)
%EETRUSS 2d/3d experimental truss element under linear or nonlinear geometry
% varargout = EETruss(action,el_no,ndm,ElemData,xyz,ElemState,varargin)
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
% ElemData.kInit  : initial stiffness in cantilever basic system
%         .ipPort : ip port
%         .ipAddr : ip address
%         .rho    : mass per unit length
%         .w      : uniform load (w(1)=longitudinal)
%         .Geom   : geometry formulation (linear, P-Delta, corotational)
%         .jntoff : rigid joint offsets in global X and Y at element ends
%
%   See also TCPSOCKET, GENERICCLIENT2D, EEFRAME2D, EEFRAME3D, EETWONODELINK2D

% GLOBAL VARIABLES
global IOW;          % output file number
global HEAD_PR;      % header print indicator
global MAGF;         % magnification factor for deformed shape

% persistent (static) variables
persistent socketID;  % tcp/ip socket identifier
persistent Time;      % current analysis time

% specify element array size, check element data and retrieve them
% =========================================================================
% geometric transformation options
geom = {'linear'; 'Pdelta'; 'corotational'};
switch ndm
   case 2
      geomfun = {'LinrTran2d' ; 'PDelTran2d' ; 'CoroTran2d'};
   case 3
      geomfun = {'LinrTran3d' ; 'PDelTran3d' ; 'CoroTran3d'};
end

switch action
   % =========================================================================
   case 'size'
      arsz      = 2*ndm;
      varargout = {arsz};  % return size of element arrays
   % =========================================================================
   case 'chec'
      if (~isfield(ElemData,'kInit'));  disp('Element');disp(el_no);error('initial stiffness missing'); end
      if (~isfield(ElemData,'ipPort')); disp('Element');disp(el_no);error('ip port missing'); end
      if (~isfield(ElemData,'ipAddr')); ElemData.ipAddr = '127.0.0.1'; end
      if (~isfield(ElemData,'rho'));    ElemData.rho = 0; end
      if (~isfield(ElemData,'w'));      ElemData.w = 0; end      
      if (~isfield(ElemData,'Geom'));   ElemData.Geom = 'linear'; end
      if (~isfield(ElemData,'jntoff')); ElemData.jntoff = zeros(ndm,2); end
      varargout = {ElemData};
   % =========================================================================
   otherwise
      % extract element properties
      kInit    = ElemData.kInit;
      ipPort   = ElemData.ipPort;
      ipAddr   = ElemData.ipAddr;
      rho      = ElemData.rho;
      w        = ElemData.w;
      
      % determine geometric transformation
      GeomTran = geomfun{strmatch(ElemData.Geom,geom)};
      
      % setup connection to experimental site
      dataSize = 4;
      sData = zeros(1,dataSize);
      if isempty(socketID)
         socketID = TCPSocket('openConnection',ipAddr,ipPort);
         if (socketID<0)
            error('TCPSocket:openConnection',['Unable to setup connection to ',...
               ipAddr,' : ',num2str(ipPort)]);
         end
         % set the data size for the experimental site
         dataSizes = int32([1 1 1 0 1, 1 0 0 1 0, dataSize]);
         TCPSocket('sendData',socketID,dataSizes,11);
      end
   % =========================================================================
end

% element actions
switch action
   % =========================================================================
   case 'data'
      % print element properties
      if (HEAD_PR)
         fprintf(IOW,['\n\n Element Properties for ',int2str(ndm),'d EETruss Element']);
         fprintf(IOW, '\n Elem     kInit     ipPort    ipAddr       rho           w');
      end
      fprintf(IOW,'\n%4d  %11.3e  %7d  %s  %11.3e  %11.3e',el_no,kInit,ipPort,ipAddr,rho,w);
      HEAD_PR = 1;
      Data.jntoff = ElemData.jntoff;
      feval(GeomTran,'data',Data);
   % =========================================================================
   case 'init'      
      % set initial time
      Time = 0;
      % history information
      ElemState.Pres.v  = zeros(1,1);
      ElemState.Pres.q  = zeros(1,1);
      ElemState.Pres.vb = zeros(4,1);
      ElemState.Pres.qb = zeros(4,1);
      varargout  = {ElemState};
   % =========================================================================
   case 'stif'
      if (Time < ElemState.Time)
         % commit state
         sData(1) = 5;
         TCPSocket('sendData',socketID,sData,dataSize);
         % save current time
         Time = ElemState.Time;
      end

      % transform end displacements from global reference to basic system
      vh = ElemState.vh;
      Data.jntoff = ElemData.jntoff;
      Data.w      = ElemData.w;
      [L dx dxd v vb] = feval(GeomTran,'disp',Data,xyz,vh);

      % set up initial element force vector in basic system
      q0 = -w(1)*L/2;

      % send trial response to experimental site
      sData(1) = 3;
      sData(2:4) = v([1,4,5]);
      sData(5) = ElemState.Time;
      TCPSocket('sendData',socketID,sData,dataSize);

      % obtain resisting forces from experimental site
      sData(1) = 10;
      TCPSocket('sendData',socketID,sData,dataSize);
      rData = TCPSocket('recvData',socketID,dataSize);
      q = rData(2) + q0;
      k = kInit;

      % store history variables
      ElemState.Pres.v = v;
      ElemState.Pres.q = q;
         
      % tranform stiffness and forces from basic to global reference system
      [qh kh qb] = feval(GeomTran,'stif',Data,xyz,q,k);

      % update history variables
      ElemState.Pres.vb = vb;
      ElemState.Pres.qb = qb;

      ElemState.kh = kh;
      ElemState.qh = qh;
      varargout = {ElemState};
   % =========================================================================
   case 'forc'
      if (Time < ElemState.Time)
         % commit state
         sData(1) = 5;
         TCPSocket('sendData',socketID,sData,dataSize);
         % save current time
         Time = ElemState.Time;
      end

      % transform end displacements from global reference to basic system
      vh = ElemState.vh;
      Data.jntoff = ElemData.jntoff;
      Data.w      = ElemData.w;
      [L dx dxd v vb] = feval(GeomTran,'disp',Data,xyz,vh);

      % set up initial element force vector in basic system
      q0 = -w(1)*L/2;

      % send trial response to experimental site
      sData(1) = 3;
      sData(2:4) = v([1,4,5]);
      sData(5) = ElemState.Time;
      TCPSocket('sendData',socketID,sData,dataSize);

      % obtain resisting forces from experimental site
      sData(1) = 10;
      TCPSocket('sendData',socketID,sData,dataSize);
      rData = TCPSocket('recvData',socketID,dataSize);
      q = rData(2) + q0;

      % store history variables
      ElemState.Pres.v = v;
      ElemState.Pres.q = q;
      
      % tranform forces from basic to global reference system
      [qh qb] = feval(GeomTran,'forc',Data,xyz,q);

      % update history variables
      ElemState.Pres.vb = vb;
      ElemState.Pres.qb = qb;

      ElemState.qh = qh;
      varargout = {ElemState};
   % =========================================================================
   case 'post'
      Post.v  = ElemState.Pres.v;
      Post.q  = ElemState.Pres.q;
      Post.vb = ElemState.Pres.vb;
      Post.qb = ElemState.Pres.qb;
      varargout = {Post};
   % =========================================================================
   case 'stre'
      qb = ElemState.Pres.qb;
      if (HEAD_PR)
         fprintf(IOW,['\n End Forces for ',int2str(ndm),'d EETruss Element']);
         fprintf(IOW,'\n Elem      Ni      Vi      Nj      Vj');
      end
      fprintf(IOW,'\n%4d  %11.3e  %11.3e  %11.3e  %11.3e', el_no,qb(:));
   % =========================================================================
   case 'mass'
      % determine element length and orientation (direction cosines)
      L = ElmLenOr(xyz);

      tm = rho*L;  % total mass of truss element
      % lumped mass matrix
      lm = 0.5*tm.*ones(2*ndm,1);
      % consistent mass matrix
      vm = tm.*ones(ndm,1);
      cm = [diag(vm)./3 diag(vm)./6;
            diag(vm)./6 diag(vm)./3];
      varargout = {lm cm};
   % =========================================================================
   case 'defo'
      % draw member in deformed configuration
      xyzd = xyz + MAGF .* reshape(ElemState.vh(:,1),ndm,2);
      switch ndm
         case 2
            line(xyzd(1,:)',xyzd(2,:)','Color','y','LineWidth',1.0);
            line (xyzd(1,[1,end])',xyzd(2,[1,end])','LineStyle','none','Marker','s','MarkerSize',4,...
               'MarkerFaceColor','w','MarkerEdgeColor','w');
         case 3
            line(xyzd(1,:)',xyzd(2,:)',xyzd(3,:)','Color','y','LineWidth',1.0);
            line(xyzd(1,[1,end])',xyzd(2,[1,end])',xyzd(3,[1,end])','LineStyle','none','Marker','s',...
               'MarkerSize',4,'MarkerFaceColor','w','MarkerEdgeColor','w');
      end
   % =========================================================================
   otherwise
      % add further actions
   % =========================================================================
end