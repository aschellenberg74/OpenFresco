%INITIALIZESIMULATION to initialize the parameters needed to build the Simulink model
%
% implemented by Andreas Schellenberg (andreas.schellenberg@gmail.com) 11/2004
%
% $Revision$
% $Date$
% $URL$

clear;
close all;
clc;

% set number of degrees-of-freedom
nDOF = 2;

% set time steps
HybridCtrlParameters.dtInt = 0.01;           % integration time step (sec)
HybridCtrlParameters.dtSim = 0.05;           % simulation time step (sec)
HybridCtrlParameters.dtCon = 1/1000;         % controller time step (sec)
HybridCtrlParameters.delay(1) = 0/1000;      % delay compensation DOF 1 (sec)
HybridCtrlParameters.delay(2) = 0/1000;      % delay compensation DOF 2 (sec)

% calculate max number of substeps
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
HybridCtrlParameters.Rate = HybridCtrlParameters.dtSim/HybridCtrlParameters.dtInt;

% signal magnitude and rate limits
HybridCtrlParameters.magnLim = [30.0 21.0];
HybridCtrlParameters.rateLim = [100.0 100.0];

disp('Model Properties:');
disp('=================');
disp(HybridCtrlParameters);

% initialization of tunable parameters
newTarget  = uint32(0);
targSignal = zeros(1,nDOF);
