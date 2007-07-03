% Script to run Hybrid Simulation interacting with Simulink
%
% Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
% Created: 11/04

close all;

outputFile = 'test01';

f = 3.0; % [Hz]
time = (0:100)'*0.01;
% target displacement vector
targDsp(:,1) = 3*sin(2*pi*f*time);
%targDsp(:,2) = 3*sin(2*pi*f*time);
%targDsp(:,3) = 3*sin(2*pi*f*time);
% target velocity vector
targVel(:,1) = 6*pi*f*cos(2*pi*f*time);
%targVel(:,2) = 6*pi*f*cos(2*pi*f*time);
%targVel(:,3) = 6*pi*f*cos(2*pi*f*time);
% target acceleration vector
targAcc(:,1) = -12*pi^2*f^2*sin(2*pi*f*time);
%targAcc(:,2) = -12*pi^2*f^2*sin(2*pi*f*time);
%targAcc:,3) = -12*pi^2*f^2*sin(2*pi*f*time);

% initialization
type = 'Dsp';
appName = 'HybridControllerPoly1';
HybridControlSimulink('init',type,appName,targDsp(1,:),targVel(1,:),targAcc(1,:));

% send target displacements and obtain resisting forces
fprintf('running......\n\n');
for i=1:length(targDsp)
   HybridControlSimulink('execute',type,appName,targDsp(i,:),targVel(i,:),targAcc(i,:));
   [measDsp(i,:),measFrc(i,:)] = HybridControlSimulink('acquire',type,appName);
end
fprintf('done\n\n');