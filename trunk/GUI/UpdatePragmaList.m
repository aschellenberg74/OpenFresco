function UpdatePragmaList(pathOPF)
%UPDATEPRAGMALIST to update pragmalist in OpenFrescoExpress.m
% UpdatePragmaList(pathOPF)
%
% pathOPF : path to the OpenFresco GUI directory
%
% Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
% Created: 09/10
% Revision: A
%
% $Revision$
% $Date$
% $URL$

% open files
FIDr = fopen('OpenFrescoExpress.m','r');
FIDw = fopen('OpenFrescoExpress.tmp','w');

% copy OpenFrescoExpress.m to OpenFrescoExpress.tmp except for '%#'
while feof(FIDr)==0
   txtLine = fgetl(FIDr);
   if ~isempty(txtLine) && length(txtLine)>2 && isequal(txtLine(1:2),'%#')
      % do nothing
   else
      fprintf(FIDw,'%s\n',txtLine);
   end
end

% write pragma list to OpenFrescoExpress.tmp
list = GetFileList('.m',pathOPF);
k = 1;
tmp = list{1};
fprintf(FIDw,'%%#function %s ',tmp(1:end-2));
for i=2:length(list)
   tmp = list{i};
   if (k==4)
      fprintf(FIDw,'\n%%#function %s ',tmp(1:end-2));
      k = 1;
   else
      fprintf(FIDw,'%s ',tmp(1:end-2));
      k = k+1;
   end
end

% close all files
fclose('all');

% replace OpenFrescoExpress.m with OpenFrescoExpress.tmp
cd(pathOPF);
copyfile('OpenFrescoExpress.tmp','OpenFrescoExpress.m','f');
delete('OpenFrescoExpress.tmp');
