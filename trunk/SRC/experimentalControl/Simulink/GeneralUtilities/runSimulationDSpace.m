%RUNSIMULATIONDSPACE script to run Hybrid Simulation interacting with dSpace board
%
% Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
% Created: 11/04

clear;
close all;
clc;

f = 3.0; % [Hz]
time = (0:500)'*0.01;
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
boardName = 'DS1104';
HybridControlDSpace('init',type,boardName,targDsp(1,:),targVel(1,:),targAcc(1,:));

% send target displacements and obtain resisting forces
for i=1:length(targDsp)
   HybridControlDSpace('execute',type,boardName,targDsp(i,:),targVel(i,:),targAcc(i,:));
   [measDsp(i,:),measFrc(i,:)] = HybridControlDSpace('acquire',type,boardName);
end

% stop the rtp application
HybridControlDSpace('stop');

% plot output
%plotDSpaceOutput(outputFile);