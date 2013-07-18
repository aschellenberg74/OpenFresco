function ExpSite(action,varargin)
%EXPSITE to handle user inputs related to the Experimental Site
% ExpSite(action,varargin)
%
% action   : selected action on the Experimental Site page
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

%#ok<*ST2NM>

% initialization tasks
handles = guidata(gcbf);

% store which tab is selected
Tab_Selection = get(gcbo,'Tag');

switch action
    % =====================================================================
    case 'tab toggle'
        % switch panel display based on tab selection
        switch Tab_Selection
            % -------------------------------------------------------------
            case 'Local Site'
                set(handles.ESI(2),'CData',handles.Store.Locl1);
                set(handles.ESI(3),'Value',0,'CData',handles.Store.Dist0);
                set(handles.ESI(4:5),'Visible','on');
                set(handles.ESI([6 9 14]),'Visible','off');
                handles.ExpSite.Type = 'Local';
            % -------------------------------------------------------------
            case 'Distributed Site'
                set(handles.ESI(2),'Value',0,'CData',handles.Store.Locl0);
                set(handles.ESI(3),'CData',handles.Store.Dist1);
                set(handles.ESI(4),'Visible','off');
                set(handles.ESI(6:8),'Visible','on');
                handles.ExpSite.Type = 'Distributed';
            % -------------------------------------------------------------
        end
    % =====================================================================
    case 'Distributed Site'
        % display options for distributed sites
        control_selection = get(gcbo,'Value');
        switch control_selection
            % -------------------------------------------------------------
            case 1  % no selection
                handles.ExpSite.Type = 'Distributed';
                handles.ExpSite.store.DistActive = (6:8);
                set(handles.ESI(9:16),'Visible','off');
                set(handles.ESI(handles.ExpSite.store.DistActive),'Visible','on');
            % -------------------------------------------------------------
            case 2  % Shadow Site
                handles.ExpSite.Type = 'Shadow';
                handles.ExpSite.store.DistActive = [6:8 9:13];
                set(handles.ESI(9:16),'Visible','off');
                set(handles.ESI(handles.ExpSite.store.DistActive),'Visible','on');
            % -------------------------------------------------------------
            case 3  % Actor Site
                handles.ExpSite.Type = 'Actor';
                handles.ExpSite.store.DistActive = [6:8 14:16];
                set(handles.ESI(9:16),'Visible','off');
                set(handles.ESI(handles.ExpSite.store.DistActive),'Visible','on');
            % -------------------------------------------------------------
        end
        
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % data input cases
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    
    % =====================================================================
    % distributed site
    % =====================================================================
    case 'ipAddr'
        handles.ExpSite.ipAddr = get(gcbo,'String');
    % =====================================================================
    case 'ipPort'
        handles.ExpSite.ipPort = str2num(get(gcbo,'String'));
    % =====================================================================
    case 'protocol'
        protocols = get(gcbo,'String');
        protocol = protocols{get(gcbo,'Value')};
        handles.ExpSite.protocol = protocol;
        if strcmp(protocol,'UDP')
            set(handles.ESI(13),'String','4')
            handles.ExpSite.dataSize = 4;
        else
            set(handles.ESI(13),'String','256')
            handles.ExpSite.dataSize = 256;
        end
    % =====================================================================
    case 'dataSize'
        handles.ExpSite.dataSize = str2num(get(gcbo,'String'));
    % =====================================================================
end

% update handles structure
guidata(gcbf, handles);
