function fileList = GetFileList(fileExt,searchDir)
%GETFILELIST find all the files in the given folder and its subfolders
% that are on the path and have extension fileExt
% fileList = GetFileList(fileExt,searchDir)
%
% fileList  : list of all the files with extension fileExt
% fileExt   : file extension. Example fileExt = '.m', '.txt', ...
% searchDir : full path of directory to search
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

p = path;
semiCol = strfind(p,pathsep);
semiCol = [0,semiCol];
j = 0;
folderList = cell(0,0);
for i=1:length(semiCol)-1
   pathName = p(semiCol(i)+1:semiCol(i+1)-1);
   if ~isempty(strfind(pathName,searchDir))
      j = j+1;
      folderList{j,1} = pathName;
   end   
end

% find the files with specified extension
k = 0;
fileList = cell(0,0);
for i=1:length(folderList)
   cd(folderList{i})
   d = dir;
   for j=1:length(d)
      if (d(j).isdir == 0)
         dotLoc = findstr(d(j).name,'.');
         if ~isempty(dotLoc)
            ext = d(j).name(dotLoc(end):end);
            if isequal(ext,fileExt)
               k = k+1;
               fileList{k,1} = d(j).name;
            end
         end
      end
   end
end
