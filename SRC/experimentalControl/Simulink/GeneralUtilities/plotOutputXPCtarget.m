function data = plotOutputXPCtarget(fileName,dofID,tStart,tEnd,respType,respUnit)
%PLOTOUTPUTXPCTARGET to plot the xPC-Target output from a hybrid simulation
% data = plotOutputXPCtarget(fileName,dofID,tStart,tEnd,respType,respUnit)
%
% data     : data structure
% fileName : .mat file to be loaded
% dofID    : id of degree-of-freedom to plot (optional)
% tStart   : time to start plotting (optional)
% tEnd     : time to end plotting (optional)
% respType : string for type of response quantity (optional)
% respUnit : string for unit of response quantity (optional)
%
% Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
% Created: 04/05
%
% Modified: Christopher Neumann (christopherrneumann@gmail.com)
% Edited to include "tEnd" 05/11/2021

%#ok<*LOAD>
%#ok<*TRYNC>
%#ok<*NASGU>

% define constants
g = 32.174*12;
SS = get(0,'screensize');  % screen size
debug = 0;
if (nargin<6)
    respType = 'Displacement';
    respUnit = 'in.';
end

% load the file and extract data
data = [];
load(fileName);
time = data(1).values(:,end);
npts = length(time);
dt = mean(diff(time));
% check if we have ATS parameters recorded
haveATS = rem(size(data(1).values,2)-6,13) == 0;
if haveATS
    numDOF = (size(data(1).values,2)-6)/13;
    targSig       = data(1).values(:,0*numDOF+1:1*numDOF);
    commSig       = data(1).values(:,1*numDOF+1:2*numDOF);
    commSigDot    = data(1).values(:,2*numDOF+1:3*numDOF);
    commSigDotDot = data(1).values(:,3*numDOF+1:4*numDOF);
    atsDsp        = data(1).values(:,4*numDOF+1:5*numDOF);
    measSig       = data(1).values(:,5*numDOF+1:7*numDOF);
    measDsp       = data(1).values(:,7*numDOF+1:8*numDOF);
    measFrc       = data(1).values(:,8*numDOF+1:9*numDOF);
    atsPar        = data(1).values(:,9*numDOF+1:13*numDOF);
    state         = data(1).values(:,13*numDOF+1);
    counter       = data(1).values(:,13*numDOF+2);
    flags         = data(1).values(:,13*numDOF+3:13*numDOF+5);
else
    numDOF = (size(data(1).values,2)-6)/8;
    targSig       = data(1).values(:,0*numDOF+1:1*numDOF);
    commSig       = data(1).values(:,1*numDOF+1:2*numDOF);
    commSigDot    = data(1).values(:,2*numDOF+1:3*numDOF);
    commSigDotDot = data(1).values(:,3*numDOF+1:4*numDOF);
    measSig       = data(1).values(:,4*numDOF+1:6*numDOF);
    measDsp       = data(1).values(:,6*numDOF+1:7*numDOF);
    measFrc       = data(1).values(:,7*numDOF+1:8*numDOF);
    state         = data(1).values(:,8*numDOF+1);
    counter       = data(1).values(:,8*numDOF+2);
    flags         = data(1).values(:,8*numDOF+3:8*numDOF+5);
end

% check for optional parameters
if (nargin<2)
    dofID = 1;
    idS = find(state >= 0,1);
    idE = npts;
elseif (nargin<3)
    idS = find(state >= 0,1);
    idE = npts;
elseif (nargin<4)
    idS = find(time>=tStart,1);
    idE = npts;
else
    idS = find(time>=tStart,1);
    idE = min(find(time<=tEnd,1,'last'),npts);
end
timeID = idS:idE;

% get measured velocities and accelerations
measVel = (1/dt)*[zeros(1,numDOF);diff(measDsp)];
measAcc = (1/dt)*[zeros(1,numDOF);diff(measVel)];

%==========================================================================
% command signal
createWindow('cen',0.80*SS(4)/3*4,0.80*SS(4),['Command ',respType]);
try
    plot(time(timeID),commSig(timeID,dofID),'b-','LineWidth',1.0);
    hold('on');
    if haveATS
        plot(time(timeID),atsDsp(timeID,dofID),'r-','LineWidth',1.0);
    end
end
grid('on');
xlabel('Time [sec]','FontWeight','bold');
ylabel(['Command ',respType,' [',respUnit,']'],'FontWeight','bold');
title(sprintf('Command %s from xPC-Target: DOF %02d',respType,dofID),'FontWeight','bold');
if haveATS
    legend('command','compensated');
end
%==========================================================================
% fft of command signal
createWindow('cen',0.80*SS(4)/3*4,0.80*SS(4),['FFT Comm ',respType]);
try
    getFFT(commSig(timeID,dofID),dt,'title',sprintf('Command %s: DOF %02d',respType,dofID));
    getFFT(atsDsp(timeID,dofID),dt,'title',sprintf('Command %s: DOF %02d',respType,dofID));
    set(gca,'YScale','log');
    legend('command','compensated');
end
%==========================================================================
% command signalDot
createWindow('cen',0.80*SS(4)/3*4,0.80*SS(4),['(Command ',respType,')Dot']);
try
    plot(time(timeID),commSigDot(timeID,dofID),'b-','LineWidth',1.0);
end
grid('on');
xlabel('Time [sec]','FontWeight','bold');
ylabel(['(Command ',respType,')^\bullet [',respUnit,'/sec]'],'FontWeight','bold');
title(['(Command ',respType,')^\bullet from xPC-Target: ',sprintf('DOF %02d',dofID)],'FontWeight','bold');
%==========================================================================
% command signalDotDot
createWindow('cen',0.80*SS(4)/3*4,0.80*SS(4),['(Command ',respType,')DotDot']);
try
    plot(time(timeID),commSigDotDot(timeID,dofID),'b-','LineWidth',1.0);
end
grid('on');
xlabel('Time [sec]','FontWeight','bold');
ylabel(['(Command ',respType,')^\bullet^\bullet [',respUnit,'/sec^2]'],'FontWeight','bold');
title(['(Command ',respType,')^\bullet^\bullet from xPC-Target: ',sprintf('DOF %02d',dofID)],'FontWeight','bold');
%==========================================================================
% target, command, and measured signals
createWindow('cen',0.80*SS(4)/3*4,0.80*SS(4),['Targ, Comm, & Meas ',respType]);
try
    plot(time(timeID),targSig(timeID,dofID),'-b','LineWidth',1.0);
    hold('on');
    targID = find(diff(flags(:,3)) > 0) + 1;
    tID = (idS <= targID & targID <= idE);
    targID = targID(tID);
    plot(time(targID),targSig(targID,dofID),'ob','MarkerSize',3);
end
try
    plot(time(timeID),commSig(timeID,dofID),'-r','LineWidth',1.0);
end
try
    plot(time(timeID),measSig(timeID,dofID),'-g','LineWidth',1.0);
end
if debug
    try
        plot(time(timeID),state(timeID),'-k','LineWidth',1.0);
    end
end
grid('on');
xlabel('Time [sec]','FontWeight','bold');
ylabel([respType,' [',respUnit,']'],'FontWeight','bold');
title(sprintf('%s from xPC-Target: DOF %02d',respType,dofID),'FontWeight','bold');
legend('target','target @ measured','command','measured');
%==========================================================================
% error between measured and target signals
createWindow('cen',0.80*SS(4)/3*4,0.80*SS(4),['Error Meas & Targ ',respType]);
timeShift = 0;
try
    targID = find(diff(flags(:,3)) > 0) + 1;
    tID = (idS <= targID & targID <= idE);
    targID = targID(tID);
    targSigAT = interp1(time(targID),targSig(targID,dofID),time,[],0.0);
    measSigAT = interp1(time(targID),measSig(targID,dofID),time,[],0.0);
    error = measSigAT - targSigAT;
    timeShift = findTimeShift(time,targSigAT,time,measSigAT);
    plot(time(timeID),targSigAT(timeID),'-b','LineWidth',1.0);
    hold('on');
    plot(time(timeID),measSigAT(timeID),'-r','LineWidth',1.0);
    plot(time(timeID)+timeShift,measSigAT(timeID),'--m','LineWidth',1.0);
    plot(time(timeID),error(timeID),'-g','LineWidth',1.0);
end
grid('on');
xlabel('Time [sec]','FontWeight','bold');
ylabel([respType,' [',respUnit,']'],'FontWeight','bold');
title(sprintf('Error between Measured and Target %s from xPC-Target: DOF %02d',respType,dofID),'FontWeight','bold');
legend('target','measured',sprintf('measured (shifted by %1.0f msec)',1000*timeShift),'error');
%==========================================================================
% error between measured and command signals
createWindow('cen',0.80*SS(4)/3*4,0.80*SS(4),['Error Meas & Comm ',respType]);
timeShift = 0;
try
    commSigAT = commSig(:,dofID);
    measSigAT = measDsp(:,dofID);
    error = measSigAT - commSigAT;
    timeShift = findTimeShift(time,commSigAT,time,measSigAT);
    plot(time(timeID),commSigAT(timeID),'-b','LineWidth',1.0);
    hold('on');
    plot(time(timeID),measSigAT(timeID),'-r','LineWidth',1.0);
    plot(time(timeID)+timeShift,measSigAT(timeID),'--m','LineWidth',1.0);
    plot(time(timeID),error(timeID),'-g','LineWidth',1.0);
end
grid('on');
xlabel('Time [sec]','FontWeight','bold');
ylabel([respType,' [',respUnit,']'],'FontWeight','bold');
title(sprintf('Error between Measured and Command %s from xPC-Target: DOF %02d',respType,dofID),'FontWeight','bold');
legend('command','measured',sprintf('measured (shifted by %1.0f msec)',1000*timeShift),'error');
%==========================================================================
% error between measured and command signalDotDot
createWindow('cen',0.80*SS(4)/3*4,0.80*SS(4),'Error Meas & Comm Acceleration');
timeShift = 0;
try
    commAccAT = (1/g)*commSigDotDot(:,dofID);
    measAccAT = (1/g)*measAcc(:,dofID);
    error = measAccAT - commAccAT;
    timeShift = findTimeShift(time,commAccAT,time,measAccAT);
    plot(time(timeID),commAccAT(timeID),'-b','LineWidth',1.0);
    hold('on');
    plot(time(timeID),measAccAT(timeID),'-r','LineWidth',1.0);
    plot(time(timeID)+timeShift,measAccAT(timeID),'--m','LineWidth',1.0);
    plot(time(timeID),error(timeID),'-g','LineWidth',1.0);
end
grid('on');
xlabel('Time [sec]','FontWeight','bold');
ylabel('Acceleration [g]','FontWeight','bold');
title(sprintf('Error between Measured and Command Accelerations from xPC-Target: DOF %02d',dofID),'FontWeight','bold');
legend('command','measured',sprintf('measured (shifted by %1.0f msec)',1000*timeShift),'error');
%==========================================================================
% transfer function between measured and command signal
createWindow('cen',0.80*SS(4)/3*4,0.80*SS(4),['Transfer Function Meas & Comm ',respType]);
try
    commSigAT = commSig(:,dofID);
    measSigAT = measDsp(:,dofID);
    
    winSize = 2.5;  % window size in [sec]
    nfft = ceil(winSize/dt);  % window frame size in points
    %nfft = 2^nextpow2(winSize/dt);  % window frame size in points
    %nfft = ceil(length(commSigAT)/10);  % window frame size in points
    %nfft = 2^nextpow2(length(commSigAT)/10);  % window frame size in points
    f = logspace(log10(0.2),log10(50),256)';  % use 256 log-spaced frequency values
    Tc2m = tfestimate(commSigAT, measSigAT, hann(nfft), [], f, 1/dt);
    Cc2m = mscohere(commSigAT, measSigAT, hann(nfft), [], f, 1/dt);
    
    fMin = min(f);
    fMax = 33;
    
    subplot(3,1,1);
    plot(f,mag2db(abs(Tc2m)),'-b','LineWidth',1.0);
    %set(gca,'XLim',[fMin fMax]);
    set(gca,'XScale','log','XLim',[fMin fMax]);
    grid('on');
    %xlabel('Frequency [Hz]','FontWeight','bold');
    ylabel('Magnitude [dB]','FontWeight','bold');
    title(sprintf('Transfer Function between Measured and Command %s from xPC-Target: DOF %02d',respType,dofID),'FontWeight','bold');
    
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
% transfer function between measured and command signalDotDot
createWindow('cen',0.80*SS(4)/3*4,0.80*SS(4),'Transfer Function Meas & Comm Acceleration');
try
    commAccAT = (1/g)*commSigDotDot(:,dofID);
    measAccAT = (1/g)*measAcc(:,dofID);
    
    winSize = 2.5;  % window size in [sec]
    nfft = ceil(winSize/dt);  % window frame size in points
    %nfft = 2^nextpow2(winSize/dt);  % window frame size in points
    %nfft = ceil(length(commSigAT)/10);  % window frame size in points
    %nfft = 2^nextpow2(length(commSigAT)/10);  % window frame size in points
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
% fft of error between measured and target signals
createWindow('cen',0.80*SS(4)/3*4,0.80*SS(4),['FFT of Error Meas & Targ ',respType]);
try
    targID = find(diff(flags(:,3)) > 0) + 1;
    tID = (idS <= targID & targID <= idE); tID(1) = 0;
    targID = targID(tID);
    t = time(targID);
    error = measSig(targID,dofID) - targSig(targID,dofID);
    tIP = linspace(t(1),t(end),length(t))';
    errorIP = interp1(t,error,tIP,[],0.0);
    dtIP = mean(diff(tIP));
    getFFT(errorIP,dtIP,'title',sprintf('Error between Measured and Target %s: DOF %02d',respType,dofID));
end
%==========================================================================
% normalized subspace plot of measured vs. target signals
createWindow('cen',0.80*SS(4)/3*4,0.80*SS(4),['Subspace Plot Meas & Targ ',respType]);
try
    targID = find(diff(flags(:,3)) > 0) + 1;
    tID = (idS <= targID & targID <= idE);
    targID = targID(tID);
    sigMin = min(min(targSig(targID,dofID)),min(measSig(targID,dofID)));
    sigMax = max(max(targSig(targID,dofID)),max(measSig(targID,dofID)));
    sigRange = max(abs(sigMin),abs(sigMax));
    plot(targSig(targID,dofID)./sigRange*100,measSig(targID,dofID)./sigRange*100,'-b','LineWidth',1.0);
    hold('on');
    lim = [sigMin,sigMax]./sigRange*100;
    plot(lim,lim,'-.r','LineWidth',1.0);
    set(gca,'XLim',lim,'YLim',lim);
    axis('square');
end
grid('on');
xlabel(['Normalized Target ',respType,' [%]'],'FontWeight','bold');
ylabel(['Normalized Measured ',respType,' [%]'],'FontWeight','bold');
title(sprintf('Subspace Plot of Measured vs. Target %s from xPC-Target: DOF %02d',respType,dofID),'FontWeight','bold');
%==========================================================================
% Mercan (2007) normalized tracking indicator of measured vs. target signals
createWindow('cen',0.80*SS(4)/3*4,0.80*SS(4),['Tracking Indicator Meas & Targ ',respType]);
try
    targID = find(diff(flags(:,3)) > 0) + 1;
    tID = (idS <= targID & targID <= idE);
    targID = targID(tID);
    sigRange = max(max(abs(targSig(targID,dofID))),max(abs(measSig(targID,dofID))));
    A  = cumsum(0.5*(measSig(targID(2:end),dofID)+measSig(targID(1:end-1),dofID)).*diff(targSig(targID,dofID)));
    TA = cumsum(0.5*(targSig(targID(2:end),dofID)+targSig(targID(1:end-1),dofID)).*diff(measSig(targID,dofID)));
    TI = 0.5*(A - TA)./sigRange^2*100;
    TI = [0;TI];
    plot(time(targID),TI,'-b','LineWidth',1.0);
    hold('on');
end
grid('on');
xlabel('Time [sec]','FontWeight','bold');
ylabel('Normalized Tracking Indicator [%]','FontWeight','bold');
title(sprintf('Tracking Indicator from xPC-Target: DOF %02d',dofID),'FontWeight','bold');
%==========================================================================
% normalized RMS error between measured and target signals
createWindow('cen',0.80*SS(4)/3*4,0.80*SS(4),['RMS Error Meas & Targ ',respType]);
try
    targID = find(diff(flags(:,3)) > 0) + 1;
    tID = (idS <= targID & targID <= idE);
    targID = targID(tID);
    targSigAT = interp1(time(targID),targSig(targID,dofID),time,[],0.0);
    measSigAT = interp1(time(targID),measSig(targID,dofID),time,[],0.0);
    error = measSigAT - targSigAT;
    noNaN = ~isnan(error);
    rmsError = sqrt(cumsum(error(noNaN).^2)./(1:length(error(noNaN)))');
    
    %errRange = max(error) - min(error);
    sigRange = max(measSigAT) - min(measSigAT);
    %sigStdev = std(measSigAT(noNaN));
    
    %normRMS = rmsError./errRange*100;
    normRMS = rmsError./sigRange*100;
    %normRMS = rmsError./sigStdev*100;
    
    %plot(time(noNaN(timeID)),rmsError(timeID),'-b','LineWidth',1.0);
    plot(time(noNaN(timeID)),normRMS(timeID),'-b','LineWidth',1.0);
end
grid('on');
xlabel('Time [sec]','FontWeight','bold');
ylabel('Normalized RMS Error [%]','FontWeight','bold');
title(sprintf('RMS Error between Measured and Target %s from xPC-Target: DOF %02d',respType,dofID),'FontWeight','bold');
%==========================================================================
% measured force
createWindow('cen',0.80*SS(4)/3*4,0.80*SS(4),'Measured Force');
try
    plot(time(timeID),measFrc(timeID,dofID),'-b','LineWidth',1.0);
end
grid('on');
xlabel('Time [sec]','FontWeight','bold');
ylabel('Measured Force [kip]','FontWeight','bold');
title(sprintf('Measured Force from xPC-Target: DOF %02d',dofID),'FontWeight','bold');
%==========================================================================
% fft of measured force
createWindow('cen',0.80*SS(4)/3*4,0.80*SS(4),'FFT of Measured Force');
try
    getFFT(measFrc(timeID,dofID),dt,'title',sprintf('Measured Force: DOF %02d',dofID));
    set(gca,'YScale','log');
end
%==========================================================================
% measured displacement vs. measured force
createWindow('cen',0.80*SS(4)/3*4,0.80*SS(4),'Measured Displacement vs. Force');
try
    plot(measDsp(timeID,dofID),measFrc(timeID,dofID),'-b','LineWidth',1.0);
end
grid('on');
xlabel('Measured Displacement [in.]','FontWeight','bold');
ylabel('Measured Force [kip]','FontWeight','bold');
title(sprintf('Measured Force from xPC-Target: DOF %02d',dofID),'FontWeight','bold');
%==========================================================================
% state of predictor-corrector
createWindow('cen',0.80*SS(4)/3*4,0.80*SS(4),'State of Predictor-Corrector');
try
    plot(time(timeID),state(timeID),'-b','LineWidth',1.0);
end
grid('on');
xlabel('Time [sec]','FontWeight','bold');
ylabel('State [-]','FontWeight','bold');
title('State of Predictor-Corrector from xPC-Target','FontWeight','bold');
%==========================================================================
if debug
    % counter of predictor-corrector
    createWindow('cen',0.80*SS(4)/3*4,0.80*SS(4),'Counter of Predictor-Corrector');
    try
        plot(time(timeID),counter(timeID),'-b','LineWidth',1.0);
    end
    grid('on');
    xlabel('Time [sec]','FontWeight','bold');
    ylabel('Counter [-]','FontWeight','bold');
    title('Counter of Predictor-Corrector from xPC-Target','FontWeight','bold');
    % flags of predictor-corrector
    createWindow('cen',0.80*SS(4)/3*4,0.80*SS(4),'Flags of Predictor-Corrector');
    try
        plot(time(timeID),flags(timeID,1),'-b','LineWidth',1.0);
    end
    hold('on');
    try
        plot(time(timeID),flags(timeID,2),'--r','LineWidth',1.0);
    end
    try
        plot(time(timeID),flags(timeID,3),'-.g','LineWidth',1.0);
    end
    grid('on');
    xlabel('Time [sec]','FontWeight','bold');
    ylabel('Flag [-]','FontWeight','bold');
    title('Flags of Predictor-Corrector from xPC-Target','FontWeight','bold');
    legend('newTarget','switchPC','atTarget');
    % task execution time (TET) of predictor-corrector
    if isfield(data,'logTET') && ~isempty(data(1).logTET)
        createWindow('cen',0.80*SS(4)/3*4,0.80*SS(4),'TET of Predictor-Corrector');
        try
            plot(time(timeID),data(1).logTET(timeID),'-b','LineWidth',1.0);
            hold('on');
            plot(time(timeID),repmat(dt,length(timeID),1),'--r','LineWidth',2.0);
            set(gca,'YScale','log');
        end
        grid('on');
        xlabel('Time [sec]','FontWeight','bold');
        ylabel('TET [sec]','FontWeight','bold');
        title('TET of Predictor-Corrector from xPC-Target','FontWeight','bold');
    end
end
%==========================================================================
