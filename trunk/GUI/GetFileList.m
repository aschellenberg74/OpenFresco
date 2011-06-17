function fileList = GetFileList(fileExt,searchDir)
%GETFILELIST find all the files in the given folder and its subfolders
% that are on the path and have extension fileExt
% fileList = GetFileList(fileExt,searchDir)
%
% fileList  : list of all the files with extension fileExt
% fileExt   : file extension. Example fileExt = '.m', '.txt', ...
% searchDir : full path of directory to search
%
% Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
% Created: 09/10
% Revision: A
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
