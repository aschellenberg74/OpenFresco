clear;
close all;
clc;

pathOPS = uigetdir('C:\Program Files\OpenSees', 'Select OpenSees base directory');
if ~exist(fullfile(pathOPS,'SRC'),'dir')
    error('Wrong OpenSees base directory specified');
end
pathOPF = uigetdir('C:\Program Files\OpenFresco', 'Select OpenFresco base directory');
if ~exist(fullfile(pathOPF,'SRC'),'dir')
    error('Wrong OpenFresco base directory specified');
end

% =========================================================================

% add OpenSees SRC folders and sub-folders to Matlab path
pathOPSsource = fullfile(pathOPS,'SRC');
addpath(genpath(pathOPSsource));
savepath;
fprintf(1,'Added OpenSees SRC folders to path.\n\n');

% update OpenSees core files
pathOPSCore = fullfile(pathOPF,'SRC','openseesCore');
d = dir(pathOPSCore);
fileNames = {d.name};
fileNames = fileNames(~[d.isdir])';

for i=1:length(fileNames)
    src = which(fileNames{i});
    if ~isempty(src)
        trg = fullfile(pathOPSCore,fileNames{i});
        if (~copyfile(src,trg,'f'))
           fprintf(1,'WARNING: Could not copy %s file!\n',fileNames{i});
        end
    else
        fprintf(1,'WARNING: Could not find %s file!\n',fileNames{i});
    end
end
fprintf(1,'Finished updating openseesCore files in OpenFresco.\n\n');

% update OpenSees extra files
pathOPSExtra = fullfile(pathOPF,'SRC','openseesExtra');
d = dir(pathOPSExtra);
fileNames = {d.name};
fileNames = fileNames(~[d.isdir])';

for i=1:length(fileNames)
    src = which(fileNames{i});
    if ~isempty(src)
        trg = fullfile(pathOPSExtra,fileNames{i});
        if (~copyfile(src,trg,'f'))
           fprintf(1,'WARNING: Could not copy %s file!\n',fileNames{i});
        end
    else
        fprintf(1,'WARNING: Could not find %s file!\n',fileNames{i});
    end
end
fprintf(1,'Finished updating openseesExtra files in OpenFresco.\n\n');

% remove SRC folders and sub-folders from Matlab path
currentPath = path;
id = [0 strfind(currentPath,pathsep)];
for i=1:length(id)-1
    pathString = currentPath(id(i)+1:id(i+1)-1);
    if contains(pathString,fullfile(pathOPS,'SRC'))
        rmpath(pathString);
    end
end
fprintf(1,'Removed OpenSees SRC folders from path.\n\n');

% =========================================================================

% update OpenSees 32-bit library files (base folder)
fprintf(1,'Added OpenSees 32-bit LIB folders to path.\n\n');
pathOPSlib = fullfile(pathOPS,'Win32','lib');
addpath(pathOPSlib);
savepath;
pathOPFlib = fullfile(pathOPF,'WIN32','lib');
d = dir(pathOPFlib);
fileNames = {d.name};
fileNames = fileNames(~[d.isdir])';
for i=1:length(fileNames)
    src = which(fileNames{i});
    if ~isempty(src)
        trg = fullfile(pathOPFlib,fileNames{i});
        if (~copyfile(src,trg,'f'))
           fprintf(1,'WARNING: Could not copy %s file!\n',fileNames{i});
        end
    else
        fprintf(1,'WARNING: Could not find %s file!\n',fileNames{i});
    end
end
rmpath(pathOPSlib);

% update OpenSees 32-bit library files (debug folder)
pathOPSlib = fullfile(pathOPS,'Win32','lib','debug');
addpath(pathOPSlib);
savepath;
pathOPFlib = fullfile(pathOPF,'WIN32','lib','debug');
d = dir(pathOPFlib);
fileNames = {d.name};
fileNames = fileNames(~[d.isdir])';
for i=1:length(fileNames)
    src = which(fileNames{i});
    if ~isempty(src)
        trg = fullfile(pathOPFlib,fileNames{i});
        if (~copyfile(src,trg,'f'))
           fprintf(1,'WARNING: Could not copy %s file!\n',fileNames{i});
        end
    else
        fprintf(1,'WARNING: Could not find %s file!\n',fileNames{i});
    end
end
rmpath(pathOPSlib);

% update OpenSees 32-bit library files (release folder)
pathOPSlib = fullfile(pathOPS,'Win32','lib','release');
addpath(pathOPSlib);
savepath;
pathOPFlib = fullfile(pathOPF,'WIN32','lib','release');
d = dir(pathOPFlib);
fileNames = {d.name};
fileNames = fileNames(~[d.isdir])';
for i=1:length(fileNames)
    src = which(fileNames{i});
    if ~isempty(src)
        trg = fullfile(pathOPFlib,fileNames{i});
        if (~copyfile(src,trg,'f'))
           fprintf(1,'WARNING: Could not copy %s file!\n',fileNames{i});
        end
    else
        fprintf(1,'WARNING: Could not find %s file!\n',fileNames{i});
    end
end
rmpath(pathOPSlib);
savepath;
fprintf(1,'Finished updating OpenSees 32-bit LIB files in OpenFresco.\n\n');
fprintf(1,'Removed OpenSees 32-bit LIB folders from path.\n\n');

% =========================================================================

% update OpenSees 64-bit library files (base folder)
fprintf(1,'Added OpenSees 64-bit LIB folders to path.\n\n');
pathOPSlib = fullfile(pathOPS,'Win64','lib');
addpath(pathOPSlib);
savepath;
pathOPFlib = fullfile(pathOPF,'WIN64','lib');
d = dir(pathOPFlib);
fileNames = {d.name};
fileNames = fileNames(~[d.isdir])';
for i=1:length(fileNames)
    src = which(fileNames{i});
    if ~isempty(src)
        trg = fullfile(pathOPFlib,fileNames{i});
        if (~copyfile(src,trg,'f'))
           fprintf(1,'WARNING: Could not copy %s file!\n',fileNames{i});
        end
    else
        fprintf(1,'WARNING: Could not find %s file!\n',fileNames{i});
    end
end
rmpath(pathOPSlib);

% update OpenSees 64-bit library files (debug folder)
pathOPSlib = fullfile(pathOPS,'Win64','lib','debug');
addpath(pathOPSlib);
savepath;
pathOPFlib = fullfile(pathOPF,'WIN64','lib','debug');
d = dir(pathOPFlib);
fileNames = {d.name};
fileNames = fileNames(~[d.isdir])';
for i=1:length(fileNames)
    src = which(fileNames{i});
    if ~isempty(src)
        trg = fullfile(pathOPFlib,fileNames{i});
        if (~copyfile(src,trg,'f'))
           fprintf(1,'WARNING: Could not copy %s file!\n',fileNames{i});
        end
    else
        fprintf(1,'WARNING: Could not find %s file!\n',fileNames{i});
    end
end
rmpath(pathOPSlib);

% update OpenSees 64-bit library files (release folder)
pathOPSlib = fullfile(pathOPS,'Win64','lib','release');
addpath(pathOPSlib);
savepath;
pathOPFlib = fullfile(pathOPF,'WIN64','lib','release');
d = dir(pathOPFlib);
fileNames = {d.name};
fileNames = fileNames(~[d.isdir])';
for i=1:length(fileNames)
    src = which(fileNames{i});
    if ~isempty(src)
        trg = fullfile(pathOPFlib,fileNames{i});
        if (~copyfile(src,trg,'f'))
           fprintf(1,'WARNING: Could not copy %s file!\n',fileNames{i});
        end
    else
        fprintf(1,'WARNING: Could not find %s file!\n',fileNames{i});
    end
end
rmpath(pathOPSlib);
savepath;
fprintf(1,'Finished updating OpenSees 64-bit LIB files in OpenFresco.\n\n');
fprintf(1,'Removed OpenSees 64-bit LIB folders from path.\n\n');

% =========================================================================
