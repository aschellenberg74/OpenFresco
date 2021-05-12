function data = endSimulationXPCtarget(outputFile)
%ENDSIMULATIONXPCTARGET to stop xPC-Target and download the data
% endSimulationXPCtarget(outputFile)
%
% data       : output structure with following fields
%     .fileName : names of retrieved files
%     .values   : array with data values
% outputFile : output file the data is saved to
%
% Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
% Created: 11/04

close all;
clc;

% connect to target and stop model
try %#ok<TRYNC>
    tg = slrt;
    tg.stop;
end

% get the variables saved on the target
data = getSLRTtargetVar({'targSig','commSig','measSig', ...
    'state','count','flag','measDsp','measFrc'}, ...
    tg);

% save data structure to file
save(outputFile,'data');

% disconnect from target
close(slrt);

% plot output
plotOutputXPCtarget(outputFile);
