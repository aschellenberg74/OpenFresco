function data = plotOutputXPCtarget(fileName,dofID,tStart)
%PLOTOUTPUTXPCTARGET to plot the xPC-Target output from a hybrid simulation
% data = plotOutputXPCtarget(fileName,dofID,tStart)
%
% data     : data structure
% fileName : .mat file to be loaded
% dofID    : id of degree-of-freedom to plot (optional)
% tStart   : time to start plotting (optional)
%
% Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
% Created: 04/05
%
% $Revision$
% $Date$
% $URL$

%#ok<*TRYNC>

% define constants
g = 32.174*12;

if (nargin<2)
    dofID = 1;
    id = 1024;
elseif (nargin<3)
    id = 1024;
else
    id = max(tStart*1024,1);
end

% load the file and extract data
data = [];
load(fileName);
targSig = data(1).values;
numDOF  = size(targSig,2)-1;
commSig = data(2).values(:,[1:numDOF,end]);
measSig = data(3).values(:,[1:numDOF,end]);
state   = data(4).values;
counter = data(5).values;
flags   = data(6).values;
measDsp = data(7).values;
measFrc = data(8).values;

if (size(data(2).values,2)-1 >= 2*numDOF)
    commSigDot = data(2).values(:,[numDOF+1:2*numDOF,end]);
end
if (size(data(2).values,2)-1 >= 3*numDOF)
    commSigDotDot = data(2).values(:,[2*numDOF+1:3*numDOF,end]);
end

time = targSig(:,end);
npts = length(time);
dt = mean(diff(time));

measVel = (1/dt)*[zeros(1,size(measDsp,2));diff(measDsp)];
measAcc = (1/dt)*[zeros(1,size(measVel,2));diff(measVel)];

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
title(sprintf('Command Displacement from xPC-Target: DOF %02d',dofID),'FontWeight','bold');
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
    title(sprintf('Command Velocity from xPC-Target: DOF %02d',dofID),'FontWeight','bold');
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
    title(sprintf('Command Acceleration from xPC-Target: DOF %02d',dofID),'FontWeight','bold');
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
    targID = find(diff(flags(:,3)) > 0) + 1;
    tID = (targID >= id);
    targID = targID(tID);
    plot(time(targID),targSig(targID,dofID),'ob','MarkerSize',3);
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
title(sprintf('Displacements from xPC-Target: DOF %02d',dofID),'FontWeight','bold');
legend('target','target @ measured','command','measured');
%==========================================================================
% error between measured and target displacements
CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
timeShift = 0;
try
    targID = find(diff(flags(:,3)) > 0) + 1;
    tID = (targID >= id);
    targID = targID(tID);
    targDspAT = interp1(time(targID),targSig(targID,dofID),time,[],0.0);
    measDspAT = interp1(time(targID),measSig(targID,dofID),time,[],0.0);
    error = measDspAT - targDspAT;
    timeShift = getTimeShift(time,targDspAT,time,measDspAT);
    plot(time(id:end),targDspAT(id:end),'-b','LineWidth',1.0);
    hold('on');
    plot(time(id:end),measDspAT(id:end),'-r','LineWidth',1.0);
    plot(time(id:end)+timeShift,measDspAT(id:end),'--m','LineWidth',1.0);
    plot(time(id:end),error(id:end),'-g','LineWidth',1.0);
end
grid('on');
xlabel('Time [sec]','FontWeight','bold');
ylabel('Displacement [in.]','FontWeight','bold');
title(sprintf('Error between Measured and Target Displacements from xPC-Target: DOF %02d',dofID),'FontWeight','bold');
legend('target','measured',sprintf('measured (shifted by %1.0f msec)',1000*timeShift),'error');
%==========================================================================
% error between measured and command displacements
CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
timeShift = 0;
try
    commDspAT = commSig(:,dofID);
    measDspAT = measDsp(:,dofID);
    error = measDspAT - commDspAT;
    timeShift = getTimeShift(time,commDspAT,time,measDspAT);
    plot(time(id:end),commDspAT(id:end),'-b','LineWidth',1.0);
    hold('on');
    plot(time(id:end),measDspAT(id:end),'-r','LineWidth',1.0);
    plot(time(id:end)+timeShift,measDspAT(id:end),'--m','LineWidth',1.0);
    plot(time(id:end),error(id:end),'-g','LineWidth',1.0);
end
grid('on');
xlabel('Time [sec]','FontWeight','bold');
ylabel('Displacement [in.]','FontWeight','bold');
title(sprintf('Error between Measured and Command Displacements from xPC-Target: DOF %02d',dofID),'FontWeight','bold');
legend('command','measured',sprintf('measured (shifted by %1.0f msec)',1000*timeShift),'error');
%==========================================================================
% error between measured and command accelerations
CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
timeShift = 0;
try
    commAccAT = (1/g)*commSigDotDot(:,dofID);
    measAccAT = (1/g)*measAcc(:,dofID);
    error = measAccAT - commAccAT;
    timeShift = getTimeShift(time,commAccAT,time,measAccAT);
    plot(time(id:end),commAccAT(id:end),'-b','LineWidth',1.0);
    hold('on');
    plot(time(id:end),measAccAT(id:end),'-r','LineWidth',1.0);
    plot(time(id:end)+timeShift,measAccAT(id:end),'--m','LineWidth',1.0);
    plot(time(id:end),error(id:end),'-g','LineWidth',1.0);
end
grid('on');
xlabel('Time [sec]','FontWeight','bold');
ylabel('Acceleration [g]','FontWeight','bold');
title(sprintf('Error between Measured and Command Accelerations from xPC-Target: DOF %02d',dofID),'FontWeight','bold');
legend('command','measured',sprintf('measured (shifted by %1.0f msec)',1000*timeShift),'error');
%==========================================================================
% transfer function between measured and command displacements
CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
try
    commDspAT = commSig(:,dofID);
    measDspAT = measDsp(:,dofID);
    
    winSize = 2.5;  % window size in [sec]
    nfft = ceil(winSize/dt);  % window frame size in points
    %nfft = 2^nextpow2(winSize/dt);  % window frame size in points
    %nfft = ceil(length(commDspAT)/10);  % window frame size in points
    %nfft = 2^nextpow2(length(commDspAT)/10);  % window frame size in points
    f = logspace(log10(0.2),log10(50),256)';  % use 256 log-spaced frequency values
    Tc2m = tfestimate(commDspAT, measDspAT, hann(nfft), [], f, 1/dt);
    Cc2m = mscohere(commDspAT, measDspAT, hann(nfft), [], f, 1/dt);
    
    fMin = min(f);
    fMax = 33;
    
    subplot(3,1,1);
    plot(f,mag2db(abs(Tc2m)),'-b','LineWidth',1.0);
    %set(gca,'XLim',[fMin fMax]);
    set(gca,'XScale','log','XLim',[fMin fMax]);
    grid('on');
    %xlabel('Frequency [Hz]','FontWeight','bold');
    ylabel('Magnitude [dB]','FontWeight','bold');
    title(sprintf('Transfer Function between Measured and Command Displacements from xPC-Target: DOF %02d',dofID),'FontWeight','bold');
    
    subplot(3,1,2);
    plot(f,rad2deg(angle(Tc2m)),'-b','LineWidth',1.0);
    %set(gca,'XLim',[fMin fMax],'YLim',[-180,180]);
    set(gca,'XScale','log','XLim',[fMin fMax],'YLim',[-180,180]);
    set(gca,'YTick',[-180 -135 -90 -45 0 45 90 135 180]);
    grid('on');
    %xlabel('Frequency [Hz]','FontWeight','bold');
    ylabel('Phase [deg]','FontWeight','bold');

    subplot(3,1,3);
    plot(f,Cc2m,'-b','LineWidth',1.0);
    %set(gca,'XLim',[fMin fMax],'YLim',[0,1]);
    set(gca,'XScale','log','XLim',[fMin fMax],'YLim',[0,1]);
    grid('on');
    xlabel('Frequency [Hz]','FontWeight','bold');
    ylabel('Coherence [-]','FontWeight','bold');
end
%==========================================================================
% transfer function between measured and command accelerations
CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
try
    commAccAT = (1/g)*commSigDotDot(:,dofID);
    measAccAT = (1/g)*measAcc(:,dofID);
    
    winSize = 2.5;  % window size in [sec]
    nfft = ceil(winSize/dt);  % window frame size in points
    %nfft = 2^nextpow2(winSize/dt);  % window frame size in points
    %nfft = ceil(length(commDspAT)/10);  % window frame size in points
    %nfft = 2^nextpow2(length(commDspAT)/10);  % window frame size in points
    f = logspace(log10(0.2),log10(50),256)';  % use 256 log-spaced frequency values
    Tc2m = tfestimate(commAccAT, measAccAT, hann(nfft), [], f, 1/dt);
    Cc2m = mscohere(commAccAT, measAccAT, hann(nfft), [], f, 1/dt);
    
    fMin = min(f);
    fMax = 33;
    
    subplot(3,1,1);
    plot(f,mag2db(abs(Tc2m)),'-b','LineWidth',1.0);
    %set(gca,'XLim',[fMin fMax]);
    set(gca,'XScale','log','XLim',[fMin fMax]);
    grid('on');
    %xlabel('Frequency [Hz]','FontWeight','bold');
    ylabel('Magnitude [dB]','FontWeight','bold');
    title(sprintf('Transfer Function between Measured and Command Accelerations from xPC-Target: DOF %02d',dofID),'FontWeight','bold');
    
    subplot(3,1,2);
    plot(f,rad2deg(angle(Tc2m)),'-b','LineWidth',1.0);
    %set(gca,'XLim',[fMin fMax],'YLim',[-180,180]);
    set(gca,'XScale','log','XLim',[fMin fMax],'YLim',[-180,180]);
    set(gca,'YTick',[-180 -135 -90 -45 0 45 90 135 180]);
    grid('on');
    %xlabel('Frequency [Hz]','FontWeight','bold');
    ylabel('Phase [deg]','FontWeight','bold');

    subplot(3,1,3);
    plot(f,Cc2m,'-b','LineWidth',1.0);
    %set(gca,'XLim',[fMin fMax],'YLim',[0,1]);
    set(gca,'XScale','log','XLim',[fMin fMax],'YLim',[0,1]);
    grid('on');
    xlabel('Frequency [Hz]','FontWeight','bold');
    ylabel('Coherence [-]','FontWeight','bold');
end
%==========================================================================
% fft of error between measured and target displacements
CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
try
    targID = find(diff(flags(:,3)) > 0) + 1;
    tID = targID >= id; tID(1) = 0;
    targID = targID(tID);
    t = time(targID);
    error = measSig(targID,dofID) - targSig(targID,dofID);
    tIP = linspace(t(1),t(end),length(t))';
    errorIP = interp1(t,error,tIP,[],0.0);
    dtIP = mean(diff(tIP));
    getFFT(errorIP,dtIP,sprintf('Error between Measured and Target Displacements: DOF %02d',dofID));
end
%==========================================================================
% subspace plot of measured vs. target displacements
CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
try
    targID = find(diff(flags(:,3)) > 0) + 1;
    tID = targID >= id;
    targID = targID(tID);
    plot(targSig(targID,dofID),measSig(targID,dofID),'-b','LineWidth',1.0);
    hold('on');
end
grid('on');
xlabel('Target Displacement [in.]','FontWeight','bold');
ylabel('Measured Displacement [in.]','FontWeight','bold');
title(sprintf('Subspace Plot of Measured vs. Target Displacements from xPC-Target: DOF %02d',dofID),'FontWeight','bold');
%==========================================================================
% Mercan (2007) tracking indicator of measured vs. target displacements
CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
try
    targID = find(diff(flags(:,3)) > 0) + 1;
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
ylabel('Tracking Indicator [in.^2]','FontWeight','bold');
title(sprintf('Tracking Indicator from xPC-Target: DOF %02d',dofID),'FontWeight','bold');
%==========================================================================
% normalized RMS error between measured and target displacements
CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
try
    targID = find(diff(flags(:,3)) > 0) + 1;
    tID = (targID >= id);
    targID = targID(tID);
    targDspAT = interp1(time(targID),targSig(targID,dofID),time,[],0.0);
    measDspAT = interp1(time(targID),measSig(targID,dofID),time,[],0.0);
    error = measDspAT - targDspAT;
    noNaN = ~isnan(error);
    rmsError = sqrt(cumsum(error(noNaN).^2)./(1:length(error(noNaN)))');
    
    %errRange = max(error) - min(error);
    sigRange = max(measDspAT) - min(measDspAT);
    %sigStdev = std(measDspAT(noNaN));
    
    %normRMS = rmsError./errRange*100;
    normRMS = rmsError./sigRange*100;
    %normRMS = rmsError./sigStdev*100;
    
    %plot(time(noNaN),rmsError,'-b','LineWidth',1.0);
    plot(time(noNaN),normRMS,'-b','LineWidth',1.0);
end
grid('on');
xlabel('Time [sec]','FontWeight','bold');
ylabel('Normalized RMS Error [%]','FontWeight','bold');
title(sprintf('RMS Error between Measured and Target Displacements from xPC-Target: DOF %02d',dofID),'FontWeight','bold');
%==========================================================================
% fft of error between measured and commmand displacements
%CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
%try
%   error = measSig(id:end,dofID) - commSig(id:end,dofID);
%   getFFT(error,dt,sprintf('Error between Measured and Command Displacements: DOF %02d',dofID));
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
title(sprintf('Measured Force from xPC-Target: DOF %02d',dofID),'FontWeight','bold');
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
    plot(time(id:end),state(id:end,1),'-b','LineWidth',1.0);
end
grid('on');
xlabel('Time [sec]','FontWeight','bold');
ylabel('State [-]','FontWeight','bold');
title('State of Predictor-Corrector from xPC-Target','FontWeight','bold');
%==========================================================================
return
% counter of predictor-corrector
CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
try
    plot(time(id:end),counter(id:end,1),'-b','LineWidth',1.0);
end
grid('on');
xlabel('Time [sec]','FontWeight','bold');
ylabel('Counter [-]','FontWeight','bold');
title('Counter of Predictor-Corrector from xPC-Target','FontWeight','bold');
%==========================================================================
% flags of predictor-corrector
CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
try
    plot(time(id:end),flags(id:end,1),'-b','LineWidth',1.0);
end
hold('on');
try
    plot(time(id:end),flags(id:end,2),'--r','LineWidth',1.0);
end
try
    plot(time(id:end),flags(id:end,3),'-.g','LineWidth',1.0);
end
grid('on');
xlabel('Time [sec]','FontWeight','bold');
ylabel('Flag [-]','FontWeight','bold');
title('Flags of Predictor-Corrector from xPC-Target','FontWeight','bold');
legend('newTarget','switchPC','atTarget');
%==========================================================================
