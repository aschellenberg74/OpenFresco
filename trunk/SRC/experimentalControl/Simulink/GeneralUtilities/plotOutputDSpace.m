function data = plotOutputDSpace(fileName,dofID,tStart,iDelay)
%PLOTOUTPUTDSPACE to plot the dSpace output from a hybrid simulation
% data = plotOutputDSpace(fileName,dofID,tStart,iDelay)
%
% data     : data structure
% fileName : .mat file to be loaded
% dofID    : id of degree-of-freedom to plot (optional)
% tStart   : time to start plotting (optional)
% iDelay   : delay compensation in number of samples (optional)
%
% Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
% Created: 04/05
%
% $Revision$
% $Date$
% $URL$

%#ok<*TRYNC>

if (nargin<2)
    dofID = 1;
    id = 1000;
    iDelay(dofID) = 0;
elseif (nargin<3)
    id = 1000;
    iDelay(dofID) = 0;
elseif (nargin<4)
    id = max(tStart*1000,1);
    iDelay(dofID) = 0;
else
    id = max(tStart*1000,1);
end

% load the file and extract data
load(fileName);
data = eval(fileName);

time = data.X.Data';
npts = length(time);
dt = data.Capture.SamplingPeriod*double(data.Capture.Downsampling);

i = ~cellfun(@isempty,strfind({data.Y.Name},'targDsp'));
targDsp = reshape([data.Y(i).Data],npts,[]);
i = ~cellfun(@isempty,strfind({data.Y.Name},'commDsp'));
commDsp = reshape([data.Y(i).Data],npts,[]);
i = ~cellfun(@isempty,strfind({data.Y.Name},'commVel'));
commVel = reshape([data.Y(i).Data],npts,[]);
i = ~cellfun(@isempty,strfind({data.Y.Name},'commAcc'));
commAcc = reshape([data.Y(i).Data],npts,[]);
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
subplot(3,1,1);
try
    plot(time(id:end),commDsp(id:end,dofID),'-');
end
grid('on');
xlabel('Time [sec]');
ylabel('Command Displacement [in.]');
title(sprintf('Command Displacement from dSpace: DOF %02d',dofID));

subplot(3,1,2);
try
    plot(time(id:end),commVel(id:end,dofID),'b-');
    hold on;
    %plot(time(id:end),commVelF(id:end,dofID),'r--');
    %commVel2 = [zeros(1,size(commDsp,2)); 1/dt*diff(commDsp,[],1)];
    %plot(time(id:end),commVel2(id:end,dofID),'r--');
end
grid('on');
xlabel('Time [sec]');
ylabel('Command Velocity [in./sec]');

subplot(3,1,3);
try
    plot(time(id:end),commAcc(id:end,dofID),'b-');
    hold on;
    %commAcc2 = [zeros(1,size(commVel2,2)); 1/dt*diff(commVel2,[],1)];
    %plot(time(id:end),commAcc2(id:end,dofID),'r--');
end
grid('on');
xlabel('Time [sec]');
ylabel('Command Acceleration [in./sec^2]');
%==========================================================================
% target, command and measured displacements
CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
try
   plot(time(id:end),max(commDsp(id:end,dofID))*(counter(id:end)==max(iDelay(dofID),1)),':y');
   hold('on');
   plot(time(id:end),min(commDsp(id:end,dofID))*(counter(id:end)==max(iDelay(dofID),1)),':y');
end
hold('on');
% try
%    plot(time(id:end),state(id:end),'-y');
% end
try
    ph(1) = plot(time(id:end),targDsp(id:end,dofID),'-b','LineWidth',1.0);
end
try
    ph(2) = plot(time(id:end),commDsp(id:end,dofID),'-r','LineWidth',1.0);
end
try
    ph(3) = plot(time(id:end),measDsp(id:end,dofID),'-g','LineWidth',1.0);
end
grid('on');
xlabel('Time [sec]');
ylabel('Displacement [in.]');
title(sprintf('Displacements from dSpace: DOF %02d',dofID));
legend(ph,'target','command','measured');
%==========================================================================
% error between measured and target displacements
figure;
%CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
try
    targID = find(counter == max(iDelay(dofID),1));
    tID = (targID >= id);
    targID = targID(tID);
    timeInterp = (0:0.001:time(end))';
    targDspAT = interp1(time(targID),targDsp(targID,:),timeInterp);
    measDspAT = interp1(time(targID),measDsp(targID,:),timeInterp); 
    error = measDspAT(:,dofID) - targDspAT(:,dofID);
    timeShift = getTimeShift(timeInterp,targDspAT(:,dofID),timeInterp,measDspAT(:,dofID));
    plot(timeInterp(id:end),targDspAT(id:end,dofID),'-b','LineWidth',1.5);
    hold('on');
    plot(timeInterp(id:end),measDspAT(id:end,dofID),'-r','LineWidth',1.5);
    plot(timeInterp(id:end)+timeShift,measDspAT(id:end,dofID),'--m','LineWidth',1.5);
    plot(timeInterp(id:end),error(id:end),'-g','LineWidth',1.5);
end
grid('on');
xlabel('Time [sec]','FontWeight','bold');
ylabel('Displacement [in.]','FontWeight','bold');
title(sprintf('Error between Measured and Target Displacements from dSpace: DOF %02d',dofID),'FontWeight','bold');
legend('target','measured',sprintf('measured (shifted by %1.0f msec)',1000*timeShift),'error');
%==========================================================================
% fft of error between measured and target displacements
figure;
%CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
try
    targID = find(counter == max(iDelay(dofID),1));
    tID = targID >= id; tID(1) = 0;
    targID = targID(tID);
    t = time(targID);
    error = measDsp(targID,dofID) - targDsp(targID,dofID);
    tIP = linspace(t(1),t(end),length(t))';
    errorIP = interp1(t,error,tIP,'linear');
    dt = tIP(2) - tIP(1);
    getFFT(errorIP,dt,sprintf('Error between Measured and Target Displacements: DOF %02d',dofID));
end
%==========================================================================
% subspace plot of measured vs. target displacements
figure;
%CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
try
    targID = find(counter == max(iDelay(dofID),1));
    tID = targID >= id;
    targID = targID(tID);
    plot(targDsp(targID,dofID),measDsp(targID,dofID),'-b','LineWidth',1.5);
    hold('on');
end
grid('on');
xlabel('Target Displacement [in.]','FontWeight','bold');
ylabel('Measured Displacement [in.]','FontWeight','bold');
title(sprintf('Subspace Plot of Measured vs. Target Displacements from dSpace: DOF %02d',dofID),'FontWeight','bold');
%==========================================================================
% Mercan (2007) tracking indicator of measured vs. target displacements
figure;
%CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
try
    targID = find(counter == max(iDelay(dofID),1));
    tID = targID >= id;
    targID = targID(tID);
    A  = cumsum(0.5*(measDsp(targID(2:end),dofID)+measDsp(targID(1:end-1),dofID)).*diff(targDsp(targID,dofID)));
    TA = cumsum(0.5*(targDsp(targID(2:end),dofID)+targDsp(targID(1:end-1),dofID)).*diff(measDsp(targID,dofID)));
    TI = 0.5*(A - TA);
    TI = [0;TI];
    t = time(targID);
    plot(t,TI,'-b','LineWidth',1.5);
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
%    error = measDsp(id:end,dofID) - commDsp(id:end,dofID);
%    dt = 1/1000;
%    getFFT(error,dt,sprintf('Error between Measured and Command Displacements: DOF %02d',dofID));
%end
%==========================================================================
% measured force
CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
try
    plot(time(id:end),measFrc(id:end,dofID),'-b','LineWidth',1.0);
end
grid('on');
xlabel('Time [sec]');
ylabel('Measured Force [kip]');
title(sprintf('Measured Force from dSpace: DOF %02d',dofID));
%==========================================================================
% fft of measured force
CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
try
    dt = 1/1000;
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
xlabel('Time [sec]');
ylabel('State [-]');
title('State of Predictor-Corrector from dSpace');
%==========================================================================
% counter of predictor-corrector
CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
try
    plot(time(id:end),counter(id:end),'-b','LineWidth',1.0);
end
grid('on');
xlabel('Time [sec]');
ylabel('Counter [-]');
title('Counter of Predictor-Corrector from dSpace');
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
xlabel('Time [sec]');
ylabel('Flag [-]');
title('Flags of Predictor-Corrector from dSpace');
legend('newTarget','switchPC','atTarget');
%==========================================================================
