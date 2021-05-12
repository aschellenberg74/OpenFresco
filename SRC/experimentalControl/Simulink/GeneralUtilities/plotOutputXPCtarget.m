function data = plotOutputXPCtarget_v2(fileName,dofID,tStart,tEnd)
%PLOTOUTPUTXPCTARGET to plot the xPC-Target output from a hybrid simulation
% data = plotOutputXPCtarget_v2(fileName,dofID,tStart,tEnd)
%
% data     : data structure
% fileName : .mat file to be loaded
% dofID    : id of degree-of-freedom to plot (optional)
% tStart   : time to start plotting (optional)
% tEnd     : time to end plotting (optional)
%
% Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
% Created: 04/05
%
% Modified: Christopher Neumann (christopherrneumann@gmail.com)
% Edited to include "tEnd" 05/11/2021

%#ok<*LOAD>
%#ok<*TRYNC>

% define constants
g = 32.174*12;
SS = get(0,'screensize');  % screen size
debug = 0;

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

time = targSig(:,end);
npts = length(time);
dt = mean(diff(time));

% check for optional parameters
if (nargin<2)
    dofID = 1;
    idS = ceil(1/dt);
    idE = npts;
elseif (nargin<3)
    idS = ceil(1/dt);
    idE = npts;
elseif (nargin<4)
    idS = max(find(time>=tStart,1),ceil(1/dt));
    idE = npts;
else
    idS = max(find(time>=tStart,1),ceil(1/dt));
    idE = min(find(time<=tEnd,1,'last'),npts);
end

% check if commSig derivatives exist
if (size(data(2).values,2)-1 >= 2*numDOF)
    commSigDot = data(2).values(:,[numDOF+1:2*numDOF,end]);
end
if (size(data(2).values,2)-1 >= 3*numDOF)
    commSigDotDot = data(2).values(:,[2*numDOF+1:3*numDOF,end]);
end

% get measured velocities and accelerations
measVel = (1/dt)*[zeros(1,size(measDsp,2));diff(measDsp)];
measAcc = (1/dt)*[zeros(1,size(measVel,2));diff(measVel)];

%==========================================================================
% command displacements
createWindow('cen',0.80*SS(4)/3*4,0.80*SS(4),'Command Displacement');
try
    plot(time(idS:idE),commSig(idS:idE,dofID),'b-','LineWidth',1.0);
    %hold on;
    %plot(time(idS:idE),measFrc(idS:idE,10),'r-','LineWidth',1.0);
end
grid('on');
xlabel('Time [sec]','FontWeight','bold');
ylabel('Command Displacement [in.]','FontWeight','bold');
title(sprintf('Command Displacement from xPC-Target: DOF %02d',dofID),'FontWeight','bold');
%==========================================================================
% command velocities
if exist('commSigDot','var')
    createWindow('cen',0.80*SS(4)/3*4,0.80*SS(4),'Command Velocity');
    try
        plot(time(idS:idE),commSigDot(idS:idE,dofID),'b-','LineWidth',1.0);
    end
    grid('on');
    xlabel('Time [sec]','FontWeight','bold');
    ylabel('Command Velocity [in./sec]','FontWeight','bold');
    title(sprintf('Command Velocity from xPC-Target: DOF %02d',dofID),'FontWeight','bold');
end
%==========================================================================
% command accelerations
if exist('commSigDotDot','var')
    createWindow('cen',0.80*SS(4)/3*4,0.80*SS(4),'Command Acceleration');
    try
        plot(time(idS:idE),(1/g)*commSigDotDot(idS:idE,dofID),'b-','LineWidth',1.0);
    end
    grid('on');
    xlabel('Time [sec]','FontWeight','bold');
    ylabel('Command Acceleration [g]','FontWeight','bold');
    title(sprintf('Command Acceleration from xPC-Target: DOF %02d',dofID),'FontWeight','bold');
end
%==========================================================================
% fft of command displacements
createWindow('cen',0.80*SS(4)/3*4,0.80*SS(4),'FFT Comm Displacement');
try
    getFFT(commSig(idS:idE,dofID),dt,sprintf('Command Displacement: DOF %02d',dofID));
    set(gca,'YScale','log');
end
%==========================================================================
% target, command, and measured displacements
createWindow('cen',0.80*SS(4)/3*4,0.80*SS(4),'Targ, Comm, & Meas Displacement');
try
    plot(time(idS:idE),targSig(idS:idE,dofID),'-b','LineWidth',1.0);
    hold('on');
    targID = find(diff(flags(:,3)) > 0) + 1;
    tID = (idS <= targID & targID <= idE);
    targID = targID(tID);
    plot(time(targID),targSig(targID,dofID),'ob','MarkerSize',3);
end
try
    plot(time(idS:idE),commSig(idS:idE,dofID),'-r','LineWidth',1.0);
end
try
    plot(time(idS:idE),measSig(idS:idE,dofID),'-g','LineWidth',1.0);
end
%try
%   plot(time(idS:idE),state(idS:idE,1),'-k','LineWidth',1.0);
%end
grid('on');
xlabel('Time [sec]','FontWeight','bold');
ylabel('Displacement [in.]','FontWeight','bold');
title(sprintf('Displacements from xPC-Target: DOF %02d',dofID),'FontWeight','bold');
legend('target','target @ measured','command','measured');
%==========================================================================
% error between measured and target displacements
createWindow('cen',0.80*SS(4)/3*4,0.80*SS(4),'Error Meas & Targ Displacement');
timeShift = 0;
try
    targID = find(diff(flags(:,3)) > 0) + 1;
    tID = (idS <= targID & targID <= idE);
    targID = targID(tID);
    targDspAT = interp1(time(targID),targSig(targID,dofID),time,[],0.0);
    measDspAT = interp1(time(targID),measSig(targID,dofID),time,[],0.0);
    error = measDspAT - targDspAT;
    timeShift = findTimeShift(time,targDspAT,time,measDspAT);
    plot(time(idS:idE),targDspAT(idS:idE),'-b','LineWidth',1.0);
    hold('on');
    plot(time(idS:idE),measDspAT(idS:idE),'-r','LineWidth',1.0);
    plot(time(idS:idE)+timeShift,measDspAT(idS:idE),'--m','LineWidth',1.0);
    plot(time(idS:idE),error(idS:idE),'-g','LineWidth',1.0);
end
grid('on');
xlabel('Time [sec]','FontWeight','bold');
ylabel('Displacement [in.]','FontWeight','bold');
title(sprintf('Error between Measured and Target Displacements from xPC-Target: DOF %02d',dofID),'FontWeight','bold');
legend('target','measured',sprintf('measured (shifted by %1.0f msec)',1000*timeShift),'error');
%==========================================================================
% error between measured and command displacements
createWindow('cen',0.80*SS(4)/3*4,0.80*SS(4),'Error Meas & Comm Displacement');
timeShift = 0;
try
    commDspAT = commSig(:,dofID);
    measDspAT = measDsp(:,dofID);
    error = measDspAT - commDspAT;
    timeShift = findTimeShift(time,commDspAT,time,measDspAT);
    plot(time(idS:idE),commDspAT(idS:idE),'-b','LineWidth',1.0);
    hold('on');
    plot(time(idS:idE),measDspAT(idS:idE),'-r','LineWidth',1.0);
    plot(time(idS:idE)+timeShift,measDspAT(idS:idE),'--m','LineWidth',1.0);
    plot(time(idS:idE),error(idS:idE),'-g','LineWidth',1.0);
end
grid('on');
xlabel('Time [sec]','FontWeight','bold');
ylabel('Displacement [in.]','FontWeight','bold');
title(sprintf('Error between Measured and Command Displacements from xPC-Target: DOF %02d',dofID),'FontWeight','bold');
legend('command','measured',sprintf('measured (shifted by %1.0f msec)',1000*timeShift),'error');
%==========================================================================
% error between measured and command accelerations
createWindow('cen',0.80*SS(4)/3*4,0.80*SS(4),'Error Meas & Comm Acceleration');
timeShift = 0;
try
    commAccAT = (1/g)*commSigDotDot(:,dofID);
    measAccAT = (1/g)*measAcc(:,dofID);
    error = measAccAT - commAccAT;
    timeShift = findTimeShift(time,commAccAT,time,measAccAT);
    plot(time(idS:idE),commAccAT(idS:idE),'-b','LineWidth',1.0);
    hold('on');
    plot(time(idS:idE),measAccAT(idS:idE),'-r','LineWidth',1.0);
    plot(time(idS:idE)+timeShift,measAccAT(idS:idE),'--m','LineWidth',1.0);
    plot(time(idS:idE),error(idS:idE),'-g','LineWidth',1.0);
end
grid('on');
xlabel('Time [sec]','FontWeight','bold');
ylabel('Acceleration [g]','FontWeight','bold');
title(sprintf('Error between Measured and Command Accelerations from xPC-Target: DOF %02d',dofID),'FontWeight','bold');
legend('command','measured',sprintf('measured (shifted by %1.0f msec)',1000*timeShift),'error');
%==========================================================================
% transfer function between measured and command displacements
createWindow('cen',0.80*SS(4)/3*4,0.80*SS(4),'Transfer Function Meas & Comm Displacement');
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
createWindow('cen',0.80*SS(4)/3*4,0.80*SS(4),'Transfer Function Meas & Comm Acceleration');
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
createWindow('cen',0.80*SS(4)/3*4,0.80*SS(4),'FFT of Error Meas & Targ Displacement');
try
    targID = find(diff(flags(:,3)) > 0) + 1;
    tID = (idS <= targID & targID <= idE); tID(1) = 0;
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
createWindow('cen',0.80*SS(4)/3*4,0.80*SS(4),'Subspace Plot Meas & Targ Displacement');
try
    targID = find(diff(flags(:,3)) > 0) + 1;
    tID = (idS <= targID & targID <= idE);
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
createWindow('cen',0.80*SS(4)/3*4,0.80*SS(4),'Tracking Indicator Meas & Targ Displacement');
try
    targID = find(diff(flags(:,3)) > 0) + 1;
    tID = (idS <= targID & targID <= idE);
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
createWindow('cen',0.80*SS(4)/3*4,0.80*SS(4),'Norm RMSE Meas & Targ Displacement');
try
    targID = find(diff(flags(:,3)) > 0) + 1;
    tID = (idS <= targID & targID <= idE);
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
    
    %plot(time(noNaN(idS:idE)),rmsError(idS:idE),'-b','LineWidth',1.0);
    plot(time(noNaN(idS:idE)),normRMS(idS:idE),'-b','LineWidth',1.0);
end
grid('on');
xlabel('Time [sec]','FontWeight','bold');
ylabel('Normalized RMS Error [%]','FontWeight','bold');
title(sprintf('RMS Error between Measured and Target Displacements from xPC-Target: DOF %02d',dofID),'FontWeight','bold');
%==========================================================================
% fft of error between measured and commmand displacements
%createWindow('cen',0.80*SS(4)/3*4,0.80*SS(4),'FFT of Error Meas & Comm Displacement');
%try
%   error = measSig(idS:idE,dofID) - commSig(idS:idE,dofID);
%   getFFT(error,dt,sprintf('Error between Measured and Command Displacements: DOF %02d',dofID));
%end
%==========================================================================
% measured force
createWindow('cen',0.80*SS(4)/3*4,0.80*SS(4),'Measured Force');
try
    plot(time(idS:idE),measFrc(idS:idE,dofID),'-b','LineWidth',1.0);
end
grid('on');
xlabel('Time [sec]','FontWeight','bold');
ylabel('Measured Force [kip]','FontWeight','bold');
title(sprintf('Measured Force from xPC-Target: DOF %02d',dofID),'FontWeight','bold');
%==========================================================================
% fft of measured force
createWindow('cen',0.80*SS(4)/3*4,0.80*SS(4),'FFT of Measured Force');
try
    getFFT(measFrc(idS:idE,dofID),dt,sprintf('Measured Force: DOF %02d',dofID));
    set(gca,'YScale','log');
end
%==========================================================================
% state of predictor-corrector
createWindow('cen',0.80*SS(4)/3*4,0.80*SS(4),'State of Predictor-Corrector');
try
    plot(time(idS:idE),state(idS:idE,1),'-b','LineWidth',1.0);
end
grid('on');
xlabel('Time [sec]','FontWeight','bold');
ylabel('State [-]','FontWeight','bold');
title('State of Predictor-Corrector from xPC-Target','FontWeight','bold');
%==========================================================================

if debug
    %==========================================================================
    % counter of predictor-corrector
    createWindow('cen',0.80*SS(4)/3*4,0.80*SS(4),'Counter of Predictor-Corrector');
    try
        plot(time(idS:idE),counter(idS:idE,1),'-b','LineWidth',1.0);
    end
    grid('on');
    xlabel('Time [sec]','FontWeight','bold');
    ylabel('Counter [-]','FontWeight','bold');
    title('Counter of Predictor-Corrector from xPC-Target','FontWeight','bold');
    %==========================================================================
    % flags of predictor-corrector
    createWindow('cen',0.80*SS(4)/3*4,0.80*SS(4),'Flags of Predictor-Corrector');
    try
        plot(time(idS:idE),flags(idS:idE,1),'-b','LineWidth',1.0);
    end
    hold('on');
    try
        plot(time(idS:idE),flags(idS:idE,2),'--r','LineWidth',1.0);
    end
    try
        plot(time(idS:idE),flags(idS:idE,3),'-.g','LineWidth',1.0);
    end
    grid('on');
    xlabel('Time [sec]','FontWeight','bold');
    ylabel('Flag [-]','FontWeight','bold');
    title('Flags of Predictor-Corrector from xPC-Target','FontWeight','bold');
    legend('newTarget','switchPC','atTarget');
    %==========================================================================
end