function data = plotOutputDSpace(fileName,dofID,tStart)
%PLOTOUTPUTDSPACE to plot the dSpace output from a hybrid simulation
% data = plotOutputDSpace(fileName,dofID,tStart)
%
% data     : data structure
% fileName : .mat file to be loaded
% dofID    : id of degree-of-freedom to plot (optional)
% tStart   : time to start plotting (optional)
%
% Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
% Created: 04/05

%#ok<*TRYNC>

% define constants
g = 32.174*12;

if (nargin<2)
    dofID = 1;
    id = 1000;
elseif (nargin<3)
    id = 1000;
else
    id = max(tStart*1000,1);
end

% load the file and extract data
load(fileName);
data = eval(fileName);

time = data.X.Data';
npts = length(time);
dt = data.Capture.SamplingPeriod*double(data.Capture.Downsampling);

i = ~cellfun(@isempty,strfind({data.Y.Name},'targSig'));
targSig = reshape([data.Y(i).Data],npts,[]);
i = ~cellfun(@isempty,strfind({data.Y.Name},'commSig'));
commSig = reshape([data.Y(i).Data],npts,[]);
i = ~cellfun(@isempty,strfind({data.Y.Name},'commSigDot'));
commSigDot = reshape([data.Y(i).Data],npts,[]);
i = ~cellfun(@isempty,strfind({data.Y.Name},'commSigDotDot'));
commSigDotDot = reshape([data.Y(i).Data],npts,[]);
i = ~cellfun(@isempty,strfind({data.Y.Name},'measSig'));
measSig = reshape([data.Y(i).Data],npts,[]);
i = ~cellfun(@isempty,strfind({data.Y.Name},'measDsp'));
measDsp = reshape([data.Y(i).Data],npts,[]);
i = ~cellfun(@isempty,strfind({data.Y.Name},'measFrc'));
measFrc = reshape([data.Y(i).Data],npts,[]);

i = ~cellfun(@isempty,strfind({data.Y.Name},'state'));
state = reshape([data.Y(i).Data],npts,[]);
i = ~cellfun(@isempty,strfind({data.Y.Name},'counter'));
counter = reshape([data.Y(i).Data],npts,[]);
i = ~cellfun(@isempty,strfind({data.Y.Name},'newTarget'));
newTarget = reshape([data.Y(i).Data],npts,[]);
i = ~cellfun(@isempty,strfind({data.Y.Name},'switchPC'));
switchPC = reshape([data.Y(i).Data],npts,[]);
i = ~cellfun(@isempty,strfind({data.Y.Name},'atTarget'));
atTarget = reshape([data.Y(i).Data],npts,[]);

% get screen size
SS = get(0,'screensize');

%==========================================================================
% command displacements
CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
try
    plot(time(id:end),commSig(id:end,dofID),'b-','LineWidth',1.0);
end
grid('on');
xlabel('Time [sec]','FontWeight','bold');
ylabel('Command Displacement [in.]','FontWeight','bold');
title(sprintf('Command Displacement from dSpace: DOF %02d',dofID),'FontWeight','bold');
%==========================================================================
% command velocities
if exist('commSigDot','var')
    CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
    try
        plot(time(id:end),commSigDot(id:end,dofID),'b-','LineWidth',1.0);
    end
    grid('on');
    xlabel('Time [sec]','FontWeight','bold');
    ylabel('Command Velocity [in./sec]','FontWeight','bold');
    title(sprintf('Command Velocity from dSpace: DOF %02d',dofID),'FontWeight','bold');
end
%==========================================================================
% command accelerations
if exist('commSigDotDot','var')
    CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
    try
        plot(time(id:end),(1/g)*commSigDotDot(id:end,dofID),'b-','LineWidth',1.0);
    end
    grid('on');
    xlabel('Time [sec]','FontWeight','bold');
    ylabel('Command Acceleration [g]','FontWeight','bold');
    title(sprintf('Command Acceleration from dSpace: DOF %02d',dofID),'FontWeight','bold');
end
%==========================================================================
% fft of command displacements
CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
try
    getFFT(commSig(id:end,dofID),dt,sprintf('Command Displacement: DOF %02d',dofID));
    set(gca,'YScale','log');
end
%==========================================================================
% target, command and measured displacements
CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
try
    plot(time(id:end),targSig(id:end,dofID),'-b','LineWidth',1.0);
    hold('on');
    targID = find(diff(atTarget) > 0) + 1;
    tID = (targID >= id);
    targID = targID(tID);
    plot(time(targID),targSig(targID,dofID),'.b','LineWidth',1.0);
end
try
    plot(time(id:end),commSig(id:end,dofID),'-r','LineWidth',1.0);
end
try
    plot(time(id:end),measSig(id:end,dofID),'-g','LineWidth',1.0);
end
%try
%   plot(time(id:end),state(id:end,1),'-k','LineWidth',1.0);
%end
grid('on');
xlabel('Time [sec]','FontWeight','bold');
ylabel('Displacement [in.]','FontWeight','bold');
title(sprintf('Displacements from dSpace: DOF %02d',dofID),'FontWeight','bold');
legend('target','target @ measured','command','measured');
%==========================================================================
% error between measured and target displacements
CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
try
    targID = find(diff(atTarget) > 0) + 1;
    tID = (targID >= id);
    targID = targID(tID);
    
    targSigAT = interp1(time(targID),targSig(targID,:),time);
    measDspAT = interp1(time(targID),measSig(targID,:),time);
    error = measDspAT(:,dofID) - targSigAT(:,dofID);
    timeShift = getTimeShift(time,targSigAT(:,dofID),time,measDspAT(:,dofID));
    plot(time(id:end),targSigAT(id:end,dofID),'-b','LineWidth',1.0);
    hold('on');
    plot(time(id:end),measDspAT(id:end,dofID),'-r','LineWidth',1.0);
    plot(time(id:end)+timeShift,measDspAT(id:end,dofID),'--m','LineWidth',1.0);
    plot(time(id:end),error(id:end),'-g','LineWidth',1.0);
end
grid('on');
xlabel('Time [sec]','FontWeight','bold');
ylabel('Displacement [in.]','FontWeight','bold');
title(sprintf('Error between Measured and Target Displacements from dSpace: DOF %02d',dofID),'FontWeight','bold');
legend('target','measured',sprintf('measured (shifted by %1.0f msec)',1000*timeShift),'error');
%==========================================================================
% fft of error between measured and target displacements
CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
try
    targID = find(diff(atTarget) > 0) + 1;
    tID = targID >= id; tID(1) = 0;
    targID = targID(tID);
    t = time(targID);
    error = measSig(targID,dofID) - targSig(targID,dofID);
    tIP = linspace(t(1),t(end),length(t))';
    errorIP = interp1(t,error,tIP,'linear');
    dtIP = mean(diff(tIP));
    getFFT(errorIP,dtIP,sprintf('Error between Measured and Target Displacements: DOF %02d',dofID));
end
%==========================================================================
% subspace plot of measured vs. target displacements
CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
try
    targID = find(diff(atTarget) > 0) + 1;
    tID = targID >= id;
    targID = targID(tID);
    plot(targSig(targID,dofID),measSig(targID,dofID),'-b','LineWidth',1.0);
    hold('on');
end
grid('on');
xlabel('Target Displacement [in.]','FontWeight','bold');
ylabel('Measured Displacement [in.]','FontWeight','bold');
title(sprintf('Subspace Plot of Measured vs. Target Displacements from dSpace: DOF %02d',dofID),'FontWeight','bold');
%==========================================================================
% Mercan (2007) tracking indicator of measured vs. target displacements
CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
try
    targID = find(diff(atTarget) > 0) + 1;
    tID = targID >= id;
    targID = targID(tID);
    A  = cumsum(0.5*(measSig(targID(2:end),dofID)+measSig(targID(1:end-1),dofID)).*diff(targSig(targID,dofID)));
    TA = cumsum(0.5*(targSig(targID(2:end),dofID)+targSig(targID(1:end-1),dofID)).*diff(measSig(targID,dofID)));
    TI = 0.5*(A - TA);
    TI = [0;TI];
    plot(time(targID),TI,'-b','LineWidth',1.0);
    hold('on');
end
grid('on');
xlabel('Time [sec]','FontWeight','bold');
ylabel('Tracking Indicator [in^2]','FontWeight','bold');
title(sprintf('Tracking Indicator from dSpace: DOF %02d',dofID),'FontWeight','bold');
%==========================================================================
% fft of error between measured and commmand displacements
%CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
%try
%    error = measSig(id:end,dofID) - commSig(id:end,dofID);
%    getFFT(error,dt,sprintf('Error between Measured and Command Displacements: DOF %02d',dofID));
%end
%==========================================================================
% measured force
CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
try
    plot(time(id:end),measFrc(id:end,dofID),'-b','LineWidth',1.0);
end
grid('on');
xlabel('Time [sec]','FontWeight','bold');
ylabel('Measured Force [kip]','FontWeight','bold');
title(sprintf('Measured Force from dSpace: DOF %02d',dofID),'FontWeight','bold');
%==========================================================================
% fft of measured force
CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
try
    getFFT(measFrc(id:end,dofID),dt,sprintf('Measured Force: DOF %02d',dofID));
    set(gca,'YScale','log');
end
%==========================================================================
% state of predictor-corrector
CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
try
    plot(time(id:end),state(id:end),'-b','LineWidth',1.0);
end
grid('on');
xlabel('Time [sec]','FontWeight','bold');
ylabel('State [-]','FontWeight','bold');
title('State of Predictor-Corrector from dSpace','FontWeight','bold');
%==========================================================================
% counter of predictor-corrector
CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
try
    plot(time(id:end),counter(id:end),'-b','LineWidth',1.0);
end
grid('on');
xlabel('Time [sec]','FontWeight','bold');
ylabel('Counter [-]','FontWeight','bold');
title('Counter of Predictor-Corrector from dSpace','FontWeight','bold');
%==========================================================================
% flags of predictor-corrector
CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
try
    plot(time(id:end),newTarget(id:end),'-b','LineWidth',1.0);
end
hold('on');
try
    plot(time(id:end),switchPC(id:end),'--r','LineWidth',1.0);
end
try
    plot(time(id:end),atTarget(id:end),'-.g','LineWidth',1.0);
end
grid('on');
xlabel('Time [sec]','FontWeight','bold');
ylabel('Flag [-]','FontWeight','bold');
title('Flags of Predictor-Corrector from dSpace','FontWeight','bold');
legend('newTarget','switchPC','atTarget');
%==========================================================================
