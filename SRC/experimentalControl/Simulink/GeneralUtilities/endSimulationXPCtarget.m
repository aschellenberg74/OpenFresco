function data = endSimulationXPCtarget(outputFile)
%ENDSIMULATIONXPCTARGET to stop xPC-Target, retrieve and plot variables
% data = endSimulationXPCtarget(outputFile)
%
% data       : output structure with following fields
%     .fileName : names of retrieved files
%     .values   : array with data values
% outputFile : output file the data is saved to
%
%
% Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
% Created: 04/05

close all;
clc;

% stop xPCtarget model
tg = xpc;
tg.stop;

% get the variables saved on the xPC Target
data = getXPCtargetVar({'targDsp.dat','commDsp.dat','measDsp.dat', ...
   'state.dat','count.dat','flag.dat','measFrc.dat'});

% save workspace
save(outputFile,'data');

% plot output
plotOutputXPCtarget(outputFile);

close(xpc);
