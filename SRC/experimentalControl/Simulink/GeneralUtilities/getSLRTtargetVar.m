function [data,errorMsg] = getSLRTtargetVar(fileName,tg,drive)
%GETSLRTTARGETVAR to retrieve the variables from the SLRT-Target disk
% [data,errorMsg] = getSLRTtargetVar(fileName,tg,drive)
%
% data     : output structure with following fields
%     .fileName : names of retrieved files
%     .values   : array with data values
% errorMsg : error message
% fileName : cell array with file names to retrieve
% tg       : Simulink real-time target object
% drive    : disk drive on target from which to retrieve variables
%
% Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
% Created: 01/18
%
% $Revision$
% $Date$
% $URL$

% initialize waitbar
wbHandle = waitbar(0,'Retrieving variables from the SLRT-Target disk ......');

% select disk drive
if (nargin < 3)
    drive = 'C:';
end

% initialize target access through target file system object
if (nargin < 2)
    tg = SimulinkRealTime.target('TargetPC1');
end
fs = SimulinkRealTime.fileSystem(tg);

errorMsg = 0;
numVar = length(fileName);
for i=1:numVar
    errorFlag = 0;
    try
        % update the waitbar
        waitbar((i-1)/(numVar+1),wbHandle,['Reading "',fileName{i},'.dat" variable ......']);
        
        % open the file on target and read it
        fid = fs.fopen(fullfile(drive,[fileName{i},'.dat']), 'r');
        temp = fs.fread(fid);
        fs.fclose(fid);
        
        % convert from target file format and save
        temp = SimulinkRealTime.utils.getFileScopeData(temp);
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
            fs.removefile(fullfile(drive,[fileName{i},'.dat']));
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
for i=1:numVar
    try
        load(fileName{i});
        data(i).fileName = temp.fileName;
        data(i).values   = temp.data;
        clear temp;
    catch errorMsg
        disp(['Could not load "',fileName{i},'.mat" variable:']);
        disp(errorMsg);
    end
end

% close the waitbar
close(wbHandle);
