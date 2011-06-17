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


%%%%%%%%%%%%%%%%%%%%%%
% Setup Loading Data
%%%%%%%%%%%%%%%%%%%%%%
b = Model.b;
numMotions = size(b,2);
switch GroundMotion.loadType
case 'Ground Motions'
    % find longest motion
    tEnd = 0.0;
    for mo=1:numMotions
        tEndi = GroundMotion.scalet{mo}(end);
        if tEndi > tEnd
            tEnd = tEndi;
        end
    end
    % change ground accelerations to analysis deltaT
    deltaT = GroundMotion.dtAnalysis;
    t = deltaT*(0:floor(tEnd/deltaT));
    iStart = 1;
    iEnd = length(t);
    ag = zeros(iEnd,numMotions);
    for mo=1:numMotions
        ag(:,mo) = interp1(GroundMotion.scalet{mo},GroundMotion.scaleag{mo},t,'linear',0.0);
    end
    % set imposed displacements to zero
    Uimp = zeros(ndf,1);
    iImpEnd = 1;
case 'Initial Conditions'
    % load analysis times for the two loading phases
    deltaT = GroundMotion.dtAnalysis;
    tImposed = deltaT*(0:floor(GroundMotion.rampTime(1)/deltaT));
    tFree = deltaT*(length(tImposed)+1:floor((GroundMotion.rampTime(1)+GroundMotion.vibTime(1))/deltaT));
    t = [tImposed tFree];
    iStart = 1;
    iEnd = length(t);
    % set ground accelerations to zero
    ag = zeros(length(t),numMotions);
    % setup imposed displacements
    uMax = GroundMotion.initialDisp';
    Uimp = repmat((0:length(tImposed)-1),ndf,1).*repmat(uMax./(length(tImposed)-1),1,length(tImposed));
    iImpEnd = size(Uimp,2);
case 'None'
    % load total analysis time
    tEnd = GroundMotion.tEnd;
    deltaT = GroundMotion.dtAnalysis;
    t = deltaT*(0:floor(tEnd/deltaT));
    iStart = 1;
    iEnd = length(t);
    % set ground accelerations to zero
    ag = zeros(length(t),numMotions);
    % set imposed displacements to zero
    Uimp = zeros(ndf,1);
    iImpEnd = 1;
end


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Perform Transient Analysis
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
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
% animation downsampling factor (to animate at 10 Hz)
downSampFact = round(0.1/deltaT);

% calculations for each time step, i
tic;
for i=iStart:iEnd-1
   
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
    if ~rem(i,downSampFact)
        [~, f, MX] = AnimateResponse(Model.Type,t(:,1:i+1),ag(i+1,:),U(:,1:i+1),Udotdot(:,1:i+1),Pr(:,1:i+1),Um(:,1:i+1));
    end
end
toc;

% disconnect from experimental sites
for el=1:numElem
   if strncmp(Element{el}.type,'Element_Exp',11)
      feval(Element{el}.type,'disconnect',Element{el});
   end
end

% calculate final error values
err = Um-U;
TI = [];
TI(1,:) = 0.5*(cumtrapz(U(1,:),Um(1,:)) - cumtrapz(Um(1,:),U(1,:)));
if ~strcmp(Model.Type,'1 DOF')
    TI(2,:) = 0.5*(cumtrapz(U(2,:),Um(2,:)) - cumtrapz(Um(2,:),U(2,:)));
end

% assign response quantities
Response.Time = t;
Response.U = U;
Response.Udot = Udot;
Response.Udotdot = Udotdot;
Response.Pr = Pr;
Response.Um = Um;
Response.error = err;
Response.f = f';
Response.MX = MX';
Response.TI = TI;
Response.ag = ag';
