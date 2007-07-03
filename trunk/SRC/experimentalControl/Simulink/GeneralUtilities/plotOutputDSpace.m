function plotOutputDSpace(fileName)
%PLOTOUTPUTDSPACE to plot the dSpace output from a hybrid simulation
% plotOutputDSpace(fileName)
%
% fileName : .mat file to be loaded
%
% Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
% Created: 04/05

load(fileName);
data = eval(fileName);

targetFlag = data.Y(7).Data';
state      = data.Y(5).Data';
counter    = data.Y(2).Data';

targDisp = data.Y(6).Data';
commDisp = data.Y(1).Data';
measDisp = data.Y(3).Data';
measForc = data.Y(4).Data';

t  = data.X.Data';
dt = data.Capture.SamplingPeriod;

%id = find(diff(stateFlag)==-1);
%dtargetM = dtarget(id);
%tM = t(id);

%fig = Create_Window (1.0,1.0,'cen','Test Result Output');
figure;
subplot(2,1,1);
plot(t,targDisp,'b-');
hold('on');
plot(t,commDisp,'g-');
plot(t,measDisp,'r-');
grid('on');
xlabel ('Time [sec]')
ylabel ('Displacement [in]');
title ('Displacement-Time-Histories');
legend('targDisp','commDisp','measDisp');

subplot(2,1,2);
plot(t,targetFlag,'b-');