%PLOTOUTPUTSIMULINK to plot the Simulink output from a hybrid simulation
%
% Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
% Created: 04/05

% save workspace
save(outputFile);

% get logged signals
sigsOut.unpack('all');

figure;
plot(targDsp.Time,targDsp.Data,'-+r');
hold('on');
plot(commDsp.Time,commDsp.Data,'-+g');
plot(measDsp.Time,measDsp.Data,'-+b');
plot(state.Time,state.Data,'-c');
grid('on');
xlabel('Time [sec]');
ylabel('Displacement [in]');
title('Displacement-Time-Histories from xPC-Target');
legend('targDsp','commDsp','measDsp','state');

figure;
plot(state.Time,state.Data,'-c');
grid('on');
xlabel('Time [sec]');
ylabel('State [-]');
title('State-Time-History');

figure
plot(counter.Time,counter.Data,'-b');
grid('on');
xlabel('Time [sec]');
ylabel('Counter [-]');
title('Counter-Time-History');
