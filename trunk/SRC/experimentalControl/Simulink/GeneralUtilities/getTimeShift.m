function [timeShiftRMS,timeShiftCC] = getTimeShift(t1,y1,t2,y2,dtSearch)
%GETTIMESHIFT to find the time shift between two signals
% timeShift = getTimeShift(t1,y1,t2,y2,dtSearch)
%
% timeShift : time shift between two signals
% t1        : time vector of first signal
% y1        : first signal vector
% t2        : time vector of second signal
% y2        : second signal vector
% dtSearch  : time increment over which to search for min rms (optional)
%
% Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
% Created: 04/05
%
% $Revision: $
% $Date: $
% $URL: $

if (nargin < 5)
    dtSearch = 0.5;
end

% get time steps
dt1 = t1(2) - t1(1);
dt2 = t2(2) - t2(1);

% interpolate signal with longer dt
if (dt2<=dt1)
   y1 = interp1(t1,y1,t2);
   id = ~isnan(y1);
elseif (dt2>dt1)
   y2 = interp1(t2,y2,t1);
   id = ~isnan(y2);
end
dt = min([dt1,dt2]);
y1 = y1(id);
y2 = y2(id);

% find time shift from cross-correlation
[c,lag] = xcorr(y1,y2);
[~,id] = max(abs(c));
timeShiftCC = lag(id)*dt;

% find time shift from rms error
nshifts = round(dtSearch/dt);
err = zeros(2*nshifts,1);
for i=1:nshifts
    err(i) = rms(y2(nshifts+1-i:end)-y1(1:end-nshifts+i));
    err(nshifts+i) = rms(y2(1:end-i)-y1(1+i:end));
end
[~,id] = min(err);
timeShiftRMS = (id-nshifts)*dt;

