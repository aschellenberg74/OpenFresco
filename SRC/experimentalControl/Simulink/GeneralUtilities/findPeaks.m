function [pk,id] = findPeaks(x,varargin)
%FINDPEAKS to find the peaks in a signal
% [pk,id] = findPeaks(x,varargin)
%
% pk       : peak values
% id       : peak location indices
% x        : input signal
% varargin : variable input arguments
%     sortstr : order to sort ('descend', 'ascend')
%     npeaks  : number of peaks to return
%
% Written: Andreas Schellenberg (andreas.schellenberg@gmail.com) 02/2016

% define default values
optArgs.sortstr = 'descend';
optArgs.npeaks = [];  % return all peaks

% get optional input arguments
for i=1:2:length(varargin)
    optArgs.(varargin{i}) = varargin{i+1};
end

% reshape input into column vector
x = reshape(x,[],1);

% get derivative of signal and find peaks
xPrime = [1; diff(x); -1];
id = find(xPrime(1:end-1)>=0 & xPrime(2:end)<=0);
pk = x(id);

% sort peaks
[pk,id2] = sort(pk,optArgs.sortstr);
id = id(id2);

% return number of requested peaks
if ~isempty(optArgs.npeaks)
    npeaks = min(length(id),optArgs.npeaks);
    pk = pk(1:npeaks);
    id = id(1:npeaks);
end
