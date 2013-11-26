function data = plotOutputXPCtarget(fileName,actID,tStart)
%PLOTOUTPUTXPCTARGET to plot the xPC-Target output from a hybrid simulation
% data = plotOutputXPCtarget(fileName,actID,iDelay,tStart)
%
% data     : data structure
% fileName : .mat file to be loaded
% actID    : id of actuator to plot (optional)
% tStart   : time to start plotting (optional)
%
% Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
% Created: 04/05

%#ok<*TRYNC>

if (nargin<2)
    actID = 1;
    id = 1024;
elseif (nargin<3)
    id = 1024;
else
    id = tStart*1024;
end

% load the file and extract data
data = [];
load(fileName);
targDsp = data(1).values;
commDsp = data(2).values;
measDsp = data(3).values;
state   = data(4).values;
counter = data(5).values;
flag    = data(6).values;
measFrc = data(7).values;

% get screen size
SS = get(0,'screensize');

%==========================================================================
% command displacements
CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
try
    plot(commDsp(id:end,end),commDsp(id:end,actID),'-');
end
grid('on');
xlabel('Time [sec]');
ylabel('Command Displacement [in.]');
title(sprintf('Command Displacement from xPC-Target: Actuator %02d',actID));
%==========================================================================
% target, command and measured displacements
CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
try
    plot(targDsp(id:end,end),targDsp(id:end,actID),'-b');
end
hold('on');
try
    plot(commDsp(id:end,end),commDsp(id:end,actID),'-r');
end
try
    plot(measDsp(id:end,end),measDsp(id:end,actID),'-g');
end
%try
%   plot(state(id:end,end),state(id:end,1),'-k');
%end
grid('on');
xlabel('Time [sec]');
ylabel('Displacement [in.]');
title(sprintf('Displacements from xPC-Target: Actuator %02d',actID));
legend('target','command','measured');
%==========================================================================
% error between measured and target displacements
CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
try
    targID = find(diff(flag(:,3)) > 0) + 1;
    tID = (targID >= id);
    targID = targID(tID);
    error = measDsp(targID,actID) - targDsp(targID,actID);
    plot(measDsp(targID,end),targDsp(targID,actID),'-b');
    hold('on');
    plot(measDsp(targID,end),measDsp(targID,actID),'-r');
    plot(measDsp(targID,end),error,'-g');
end
grid('on');
xlabel('Time [sec]');
ylabel('Displacement [in.]');
title(sprintf('Error between Measured and Target Displacements from xPC-Target: Actuator %02d',actID));
legend('target','measured','error');
%==========================================================================
% fft of error between measured and target displacements
CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
try
    targID = find(diff(flag(:,3)) > 0) + 1;
    tID = targID >= id; tID(1) = 0;
    targID = targID(tID);
    t = measDsp(targID,end);
    error = measDsp(targID,actID) - targDsp(targID,actID);
    tIP = linspace(t(1),t(end),length(t))';
    errorIP = interp1(t,error,tIP,'linear');
    dt = tIP(2) - tIP(1);
    getFFT(errorIP,dt,sprintf('Error between Measured and Target Displacements: Actuator %02d',actID));
end
%==========================================================================
% subspace plot of measured vs. target displacements
CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
try
    targID = find(diff(flag(:,3)) > 0) + 1;
    tID = targID >= id;
    targID = targID(tID);
    plot(targDsp(targID,actID),measDsp(targID,actID),'-b');
    hold('on');
end
grid('on');
xlabel('Target Displacement [in.]');
ylabel('Measured Displacement [in.]');
title(sprintf('Subspace Plot of Measured vs. Target Displacements from xPC-Target: Actuator %02d',actID));
%==========================================================================
% Mercan (2007) tracking indicator of measured vs. target displacements
CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
try
    targID = find(diff(flag(:,3)) > 0) + 1;
    tID = targID >= id;
    targID = targID(tID);
    A  = cumsum(0.5*(measDsp(targID(2:end),actID)+measDsp(targID(1:end-1),actID)).*diff(targDsp(targID,actID)));
    TA = cumsum(0.5*(targDsp(targID(2:end),actID)+targDsp(targID(1:end-1),actID)).*diff(measDsp(targID,actID)));
    TI = 0.5*(A - TA);
    TI = [0;TI];
    t = measDsp(targID,end);
    plot(t,TI,'-b');
    hold('on');
end
grid('on');
xlabel('Time [sec]');
ylabel('Tracking Indicator [in^2]');
title(sprintf('Tracking Indicator from xPC-Target: Actuator %02d',actID));
%==========================================================================
% fft of error between measured and commmand displacements
%CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
%try
%   error = measDsp(id:end,actID) - commDsp(id:end,actID);
%   dt = 1/1024;
%   getFFT(error,dt,sprintf('Error between Measured and Command Displacements: Actuator %02d',actID));
%end
%==========================================================================
% measured force
CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
try
    plot(measFrc(id:end,end),measFrc(id:end,actID),'-');
end
grid('on');
xlabel('Time [sec]');
ylabel('Measured Force [kip]');
title(sprintf('Measured Force from xPC-Target: Actuator %02d',actID));
%==========================================================================
% fft of measured force
CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
try
    dt = 1/1024;
    getFFT(measFrc(id:end,actID),dt,sprintf('Measured Force: Actuator %02d',actID));
    set(gca,'YScale','log');
end
%==========================================================================
% state of predictor-corrector
CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
try
    plot(state(id:end,end),state(id:end,1),'-b');
end
grid('on');
xlabel('Time [sec]');
ylabel('State [-]');
title('State of Predictor-Corrector from xPC-Target');
%==========================================================================
% counter of predictor-corrector
CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
try
    plot(counter(id:end,end),counter(id:end,1),'-b');
end
grid('on');
xlabel('Time [sec]');
ylabel('Counter [-]');
title('Counter of Predictor-Corrector from xPC-Target');
%==========================================================================
% flags of predictor-corrector
CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
try
    plot(flag(id:end,end),flag(id:end,1),'-b');
end
hold('on');
try
    plot(flag(id:end,end),flag(id:end,2),'--r');
end
try
    plot(flag(id:end,end),flag(id:end,3),'-.g');
end
grid('on');
xlabel('Time [sec]');
ylabel('Flag [-]');
title('Flags of Predictor-Corrector from xPC-Target');
legend('newTarget','switchPC','atTarget');
%==========================================================================
