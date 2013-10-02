%INITIALIZESIMULATION to initialize the parameters needed to build the Simulink model
%
% implemented by Andreas Schellenberg (andreas.schellenberg@gmail.com) 11/2004

clear;
close all;
clc;

% set time steps
HybridCtrlParameters.dtInt = 0.02;           % integration time step (sec)
HybridCtrlParameters.dtSim = 0.04;           % simulation time step (sec)
HybridCtrlParameters.dtCon = 1/1000;         % controller time step (sec)
HybridCtrlParameters.delay = 0.0;            % delay due to undershoot (sec)

% calculate max number of substeps
HybridCtrlParameters.N = round(HybridCtrlParameters.dtSim/HybridCtrlParameters.dtCon);
% update simulation time step
HybridCtrlParameters.dtSim = HybridCtrlParameters.N*HybridCtrlParameters.dtCon;

% calculate number of delay steps
HybridCtrlParameters.iDelay = round(HybridCtrlParameters.delay./HybridCtrlParameters.dtCon);

% check that finite state machine does not deadlock
delayRatio = HybridCtrlParameters.iDelay/HybridCtrlParameters.N;
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
HybridCtrlParameters.Rate = HybridCtrlParameters.dtSim/HybridCtrlParameters.dtInt;

% displacement and velocity limits
HybridCtrlParameters.dspLim =  7.5;
HybridCtrlParameters.velLim = 23.0;

disp('Model Properties:');
disp('=================');
disp(HybridCtrlParameters);

% initialization of tunable parameters
nAct = 1;
newTarget = 0;
targDsp = zeros(1,nAct);
