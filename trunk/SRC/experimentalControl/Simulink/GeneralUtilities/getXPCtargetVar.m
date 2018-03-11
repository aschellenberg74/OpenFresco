function [data,errorMsg] = getXPCtargetVar(fileName,ipAddress,ipPort)
%GETXPCTARGETVAR to retrieve the variables from the xPC-Target disk
% [data,errorMsg] = getXPCtargetVar(fileName,ipAddress,ipPort)
%
% data      : output structure with following fields
%     .fileName : names of retrieved files
%     .values   : array with data values
% errorMsg  : error message
% fileName  : cell array with file names to retrieve
% ipAddress : IP-Address of xPC-Target
% ipPort    : IP-Port of xPC-Target
%
% Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
% Created: 04/05
%
% $Revision$
% $Date$
% $URL$

% initialize waitbar
wbHandle = waitbar(0,'Retrieving variables from the xPC-Target disk ......');

% initialize target access
if (nargin < 3)
    ftp = xpctarget.ftp('TCPIP','192.168.2.20','22222');
    fs  = xpctarget.fs('TCPIP','192.168.2.20','22222');
else
    ftp = xpctarget.ftp('TCPIP',ipAddress,ipPort);
    fs  = xpctarget.fs('TCPIP',ipAddress,ipPort);
end

drive = 'C:';
errorMsg = 0;
numVar = length(fileName);
for i=1:numVar
    errorFlag = 0;
    try
        % update the waitbar
        waitbar((i-1)/(numVar+1),wbHandle,['Downloading "',fileName{i},'.dat" variable ......']);
        
        % get the file from target
        ftp.get(fullfile(drive,[fileName{i},'.dat']));
        
        % convert from target file format and save
        temp = readxpcfile([fileName{i},'.dat']);
        temp.fileName = fileName{i};
        save(fileName{i},'temp');
    catch errorMsg
        errorFlag = 1;
        disp(['Could not download "',fileName{i},'.dat" variable:']);
        disp(errorMsg);
    end
    
    % delete the file if download and conversion was successful
    if ~errorFlag
        try
            clear temp;
            delete([fileName{i},'.dat']);
            removefile(fs,fullfile(drive,[fileName{i},'.dat']));
        catch errorMsg
            disp(['Could not delete "',fileName{i},'.dat" variable:']);
            disp(errorMsg);
        end
    end
    
    % update the waitbar
    waitbar(i/(numVar+1),wbHandle,['Downloading "',fileName{i},'.dat" variable ......']);
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
