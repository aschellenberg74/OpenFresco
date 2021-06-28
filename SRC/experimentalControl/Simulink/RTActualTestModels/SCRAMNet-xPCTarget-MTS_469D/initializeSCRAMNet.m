%INITIALIZESCRAMNET to initialize the parameters needed for SCRAMNet
%
% created by Brad Thoen (MTS)
% modified by Andreas Schellenberg (andreas.schellenberg@gmail.com) 01/2014

%%%%%%%%%% SIGNAL COUNTS %%%%%%%%%%

nTbl	= 1;			    % number of table channels
nAct    = 4;			    % number of actuators
nDinp	= 4;				% no. of digital inputs written to scramnet
nDout	= 4;				% no. of digital outputs driven by scramnet
nUDPOut	= 1+7*nAct+nDinp;	% no. of outputs from simulink bridge
nUDPInp = 1+6*nAct+nDout;	% no. of inputs to simulink bridge

%%%%%%%%%% SAMPLE PERIODS %%%%%%%%%%

controlPeriod = 1/2048;		% controller sample period (sec)
samplePeriod  = controlPeriod/HybridCtrlParameters.upFact;

%%%%%%%%%% SCRAMNET PARTITIONS %%%%%%%%%%

stsNode  = 1;   % MTS STS node
syncNode = 2;	% synchronization node: MTS 469D
xpcNode	 = 3;	% xPC-Target node
opfNode  = 4;   % OpenFresco node


%%%%%%%%%% START MTS STS %%%%%%%%%%

%%%%%%%%%% outputs to scramnet %%%%%%%%%%

% master span
baseAddress				= 0+0;
outpartition(1).Address	= ['0x', dec2hex(baseAddress*4)];
outpartition(1).Type	= 'single';
outpartition(1).Size	= '1';

% control modes
outpartition(2).Type	= 'uint32';
outpartition(2).Size	= num2str(nAct);

% displ commands
outpartition(3).Type	= 'single';
outpartition(3).Size	= num2str(nAct);

% force commands
outpartition(4).Type	= 'single';
outpartition(4).Size	= num2str(nAct);

% displ feedbacks (used only in realtime simulation mode)
outpartition(5).Type	= 'single';
outpartition(5).Size	= num2str(nAct);

% force feedbacks (used only in realtime simulation mode)
outpartition(6).Type	= 'single';
outpartition(6).Size	= num2str(nAct);

% dp force feedbacks (used only in realtime simulation mode)
outpartition(7).Type	= 'single';
outpartition(7).Size	= num2str(nAct);

% digital outputs to controller
outpartition(8).Type	= 'uint32';
outpartition(8).Size	= num2str(nDout);

%%%%%%%%%% inputs from scramnet %%%%%%%%%%

% master span
baseAddress				= 0+64;
inppartition(1).Address	= ['0x', dec2hex(baseAddress*4)];
inppartition(1).Type	= 'single';
inppartition(1).Size	= '1';

% control modes
inppartition(2).Type	= 'uint32';
inppartition(2).Size	= num2str(nAct);

% displ commands
inppartition(3).Type	= 'single';
inppartition(3).Size	= num2str(nAct);

% force commands
inppartition(4).Type	= 'single';
inppartition(4).Size	= num2str(nAct);

% displ feedbacks
inppartition(5).Type	= 'single';
inppartition(5).Size	= num2str(nAct);

% force feedbacks
inppartition(6).Type	= 'single';
inppartition(6).Size	= num2str(nAct);

% dp force feedbacks
inppartition(7).Type	= 'single';
inppartition(7).Size	= num2str(nAct);

% valve commands
inppartition(8).Type	= 'single';
inppartition(8).Size	= num2str(nAct);

% digital inputs from controller
inppartition(9).Type	= 'uint32';
inppartition(9).Size	= num2str(nDinp);

% assemble input node data structure
node.Interface.NodeID                      = num2str(xpcNode);
node.Interface.Timeout.NumOfNodesInRing    = '10';
node.Interface.Timeout.TotalCableLengthInM = '50';
outpartition	= completepartitionstruct(outpartition);
inppartition	= completepartitionstruct(inppartition);
node.Partitions	= [outpartition inppartition];
node            = completenodestruct(node);

% assign node
stsNode = node;
clear inppartition outpartition node;

%%%%%%%%%% END MTS STS %%%%%%%%%%


%%%%%%%%%% START MTS 469D %%%%%%%%%%

%%%%%%%%%% outputs to scramnet %%%%%%%%%%

% master span
baseAddress				= 128+0;
outpartition(1).Address	= ['0x', dec2hex(baseAddress*4)];
outpartition(1).Type	= 'single';
outpartition(1).Size	= '1';

% reference
outpartition(2).Type	= 'single';
outpartition(2).Size	= num2str(nTbl);

%%%%%%%%%% inputs from scramnet %%%%%%%%%%

% master span
baseAddress				= 128+64;
inppartition(1).Address	= ['0x', dec2hex(baseAddress*4)];
inppartition(1).Type	= 'single';
inppartition(1).Size	= '1';

% reference
inppartition(2).Type	= 'single';
inppartition(2).Size	= num2str(nTbl);

% displ feedback
inppartition(3).Type	= 'single';
inppartition(3).Size	= num2str(nTbl);

% veloc feedback
inppartition(4).Type	= 'single';
inppartition(4).Size	= num2str(nTbl);

% accel feedback
inppartition(5).Type	= 'single';
inppartition(5).Size	= num2str(nTbl);

% force feedback
inppartition(6).Type	= 'single';
inppartition(6).Size	= num2str(nTbl);
irqPartition = 6;

%%%%%%%%%% scramnet interrupt configuration %%%%%%%%%%

% enable scramnet receive interrupt
inppartition(irqPartition).RIE					     = 'last';
node.Interface.Interrupts.HostInterrupt              = 'on';
node.Interface.Interrupts.InterruptOnMemoryMaskMatch = 'on';
node.Interface.Interrupts.NetworkInterrupt           = 'on';

% assemble input node data structure
node.Interface.NodeID                      = num2str(xpcNode);
node.Interface.Timeout.NumOfNodesInRing    = '10';
node.Interface.Timeout.TotalCableLengthInM = '50';
outpartition	= completepartitionstruct(outpartition);
inppartition	= completepartitionstruct(inppartition);
node.Partitions	= [outpartition inppartition];
node            = completenodestruct(node);

% assign node
lhpNode = node;
clear inppartition outpartition node;

%%%%%%%%%% END MTS 469D %%%%%%%%%%


%%%%%%%%%% START OPENFRESCO %%%%%%%%%%

%%%%%%%%%% inputs from OpenFresco %%%%%%%%%%

% newTarget (from)
baseAddress = 1024;
opfpartition(1).Address	= ['0x', dec2hex(baseAddress*4)];
opfpartition(1).Type = 'int32';
opfpartition(1).Size = '1';

% disp commands (from)
opfpartition(2).Type = 'single';
opfpartition(2).Size = num2str(nTbl);

% vel commands (from)
opfpartition(3).Type = 'single';
opfpartition(3).Size = num2str(nTbl);

% accel commands (from)
opfpartition(4).Type = 'single';
opfpartition(4).Size = num2str(nTbl);

% force commands (from)
opfpartition(5).Type = 'single';
opfpartition(5).Size = num2str(nTbl);

% time commands (from)
opfpartition(6).Type = 'single';
opfpartition(6).Size = num2str(nTbl);

%%%%%%%%%% outputs to OpenFresco %%%%%%%%%%

% switchPC (to)
opfpartition(7).Type = 'int32';
opfpartition(7).Size = '1';

% atTarget (to)
opfpartition(8).Type = 'int32';
opfpartition(8).Size = '1';

% disp feedbacks (to)
opfpartition(9).Type = 'single';
opfpartition(9).Size = num2str(nTbl);

% vel feedbacks (to)
opfpartition(10).Type = 'single';
opfpartition(10).Size = num2str(nTbl);

% accel feedbacks (to)
opfpartition(11).Type = 'single';
opfpartition(11).Size = num2str(nTbl);

% force feedbacks (to)
opfpartition(12).Type = 'single';
opfpartition(12).Size = num2str(nTbl);

% time feedbacks (to)
opfpartition(13).Type = 'single';
opfpartition(13).Size = num2str(nTbl);

% assemble input node data structure
node.Interface.NodeID                      = num2str(xpcNode);
node.Interface.Timeout.NumOfNodesInRing    = '10';
node.Interface.Timeout.TotalCableLengthInM = '50';
node.Partitions	= completepartitionstruct(opfpartition);
node            = completenodestruct(node);

% assign node
opfNode = node;
clear opfpartition node;

%%%%%%%%%% END OPENFRESCO %%%%%%%%%%
