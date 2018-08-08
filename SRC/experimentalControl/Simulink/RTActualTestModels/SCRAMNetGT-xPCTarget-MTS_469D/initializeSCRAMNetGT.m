%INITIALIZESCRAMNETGT to initialize the parameters needed for SCRAMNet GT
%
% created by Brad Thoen (MTS)
% modified by Andreas Schellenberg (andreas.schellenberg@gmail.com) 01/2014

%%%%%%%%%% SIGNAL COUNTS %%%%%%%%%%

nAct    = 12;                               % number of actuators
nAcc    = 11;                               % number of accelerometers
nTbl    = 6;                                % number of table degree-of-freedom
nAdcU   = 40;                               % number of user a/d channels
nDucU   = 4;                                % number of user duc's
nUDPInp = 1+nTbl+2*nAct+nAcc+nAdcU+nDucU;   % no. of outputs from simulink bridge
nUDPOut = 1+6*nTbl+nAct+nAdcU+nDucU;        % no. of inputs to simulink bridge

%%%%%%%%%% SAMPLE PERIODS %%%%%%%%%%

controlPeriod = 1/2048;  % controller sample period [sec]
samplePeriod  = controlPeriod/HybridCtrlParameters.upFact;

%%%%%%%%%% SCRAMNET PARTITIONS %%%%%%%%%%

syncNode = 1;   % synchronization node: MTS 469D
xpcNode	 = 2;   % xPC-Target node
opfNode  = 3;   % OpenFresco node


%%%%%%%%%% START MTS 469D %%%%%%%%%%

%%%%%%%%%% outputs to scramnet %%%%%%%%%%

% master span
baseAddress = 0;
outpartition(1).Address	= ['0x', dec2hex(baseAddress*4)];
outpartition(1).Type = 'single';
outpartition(1).Size = '1';

% reference
outpartition(2).Type = 'single';
outpartition(2).Size = num2str(nTbl);

% displ feedback (used only in realtime simulation mode)
outpartition(3).Type = 'single';
outpartition(3).Size = num2str(nAct);

% accel feedback (used only in realtime simulation mode)
outpartition(4).Type = 'single';
outpartition(4).Size = num2str(nAcc);

% force feedback (used only in realtime simulation mode)
outpartition(5).Type = 'single';
outpartition(5).Size = num2str(nAct);

% servovalve offsets
outpartition(6).Type = 'single';
outpartition(6).Size = num2str(nAct);

% user ADCs (used only in realtime simulation mode)
outpartition(7).Type = 'single';
outpartition(7).Size = num2str(nAdcU);

% user DUCs (used only in realtime simulation mode)
outpartition(8).Type = 'single';
outpartition(8).Size = num2str(nDucU);

%%%%%%%%%% inputs from scramnet %%%%%%%%%%

% master span
baseAddress = 256;
inppartition(1).Address	= ['0x', dec2hex(baseAddress*4)];
inppartition(1).Type = 'single';
inppartition(1).Size = '1';

% reference
inppartition(2).Type = 'single';
inppartition(2).Size = num2str(nTbl);

% feedback
inppartition(3).Type = 'single';
inppartition(3).Size = num2str(nTbl);

% displ feedback
inppartition(4).Type = 'single';
inppartition(4).Size = num2str(nTbl);

% veloc feedback
inppartition(5).Type = 'single';
inppartition(5).Size = num2str(nTbl);

% accel feedback
inppartition(6).Type = 'single';
inppartition(6).Size	= num2str(nTbl);

% force feedback
inppartition(7).Type = 'single';
inppartition(7).Size = num2str(nTbl);

% valve command
inppartition(8).Type = 'single';
inppartition(8).Size = num2str(nAct);

% user ADCs
inppartition(9).Type = 'single';
inppartition(9).Size = num2str(nAdcU);

% user DUCs
inppartition(10).Type = 'single';
inppartition(10).Size = num2str(nDucU);

%%%%%%%%%% END MTS 469D %%%%%%%%%%


%%%%%%%%%% START OPENFRESCO %%%%%%%%%%

%%%%%%%%%% inputs from OpenFresco %%%%%%%%%%

% newTarget (from)
baseAddress = 512;
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
