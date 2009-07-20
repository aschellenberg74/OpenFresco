%INITIALIZESIMULATION to initialization the parameters needed to build the Simulink model
%
% created by MTS
% modified by Andreas Schellenberg (andreas.schellenberg@gmx.net) 11/2004

clear;
close all;
clc;

%%%%%%%%%% HYBRID CONTROLLER PARAMETERS %%%%%%%%%%

% set time steps
HybridCtrlParameters.dtInt = 1/128;       %8.164e-003;  % integration time step (sec)
HybridCtrlParameters.dtSim = 1;           % simulation time step (sec)
HybridCtrlParameters.dtCon = 1/1024;      % controller time step (sec)
HybridCtrlParameters.delay = 0;           % delay due to undershoot (sec)
%HybridCtrlParameters.delay = 0.0661+0.0275;  % delay due to undershoot (sec)
%HybridCtrlParameters.delay = 0.0661+0.0488;  % delay due to undershoot (sec)

% calculate max number of substeps
HybridCtrlParameters.N = round(HybridCtrlParameters.dtSim/HybridCtrlParameters.dtCon);
% update simulation time step
HybridCtrlParameters.dtSim = HybridCtrlParameters.N*HybridCtrlParameters.dtCon;

% calculate number of delay steps
if (HybridCtrlParameters.delay == 0.0)
   HybridCtrlParameters.iDelay = HybridCtrlParameters.N;
else
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
end

% calculate testing rate
HybridCtrlParameters.Rate = HybridCtrlParameters.dtSim/HybridCtrlParameters.dtInt;

disp('Model Properties:');
disp('=================');
disp(HybridCtrlParameters);


%%%%%%%%%% SIGNAL COUNTS %%%%%%%%%%

nAct	  = 8;                                    % number of actuators
nAdcU	  = 12;                                   % number of user a/d channels
nDucU   = 8;                                    % number of user ducs
nEncU	  = 2;                                    % number of user encoders
nDinp	  = 4;                                    % no. of digital inputs written to scramnet
nDout	  = 4;                                    % no. of digital outputs driven by scramnet
nUDPOut = 1+7*nAct+nAdcU+nDucU+nEncU+nDinp;     % no. of outputs from simulink bridge
nUDPInp = 1+6*nAct+nAdcU+nDucU+nEncU+nDinp;     % no. of inputs to simulink bridge

%%%%%%%%%% SAMPLE PERIOD %%%%%%%%%%

samplePeriod = 1/1024;

%%%%%%%%%% SCRAMNET PARTITIONS %%%%%%%%%%

%%%%%%%%%% outputs to scramnet %%%%%%%%%%

% master span
baseAddress	         = 0;
partition(1).Address = ['0x', dec2hex(baseAddress*4)];
partition(1).Type	   = 'single';
partition(1).Size	   = '1';

% control modes
partition(2).Type	= 'uint32';
partition(2).Size	= num2str(nAct);

% displ commands
partition(3).Type	= 'single';
partition(3).Size	= num2str(nAct);

% force commands
partition(4).Type	= 'single';
partition(4).Size	= num2str(nAct);

% displ feedbacks (used only in realtime simulation mode)
partition(5).Type	= 'single';
partition(5).Size	= num2str(nAct);

% force feedbacks (used only in realtime simulation mode)
partition(6).Type	= 'single';
partition(6).Size	= num2str(nAct);

% deltaP feedbacks (used only in realtime simulation mode)
partition(7).Type	= 'single';
partition(7).Size	= num2str(nAct);

% user ADCs (used only in realtime simulation mode)
partition(8).Type = 'single';
partition(8).Size	= num2str(nAdcU);

% user DUCs (used only in realtime simulation mode)
partition(9).Type	= 'single';
partition(9).Size	= num2str(nDucU);

% user ENCs (used only in realtime simulation mode)
partition(10).Type = 'single';
partition(10).Size = num2str(nEncU);

% digital outputs to controller
partition(11).Type = 'uint32';
partition(11).Size = num2str(nDout);

%%%%%%%%%% inputs from scramnet %%%%%%%%%%

% master span
partition(12).Type = 'single';
partition(12).Size = '1';

% control modes
partition(13).Type = 'uint32';
partition(13).Size = num2str(nAct);

% displ commands
partition(14).Type = 'single';
partition(14).Size = num2str(nAct);

% force commands
partition(15).Type = 'single';
partition(15).Size = num2str(nAct);

% displ feedbacks
partition(16).Type = 'single';
partition(16).Size = num2str(nAct);

% force feedbacks
partition(17).Type = 'single';
partition(17).Size = num2str(nAct);

% deltaP feedbacks
partition(18).Type = 'single';
partition(18).Size = num2str(nAct);

% valve commands
partition(19).Type = 'single';
partition(19).Size = num2str(nAct);

% user ADCs
partition(20).Type = 'single';
partition(20).Size = num2str(nAdcU);

% user DUCs
partition(21).Type = 'single';
partition(21).Size = num2str(nDucU);

% user ENCs
partition(22).Type = 'single';
partition(22).Size = num2str(nEncU);
irqPartition       = 22;

% digital inputs from controller
partition(23).Type = 'uint32';
partition(23).Size = num2str(nDinp);

% blank space
partition(24).Type = 'uint32';
partition(24).Size = '866';

%%%%%%%%%% START PACIFIC %%%%%%%%%%

% PI software parameters
chanID         = [1,2,5,6,7,10]   % index vector of channels to be extracted from SCRAMNET
chanOffSet     = 0;        % offset (0, 16, 32, 48, 64, 80, 96, 112, 128) since total number of channels are divided in groups of 16; limited by xPC
CVTcompression = 8;        % number of channels per card allocated by software (do not change), current config (March 2005): 8 channels
% PI hardware parameters
nCard          = 4;        % max number of PI cards; limited by xPC (do not change), current config (March 2005): 4 cards
nChan          = 4;        % number of channels per card (do not change), current config (March 2005): 4 channels per card
% index vectors
chanID         = chanID - chanOffSet;
countsID       = ((ceil(chanID/nChan)-1)*CVTcompression + (chanID/nChan-(ceil(chanID/nChan)-1))*nChan)*2;
calibrationID  = [(countsID./2-1)*11+6,(countsID./2-1)*11+7,(countsID./2-1)*11+8,(countsID./2-1)*11+9,(countsID./2-1)*11+10];

%%%%%%%%%% inputs from scramnet %%%%%%%%%%

% heartbeat (memory address 0X1000)
partition(25).Type = 'uint32';
partition(25).Size = '1';

% raw data [counts]
partition(26).Type = 'int16';
partition(26).Size = num2str(nCard*CVTcompression*2);

% blank space (remainder of 1024 words - heartbeat - raw data)
partition(27).Type = 'uint32';
partition(27).Size = num2str(1024 - 1 - nCard*CVTcompression);

% calibration header (memory address 0X2000)
partition(28).Type = 'uint8';
partition(28).Size = '88';

% calibration constants
partition(29).Type = 'double';
partition(29).Size = num2str(nCard*CVTcompression*11);

% blank space (remainder of 1024 words - header - constants)
partition(30).Type = 'uint32';
partition(30).Size = num2str(1024 - 11 - nCard*CVTcompression*11);

%%%%%%%%%% scramnet interrupt configuration %%%%%%%%%%

% enable scramnet receive interrupt
partition(irqPartition).RIE                          = 'last';
node.Interface.Interrupts.HostInterrupt              = 'on';
node.Interface.Interrupts.InterruptOnMemoryMaskMatch = 'on';
node.Interface.Interrupts.NetworkInterrupt           = 'on';

% assemble input node data structure
% NOTE:  Use the modified completepartitionstruct() that
%        allows discontiguous partitions--- the original one
%        provide by Mathworks packs them together.
node.Interface.NodeID                      = '0';
node.Interface.Timeout.NumOfNodesInRing    = '10';
node.Interface.Timeout.TotalCableLengthInM = '50';
bub = completepartitionstruct(partition);
node.Partitions = completepartitionstruct(partition);
node            = completenodestruct(node);

% display interrupt longword location
irqAddr = node.Partitions(irqPartition).Internal.Address / 4 ...
		+ node.Partitions(irqPartition).Internal.Sizes(1) - 1;