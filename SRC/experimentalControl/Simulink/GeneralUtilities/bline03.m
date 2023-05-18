function [acc,vel,dsp,time] = bline03(acc,dt,polyOrder,varargin)
%BLINE03 to perform time domain baseline correction
% [acc,vel,dsp,time] = bline03(acc,dt,polyOrder,varargin)
%
% acc       : baseline corrected ground acceleration history
% vel       : baseline corrected ground velocity history
% dsp       : baseline corrected ground displacement history
% time      : baseline corrected time vector
% acc       : input ground acceleration history
% dt        : time step size of input ground acceleration history
% polyOrder : order of polynomial for baseline correction counted from
%             the cubic term (usally between 3 and 9)
% varargin  : variable input arguments
%     padEnd   : percent time to add to the end of the displacement
%                history to improve the baseline function fit (default 10%)
%     taperEnd : percent taper to apply to the end of the displacement
%                history (default 5%)
%     plotFlag : to plot the baseline polynomial and corrected histories
%
%
% Written by   : Andreas Schellenberg (andreas.schellenberg@gmail.com)
% Adapted from : Bline03.for code developed by Norm Abrahamson

% define default values
optArgs.padEnd   = 10;
optArgs.taperEnd = 5;
optArgs.plotFlag = 0;

% get optional input arguments
for i=1:2:length(varargin)
    optArgs.(varargin{i}) = varargin{i+1};
end

% reshape input into column vector
acc = reshape(acc,[],1);

% remove DC of initial part of record and taper beginning of record
npflag = 1;  % don't run this!!!
if (npflag == 0)
    % remove mean value of first 10 points of record
    npts = 10;
    acc = acc - sum(acc(1:npts))/length(acc);
    
    % taper beginning of record over first 10 points
    npts = 10;
    taper = 0.5*(1 + cos(pi*(0:npts-1)./npts + pi));
    acc(1:npts) = acc(1:npts).*taper';
end

% integrate to displacement (time domain) for fitting baseline
time = dt*(0:length(acc)-1)';
vel  = dt*cumtrapz(acc);
dsp  = dt*cumtrapz(vel);

% plot original record
if (optArgs.plotFlag > 0)
    hFig = figure;
    set(hFig,'Position',[1 31 1600 794]);
    subplot(2,3,1);
    plot(time,acc,'b');
    hold on;
    grid on;
    subplot(2,3,2);
    plot(time,vel,'b');
    hold on;
    grid on;
    subplot(2,3,3);
    plot(time,dsp,'b');
    hold on;
    grid on;
end

% pad displacement for more stable baseline at end of trace
npts = ceil(optArgs.padEnd/100*length(dsp));
dspTmp = [dsp; repmat(dsp(end),npts,1)];
tTmp = (0:length(dspTmp)-1)'*dt;

% fit an n-th order polynomial
% construct Vandermonde matrix
V(:,polyOrder) = tTmp.^2.*ones(length(tTmp),1,class(tTmp));
for i = polyOrder-1:-1:1
    V(:,i) = tTmp.*V(:,i+1);
end
% solve least squares problem
[Q,R] = qr(V,0);
ws = warning('off','all');
coef = R\(Q'*dspTmp);
warning(ws);
% get all polynomial coefficients
coef = [coef' 0 0];
coefPrime = polyder(coef);
coefPrime2 = polyder(coefPrime);

% compute baseline for uncorrected displacement trace
b = polyval(coef,time);
bPrime = polyval(coefPrime,time);
bPrime2 = polyval(coefPrime2,time);

% plot baseline correction polynomial
if (optArgs.plotFlag > 1)
    figure(hFig);
    subplot(2,3,1);
    plot(time,bPrime2,'r');
    subplot(2,3,2);
    plot(time,bPrime,'r');
    subplot(2,3,3);
    plot(time,b,'r');
end

% set taper (cosine bell)
npts = ceil(optArgs.taperEnd/100*length(acc));
w = 0.5*(cos(pi*(0:npts-1)'./npts) + 1);
wPrime = -0.5*pi/(npts*dt)*sin(pi*(0:npts-1)'./npts);
wPrime2 = -0.5*(pi/(npts*dt))^2*cos(pi*(0:npts-1)'./npts);

id = 1:length(acc)-npts;
acc(id) = acc(id) - bPrime2(id);
id = length(acc)-npts+1:length(acc);
acc(id) = (acc(id) - bPrime2(id)).*w ...
        + (vel(id) - bPrime(id)).*wPrime.*2.0 ...
        + (dsp(id) - b(id)).*wPrime2;

% integrate corrected acc to vel and displacement
vel = dt*cumtrapz(acc);
dsp = dt*cumtrapz(vel);

% plot corrected record
if (optArgs.plotFlag > 2)
    figure(hFig);
    subplot(2,3,4);
    plot(time,acc,'b');
    hold on;
    grid on;
    subplot(2,3,5);
    plot(time,vel,'b');
    hold on;
    grid on;
    subplot(2,3,6);
    plot(time,dsp,'b');
    hold on;
    grid on;
    pause;
    close(hFig);
end
