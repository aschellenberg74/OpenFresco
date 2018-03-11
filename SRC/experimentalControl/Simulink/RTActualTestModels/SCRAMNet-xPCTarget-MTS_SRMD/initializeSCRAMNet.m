%INITIALIZESCRAMNET to initialize the parameters needed for SCRAMNet
%
% created by Brad Thoen (MTS)
% modified by Andreas Schellenberg (andreas.schellenberg@gmail.com) 11/2015
%
% $Revision: $
% $Date: $
% $URL: $

%%%%%%%%%% SIGNAL COUNTS %%%%%%%%%%

nRef    = 7;    % number of references
nInpU   = 8;    % number of user inputs
nAcc    = 3;    % number of acceleration feedbacks
nFrcTbl = 6;    % number of table force feedbacks
nFrcNrm = 5;    % number of normal force feedbacks (outriggers + lift pressure)
nAdcU   = 12;   % number of user a/d channels

%%%%%%%%%% SAMPLE PERIODS %%%%%%%%%%

controlPeriod = 1/1000;  % controller sample period (sec)
samplePeriod  = controlPeriod/HybridCtrlParameters.upFact;

%%%%%%%%%% SCRAMNET PARTITIONS %%%%%%%%%%

syncNode = 1;	% synchronization node: MTS SRMD
xpcNode	 = 2;	% xPC-Target node
opfNode  = 3;   % OpenFresco node


%%%%%%%%%% START MTS (STS) %%%%%%%%%%

%%%%%%%%%% outputs to scramnet %%%%%%%%%%

% displ and compression references
baseAddress = 0;
partition(1).Address = ['0x', dec2hex(baseAddress*4)];
partition(1).Type = 'single';
partition(1).Size = num2str(nRef);

% user inputs
partition(2).Type = 'single';
partition(2).Size = num2str(nInpU);

% blank space (up to 64)
partition(3).Type = 'uint32';
partition(3).Size = '49';

%%%%%%%%%% inputs from scramnet %%%%%%%%%%

% displ and compression references
partition(4).Type = 'single';
partition(4).Size = num2str(nRef);

% displ and compression feedbacks
partition(5).Type = 'single';
partition(5).Size = num2str(nRef);

% accel feedbacks
partition(6).Type = 'single';
partition(6).Size = num2str(nAcc);

% table force feedbacks
partition(7).Type = 'single';
partition(7).Size = num2str(nFrcTbl);

% normal force feedbacks
partition(8).Type = 'single';
partition(8).Size = num2str(nFrcNrm);

% user ADCs
partition(9).Type = 'single';
partition(9).Size = num2str(nAdcU);

% user inputs
partition(10).Type = 'single';
partition(10).Size = num2str(nInpU);
irqPartition = 10;

% blank space (up to 1024)
partition(11).Type = 'uint32';
partition(11).Size = '912';

%%%%%%%%%% END MTS (STS) %%%%%%%%%%


%%%%%%%%%% START OPENFRESCO %%%%%%%%%%

%%%%%%%%%% inputs from OpenFresco %%%%%%%%%%

% newTarget (from)
partition(12).Type = 'int32';
partition(12).Size = '1';

% disp commands (from)
partition(13).Type = 'single';
partition(13).Size = num2str(nRef);

% vel commands (from)
partition(14).Type = 'single';
partition(14).Size = num2str(nRef);

% accel commands (from)
partition(15).Type = 'single';
partition(15).Size = num2str(nRef);

% force commands (from)
partition(16).Type = 'single';
partition(16).Size = num2str(nRef);

% time commands (from)
partition(17).Type = 'single';
partition(17).Size = num2str(nRef);

%%%%%%%%%% outputs to OpenFresco %%%%%%%%%%

% switchPC (to)
partition(18).Type = 'int32';
partition(18).Size = '1';

% atTarget (to)
partition(19).Type = 'int32';
partition(19).Size = '1';

% disp feedbacks (to)
partition(20).Type = 'single';
partition(20).Size = num2str(nRef);

% vel feedbacks (to)
partition(21).Type = 'single';
partition(21).Size = num2str(nRef);

% accel feedbacks (to)
partition(22).Type = 'single';
partition(22).Size = num2str(nRef);

% force feedbacks (to)
partition(23).Type = 'single';
partition(23).Size = num2str(nRef);

% time feedbacks (to)
partition(24).Type = 'single';
partition(24).Size = num2str(nRef);

%%%%%%%%%% END OPENFRESCO %%%%%%%%%%


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
