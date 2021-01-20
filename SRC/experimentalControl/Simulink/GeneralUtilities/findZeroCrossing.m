function [zc,id] = findZeroCrossing(x,varargin)
%FINDZEROCROSSING to find the zero-crossings in a signal
% [zc,id] = findZeroCrossing(x,varargin)
%
% zc       : zero-crossing values
% id       : zero-crossing location indices
% x        : input signal
% varargin : variable input arguments
%     nzc : number of zero-crossings to return
%
% Written: Andreas Schellenberg (andreas.schellenberg@gmail.com) 09/2019

% define default values
optArgs.nzc = [];  % return all zero-crossings

% get optional input arguments
for i=1:2:length(varargin)
    optArgs.(varargin{i}) = varargin{i+1};
end

% reshape input into column vector
x = reshape(x,[],1);

% find where signal changes sign
id = find(abs(diff(sign(x)))==2);
zc = 0.5*(x(id)+x(id+1));

% return number of requested zero-crossings
if ~isempty(optArgs.nzc)
    nzc = min(length(id),optArgs.nzc);
    zc = zc(1:nzc);
    id = id(1:nzc);
end
