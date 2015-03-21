function data = endSimulationXPCtarget(outputFile)
%ENDSIMULATIONXPCTARGET to stop xPC-Target and download the data
% endSimulationXPCtarget(outputFile)
%
% data       : output structure with following fields
%     .fileName : names of retrieved files
%     .values   : array with data values
% outputFile : output file the data is saved to
%
% implemented by Andreas Schellenberg 11/2004

close all;
clc;

% connect to target and stop model
try %#ok<TRYNC>
    tg = xpc;
    tg.stop;
end

% get the variables saved on the target
data = getXPCtargetVar({'targSig','commSig','measSig', ...
    'state','count','flag','measDsp','measFrc'}, ...
    '192.168.2.20','22222');

% save data structure to file
save(outputFile,'data');

% disconnect from target
close(xpc);

% plot output
plotOutputXPCtarget(outputFile);
