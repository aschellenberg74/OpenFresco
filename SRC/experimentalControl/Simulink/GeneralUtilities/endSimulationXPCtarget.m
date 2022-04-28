function data = endSimulationXPCtarget(outputFile)
%ENDSIMULATIONXPCTARGET to stop xPC-Target and download the data
% data = endSimulationXPCtarget(outputFile)
%
% data : output structure with following fields
%     .tg       : slrt object with target computer status
%     .fileName : cell array with names of retrieved files
%     .sigNames : cell array with signal names
%     .values   : array with signal values
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
data = getSLRTtargetVar({'xpcRec'},tg);

% save data structure to file
save(outputFile,'data');

% disconnect from target
close(slrt);

% plot output
plotOutputXPCtarget(outputFile);
