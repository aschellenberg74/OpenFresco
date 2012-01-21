clear;
close all;
clc;

% add OpenSees SRC folders and sub-folders to Matlab path
pathOPSsource = 'C:\Documents and Settings\Andreas\My Documents\OpenSees\SourceCode\SRC';
addpath(genpath(pathOPSsource));
savepath;
fprintf(1,'Added OpenSees SRC folders to path.\n\n');

% update OpenSees core files
pathOPSCore = 'C:\Documents and Settings\Andreas\My Documents\OpenFresco\trunk\SRC\openseesCore';
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
fprintf(1,'Finished updating OPS CORE files.\n\n');

% update OpenSees extra files
pathOPSExtra = 'C:\Documents and Settings\Andreas\My Documents\OpenFresco\trunk\SRC\openseesExtra';
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
fprintf(1,'Finished updating OPS EXTRA files.\n\n');

% remove SRC folders and sub-folders from Matlab path
currentPath = path;
id = [0 strfind(currentPath,pathsep)];
for i=1:length(id)-1
    pathString = currentPath(id(i)+1:id(i+1)-1);
    if strfind(pathString,'OpenSees\SourceCode\SRC')
        rmpath(pathString);
    end
end
fprintf(1,'Removed OpenSees SRC folders from path.\n');

