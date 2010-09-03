function Response = Integrator_AlphaOS(Model,GroundMotion)
%INTEGRATOR_ALPHAOS to perform a direct integration analysis using
% the second order Alpha-Operator-Splitting method 
% Response = Integrator_AphaOS(Model,GroundMotion)
%
% Response     : data structure with response results
% Model        : data structure with structural model information
% GroundMotion : data structure with ground motion information

% Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
% Created: 07/10
% Revision: A

%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Extract Model Parameters
%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% number of global DOF in the model
ndf = Model.ndf;

% mass matrix
M = Model.M;
if ~isequal(size(M),[ndf,ndf])
   error('Supplied mass matrix has wrong size');
end

% element properties
Element = Model.Element;
numElem = length(Element);

% initial stiffness matrix
K = zeros(ndf,ndf);
for el=1:numElem
   feval(Element{el}.type,'initialize',Element{el});
   id = Element{el}.id;
   K(id,id) = K(id,id) + feval(Element{el}.type,'getInitialStiff',Element{el});
end

% damping matrix
C = Model.alphaM*M + Model.betaK*K;


%%%%%%%%%%%%%%%%%%%%%%%%%%
% Load GroundMotion Data
%%%%%%%%%%%%%%%%%%%%%%%%%%
% find longest motion
numMotions = length(GroundMotion.dt);
tEnd = 0.0;
for mo=1:numMotions
   tEndi = GroundMotion.scalet{mo}(end);
   if tEndi > tEnd
      tEnd = tEndi;
   end
end
% change to analysis deltaT
deltaT = GroundMotion.dtAnalysis;
t = deltaT*(0:floor(tEnd/deltaT));
npts = length(t);
ag = zeros(npts,numMotions);
for mo=1:numMotions
   ag(:,mo) = interp1(GroundMotion.scalet{mo},GroundMotion.scaleag{mo},t,'linear',0.0);
end
b = Model.b;


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Perform Transient Analysis
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
tic;
% analysis parameters
alpha = 1.0;
beta  = 0.25*(2.0-alpha)^2;
gamma = 1.5-alpha;

% initialize global response variables
U = zeros(ndf,npts);
Up = zeros(ndf,npts);
Udot = zeros(ndf,npts);
Udotdot = zeros(ndf,npts);
Pr = zeros(ndf,npts);
P = zeros(ndf,npts);
Um = zeros(ndf,npts);

% set the constants
c1 = 1.0;
c2 = gamma/(beta*deltaT);
c3 = 1.0/(beta*deltaT*deltaT);
a1 = (0.5 - beta)*deltaT*deltaT;
a2 = deltaT*(1.0 - gamma);

% calculations for each time step, i
for i=1:npts-1
   
   % get new response quantities
   Up(:,i+1) = U(:,i) + deltaT*Udot(:,i) + a1*Udotdot(:,i);
   Udot(:,i+1) = Udot(:,i) + a2*Udotdot(:,i);
   Udotdot(:,i+1) = 0.0;
   
   % get applied loads
   P(:,i+1) = -M*b*ag(i+1,:)';
   
   % Linear algorithm
   % set trial response in elements
   for el=1:numElem
      id = Element{el}.id;
      feval(Element{el}.type,'setTrialDisp',Element{el},Up(id,i+1));
   end
   
   % get resisting forces from elements
   Pr(:,i+1) = zeros(ndf,1);
   for el=1:numElem
      id = Element{el}.id;
      Pr(id,i+1) = Pr(id,i+1) + feval(Element{el}.type,'getResistingForce',Element{el});
      Um(id,i+1) = Um(id,i+1) + feval(Element{el}.type,'getDisp',Element{el});
   end
   
   % get rhs and jacobian
   F  = alpha*(C*Udot(:,i+1) + Pr(:,i+1) - P(:,i+1)) ...
      + (1-alpha)*(C*Udot(:,i) + Pr(:,i) + K*U(:,i) - K*Up(:,i) - P(:,i));
   DF = c3*M + alpha*c2*C + alpha*c1*K;
   
   % solve for displacement increments
   deltaU = DF\(-F);
   
   % update response quantities
   U(:,i+1) = Up(:,i+1) + c1*deltaU;
   Udot(:,i+1) = Udot(:,i+1) + c2*deltaU;
   Udotdot(:,i+1) = c3*deltaU;
   
   % commit the elements
   for j=el:numElem
      feval(Element{el}.type,'commitState',Element{el});
   end
   
   % animate response
   AnimateResponse(U(:,1:i+1),Udot,Udotdot,Pr,Um);
end
toc;

% disconnect from experimental sites
for el=1:numElem
   if strncmp(Element{el}.type,'Element_Exp',11)
      feval(Element{el}.type,'disconnect',Element{el});
   end
end

% assign response quantities
Response.Time = t;
Response.U = U;
Response.Udot = Udot;
Response.Udotdot = Udotdot;
Response.Pr = Pr;
Response.Um = Um;
