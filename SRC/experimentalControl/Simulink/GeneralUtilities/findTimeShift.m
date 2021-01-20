function [tsRMS,tsCC] = findTimeShift(t1,x1,t2,x2,varargin)
%FINDTIMESHIFT to find the time shift between two signals
% [tsRMS,tsCC] = findTimeShift(t1,x1,t2,x2,varargin)
%
% tsRMS    : time shift between two signals from RMS error
% tsCC     : time shift between two signals from cross-correlation
% t1       : time vector of first signal
% x1       : first signal vector
% t2       : time vector of second signal
% x2       : second signal vector
% varargin : variable input arguments
%     dtSearch : time increment over which to search for min rms
%
% Written: Andreas Schellenberg (andreas.schellenberg@gmail.com) 04/2005

% define default values
optArgs.dtSearch = 0.5;

% get optional input arguments
for i=1:2:length(varargin)
    optArgs.(varargin{i}) = varargin{i+1};
end

% get time steps
dt1 = mean(diff(t1));
dt2 = mean(diff(t2));

% interpolate signal with longer dt
if (dt2<=dt1)
    x1 = interp1(t1,x1,t2);
    id = ~isnan(x1);
elseif (dt2>dt1)
    x2 = interp1(t2,x2,t1);
    id = ~isnan(x2);
end
dt = min([dt1,dt2]);
x1 = x1(id);
x2 = x2(id);

% find time shift from cross-correlation
[c,lag] = xcorr(x1,x2);
[~,id] = max(abs(c));
tsCC = lag(id)*dt;

% find time shift from rms error
nshifts = round(optArgs.dtSearch/dt);
err = zeros(2*nshifts,1);
for i=1:nshifts
    err(i) = rms(x2(nshifts+1-i:end)-x1(1:end-nshifts+i));
    err(nshifts+i) = rms(x2(1:end-i)-x1(1+i:end));
end
[~,id] = min(err);
tsRMS = (id-nshifts)*dt;
