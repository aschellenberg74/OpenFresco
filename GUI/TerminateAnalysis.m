function Response = TerminateAnalysis(Model,GroundMotion,LastState,Analysis)
%TERMINATEANALYSIS to stop a running analysis in OpenFresco Express
% Response = TerminateAnalysis(Model,GroundMotion,LastState,Analysis)
%
% Response     : data structure with response results
% Model        : data structure with structural model information
% GroundMotion : data structure with ground motion information
% LastState    : data structure with last state conditions
% Analysis     : handles for analysis page
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

% get handles of main window
handles = guidata(findobj('Tag','OpenFrescoExpress'));

switch handles.Store.StopOption
    % =====================================================================
    case 'Unload'
        % update damping values for free vibration
        zeta = 0.1;
        Model.alphaM = 0.0;
        Model.betaK  = 2*zeta/Model.Omega(1);
        
        % update loading to no load
        GroundMotion.loadType = 'None';
        GroundMotion.tEnd = LastState.Time(end) + 10;
        
        % reset analysis control buttons
        set(handles.Analysis(7),'Value',1,'CData',handles.Store.Start1a);
        set(handles.Analysis(8),'CData',handles.Store.Pause0a);
        set(handles.Analysis(9),'CData',handles.Store.Stop0a);
        set(handles.Sidebar(4),'Value',1,'CData',handles.Store.Start1b);
        set(handles.Sidebar(5),'CData',handles.Store.Pause0b);
        set(handles.Sidebar(6),'CData',handles.Store.Stop0b);
        
        % reset stop flag
        handles.Model.StopFlag = 0;
        guidata(findobj('Tag','OpenFrescoExpress'), handles);
        
        % execute free vibration analysis
        if strcmp(handles.GM.integrator,'NewmarkExplicit')
            Response = Integrator_NewmarkExplicit(Model,GroundMotion,LastState,Analysis);
        elseif strcmp(handles.GM.integrator,'AlphaOS')
            Response = Integrator_AlphaOS(Model,GroundMotion,LastState,Analysis);
        end
    % =====================================================================
    case 'Save State'
        Response = LastState;
    % =====================================================================
end