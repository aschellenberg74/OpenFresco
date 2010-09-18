function Response = Integrator_NewmarkExplicit(Model,GroundMotion,InitialState,Analysis)
%INTEGRATOR_NEWMARKEXPLICIT to perform a direct integration analysis using
% the second order explicit Newmark method
% Response = Integrator_NewmarkExplicit(Model,GroundMotion,InitCond,Analysis)
%
% Response     : data structure with response results
% Model        : data structure with structural model information
% GroundMotion : data structure with ground motion information
% InitialState : data structure with initial conditions
% Analysis     : handles for analysis page

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
iStart = 1;
iEnd = length(t);
ag = zeros(iEnd,numMotions);
for mo=1:numMotions
    ag(:,mo) = interp1(GroundMotion.scalet{mo},GroundMotion.scaleag{mo},t,'linear',0.0);
end
b = Model.b;
% animation downsampling factor (to animate at 10 Hz)
downSampFact = round(0.1/deltaT);


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Perform Transient Analysis
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
tic;
% analysis parameters
gamma = 0.50;

% initialize global response variables
U = zeros(ndf,iEnd);
Udot = zeros(ndf,iEnd);
Udotdot = zeros(ndf,iEnd);
Pr = zeros(ndf,iEnd);
Um = zeros(ndf,iEnd);

% set initial conditions if existing
if ~isempty(InitialState)
   iStart = length(InitialState.Time);
   if isfield(InitialState,'U') && ~isempty(InitialState.U)
       U(:,1:iStart) = InitialState.U;
   end
   if isfield(InitialState,'Udot') && ~isempty(InitialState.Udot)
       Udot(:,1:iStart) = InitialState.Udot;
   end
   if isfield(InitialState,'Udotdot') && ~isempty(InitialState.Udotdot)
       Udotdot(:,1:iStart) = InitialState.Udotdot;
   end
   if isfield(InitialState,'Pr') && ~isempty(InitialState.Pr)
       Pr(:,1:iStart) = InitialState.Pr;
   end
   if isfield(InitialState,'Um') && ~isempty(InitialState.Um)
       Um(:,1:iStart) = InitialState.Um;
   end
end

% set the constants
c2 = gamma*deltaT;
c3 = 1.0;
a1 = 0.5*deltaT*deltaT;
a2 = deltaT*(1.0 - gamma);

% calculations for each time step, i
for i=iStart:iEnd-1
    
    % check if pause button is pressed
    while (~get(Analysis(7),'Value'))
        % check if stop button is pressed
        if get(Analysis(9),'Value')
            CurrentState.Time = t(:,1:i);
            CurrentState.U = U(:,1:i);
            CurrentState.Udot = Udot(:,1:i);
            CurrentState.Udotdot = Udotdot(:,1:i);
            CurrentState.Pr = Pr(:,1:i);
            CurrentState.Um = Um(:,1:i);
            Response = TerminateAnalysis(Model,GroundMotion,CurrentState,Analysis);
            % disconnect from experimental sites if not done so already
            for el=1:numElem
                if strncmp(Element{el}.type,'Element_Exp',11)
                    feval(Element{el}.type,'disconnect',Element{el});
                end
            end
            return;
        end
        pause(0.01);
    end
    
    % get new response quantities
    U(:,i+1) = U(:,i) + deltaT*Udot(:,i) + a1*Udotdot(:,i);
    Udot(:,i+1) = Udot(:,i) + a2*Udotdot(:,i);
    Udotdot(:,i+1) = 0.0;
    
    % get applied loads
    Ptp1 = -M*b*ag(i+1,:)';
    
    % Linear algorithm
    % set trial response in elements
    for el=1:numElem
        id = Element{el}.id;
        feval(Element{el}.type,'setTrialDisp',Element{el},U(id,i+1));
    end
    
    % get resisting forces from elements
    Pr(:,i+1) = zeros(ndf,1);
    for el=1:numElem
        id = Element{el}.id;
        Pr(id,i+1) = Pr(id,i+1) + feval(Element{el}.type,'getResistingForce',Element{el});
        Um(id,i+1) = Um(id,i+1) + feval(Element{el}.type,'getDisp',Element{el});
    end
    
    % get rhs and jacobian
    F  = M*Udotdot(:,i+1) + C*Udot(:,i+1) + Pr(:,i+1) - Ptp1;
    DF = c3*M + c2*C;
    
    % solve for accelerations at t+deltaT
    Udotdot(:,i+1) = DF\(-F);
    
    % update response quantities
    Udot(:,i+1) = Udot(:,i+1) + c2*Udotdot(:,i+1);
    
    % commit the elements
    for j=el:numElem
        feval(Element{el}.type,'commitState',Element{el});
    end
    
    % animate response
    if ~rem(i,downSampFact)
        AnimateResponse(Model.Type,t(:,1:i+1),ag(i+1,:),U(:,1:i+1),Udotdot(:,1:i+1),Pr(:,1:i+1),Um(:,1:i+1));
    end
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
