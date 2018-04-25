%%%%%%%%%% SCRAMNET PARTITIONS %%%%%%%%%%

synNode = 1; % synchronization node (STS 493)
lhpNode = 2; % 469D 
xpcNode	= 3; % xPC Target node (Original)
opfNode = 4; % OpenFresco node


%%%%%%%%%% STS SCRAMNET PARTITIONS %%%%%%%%%%

inppartition = [];
outpartition = [];
node = [];

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
irqPartition = 9;

%%%%%%%%%% scramnet interrupt configuration %%%%%%%%%%

% % enable scramnet receive interrupt
% inppartition(irqPartition).RIE                       = 'last';
% node.Interface.Interrupts.HostInterrupt              = 'on';
% node.Interface.Interrupts.InterruptOnMemoryMaskMatch = 'on';
% node.Interface.Interrupts.NetworkInterrupt           = 'on';

% assemble input node data structure
node.Interface.NodeID                        = num2str(xpcNode);
node.Interface.Timeout.NumOfNodesInRing      = '10';
node.Interface.Timeout.TotalCableLengthInM   = '50';
outpartition	= completepartitionstruct(outpartition);
inppartition	= completepartitionstruct(inppartition);
node.Partitions	= [outpartition inppartition];
node            = completenodestruct(node);

% assign node
stsNode = node;

%%%%%%%%%% LHP SCRAMNET PARTITIONS %%%%%%%%%%

inppartition = [];
outpartition = [];
node = [];

%%%%%%%%%% signal counts %%%%%%%%%%

nTbl	= 1;			% number of table channels

%%%%%%%%%% outputs to scramnet %%%%%%%%%%

% master span
baseAddress				= 128;
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
node.Interface.NodeID                        = num2str(xpcNode);
node.Interface.Timeout.NumOfNodesInRing      = '10';
node.Interface.Timeout.TotalCableLengthInM   = '50';
outpartition	= completepartitionstruct(outpartition);
inppartition	= completepartitionstruct(inppartition);
node.Partitions	= [outpartition inppartition];
node            = completenodestruct(node);

% assign node
lhpNode = node;

% display interrupt longword location
irqAddr = node.Partitions(irqPartition).Internal.Address / 4 ...
		+ node.Partitions(irqPartition).Internal.Sizes(1) - 1
