function [dsp,vel,acc,time] = feedbackGenerator(dsp,acc,dt,fco1,fco2,varargin)
%FEEDBACKGENERATOR to get disp, vel, and accel feedback
% [dsp,vel,acc,time] = feedbackGenerator(dsp,acc,dt,fco,varargin)
%
% zeta     : damping ratio
% period   : period
% x        : input signal
% varargin : variable input arguments
%     ncycles : number of cycles to use for calculation
%
% Written: Andreas Schellenberg (andreas.schellenberg@gmail.com) 09/2019

% define default values
optArgs.fhp = 0.0;
optArgs.flp = inf;

% get optional input arguments
for i=1:2:length(varargin)
    optArgs.(varargin{i}) = varargin{i+1};
end

if isempty(dsp) && ~isempty(acc)
    dsp = zeros(length(acc),1);
elseif ~isempty(dsp) && isempty(acc)
    acc = zeros(length(dsp),1);
elseif isempty(dsp) && isempty(acc)
    error('');
end
time = dt*(0:length(dsp)-1)';

% filter and then differentiate displacement signal
filter = [optArgs.fhp optArgs.fhp fco1 fco2];
dspA = bandpassFilter(dsp,dt,filter);
velA = 1/dt*[diff(dspA);0.0];
accA = 1/dt*[diff(velA);0.0];

% filter and then integrate acceleration signal
polyOrder = 1;
filter = [fco1 fco2 optArgs.flp optArgs.flp];
accB = bandpassFilter(acc,dt,filter);
%[dspB,velB,accB] = baselineCorrect(accB,dt,polyOrder,'plotFlag',3);
velB = dt*cumtrapz(accB);
dspB = dt*cumtrapz(velB);

% add signals
dsp = dspA + dspB;
vel = velA + velB;
acc = accA + accB;

