function varargout = ReadWriteTHFile(action,fileIN,fileOUT)
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
% $Revision: 217 $
% $Date: 2007-01-02 11:09:29 -0800 (Tue, 02 Jan 2007) $
% $URL: $

switch action
   %=========================================================================
   case 'readDT'
      fin = fopen(fileIN,'r');
      dt = [];
      while feof(fin)==0
         txtLine = fgetl(fin);
         if ~isempty(txtLine) & ~isequal(txtLine,-1)
            numLine = str2num(txtLine);
            if isempty(numLine)
               if ~isempty(findstr(lower(txtLine),'dt='))
                  words = strread(lower(txtLine),'%s');
                  for i=1:length(words)
                     if isequal(words{i},'dt=')
                        dt = str2num(words{i+1});
                     end
                  end
               end
            end
         end
      end
      fclose(fin);
      varargout = {dt};
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
               if ~isempty(findstr(lower(txtLine),'dt='))
                  words = strread(lower(txtLine),'%s');
                  for i=1:length(words)
                     if isequal(words{i},'dt=')
                        dt = str2num(words{i+1});
                     end
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