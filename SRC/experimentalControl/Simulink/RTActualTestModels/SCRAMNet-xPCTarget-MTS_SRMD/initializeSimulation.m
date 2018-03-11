%INITIALIZESIMULATION to initialize the parameters needed to build the Simulink model
%
% created by Andreas Schellenberg (andreas.schellenberg@gmail.com) 11/2004
%
% $Revision: 373 $
% $Date: 2014-10-09 01:56:20 -0700 (Thu, 09 Oct 2014) $
% $URL: svn://openfresco.berkeley.edu/usr/local/svn/OpenFresco/trunk/SRC/experimentalControl/Simulink/RTActualTestModels/cmAPI-xPCTarget-STS/initializeSimulation.m $

clear;
close all;
clc;

%%%%%%%%%% HYBRID CONTROLLER PARAMETERS %%%%%%%%%%

% set number of degrees-of-freedom
HybridCtrlParameters.nDOF = 1;

% set time steps
HybridCtrlParameters.upFact   = 4;          % upsample factor
HybridCtrlParameters.dtInt    = 0.02;       % integration time step (sec)
HybridCtrlParameters.dtSim    = 0.02;       % simulation time step (sec)
HybridCtrlParameters.dtCon    = 1/1000;     % controller time step (sec)
HybridCtrlParameters.delay    = zeros(1,HybridCtrlParameters.nDOF);
%HybridCtrlParameters.delay(1) = 0/1024;     % delay compensation DOF 1 (sec)
%HybridCtrlParameters.delay(2) = 0/1024;     % delay compensation DOF 2 (sec)

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
