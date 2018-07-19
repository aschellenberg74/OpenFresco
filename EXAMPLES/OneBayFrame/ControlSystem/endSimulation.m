function data = endSimulation(outputFile)
%ENDSIMULATION to save the data after a simulation
% endSimulation(outputFile)
%
% data                : Simulink.SimulationData.Dataset object
%     .get('')        : method to get variable
%     .get('').Values : array with data values
% outputFile          : output file the data is saved to
%
% implemented by Andreas Schellenberg 10/2015

close all;
clc;

if evalin('base','exist(''sigsOut'',''var'')')
    
    data = evalin('base','sigsOut');
    
    % save data structure to file
    save(outputFile,'data');
end

% plot output
plotOutputSimulink(outputFile);
