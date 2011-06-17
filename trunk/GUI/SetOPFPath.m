%SETOPFPATH to add OpenFresco GUI folders to Matlab search path
% SetOPFPath
%
% Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
% Created: 09/10
% Revision: A
%
% $Revision$
% $Date$
% $URL$

% clean start
clear all; close all; clc;

% get path to OPF directory
pathOPF = pwd;
pathOPF = pathOPF(1:end-4);

folderList = { ...
   fullfile('WIN32','bin') ...
   fullfile('GUI') ...
   fullfile('GUI','Buttons') ...
   }';

for i=length(folderList):-1:1
   addpath(fullfile(pathOPF,folderList{i}));
end
savepath;
