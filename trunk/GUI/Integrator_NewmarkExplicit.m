function Response = Integrator_NewmarkExplicit(Model,GroundMotion,InitialState,Analysis)
%INTEGRATOR_NEWMARKEXPLICIT to perform a direct integration analysis using
% the second order explicit Newmark method
% Response = Integrator_NewmarkExplicit(Model,GroundMotion,InitialState,Analysis)
%
% Response     : data structure with response results
% Model        : data structure with structural model information
% GroundMotion : data structure with ground motion information
% InitialState : data structure with initial conditions
% Analysis     : handles for analysis page
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%                          OpenFresco Express                          %%
%%    GUI for the Open Framework for Experimental Setup and Control     %%
%%                                                                      %%
%%   (C) Copyright 2011, The Pacific Earthquake Engineering Research    %%
%%            Center (PEER) & MTS Systems Corporation (MTS)             %%
%%                         All Rights Reserved.                         %%
%%                                                                      %%
%%     Commercial use of this program without express permission of     %%
%%                 PEER and MTS is strictly prohibited.                 %%
%%     See Help -> OpenFresco Express Disclaimer for information on     %%
%%   usage and redistribution, and for a DISCLAIMER OF ALL WARRANTIES.  %%
%%                                                                      %%
%%   Developed by:                                                      %%
%%     Andreas Schellenberg (andreas.schellenberg@gmail.com)            %%
%%     Carl Misra (carl.misra@gmail.com)                                %%
%%     Stephen A. Mahin (mahin@berkeley.edu)                            %%
%%                                                                      %%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% $Revision$
% $Date$
% $URL$

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
% animation downsampling factor (to animate at 10 Hz)
downSampFact = round(0.1/deltaT);

% calculations for each time step, i
tic;
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
    Udotdot(:,i+1) = zeros(ndf,1);
    
    % get applied loads
    Ptp1 = -M*b*ag(i+1,:)';
    
    % impose displacements statically
    if (i < iImpEnd)
        U(:,i+1) = Uimp(:,i+1);
    end
    
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
    
    if (i >= iImpEnd)
        % update Udot
        Udot(:,i+1) = Udot(:,i+1) + c2*Udotdot(:,i+1);
    else
        % zero Udot because U is imposed statically
        Udot(:,i+1) = zeros(ndf,1);
    end
    
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
