function [zeta,period] = findDamping(x,varargin)
%FINDDAMPING to find the damping ratio and period of a signal
% [zeta,T] = findDamping(x,varargin)
%
% zeta     : damping ratio
% period   : period
% x        : input signal
% varargin : variable input arguments
%     ncycles : number of cycles to use for calculation
%
% Written: Andreas Schellenberg (andreas.schellenberg@gmail.com) 09/2019

% define default values
optArgs.ncycles = 1;

% get optional input arguments
for i=1:2:length(varargin)
    optArgs.(varargin{i}) = varargin{i+1};
end

% find first N+1 peaks in data series
[pks,locs] = findPeaks(x,'npeaks',optArgs.ncycles+1);

% compute period
period = min(diff(sort(locs)));

% compute damping ratio
a = log(pks(1)/pks(end));
zeta = sqrt(a^2/((2*pi*optArgs.ncycles)^2 - a^2));
