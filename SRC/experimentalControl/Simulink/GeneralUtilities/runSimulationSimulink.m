% RUNSIMULATIONSIMULINK script to run Hybrid Simulation interacting with Simulink
%
% Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
% Created: 11/04
%
% $Revision$
% $Date$
% $URL$

close all;

outputFile = 'test01';

f = 3.0; % [Hz]
time = (0:100)'*0.01;
% control displacement vector
ctrlDsp(:,1) = 3*sin(2*pi*f*time);
%ctrlDsp(:,2) = 3*sin(2*pi*f*time);
%ctrlDsp(:,3) = 3*sin(2*pi*f*time);
% control velocity vector
ctrlVel(:,1) = 6*pi*f*cos(2*pi*f*time);
%ctrlVel(:,2) = 6*pi*f*cos(2*pi*f*time);
%ctrlVel(:,3) = 6*pi*f*cos(2*pi*f*time);
% control acceleration vector
ctrlAcc(:,1) = -12*pi^2*f^2*sin(2*pi*f*time);
%ctrlAcc(:,2) = -12*pi^2*f^2*sin(2*pi*f*time);
%ctrlAcc:,3) = -12*pi^2*f^2*sin(2*pi*f*time);

% initialization
type = 'Dsp';
appName = 'HybridControllerP3P3';
HybridControlSimulink('init',type,appName,ctrlDsp(1,:),ctrlVel(1,:),ctrlAcc(1,:));

% send control displacements and obtain resisting forces
fprintf('running......\n\n');
for i=1:length(ctrlDsp)
    HybridControlSimulink('control',type,appName,ctrlDsp(i,:),ctrlVel(i,:),ctrlAcc(i,:));
    [measDsp(i,:),measFrc(i,:)] = HybridControlSimulink('acquire',type,appName);
end
fprintf('done\n\n');
