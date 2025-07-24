% use with ..\SRC\simApplicationClient\simulink\Test_TCPSocket_Server.slx
% or ..\SRC\simApplicationClient\simulink\Test_UDPSocket_Server.slx

%#ok<*CLALL>
%#ok<*FVAL>

clear all;

socketType = 'UDPSocket';  % 'TCPSocket' or 'UDPSocket'

% setup connection to server
socketID1 = feval(socketType,'openConnection','127.0.0.1',8090);
if (socketID1<0)
   errordlg('Unable to setup connection ID1.');
   return;
end
socketID2 = feval(socketType,'openConnection','127.0.0.1',8091);
if (socketID2<0)
   errordlg('Unable to setup connection ID2.');
   return;
end

% set the parameters
dt = 0.001;
tEnd = 100;
dataSize = 1;
sData = zeros(1,dataSize);
t = 0:dt:tEnd;
omega = pi;
amp = 10;

d = zeros(length(t),1);
for i=1:length(t)
   % send sine wave
   sData(1) = amp*sin(omega*t(i));
   feval(socketType,'sendData',socketID1,sData,dataSize);
   
   % receive modified (x2) sine wave
   rData = feval(socketType,'recvData',socketID2,dataSize);
   d(i,1) = rData(1);
end

% disconnect
feval(socketType,'closeConnection',socketID1);
feval(socketType,'closeConnection',socketID2);

figure;
plot(t,d,'b-');
grid('on');
