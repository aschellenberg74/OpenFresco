function data = plotOutputSTS(fileName,actID)
%PLOTOUTPUTSTS to plot the MTS-STS output from a hybrid simulation
% data = plotOutputSTS(fileName,actID)
%
% data     : data structure
% fileName : .bin file to be loaded
% actID    : id of actuator to plot (optional)
%
% Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
% Created: 04/05

% load the file
data = [];
[data,deltaT,desc,units,fileInfo,fileDate,header,isText] = loadbin(fileName);
time = deltaT*(0:size(data,1)-1)';

if nargin<2
   actID = 1;
end

dataID = [];
displCmdID = [];
displScrID = [];
displFbkID = [];
forceFbkID = [];
for i=1:length(actID)
   dataID = [dataID,find(str2num(desc(:,5))' == actID(i))];
end
for i=1:length(dataID)
   if strcmp(desc(dataID(i),7:15),'Displ cmd')
      displCmdID = [displCmdID,dataID(i)];
   end
   if strcmp(desc(dataID(i),7:15),'Displ scr')
      displScrID = [displScrID,dataID(i)];
   end
   if strcmp(desc(dataID(i),7:15),'Displ fbk')
      displFbkID = [displFbkID,dataID(i)];
   end
   if strcmp(desc(dataID(i),7:15),'Force fbk')
      forceFbkID = [forceFbkID,dataID(i)];
   end
end

% extract data vectors
displCmd = data(:,displCmdID);
displScr = data(:,displScrID);
displFbk = data(:,displFbkID);
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
plot(time,displCmd,'-b');
hold('on');
plot(time,displFbk,'-r');
grid('on');
xlabel('Time [sec]');
ylabel('Displacement [in.]');
title(['Displacements from STS: Actuator ',num2str(actID)]);
legend('commDsp','measDsp');
%==========================================================================
% fft of error between command and measured displacements
CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
error = displFbk - displCmd;
getFFT(error,deltaT,['Error between measured and command displacement: Actuator ',num2str(actID)]);
%set(gca,'YScale','log');
%==========================================================================
% command vs. measured displacements
CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
plot(displCmd,displFbk,'-b');
hold('on');
grid('on');
xlabel('Command Displacement [in.]');
ylabel('Measured Displacement [in.]');
title(['Displacements from STS: Actuator ',num2str(actID)]);
%==========================================================================
% fft of measured forces
CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
getFFT(forceFbk,deltaT,['Measured force: Actuator ',num2str(actID)]);
%set(gca,'YScale','log');
%==========================================================================
