clear all;

% setup connection
socketID = TCPSocket('openConnection',8090);
if (socketID<0)
   errordlg('Unable to setup connection.');
   return;
end

% set the data sizes for the experimental site
dataSize = 2;
sData = zeros(1,dataSize);
dataSizes = int32([1 0 0 0 0, 0 0 0 1 0, dataSize]);
TCPSocket('sendData',socketID,dataSizes,11);

% send trial response to experimental site
sData(1) = 3;
sData(2) = -1.00123;
TCPSocket('sendData',socketID,sData,dataSize);

% get measured resisting forces
sData(1) = 10;
TCPSocket('sendData',socketID,sData,dataSize);
rData = TCPSocket('recvData',socketID,dataSize);
q = rData(1)

% disconnect from experimental site
sData(1) = 99;
TCPSocket('sendData',socketID,sData,dataSize);
TCPSocket('closeConnection',socketID);
