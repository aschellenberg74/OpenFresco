%INITIALIZESCRAMNETGT to initialize the parameters needed for SCRAMNet GT
%
% created by Brad Thoen (MTS)
% modified by Andreas Schellenberg (andreas.schellenberg@gmail.com) 01/2014

%%%%%%%%%% SIGNAL COUNTS %%%%%%%%%%

nAct  = 6;   % number of actuators
nAdcU = 8;   % number of user a/d channels

%%%%%%%%%% SCRAMNET PARTITIONS %%%%%%%%%%

opfNode  = 10;   % OpenFresco node
xpcNode	 = 20;   % xPC-Target node
syncNode = 30;   % synchronization node: MTS FlexTest

%%%%%%%%%% START MTS (FlexTest) %%%%%%%%%%

%%%%%%%%%% outputs to FlexTest from xPC %%%%%%%%%%

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

%%%%%%%%%% inputs from FlexTest to xPC %%%%%%%%%%

% master span
baseAddress = 64;
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

% valve commands
inppartition(7).Type = 'single';
inppartition(7).Size = num2str(nAct);

% user ADCs
inppartition(8).Type = 'single';
inppartition(8).Size = num2str(nAdcU);

%%%%%%%%%% END MTS (FlexTest) %%%%%%%%%%


%%%%%%%%%% START OPENFRESCO %%%%%%%%%%

%%%%%%%%%% inputs from OpenFresco to xPC %%%%%%%%%%

% newTarget (from)
baseAddress = 1024;
opfpartition(1).Address = ['0x', dec2hex(baseAddress*4)];
opfpartition(1).Type = 'int32';
opfpartition(1).Size = '1';

% control signals (from)
opfpartition(2).Type = 'single';
opfpartition(2).Size = num2str(nDOF);

%%%%%%%%%% outputs to OpenFresco from xPC %%%%%%%%%%

% switchPC (to)
opfpartition(3).Type = 'int32';
opfpartition(3).Size = '1';

% atTarget (to)
opfpartition(4).Type = 'int32';
opfpartition(4).Size = '1';

% daq signals (to)
opfpartition(5).Type = 'single';
opfpartition(5).Size = num2str(2*nDOF);

%%%%%%%%%% END OPENFRESCO %%%%%%%%%%


%%%%%%%%%% scramnet node configuration %%%%%%%%%%

mask = sprintf('0x%8.8X', bitshift(1, syncNode));
node = scgtnodestruct([]);
node.Interface.NodeID                                  = num2str(xpcNode);
node.Interface.Interrupts.ChangeBroadcastInterruptMask = 'yes';
node.Interface.Interrupts.BroadcastInterruptMask       = mask;
outpartition    = scgtpartitionstruct(outpartition);
inppartition    = scgtpartitionstruct(inppartition);
opfpartition    = scgtpartitionstruct(opfpartition);
node.Partitions = [outpartition inppartition opfpartition];
node            = scgtnodestruct(node);
