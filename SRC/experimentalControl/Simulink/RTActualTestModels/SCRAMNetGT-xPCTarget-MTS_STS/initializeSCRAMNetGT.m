%INITIALIZESCRAMNETGT to initialize the parameters needed for SCRAMNet GT
%
% created by Brad Thoen (MTS)
% modified by Andreas Schellenberg (andreas.schellenberg@gmail.com) 01/2014

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

controlPeriod = 1/2048;  % controller sample period [sec]
samplePeriod  = controlPeriod/HybridCtrlParameters.upFact;

%%%%%%%%%% SCRAMNET PARTITIONS %%%%%%%%%%

syncNode = 1;   % synchronization node: MTS STS
xpcNode	 = 2;   % xPC-Target node
opfNode  = 3;   % OpenFresco node


%%%%%%%%%% START MTS (STS) %%%%%%%%%%

%%%%%%%%%% outputs to scramnet %%%%%%%%%%

% master span
baseAddress = 0;
outpartition(1).Address = ['0x', dec2hex(baseAddress*4)];
outpartition(1).Type = 'single';
outpartition(1).Size = '1';

% control modes
outpartition(2).Type = 'uint32';
outpartition(2).Size = num2str(nAct);

% displ commands
outpartition(3).Type = 'single';
outpartition(3).Size = num2str(nAct);

% force commands
outpartition(4).Type = 'single';
outpartition(4).Size = num2str(nAct);

% displ feedbacks (used only in realtime simulation mode)
outpartition(5).Type = 'single';
outpartition(5).Size = num2str(nAct);

% force feedbacks (used only in realtime simulation mode)
outpartition(6).Type = 'single';
outpartition(6).Size = num2str(nAct);

% deltaP feedbacks (used only in realtime simulation mode)
outpartition(7).Type = 'single';
outpartition(7).Size = num2str(nAct);

% user ADCs (used only in realtime simulation mode)
outpartition(8).Type = 'single';
outpartition(8).Size = num2str(nAdcU);

% user DUCs (used only in realtime simulation mode)
outpartition(9).Type = 'single';
outpartition(9).Size = num2str(nDucU);

% user ENCs (used only in realtime simulation mode)
outpartition(10).Type = 'single';
outpartition(10).Size = num2str(nEncU);

% digital outputs to controller
outpartition(11).Type = 'uint32';
outpartition(11).Size = num2str(nDout);

%%%%%%%%%% inputs from scramnet %%%%%%%%%%

% master span
baseAddress = 75;
inppartition(1).Address	= ['0x', dec2hex(baseAddress*4)];
inppartition(1).Type = 'single';
inppartition(1).Size = '1';

% control modes
inppartition(2).Type = 'uint32';
inppartition(2).Size = num2str(nAct);

% displ commands
inppartition(3).Type = 'single';
inppartition(3).Size = num2str(nAct);

% force commands
inppartition(4).Type = 'single';
inppartition(4).Size = num2str(nAct);

% displ feedbacks
inppartition(5).Type = 'single';
inppartition(5).Size = num2str(nAct);

% force feedbacks
inppartition(6).Type = 'single';
inppartition(6).Size = num2str(nAct);

% deltaP feedbacks
inppartition(7).Type = 'single';
inppartition(7).Size = num2str(nAct);

% valve commands
inppartition(8).Type = 'single';
inppartition(8).Size = num2str(nAct);

% user ADCs
inppartition(9).Type = 'single';
inppartition(9).Size = num2str(nAdcU);

% user DUCs
inppartition(10).Type = 'single';
inppartition(10).Size = num2str(nDucU);

% user ENCs
inppartition(11).Type = 'single';
inppartition(11).Size = num2str(nEncU);

% digital inputs from controller
inppartition(12).Type = 'uint32';
inppartition(12).Size = num2str(nDinp);

% nEncCmd outputs to scramnet
inppartition(13).Type = 'single';
inppartition(13).Size = num2str(nEncCmd);

% nEncCmd inputs from scramnet
inppartition(14).Type = 'single';
inppartition(14).Size = num2str(nEncCmd);

%%%%%%%%%% END MTS (STS) %%%%%%%%%%


%%%%%%%%%% START PACIFIC %%%%%%%%%%

% blank space (since it is not used here)
baseAddress = 1024;
pacpartition(1).Address	= ['0x', dec2hex(baseAddress*4)];
pacpartition(1).Type = 'uint32';
pacpartition(1).Size = '94231';

%%%%%%%%%% END PACIFIC %%%%%%%%%%


%%%%%%%%%% START OPENFRESCO %%%%%%%%%%

%%%%%%%%%% inputs from OpenFresco %%%%%%%%%%

% newTarget (from)
baseAddress = 95255;
opfpartition(1).Address = ['0x', dec2hex(baseAddress*4)];
opfpartition(1).Type = 'int32';
opfpartition(1).Size = '1';

% disp commands (from)
opfpartition(2).Type = 'single';
opfpartition(2).Size = num2str(nAct);

% vel commands (from)
opfpartition(3).Type = 'single';
opfpartition(3).Size = num2str(nAct);

% accel commands (from)
opfpartition(4).Type = 'single';
opfpartition(4).Size = num2str(nAct);

% force commands (from)
opfpartition(5).Type = 'single';
opfpartition(5).Size = num2str(nAct);

% time commands (from)
opfpartition(6).Type = 'single';
opfpartition(6).Size = num2str(nAct);

%%%%%%%%%% outputs to OpenFresco %%%%%%%%%%

% switchPC (to)
opfpartition(7).Type = 'int32';
opfpartition(7).Size = '1';

% atTarget (to)
opfpartition(8).Type = 'int32';
opfpartition(8).Size = '1';

% disp feedbacks (to)
opfpartition(9).Type = 'single';
opfpartition(9).Size = num2str(nAct);

% vel feedbacks (to)
opfpartition(10).Type = 'single';
opfpartition(10).Size = num2str(nAct);

% accel feedbacks (to)
opfpartition(11).Type = 'single';
opfpartition(11).Size = num2str(nAct);

% force feedbacks (to)
opfpartition(12).Type = 'single';
opfpartition(12).Size = num2str(nAct);

% time feedbacks (to)
opfpartition(13).Type = 'single';
opfpartition(13).Size = num2str(nAct);

%%%%%%%%%% END OPENFRESCO %%%%%%%%%%


%%%%%%%%%% scramnet node configuration %%%%%%%%%%

mask = sprintf('0x%8.8X', bitshift(1, syncNode));
node = scgtnodestruct([]);
node.Interface.NodeID                                  = num2str(xpcNode);
node.Interface.Interrupts.ChangeBroadcastInterruptMask = 'yes';
node.Interface.Interrupts.BroadcastInterruptMask       = mask;
outpartition    = scgtpartitionstruct(outpartition);
inppartition    = scgtpartitionstruct(inppartition);
pacpartition    = scgtpartitionstruct(pacpartition);
opfpartition    = scgtpartitionstruct(opfpartition);
node.Partitions = [outpartition inppartition pacpartition opfpartition];
node            = scgtnodestruct(node);
