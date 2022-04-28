%INITIALIZESIMULATION to initialize the parameters needed to build the Simulink model
%
% created by Andreas Schellenberg (andreas.schellenberg@gmail.com) 01/2014

%#ok<*NBRAK> 

clear;
close all;
clc;

%%%%%%%%%% MAIN SAMPLE PERIOD %%%%%%%%%%

controlPeriod = 1/2048;  % controller sample period [sec]

%%%%%%%%%% HYBRID CONTROLLER PARAMETERS %%%%%%%%%%

% set number of dof to control
nDOF = 2;  % Andreas said "use dof #2 for extra load cell"
HybridCtrlParameters.nDOF = nDOF;

% set time step related parameters
HybridCtrlParameters.dtInt  = 10/2048;      % integration time step [sec]
HybridCtrlParameters.dtSim  = 10/2048;      % simulation time step [sec]
HybridCtrlParameters.delay  = zeros(nDOF);  % average delay per dof [sec]
HybridCtrlParameters.upFact = 5;            % upsample factor for xPC HC block

%%%%%%%%%% CALCULATED PARAMETERS %%%%%%%%%%

% update controller time step
HybridCtrlParameters.dtCon = controlPeriod/HybridCtrlParameters.upFact;
% calculate number of substeps
HybridCtrlParameters.N = round(HybridCtrlParameters.dtSim/HybridCtrlParameters.dtCon);
% update simulation time step
HybridCtrlParameters.dtSim = HybridCtrlParameters.N*HybridCtrlParameters.dtCon;

% calculate number of delay steps
HybridCtrlParameters.iDelay = round(HybridCtrlParameters.delay./HybridCtrlParameters.dtCon);

% check that finite state machine does not deadlock
delayRatio = max(HybridCtrlParameters.iDelay)/HybridCtrlParameters.N;
if (delayRatio>0.8 && delayRatio<0.9)
    warndlg(['The delay compensation exceeds 80% of the simulation time step.', ...
        'Please consider increasing the simulation time step in order to avoid oscillations.'], ...
        'WARNING');
elseif (delayRatio>=0.9)
    errordlg(['The delay compensation exceeds 90% of the simulation time step.', ...
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
