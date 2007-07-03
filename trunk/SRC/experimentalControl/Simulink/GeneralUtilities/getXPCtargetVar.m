function data = getXPCtargetVar(fileName,ipAddress,ipPort)
%GETXPCTARGETVAR to retrieve the variables from the xPC-Target disk
% data = getXPCtargetVar(fileName)
%
% data      : output structure with following fields
%     .fileName : names of retrieved files
%     .values   : array with data values
% fileName  : cell array with file names to retrieve
% ipAddress : IP-Address of xPC-Target
% ipPort    : IP-Port of xPC-Target
%
% Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
% Created: 04/05

% initialize waitbar
wbHandle = waitbar(0,'Retrieving variables from the xPC-Target disk ......');

% initialize xPC-Target access
if (nargin < 3)
   fsys = xpctarget.fs('TCPIP','192.168.2.20','22222');
else
   fsys = xpctarget.fs('TCPIP',ipAddress,ipPort);
end

data = [];
numVar = length(fileName);
for i=1:numVar
   errorFlag = 0;
   try
      % update the waitbar
      waitbar(i/numVar,wbHandle,['Downloading "',fileName{i},'" variable ......']);
      
      % open the file and get the file handle
      fileHandle = fopen(fsys,fileName{i});

      % read the file
      temp1 = fread(fsys,fileHandle);

      % close the file
      fclose(fsys,fileHandle);

      % convert from xPC-Target file format
      temp2 = readxpcfile(temp1);

      % save the fileName and the data values
      data(i).fileName = fileName{i};
      data(i).values   = temp2.data;
   catch
      errorFlag = 1;
      disp(['Could not download "',fileName{i},'" variable.']);
   end
   % delete the file if download was sucessful
   if ~errorFlag
      removefile(fsys,fileName{i});
   end
end

% close the waitbar
close(wbHandle);
