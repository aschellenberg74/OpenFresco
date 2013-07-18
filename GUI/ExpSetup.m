function ExpSetup(action,varargin)
%EXPSETUP to handle user inputs related to the Experimental Setup
% ExpSetup(action,varargin)
%
% action   : selected action on the Experimental Setup page
% varargin : variable length input argument list
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

% initialization tasks
handles = guidata(gcbf);

% identify chosen DOF
DOF_selection = handles.Model.Type;

switch action
    % =====================================================================
    case 'choose DOF'
        % prevent user from changing setup
        TF = strcmp(get(get(handles.ES(2), 'SelectedObject'),'Tag'), DOF_selection);
        if TF ~=1
            msgbox({'Cannot change experimental setup type!','Can only change structure type'},'Error','error');
            switch DOF_selection
                case '1 DOF'
                    set(handles.ES(3),'Value',1);
                case '2 DOF A'
                    set(handles.ES(4),'Value',1);
                case '2 DOF B'
                    set(handles.ES(5),'Value',1);
            end
        end
    % =====================================================================
    case 'extra options'
        % this callback has not been fully checked for functionality yet
        % nonlinear options for 2 DOF B case
        if strcmp(DOF_selection, '2 DOF B')
            nonlinear = questdlg('Would you like to account for large displacement effects?','Nonlinear Geometry?','Yes','No','Yes');
            switch nonlinear
                case 'Yes'
                    msgbox('Sorry! This is not implemented yet!','Warning','warn');
                    %Act_Lengths = inputdlg({['Please input actuator lengths'; 'Actuator 1                   '] 'Actuator 2'},'Actuator Lengths');
                    %if ~isempty(Act_Lengths)
                    %    handles.Model.ActLength1 = str2num(Act_Lengths{1});
                    %    handles.Model.ActLength2 = str2num(Act_Lengths{2});
                    %else
                    %    return
                    %end
            end
        else
            msgbox({'Cannot change experimental setup type!','Can only change structure type'},'Error','error');
            switch DOF_selection
                case '1 DOF'
                    set(handles.ES(3),'Value',1);
                case '2 DOF A'
                    set(handles.ES(4),'Value',1);
                case '2 DOF B'
                    set(handles.ES(5),'Value',1);
            end
        end
    % =====================================================================
end

% based on setup chosen, return setup type to ExpSetup.Type
% example: ExpSetup.Type = 'OneActuator'

% update handles structure
guidata(gcbf,handles);
