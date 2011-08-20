function [f,MX] = GetFFT(data,dt)
%GETFFT to get the scaled FFT and plot it if requested
% [f,MX] = GetFFT(data,dt)
%
% f    : frequency vector [Hz]
% MX   : scaled magnitude vector
% data : input data (column) vector
% dt   : sampling time [sec]
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%                          OpenFresco Express                          %%
%%    GUI for the Open Framework for Experimental Setup and Control     %%
%%                                                                      %%
%%   (C) Copyright 2011, The Pacific Earthquake Engineering Research    %%
%%            Center (PEER) & MTS Systems Corporation (MTS)             %%
%%                         All Rights Reserved.                         %%
%%                                                                      %%
%%     Commercial use of this program without express permission of     %%
%%                 PEER and MTS is strictly prohibited.                 %%
%%     See Help -> OpenFresco Express Disclaimer for information on     %%
%%   usage and redistribution, and for a DISCLAIMER OF ALL WARRANTIES.  %%
%%                                                                      %%
%%   Developed by:                                                      %%
%%     Andreas Schellenberg (andreas.schellenberg@gmail.com)            %%
%%     Carl Misra (carl.misra@gmail.com)                                %%
%%     Stephen A. Mahin (mahin@berkeley.edu)                            %%
%%                                                                      %%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% $Revision$
% $Date$
% $URL$

Fs = 1/dt;              % sampling frequency
Fn = Fs/2;              % Nyquist frequency
npts = size(data,1);    % number of points

% next highest power of 2 greater than or equal to length(x):
NFFT = 2.^(ceil(log(npts)/log(2)));

% take fft, padding with zeros, length(FFTX)==NFFT
FFTX = fft(data,NFFT);
NumUniquePts = ceil((NFFT+1)/2);

% fft is symmetric, throw away second half
FFTX = FFTX(1:NumUniquePts,:);
% take magnitude of X
MX = abs(FFTX);

% multiply by 2 to take into account the fact that we threw out second half of FFTX above
MX = MX*2;
% account for endpoint uniqueness
MX(1,:) = MX(1,:)./2;
% we know NFFT is even
MX(size(MX,1),:) = MX(size(MX,1),:)./2;
% scale the FFT so that it is not a function of the length of data
MX = MX/npts;

% frequency vector
f = 2*Fn/NFFT*(0:NumUniquePts-1)';
