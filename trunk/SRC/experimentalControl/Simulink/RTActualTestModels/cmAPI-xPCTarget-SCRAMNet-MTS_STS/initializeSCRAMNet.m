%INITIALIZESCRAMNET to initialize the parameters needed for SCRAMNet
%
% created by Brad Thoen (MTS)
% modified by Andreas Schellenberg (andreas.schellenberg@gmail.com) 11/2004
%
% $Revision: 373 $
% $Date: 2014-10-09 01:56:20 -0700 (Thu, 09 Oct 2014) $
% $URL: svn://openfresco.berkeley.edu/usr/local/svn/OpenFresco/trunk/SRC/experimentalControl/Simulink/RTActualTestModels/cmAPI-xPCTarget-STS/initializeSimulation.m $

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

controlPeriod = 1/1024;  % controller sample period (sec)
samplePeriod  = controlPeriod/HybridCtrlParameters.upFact;

%%%%%%%%%% SCRAMNET PARTITIONS %%%%%%%%%%

syncNode = 1;   % synchronization node: MTS STS
xpcNode  = 2;   % xPC-Target node


%%%%%%%%%% START MTS (STS) %%%%%%%%%%

%%%%%%%%%% outputs to scramnet %%%%%%%%%%

% master span
baseAddress = 0;
partition(1).Address = ['0x', dec2hex(baseAddress*4)];
partition(1).Type = 'single';
partition(1).Size = '1';

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
irqPartition = 25;

% blank space (up to 1024)
partition(26).Type = 'uint32';
partition(26).Size = '862';

%%%%%%%%%% END MTS (STS) %%%%%%%%%%


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
node.Interface.NodeID                      = num2str(xpcNode);
node.Interface.Timeout.NumOfNodesInRing    = '10';
node.Interface.Timeout.TotalCableLengthInM = '50';
bub = completepartitionstruct(partition);
node.Partitions = completepartitionstruct(partition);
node            = completenodestruct(node);

% display interrupt longword location
irqAddr = node.Partitions(irqPartition).Internal.Address / 4 ...
    + node.Partitions(irqPartition).Internal.Sizes(1) - 1;

disp('SCRAMNet Interrupt Longword Location:');
disp('=====================================');
disp(irqAddr);
