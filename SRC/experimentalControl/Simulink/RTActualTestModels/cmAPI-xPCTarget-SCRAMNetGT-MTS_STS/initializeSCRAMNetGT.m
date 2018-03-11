%INITIALIZESCRAMNETGT to initialize the parameters needed for SCRAMNet GT
%
% created by Brad Thoen (MTS)
% modified by Andreas Schellenberg (andreas.schellenberg@gmail.com) 01/2014
%
% $Revision: $
% $Date: $
% $URL: $

%%%%%%%%%% SIGNAL COUNTS %%%%%%%%%%

nAct    = 8;                                         % number of actuators
nAdcU   = 12;                                        % number of user a/d channels
nDucU   = 8;                                         % number of user ducs
nEncU   = 2;                                         % number of user encoders
nDinp   = 4;                                         % no. of digital inputs written to scramnet
nDout   = 4;                                         % no. of digital outputs driven by scramnet
nEncCmd = 2;                                         % number of encoders commands
nUDPOut = 1+7*nAct+nAdcU+nDucU+nEncU+nDinp+nEncCmd;  % no. of outputs from simulink bridge
nUDPInp = 1+6*nAct+nAdcU+nDucU+nEncU+nDinp+nEncCmd;  % no. of inputs to simulink bridge

%%%%%%%%%% SAMPLE PERIODS %%%%%%%%%%

controlPeriod = 1/2048;		% controller sample period (sec)
samplePeriod  = controlPeriod/HybridCtrlParameters.upFact;

%%%%%%%%%% SCRAMNET PARTITIONS %%%%%%%%%%

syncNode = 1;   % synchronization node: MTS STS
xpcNode  = 2;   % xPC-Target node


%%%%%%%%%% START MTS (STS) %%%%%%%%%%

%%%%%%%%%% outputs to scramnet %%%%%%%%%%

% master span
baseAddress          = 0;
partition(1).Address = ['0x', dec2hex(baseAddress*4)];
partition(1).Type    = 'single';
partition(1).Size    = '1';

% control modes
partition(2).Type = 'uint32';
partition(2).Size = num2str(nAct);

% displ commands
partition(3).Type = 'single';
partition(3).Size = num2str(nAct);

% force commands
partition(4).Type = 'single';
partition(4).Size = num2str(nAct);

% displ feedbacks (used only in realtime simulation mode)
partition(5).Type = 'single';
partition(5).Size = num2str(nAct);

% force feedbacks (used only in realtime simulation mode)
partition(6).Type = 'single';
partition(6).Size = num2str(nAct);

% deltaP feedbacks (used only in realtime simulation mode)
partition(7).Type = 'single';
partition(7).Size = num2str(nAct);

% user ADCs (used only in realtime simulation mode)
partition(8).Type = 'single';
partition(8).Size = num2str(nAdcU);

% user DUCs (used only in realtime simulation mode)
partition(9).Type = 'single';
partition(9).Size = num2str(nDucU);

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

% digital inputs from controller
partition(23).Type = 'uint32';
partition(23).Size = num2str(nDinp);

% nEncCmd outputs to scramnet
partition(24).Type = 'single';
partition(24).Size = num2str(nEncCmd);

% nEncCmd inputs from scramnet
partition(25).Type = 'single';
partition(25).Size = num2str(nEncCmd);
irqPartition       = 25;

% blank space (up to 1024)
partition(26).Type = 'uint32';
partition(26).Size = '862';

%%%%%%%%%% END MTS (STS) %%%%%%%%%%


%%%%%%%%%% scramnet node configuration %%%%%%%%%%

mask = sprintf('0x%8.8X', bitshift(1, syncNode));
node = scgtnodestruct([]);
node.Interface.NodeID                                  = num2str(xpcNode);
node.Interface.Interrupts.ChangeBroadcastInterruptMask = 'yes';
node.Interface.Interrupts.BroadcastInterruptMask       = mask;
node.Partitions	= scgtpartitionstruct(partition);
node            = scgtnodestruct(node);
