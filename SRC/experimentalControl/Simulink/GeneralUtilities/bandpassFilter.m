function y = bandpassFilter(x,dt,filter)
%BANDPASSFILTER to bandpass filter a signal in the frequency domain
% y = bandpassFilter(x,dt,filter)
%
% y      : filtered output signal
% x      : input signal
% dt     : time increment
% filter : [fhcut,fhcor,flcor,flcut]
%          fhcut : high pass filter cutoff [Hz]
%          fhcor : high pass filter corner [Hz]
%          flcor : low pass filter corner [Hz]
%          flcut : low pass filter cutoff [Hz]

% convert to frequency domain
npts = length(x);
nfft = 2^nextpow2(npts);
fd = fft(x,nfft);

% Nyquist frequency [Hz]
fNyq = 1/(2*dt);

% extract filter parameters
fhcut = min(max(min(filter(1:2)),0.0),fNyq);
fhcor = min(max(max(filter(1:2)),0.0),fNyq);
flcor = min(min(filter(3:4)),fNyq);
flcut = min(max(filter(3:4)),fNyq);

% apply highpass filter
nfcut = ceil(fhcut*dt*nfft);
nfcor = ceil(fhcor*dt*nfft);
flat = nfft - 2*nfcor;
tran = nfcor - nfcut;
if (tran==0)
   flt = [zeros(nfcut,1);ones(flat,1);zeros(nfcut,1)];
else
   flt = [zeros(nfcut,1);linspace(0,1,tran)';ones(flat,1);linspace(1,0,tran)';zeros(nfcut,1)];
end
fd = fd.*flt;

% apply lowpass filter
nfcor = ceil(flcor*dt*nfft);
nfcut = ceil(flcut*dt*nfft);
flat = nfft - 2*nfcut;
tran = nfcut - nfcor;
if (tran==0)
   flt = [ones(nfcor,1);zeros(flat,1);ones(nfcor,1)];
else
   flt = [ones(nfcor,1);linspace(1,0,tran)';zeros(flat,1);linspace(0,1,tran)';ones(nfcor,1)];
end
fd = fd.*flt;

% convert back to time domain
y = ifft(fd);
y = real(y(1:npts));
