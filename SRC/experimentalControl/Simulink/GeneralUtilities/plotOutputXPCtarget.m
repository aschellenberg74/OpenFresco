function data = plotOutputXPCtarget(fileName,actID,iDelay,tStart)
%PLOTOUTPUTXPCTARGET to plot the xPC-Target output from a hybrid simulation
% data = plotOutputXPCtarget(fileName,actID,iDelay,tStart)
%
% data     : data structure
% fileName : .mat file to be loaded
% actID    : id of actuator to plot (optional)
% tStart   : time to start plotting (optional)
%
% Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
% Created: 04/05

if (nargin<2)
   actID = 1;
   iDelay = 0;
   id = 1024;
elseif (nargin<3)
   iDelay = 0;
   id = 1024;
elseif (nargin<4)
   id = 1024;   
else
   id = tStart*1024;
end

% load the file and extract data
data = [];
%iDelay = 93;
%iDelay = 118;
load(fileName);
targDsp = data(1).values;
commDsp = data(2).values;
measDsp = data(3).values;
state   = data(4).values;
counter = data(5).values;
flag    = data(6).values;
measFrc = data(7).values;

% get screen size
SS = get(0,'screensize');

%==========================================================================
% command displacements
CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
try
   plot(commDsp(id:end,end),commDsp(id:end,actID),'-');
end
grid('on');
xlabel('Time [sec]');
ylabel('Command Displacement [in.]');
title('Command Displacement from xPC-Target');
%==========================================================================
% target, command and measured displacements
CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
try
   plot(targDsp(id:end,end),targDsp(id:end,actID),'-b');
end
hold('on');
try
   plot(commDsp(id:end,end),commDsp(id:end,actID),'-r');
end
try
   plot(measDsp(id:end,end),measDsp(id:end,actID),'-g');
end
% try
%    plot(state(id:end,end),state(id:end,1),'-k');
% end
grid('on');
xlabel('Time [sec]');
ylabel('Displacement [in.]');
title('Displacements from xPC-Target');
legend('targDsp','commDsp','measDsp');
%==========================================================================
% error between measured and target displacements
CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
try
   if (iDelay==0)
      N = max(counter(:,1));
      targID = find(counter(:,1) == N);
   else      
      targID = find(counter(:,1) >= 1+iDelay & counter(:,1) < 2+iDelay);
   end
   tID = find(targID >= id);
   targID = targID(tID);
   error = measDsp(targID,actID) - targDsp(targID,actID);
   plot(measDsp(targID,end),targDsp(targID,actID),'-b');
   hold('on');
   plot(measDsp(targID,end),measDsp(targID,actID),'-r');
   plot(measDsp(targID,end),error,'-g');
end
grid('on');
xlabel('Time [sec]');
ylabel('Displacements [in.]');
title('Displacement Errors from xPC-Target');
legend('targDsp','measDsp','error');
%==========================================================================
% fft of error
CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
try
   N = max(counter(:,1));
   if (iDelay==0)
      targID = find(counter(:,1) == N);
   else      
      targID = find(counter(:,1) >= 1+iDelay & counter(:,1) < 2+iDelay);
   end
   tID = find(targID >= id);
   targID = targID(tID);
   error = measDsp(targID,actID) - targDsp(targID,actID);
   dt = N/1024;
   getFFT(error,dt,'Error between Measured and Target Displacement');
end
%==========================================================================
% measured vs. target displacements
CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
try
   if (iDelay==0)
      N = max(counter(:,1));
      targID = find(counter(:,1) == N);
   else      
      targID = find(counter(:,1) >= 1+iDelay & counter(:,1) < 2+iDelay);
   end
   tID = find(targID >= id);
   targID = targID(tID);
   plot(targDsp(targID,actID),measDsp(targID,actID),'-b');
   hold('on');
end
grid('on');
xlabel('Target Displacement [in.]');
ylabel('Measured Displacement [in.]');
title('Displacements from xPC-Target');
%==========================================================================
% measured force
CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
try
   plot(measFrc(id:end,end),measFrc(id:end,actID),'-');
end
grid('on');
xlabel('Time [sec]');
ylabel('Measured Force [kip]');
title('Measured Force from xPC-Target');
%==========================================================================
% fft of measured force
CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
try
   dt = 1/1024;
   getFFT(measFrc(id:end,actID),dt,'Measured Force');
   set(gca,'YScale','log');
end
%==========================================================================
% state of predictor-corrector
CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
try
   plot(state(id:end,end),state(id:end,1),'-b');
end
grid('on');
xlabel('Time [sec]');
ylabel('State [-]');
title('State from xPC-Target');
%==========================================================================
% counter of predictor-corrector
CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
try
   plot(counter(id:end,end),counter(id:end,1),'-b');
end
grid('on');
xlabel('Time [sec]');
ylabel('Counter [-]');
title('Counter from xPC-Target');
%==========================================================================
% update and target flags of predictor-corrector
CreateWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
try
   plot(flag(id:end,end),flag(id:end,1),'-b');
end
hold('on');
try
   plot(flag(id:end,end),flag(id:end,2),'-r');
end
try
   plot(flag(id:end,end),flag(id:end,3),'-g');
end
grid('on');
xlabel('Time [sec]');
ylabel('Flag [-]');
title('Flags from xPC-Target');
legend('newTarget','switchPC','atTarget');
%==========================================================================
