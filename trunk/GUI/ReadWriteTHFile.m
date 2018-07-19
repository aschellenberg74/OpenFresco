function varargout = ReadWriteTHFile(action,fileIN,fileOUT)
%READWRITETHFILE to read and write a time history file, modified for use
% with OpenFresco Express
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

%#ok<*ST2NM>

switch action
    % =====================================================================
    case 'readDT'
        dt = 1;
        fin = fopen(fileIN,'r');
        txtLine = fgetl(fin);
        if contains(txtLine,'PEER')
            while ~feof(fin)
                txtLine = fgetl(fin);
                if ~isempty(txtLine) && ~isequal(txtLine,-1)
                    if contains(txtLine,'DT=')
                        % PEER STRONG MOTION DATABASE RECORD
                        token = textscan(txtLine,'%q');
                        dt = cell2mat(textscan(token{1}{4},'%f'));
                        databaseType = 'PEER';
                        break;
                    elseif contains(txtLine,'NPTS, DT')
                        % PEER NGA STRONG MOTION DATABASE RECORD
                        token = textscan(txtLine,'%q');
                        dt = cell2mat(textscan(token{1}{2},'%f'));
                        databaseType = 'PEER NGA';
                        break;
                    end
                end
            end
        else
            answer = [];
            while isempty(answer) || strcmp(answer, '') || str2num(answer{1}) <= 0
                answer = inputdlg(['Unrecognized time history database.';...
                    'Please enter dt manually:          '],'Input dt');
            end
            dt = str2num(answer{1});
            databaseType = 'UNKNOWN';
        end
        fclose(fin);
        varargout = {dt databaseType};
    % =====================================================================
    case 'readTHF'
        dt = 1;
        ag = [];
        fin = fopen(fileIN,'r');
        while ~feof(fin)
            txtLine = fgetl(fin);
            if ~isempty(txtLine) && ~isequal(txtLine,-1)
                numLine = str2num(txtLine);
                if ~isempty(numLine)
                    n = length(numLine);
                    ag(end+1:end+n) = numLine';
                elseif contains(txtLine,'DT=')
                    % PEER STRONG MOTION DATABASE RECORD
                    token = textscan(txtLine,'%q');
                    dt = cell2mat(textscan(token{1}{4},'%f'));
                elseif contains(txtLine,'NPTS, DT')
                    % PEER NGA STRONG MOTION DATABASE RECORD
                    token = textscan(txtLine,'%q');
                    dt = cell2mat(textscan(token{1}{2},'%f'));
                end
            end
        end
        fclose(fin);
        t = (0:length(ag)-1)'*dt;
        varargout = {t ag};
    % =====================================================================
    case 'writeTHF'
        if isequal(fileIN,fileOUT)
            return
        end
        fin  = fopen(fileIN,'r');
        fout = fopen(fileOUT,'w');
        while feof(fin)==0
            txtLine = fgetl(fin);
            if ~isempty(txtLine) && ~isequal(txtLine,-1)
                numLine = str2num(txtLine);
                if ~isempty(numLine)
                    fprintf(fout,'%s\n',txtLine);
                end
            end
        end
        fclose(fin);
        fclose(fout);
        varargout = {};
    % =====================================================================
end
