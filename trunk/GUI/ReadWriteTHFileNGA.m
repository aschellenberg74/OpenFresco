function varargout = ReadWriteTHFileNGA(action,fileIN,fileOUT)
%READWRITETHFILE to read and write a time history file
% varargout = ReadWriteTHFile(action,fileIN,fileOUT)
%
% action  : switch with following possible values
%              'readDT'     read time increment dt from file and output dt
%              'readTHF'    read the time history file and output vectors t and ag
%              'writeTHF'   read the time history file and write to new file which OpenSees can read
% fileIn  : text string containing the input time history filename and its path
% fileOUT : text string containing the output time history filename and its path
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%                          OpenSees Navigator                          %%
%% Matlab Engineering Toolbox for Analysis of Structures using OpenSees %%
%%                                                                      %%
%%                   Andreas Schellenberg & Tony Yang                   %%
%%        andreas.schellenberg@gmail.com, yangtony2004@gmail.com        %%
%%                                                                      %%
%%    Department of Civil and Environmental Engineering, UC Berkeley    %%
%%   (C) Copyright 2004, The Regents of the University of California    %%
%%                         All Rights Reserved                          %%
%%                                                                      %%
%%   Commercial use of this program without express permission of the   %%
%%     University of California, Berkeley, is strictly prohibited.      %%
%%     See Help -> OpenSees Navigator Disclaimer for information on     %%
%%  usage and redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.  %%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% $Revised to handle new and old versions of PEER database, as well as
% unrecognized formats. Designed for use with OpenFresco Quick Start.
% $Date: 2010-07-13 (Tue, 13 Jul 2010) $
% $URL: $

%  Initialization tasks
handles = guidata(gcbf);

%Identify which PEER database, if any, has been used
fin = fopen(fileIN,'r');
firstLine = fgetl(fin);
words = strread(lower(firstLine),'%s');
string_find = [];
handles.GM.databaseType = 'Unknown';
guidata(gcbf, handles);
for i=1:length(words)
    if isequal(words{i},'peer')
        for j = 1:length(words)
            if isequal(words{j},'nga')
                string_find = 1;
            end
        end
        if string_find == 1
            handles.GM.databaseType = 'PEER new';
        else
            handles.GM.databaseType = 'PEER old';
        end
    end
end

switch action
   %=========================================================================
   case 'readDT'
      if strcmp(handles.GM.databaseType, 'Unknown')
          answer = inputdlg(['Unrecognized time history database'; 'Please enter dt manually          '],'Input dt');
          while isempty(answer) || strcmp(answer, '')
              answer = inputdlg(['Unrecognized time history database'; 'Please enter dt manually          '],'Input dt');
          end
          dt = str2num(answer{1});
      else
          fin = fopen(fileIN,'r');
          dt = [];
          while feof(fin)==0
              txtLine = fgetl(fin);
              if ~isempty(txtLine) & ~isequal(txtLine,-1)
                  numLine = str2num(txtLine);
                  if isempty(numLine)
                      switch handles.GM.databaseType
                          case 'PEER old'
                              if ~isempty(findstr(lower(txtLine),'dt='))
                                  words = strread(lower(txtLine),'%s');
                                  for i=1:length(words)
                                      if isequal(words{i},'dt=')
                                          dt = str2num(words{i+1});
                                      end
                                  end
                              end
                          case 'PEER new'
                              if ~isempty(findstr(lower(txtLine),'dt'))
                                  words = strread(lower(txtLine),'%s');
                                  for i=1:length(words)
                                      if isequal(words{i},'dt')
                                          dt = str2num(words{i-2});
                                      end
                                  end
                              end
                      end
                  end
              end
          end
      end
      fclose(fin);
      varargout = {dt handles.GM.databaseType};
   %=========================================================================
   case 'readTHF'
      fin  = fopen(fileIN,'r');
      dt = 1;
      ag = [];
      while feof(fin)==0
          txtLine = fgetl(fin);
          if ~isempty(txtLine) & ~isequal(txtLine,-1)
              numLine = str2num(txtLine);
              if isempty(numLine)
                  switch handles.GM.databaseType
                      case 'PEER old'
                          if ~isempty(findstr(lower(txtLine),'dt='))
                              words = strread(lower(txtLine),'%s');
                              for i=1:length(words)
                                  if isequal(words{i},'dt=')
                                      dt = str2num(words{i+1});
                                  end
                              end
                          end
                      case 'PEER new'
                          if ~isempty(findstr(lower(txtLine),'dt'))
                              words = strread(lower(txtLine),'%s');
                              for i=1:length(words)
                                  if isequal(words{i},'dt')
                                      dt = str2num(words{i-2});
                                  end
                              end
                          end
                      case 'Unknown'
                          direction = handles.GM.store.direction;
                          switch direction
                              case 'Direction 1'
                                  dt = handles.GM.dt(1);
                              case 'Direction 2'
                                  dt = handles.GM.dt(2);
                          end
                  end
              else
                  ag = [ag; str2num(sprintf('%s',txtLine))'];
              end
          end
      end
       t = [0:length(ag)-1]'*dt;
       fclose(fin);
       varargout = {t ag};
   %=========================================================================
   case 'writeTHF'
      if isequal(fileIN,fileOUT)
         return
      end
      fin  = fopen(fileIN,'r');
      fout = fopen(fileOUT,'w');
      while feof(fin)==0
         txtLine = fgetl(fin);
         if ~isempty(txtLine) & ~isequal(txtLine,-1)
            numLine = str2num(txtLine);
            if ~isempty(numLine)
               fprintf(fout,'%s\n',txtLine);
            end
         end
      end
      fclose(fin);
      fclose(fout);
      varargout = {};
   %=========================================================================
end

%Update handles structure
guidata(gcbf, handles);