function varargout = EEFrame2d(action,el_no,ndm,ElemData,xyz,ElemState,varargin)
%EEFRAME2D 2d experimental frame element under linear or nonlinear geometry
% varargout = EEFrame2d(action,el_no,ndm,ElemData,xyz,ElemState,varargin)
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
%         .w      : uniform load (w(1)=longitudinal, w(2)=transverse)
%         .Geom   : geometry formulation (linear, P-Delta, corotational)
%         .jntoff : rigid joint offsets in global X and Y at element ends
%
%   See also TCPSOCKET, GENERICCLIENT2D, EETRUSS, EEFRAME3D, EETWONODELINK2D

% GLOBAL VARIABLES
global IOW;          % output file number
global HEAD_PR;      % header print indicator
global MAGF;         % magnification factor for deformed shape

% persistent (static) variables
persistent socketID;  % tcp/ip socket identifier
persistent Time;      % current analysis time

% specify element array size, check element data and retrieve them
% =========================================================================
ndf = 3;    % no of element dof's per node (2node, 2d frame element)
% geometric transformation options
geom    = {'linear'; 'Pdelta'; 'corotational'};
geomfun = {'LinrTran2d' ; 'PDelTran2d' ; 'CoroTran2d'};

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
      if (~isfield(ElemData,'rho'));    ElemData.rho = 0; end
      if (~isfield(ElemData,'w'));      ElemData.w = [0;0]; end      
      if (~isfield(ElemData,'Geom'));   ElemData.Geom = 'linear'; end
      if (~isfield(ElemData,'jntoff')); ElemData.jntoff = [0 0;0 0]; end
      varargout = {ElemData};
   % =========================================================================
   otherwise
      % extract element properties
      kInit  = ElemData.kInit;
      ipPort = ElemData.ipPort;
      ipAddr = ElemData.ipAddr;
      rho    = ElemData.rho;
      w      = ElemData.w;
      jntoff = ElemData.jntoff;
      
      % determine geometric transformation
      GeomTran = geomfun{strmatch(ElemData.Geom,geom)};
      
      % setup connection to experimental site
      dataSize = 1+3*ndf;
      sData = zeros(1,dataSize);
      if isempty(socketID)
         socketID = TCPSocket('openConnection',ipAddr,ipPort);
         if (socketID<0)
            error('TCPSocket:openConnection',['Unable to setup connection to ',...
               ipAddr,' : ',num2str(ipPort)]);
         end
         % set the data size for the experimental site
         dataSizes = int32([ndf ndf ndf 0 1, ndf 0 0 ndf 0, dataSize]);
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
         fprintf(IOW,'\n\n Element Properties for EEFrame2d Element');
         fprintf(IOW,'\n Elem     kInit     ipPort    ipAddr       rho           wx           wy');
      end
      fprintf(IOW,'\n%4d  %11.3e  %7d  %s  %11.3e  %11.3e  %11.3e  %11.3e',el_no,kInit,ipPort,ipAddr,rho,w(1),w(2));
      HEAD_PR = 1;
      Data.jntoff = ElemData.jntoff;
      feval(GeomTran,'data',Data);
   % =========================================================================
   case 'init'
      % set initial time
      Time = 0;
      % history information
      ElemState.Pres.v  = zeros(ndf,1);
      ElemState.Pres.q  = zeros(ndf,1);
      ElemState.Pres.vb = zeros(2*ndf,1);
      ElemState.Pres.qb = zeros(2*ndf,1);
      varargout = {ElemState};
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
      q0 = [-w(1)*L/2   ;
         -w(2)*L*L/12;
         w(2)*L*L/12];

      % transform displacements from basic system A to basic system B
      a = TranBA(ndm,ndf,L);
      v = a*v;

      % send trial response to experimental site
      sData(1) = 3;
      sData(2:1+3*ndf) = v(:,[1,4,5]);
      sData(2+3*ndf) = ElemState.Time;
      TCPSocket('sendData',socketID,sData,dataSize);
      
      % obtain resisting forces from experimental site
      sData(1) = 10;
      TCPSocket('sendData',socketID,sData,dataSize);
      rData = TCPSocket('recvData',socketID,dataSize);
      q = rData(ndf+1:2*ndf)';

      % store history variables
      ElemState.Pres.v = v;
      ElemState.Pres.q = q;

      % transform stiffness and forces from basic system B to basic system A
      k = a'*kInit*a;
      q = a'*q + q0;

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
      q0 = [-w(1)*L/2   ;
         -w(2)*L*L/12;
         w(2)*L*L/12];

      % transform displacements from basic system A to basic system B
      a = TranBA(ndm,ndf,L);
      v = a*v;

      % send trial response to experimental site
      sData(1) = 3;
      sData(2:1+3*ndf) = v(:,[1,4,5]);
      sData(2+3*ndf) = ElemState.Time;
      TCPSocket('sendData',socketID,sData,dataSize);
      
      % obtain resisting forces from experimental site
      sData(1) = 10;
      TCPSocket('sendData',socketID,sData,dataSize);
      rData = TCPSocket('recvData',socketID,dataSize);
      q = rData(ndf+1:2*ndf)';

      % store history variables
      ElemState.Pres.v = v;
      ElemState.Pres.q = q;

      % transform forces from basic system B to basic system A
      q = a'*q + q0;
         
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
         fprintf(IOW,'\n End Forces for EEFrame2d Element');
         fprintf(IOW,'\n Elem      Ni           Vi           Mi           Nj           Vj           Mj');
      end
      fprintf(IOW,'\n%4d  %11.3e  %11.3e  %11.3e  %11.3e  %11.3e  %11.3e', el_no,qb);
   % =========================================================================
   case 'mass'
      % determine element length and orientation (direction cosines)
      [L, dx] = ElmLenOr(xyz);
      % set up rotation transformation matrix ar from global to local coordinates
      ar = TranLG(ndm,ndf,dx);

      tm = rho*L;    % total mass of frame element
      % lumped mass matrix
      lm = zeros(2*ndf,1);
      lm ([1 2 4 5]) = 0.5*tm.*ones(2*ndm,1);
      % consistent mass matrix
      cmb = tm/420.*[140  0     0     70    0     0;
                      0  156   22*L    0   54   -13*L;
                      0  22*L  4*L*L   0  13*L  -3*L*L;
                     70   0     0     140   0     0;
                      0  54    13*L    0   156   -22*L;
                      0 -13*L -3*L*L   0  -22*L  4*L*L];

      % tranform consistent mass matrix to global reference system
      cmh = ar'*cmb*ar;
      varargout = {lm cmh};
   % =========================================================================
   case 'defo'
      % draw member in deformed configuration
      % transform end displacements from global reference to basic system
      vh = ElemState.vh(:,1);    % extract end displacements from ElemState
      Data.jntoff = ElemData.jntoff;
      Data.w      = ElemData.w;
      [L dx dxd v vb] = feval(GeomTran,'disp',Data,xyz,vh);
      cost = dx(1);
      sint = dx(2);

      % original element position
      XYi0 = xyz(:,1);
      XYj0 = xyz(:,2);

      % subdivide element into smaller parts to draw the displaced shape
      ns  = 50;
      xyd = zeros(2,ns+2);

      xyd(:,1)    = XYi0 + vh(1:2,1).*MAGF;    % node i of element under magnified displacements
      xyd(:,ns+2) = XYj0 + vh(4:5,1).*MAGF;    % node j of element under magnified displacements

      i   = 1:ns;
      xi  = (i-1)/(ns-1);
      u   = [xi.*v(1)+vb(1);
            (xi-2.*xi.^2+xi.^3).*L.*v(2)+(-xi.^2+xi.^3).*L.*v(3)+vb(2).*(1-xi)+vb(5).*xi];
      ud  = [xi.*L;0.*xi] + u.*MAGF;
      XYi = XYi0(:,1) + jntoff (:,1);
      Ud  = [cost -sint;sint cost]*ud + repmat(XYi,1,ns);
      xyd(:,2:ns+1) = Ud;

      hold on;
      % draw deformed shape of element
      line(xyd(1,:)',xyd(2,:)','Color','y','LineWidth',1.0);
      line(xyd(1,[1,end])',xyd(2,[1,end])','LineStyle','none','Marker','s','MarkerSize',4,...
         'MarkerFaceColor','w','MarkerEdgeColor','w');
   % =========================================================================
   otherwise
      % add further actions
   % =========================================================================
end