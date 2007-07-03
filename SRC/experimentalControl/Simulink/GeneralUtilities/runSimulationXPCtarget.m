function runSimulationXPCtarget(appName,outputFile)
%RUNSIMULATIONXPCTARGET to run Hybrid Simulation interacting with xPC-Target
% runSimulationXPCtarget(appName,outputFile)
%
% appName    : name of application to run
% outputFile : output file the data is saved to
%
% Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
% Created: 11/04

close all;
clc;

% dt = 8.164e-003;
% DIR = 'D:\OpenSeesNavigator2.0\Examples\ZipperExp\OutputFiles\LA22YY\';
% % target displacement vector
% inputFile = 'LA22YY_Node_DispNodeRecorder_Dsp.out';
% Data = load([DIR inputFile]);
% time = Data(:,1);
% targDsp(:,1) = Data(:,29);
% targDsp(:,2) = Data(:,30);
% targDsp(:,3) = Data(:,30);
% % target velocity vector
% inputFile = 'LA22YY_Node_VelNodeRecorder_Vel.out';
% Data = load([DIR inputFile]);
% targVel(:,1) = Data(:,29);
% targVel(:,2) = Data(:,30);
% targVel(:,3) = Data(:,30);
% % target acceleration vector
% inputFile = 'LA22YY_Node_AccNodeRecorder_Acc.out';
% Data = load([DIR inputFile]);
% targAcc(:,1) = Data(:,29);
% targAcc(:,2) = Data(:,30);
% targAcc(:,3) = Data(:,30);

tStart  = 0.4;
tFinish = 2.5;
time = (0:300)'*0.01;
T = 0.2;
amp1 = 1.0; amp2 = 1.0; amp3 = 1.0;
% target displacement vector
targDsp(:,1) = SineSeries(time,tStart,tFinish,T,0,amp1);
targDsp(:,2) = SineSeries(time,tStart,tFinish,T,0,amp2);
targDsp(:,3) = SineSeries(time,tStart,tFinish,T,0,amp3);
% target velocity vector
targVel(:,1) = SineSeries(time,tStart,tFinish,T,T/4,2*pi/T*amp1);
targVel(:,2) = SineSeries(time,tStart,tFinish,T,T/4,2*pi/T*amp2);
targVel(:,3) = SineSeries(time,tStart,tFinish,T,T/4,2*pi/T*amp3);
% target acceleration vector
targAcc(:,1) = SineSeries(time,tStart,tFinish,T,0,4*pi^2/T^2*amp1);
targAcc(:,2) = SineSeries(time,tStart,tFinish,T,0,4*pi^2/T^2*amp2);
targAcc(:,3) = SineSeries(time,tStart,tFinish,T,0,4*pi^2/T^2*amp3);

% initialization
if ismember(appName,{'HybridControllerPoly1','HybridControllerPoly2','HybridControllerPoly3',...
      'HybridControllerD1D1','HybridControllerD2D1','HybridControllerD3D1'})
   type = 'Dsp';
elseif ismember(appName,'HybridControllerDV')
   type = 'DspVel';
elseif ismember(appName,'HybridControllerDVA')
   type = 'DspVelAcc';
end
%appName = 'HybridControllerPoly1';
HybridControlxPCtarget('init',3,6,type,appName,targDsp(1,:),targVel(1,:),targAcc(1,:));

% send target displacements and obtain resisting forces
for i=1:length(targDsp)
   HybridControlxPCtarget('execute',3,6,type,appName,targDsp(i,:),targVel(i,:),targAcc(i,:));
   [measDsp(i,:),measFrc(i,:)] = HybridControlxPCtarget('acquire',3,6,type,appName);
end

% stop xPCtarget model
HybridControlxPCtarget('stop');

% get the variables saved on the xPC Target
data = getXPCtargetVar({'state.dat','dsp.dat','count.dat'});

% save workspace
save(outputFile);

% plot output
plotOutputXPCtarget(outputFile);