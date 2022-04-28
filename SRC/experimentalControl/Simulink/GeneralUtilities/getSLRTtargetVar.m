function [data,errorMsg] = getSLRTtargetVar(fileName,tg,drive)
%GETSLRTTARGETVAR to retrieve the variables from the SLRT-Target disk
% [data,errorMsg] = getSLRTtargetVar(fileName,tg,drive)
%
% data     : output structure with following fields
%     .tg       : slrt object with target computer status
%     .fileName : cell array with names of retrieved files
%     .sigNames : cell array with signal names
%     .values   : array with signal values
% errorMsg : error message
% fileName : cell array with file names to retrieve
% tg       : Simulink real-time target object
% drive    : disk drive on target from which to retrieve variables
%
% Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
% Created: 01/18

% initialize waitbar
wbHandle = waitbar(0,'Retrieving variables from the SLRT-Target disk ......');

% select disk drive
if (nargin < 3)
    drive = 'H:';
end

% initialize target computer access through ftp object
if (nargin < 2)
    tg = slrt;
end
ftp = SimulinkRealTime.openFTP(tg);

% loop through variables and retrieve them
errorMsg = 0;
numVar = length(fileName);
for i=1:numVar
    errorFlag = 0;
    fName = [fileName{i},'.dat'];
    try
        % update the waitbar
        waitbar((i-1)/(numVar+1),wbHandle,['Reading "',fileName{i},'.dat" variable ......']);
        
        % get the file from target
        ftp.cd(drive);
        ftp.mget(fName);
        
        % convert from target file format and save
        temp = SimulinkRealTime.utils.getFileScopeData(fName);
        temp.fileName = fileName{i};
        save(fileName{i},'temp');
    catch errorMsg
        errorFlag = 1;
        disp(['Could not open or read "',fileName{i},'.dat" variable:']);
        disp(errorMsg);
    end
    
    % delete the file if download and conversion was successful
    if ~errorFlag
        try
            clear temp;
            delete(fName);
            ftp.delete(fName);
        catch errorMsg
            disp(['Could not delete "',fileName{i},'.dat" variable:']);
            disp(errorMsg);
        end
    end
    
    % update the waitbar
    waitbar(i/(numVar+1),wbHandle,['Reading "',fileName{i},'.dat" variable ......']);
end

% assemble the individual variables into the data structure
data = struct([]);
% update the waitbar
waitbar((i+1)/(numVar+1),wbHandle,'Assembling variables ......');
% add variables that we have downloaded from the target
for i=1:numVar
    % first save part of the tg variable
    data(i).app = tg.Application;
    data(i).cpuOverload = tg.CPUOverload;
    data(i).execTime = tg.ExecTime;
    data(i).sampleTime = tg.SampleTime;
    data(i).avgTET = tg.AvgTET;
    data(i).minTET = tg.MinTET;
    data(i).maxTET = tg.MaxTET;
    data(i).logTET = tg.TETlog;
    try
        load(fileName{i});
        data(i).fileName = temp.fileName;
        data(i).sigNames = temp.signalNames;
        data(i).values   = temp.data;
        clear temp;
    catch errorMsg
        disp(['Could not load "',fileName{i},'.mat" variable:']);
        disp(errorMsg);
    end
end

% close the waitbar
close(wbHandle);
