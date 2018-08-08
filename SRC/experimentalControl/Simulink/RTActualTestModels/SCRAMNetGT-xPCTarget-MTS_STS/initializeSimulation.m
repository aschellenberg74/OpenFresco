%INITIALIZESIMULATION to initialize the parameters needed to build the Simulink model
%
% created by Andreas Schellenberg (andreas.schellenberg@gmail.com) 01/2014

clear;
close all;
clc;

%%%%%%%%%% HYBRID CONTROLLER PARAMETERS %%%%%%%%%%

% set number of degrees-of-freedom
dofID = [1];
HybridCtrlParameters.nDOF = length(dofID);

% set time steps
HybridCtrlParameters.upFact   = 5;          % upsample factor
HybridCtrlParameters.dtInt    = 10/2048;    % integration time step [sec]
HybridCtrlParameters.dtSim    = 10/2048;    % simulation time step [sec]
HybridCtrlParameters.dtCon    = 1/2048;     % controller time step [sec]
HybridCtrlParameters.delay    = zeros(1,HybridCtrlParameters.nDOF);
%HybridCtrlParameters.delay(1) = 0/2048;     % delay compensation DOF 1 [sec]
%HybridCtrlParameters.delay(2) = 0/2048;     % delay compensation DOF 2 [sec]

% update controller time step
HybridCtrlParameters.dtCon = HybridCtrlParameters.dtCon/HybridCtrlParameters.upFact;
% calculate number of substeps
HybridCtrlParameters.N = round(HybridCtrlParameters.dtSim/HybridCtrlParameters.dtCon);
% update simulation time step
HybridCtrlParameters.dtSim = HybridCtrlParameters.N*HybridCtrlParameters.dtCon;

% calculate number of delay steps
HybridCtrlParameters.iDelay = round(HybridCtrlParameters.delay./HybridCtrlParameters.dtCon);

% check that finite state machine does not deadlock
delayRatio = max(HybridCtrlParameters.iDelay)/HybridCtrlParameters.N;
if (delayRatio>0.6 && delayRatio<0.8)
    warndlg(['The delay compensation exceeds 60% of the simulation time step.', ...
        'Please consider increasing the simulation time step in order to avoid oscillations.'], ...
        'WARNING');
elseif (delayRatio>=0.8)
    errordlg(['The delay compensation exceeds 80% of the simulation time step.', ...
        'The simulation time step must be increased in order to avoid deadlock.'], ...
        'ERROR');
    return
end

% update delay time
HybridCtrlParameters.delay = HybridCtrlParameters.iDelay*HybridCtrlParameters.dtCon;

% calculate testing rate
HybridCtrlParameters.rate = HybridCtrlParameters.dtSim/HybridCtrlParameters.dtInt;

disp('Model Properties:');
disp('=================');
disp(HybridCtrlParameters);
