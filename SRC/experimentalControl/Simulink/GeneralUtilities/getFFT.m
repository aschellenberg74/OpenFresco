function [f,mag] = getFFT(x,dt,varargin)
%GETFFT to get the scaled FFT of a signal
% [f,mag] = getFFT(x,dt,varargin)
%
% f        : frequency vector [Hz]
% mag      : scaled magnitude vector
% x        : input signal
% dt       : sampling interval [sec]
% varargin : variable input arguments
%     title : title for plotting signal in time and frquency domain
%
% Written: Tony Yang (yangtony2004@gmail.com) 08/2003
% Modified: Andreas Schellenberg (andreas.schellenberg@gmail.com) 04/2005

% define default values
optArgs.title = [];

% get optional input arguments
for i=1:2:length(varargin)
    optArgs.(varargin{i}) = varargin{i+1};
end

% reshape input into column vector
x = reshape(x,[],1);

fs = 1/dt;           % sampling frequency [Hz]
fn = fs/2;           % Nyquist frequency [Hz]
npts = length(x);    % number of points
t = dt*(0:npts-1)';  % time vector

% next highest power of 2 greater than or equal to length(x)
nfft = 2^nextpow2(npts);

% take fft, padding with zeros, length(fftx)==nfft
fftx = fft(x,nfft);
numUniquePts = ceil((nfft+1)/2);

% fft is symmetric, throw away second half
fftx = fftx(1:numUniquePts,:);
% take magnitude of X
mag = abs(fftx);

% multiply by 2 to take into account the fact that we threw out second half of fftx above
mag = mag*2;
% account for endpoint uniqueness
mag(1,:) = mag(1,:)./2;
% we know nfft is even
mag(size(mag,1),:) = mag(size(mag,1),:)./2;
% scale the FFT so that it is not a function of the length of data
mag = mag/npts;

% frequency vector
f = 2*fn/nfft*(0:numUniquePts-1)';

% plot the results
if ~isempty(optArgs.title)
    subplot(2,1,1)
    plot(t,x,'-','LineWidth',1.5);
    hold('on');
    grid('on');
    xlabel('Time [sec]','FontWeight','bold');
    ylabel('P(time)','FontWeight','bold');
    title([optArgs.title,': Time domain'],'FontWeight','bold');
    subplot(2,1,2)
    plot(f,mag,'-','LineWidth',1.5);
    hold('on');
    grid('on');
    xlabel('Frequency [Hz]','FontWeight','bold');
    ylabel('P(freq)','FontWeight','bold');
    title([optArgs.title,': Frequency domain'],'FontWeight','bold');
end
