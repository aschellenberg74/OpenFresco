function data = plotOutputDSpace(fileName,actID,tStart)
%PLOTOUTPUTDSPACE to plot the dSpace output from a hybrid simulation
% data = plotOutputDSpace(fileName)
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
    id = 1000;
elseif (nargin<3)
    id = 1000;
else
    id = tStart*1000;
end

% load the file and extract data
load(fileName);
data = eval(fileName);

i = ~cellfun(@isempty,strfind({data.Y.Name},'targDsp'));
targDsp = data.Y(i).Data';
i = ~cellfun(@isempty,strfind({data.Y.Name},'commDsp'));
commDsp = data.Y(i).Data';
i = ~cellfun(@isempty,strfind({data.Y.Name},'measDsp'));
measDsp = data.Y(i).Data';
i = ~cellfun(@isempty,strfind({data.Y.Name},'measFrc'));
measFrc = data.Y(i).Data';

i = ~cellfun(@isempty,strfind({data.Y.Name},'state'));
state = data.Y(i).Data';
i = ~cellfun(@isempty,strfind({data.Y.Name},'counter'));
counter = data.Y(i).Data';
i = ~cellfun(@isempty,strfind({data.Y.Name},'newTarget'));
newTarget = data.Y(i).Data';
i = ~cellfun(@isempty,strfind({data.Y.Name},'switchPC'));
switchPC = data.Y(i).Data';
i = ~cellfun(@isempty,strfind({data.Y.Name},'atTarget'));
atTarget = data.Y(i).Data';

time = data.X.Data';
dt = data.Capture.SamplingPeriod;

% get screen size
SS = get(0,'screensize');

%==========================================================================
% command displacements
CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
try
    plot(time(id:end),commDsp(id:end,actID),'-');
end
grid('on');
xlabel('Time [sec]');
ylabel('Command Displacement [in.]');
title(sprintf('Command Displacement from dSpace: Actuator %02d',actID));
%==========================================================================
% target, command and measured displacements
CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
try
    plot(time(id:end),targDsp(id:end,actID),'-b');
end
hold('on');
try
    plot(time(id:end),commDsp(id:end,actID),'-r');
end
try
    plot(time(id:end),measDsp(id:end,actID),'-g');
end
% try
%    plot(time(id:end),state(id:end),'-k');
% end
grid('on');
xlabel('Time [sec]');
ylabel('Displacement [in.]');
title(sprintf('Displacements from dSpace: Actuator %02d',actID));
legend('target','command','measured');
%==========================================================================
% error between measured and target displacements
CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
try
    targID = find(diff(atTarget) > 0) + 1;
    tID = (targID >= id);
    targID = targID(tID);
    error = measDsp(targID,actID) - targDsp(targID,actID);
    plot(time(targID),targDsp(targID,actID),'-b');
    hold('on');
    plot(time(targID),measDsp(targID,actID),'-r');
    plot(time(targID),error,'-g');
end
grid('on');
xlabel('Time [sec]');
ylabel('Displacement [in.]');
title(sprintf('Error between Measured and Target Displacements from dSpace: Actuator %02d',actID));
legend('target','measured','error');
%==========================================================================
% fft of error between measured and target displacements
CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
try
    targID = find(diff(atTarget) > 0) + 1;
    tID = targID >= id; tID(1) = 0;
    targID = targID(tID);
    t = time(targID);
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
    targID = find(diff(atTarget) > 0) + 1;
    tID = targID >= id;
    targID = targID(tID);
    plot(targDsp(targID,actID),measDsp(targID,actID),'-b');
    hold('on');
end
grid('on');
xlabel('Target Displacement [in.]');
ylabel('Measured Displacement [in.]');
title(sprintf('Subspace Plot of Measured vs. Target Displacements from dSpace: Actuator %02d',actID));
%==========================================================================
% Mercan (2007) tracking indicator of measured vs. target displacements
CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
try
    targID = find(diff(atTarget) > 0) + 1;
    tID = targID >= id;
    targID = targID(tID);
    A  = cumsum(0.5*(measDsp(targID(2:end),actID)+measDsp(targID(1:end-1),actID)).*diff(targDsp(targID,actID)));
    TA = cumsum(0.5*(targDsp(targID(2:end),actID)+targDsp(targID(1:end-1),actID)).*diff(measDsp(targID,actID)));
    TI = 0.5*(A - TA);
    TI = [0;TI];
    t = time(targID);
    plot(t,TI,'-b');
    hold('on');
end
grid('on');
xlabel('Time [sec]');
ylabel('Tracking Indicator [in^2]');
title(sprintf('Tracking Indicator from dSpace: Actuator %02d',actID));
%==========================================================================
% fft of error between measured and commmand displacements
%CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
%try
%    error = measDsp(id:end,actID) - commDsp(id:end,actID);
%    dt = 1/1000;
%    getFFT(error,dt,sprintf('Error between Measured and Command Displacements: Actuator %02d',actID));
%end
%==========================================================================
% measured force
CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
try
    plot(time(id:end),measFrc(id:end,actID),'-');
end
grid('on');
xlabel('Time [sec]');
ylabel('Measured Force [kip]');
title(sprintf('Measured Force from dSpace: Actuator %02d',actID));
%==========================================================================
% fft of measured force
CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
try
    dt = 1/1000;
    getFFT(measFrc(id:end,actID),dt,sprintf('Measured Force: Actuator %02d',actID));
    set(gca,'YScale','log');
end
%==========================================================================
% state of predictor-corrector
CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
try
    plot(time(id:end),state(id:end),'-b');
end
grid('on');
xlabel('Time [sec]');
ylabel('State [-]');
title('State of Predictor-Corrector from dSpace');
%==========================================================================
% counter of predictor-corrector
CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
try
    plot(time(id:end),counter(id:end),'-b');
end
grid('on');
xlabel('Time [sec]');
ylabel('Counter [-]');
title('Counter of Predictor-Corrector from dSpace');
%==========================================================================
% flags of predictor-corrector
CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
try
    plot(time(id:end),newTarget(id:end),'-b');
end
hold('on');
try
    plot(time(id:end),switchPC(id:end),'--r');
end
try
    plot(time(id:end),atTarget(id:end),'-.g');
end
grid('on');
xlabel('Time [sec]');
ylabel('Flag [-]');
title('Flags of Predictor-Corrector from dSpace');
legend('newTarget','switchPC','atTarget');
%==========================================================================
