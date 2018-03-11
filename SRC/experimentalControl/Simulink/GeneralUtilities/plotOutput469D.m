function data = plotOutput469D(fileName,dofID)
%PLOTOUTPUT469D to plot the MTS-469D output from a hybrid shake table test
% data = plotOutput469D(fileName,actID)
%
% data     : data structure
% fileName : .bin file to be loaded
% dofID    : id of DOF to plot (optional)
%
% Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
% Created: 05/15
%
% $Revision$
% $Date$
% $URL$

% check for file extension
if isempty(strfind(fileName,'.bin'))
    fileName = [fileName,'.bin'];
end

% load the file
% [data,deltaT,desc,units,fileInfo,fileDate,header,isText] = loadbin(fileName);
[data,deltaT,desc] = loadbin(fileName);
npts = size(data,1);
time = deltaT*(0:npts-1)';
desc = cellstr(desc)

if nargin<2
    dofID = 1;
end
dofStr = {'Long','Lat','Vert','Roll','Pitch','Yaw'};

displCmdID = [];
displFbkID = [];
accelCmdID = [];
accelFbkID = [];
forceFbkID = [];
for i=1:length(dofID)
    id = ~cellfun(@isempty,strfind(desc,dofStr{dofID(i)}));
    displCmdID(end+1) = find(~cellfun(@isempty,strfind(desc,'Displ ref')) & id);
    displFbkID(end+1) = find(~cellfun(@isempty,strfind(desc,'Displ fbk')) & id);
    accelCmdID(end+1) = find(~cellfun(@isempty,strfind(desc,'Accel ref')) & id);
    accelFbkID(end+1) = find(~cellfun(@isempty,strfind(desc,'Accel fbk')) & id);
    forceFbkID(end+1) = find(~cellfun(@isempty,strfind(desc,'Force fbk')) & id);
end

% extract data vectors
displCmd = data(:,displCmdID);
displFbk = data(:,displFbkID);
accelCmd = data(:,accelCmdID);
accelFbk = data(:,accelFbkID);
forceFbk = data(:,forceFbkID);

% remove initial offset
if ~isempty(displCmd)
    displCmd = displCmd - mean(displCmd(1:1024));
end
if ~isempty(displFbk)
    displFbk = displFbk - mean(displFbk(1:1024));
end

% get screen size
SS = get(0,'screensize');

%==========================================================================
% command and measured displacements
CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
plot(time,displFbk,'-r');
hold('on');
plot(time,displCmd,'-b');
grid('on');
xlabel('Time [sec]');
ylabel('Displacement [in.]');
title(['Displacements from 469D: DOF ',dofStr{dofID}]);
legend('measDsp','commDsp');
%==========================================================================
% command and measured accelerations
CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
plot(time,accelFbk,'-r');
hold('on');
plot(time,accelCmd,'-b');
grid('on');
xlabel('Time [sec]');
ylabel('Acceleration [in./sec^2]');
title(['Accelerations from 469D: DOF ',dofStr{dofID}]);
legend('measAcc','commAcc');
%==========================================================================
% fft of error between command and measured displacements
CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
error = displFbk - displCmd;
getFFT(error,deltaT,['Error between measured and command displacement: DOF ',dofStr{dofID}]);
set(gca,'YScale','log');
%==========================================================================
% command vs. measured displacements
CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
plot(displCmd,displFbk,'-b');
hold('on');
grid('on');
xlabel('Command Displacement [in.]');
ylabel('Measured Displacement [in.]');
title(['Displacements from 469D: DOF ',dofStr{dofID}]);
%==========================================================================
% Mercan (2007) tracking indicator
CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
A  = cumsum(0.5*(displFbk(2:end)+displFbk(1:end-1)).*diff(displCmd));
TA = cumsum(0.5*(displCmd(2:end)+displCmd(1:end-1)).*diff(displFbk));
TI = 0.5*(A - TA);
TI = [0;TI];
plot(deltaT*(0:size(TI,1)-1)',TI,'-b','LineWidth',1.0);
hold('on');
grid('on');
xlabel('Time [sec]');
ylabel('Tracking Indicator [in^2]');
title(['Tracking Indicator from 469D: DOF ',dofStr{dofID}]);
%==========================================================================
% fft of measured forces
CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
getFFT(forceFbk,deltaT,['Measured force: DOF ',dofStr{dofID}]);
set(gca,'YScale','log');
%==========================================================================
% hysteresis loop
mass = [86.8 86.8 86.8 0.0 0.0 0.0];
CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
%plot(displFbk,-forceFbk,'-b','LineWidth',1.0);
plot(displFbk,-forceFbk+repmat(mass(dofID),npts,1).*accelFbk,'-r','LineWidth',1.0);
hold('on');
set(gca,'Box','on');
grid('on');
xlabel('Measured Displacement [in.]');
ylabel('Measured Force [kip]');
title(['Hysteresis Loop from 469D: DOF ',dofStr{dofID}]);
%==========================================================================
