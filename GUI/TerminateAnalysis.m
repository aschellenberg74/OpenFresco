function Response = TerminateAnalysis(Model,GroundMotion,LastState,Analysis)
%INTEGRATOR_NEWMARKEXPLICIT to perform a direct integration analysis using
% the second order explicit Newmark method
% Response = Integrator_NewmarkExplicit(Model,GroundMotion,Analysis)
%
% Model        : data structure with structural model information
% GroundMotion : data structure with ground motion information
% LastState    : data structure with last state conditions
% Analysis     : handles for analysis page

% Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
% Created: 07/10
% Revision: A

% get handles of main window
handles = guidata(findobj('Tag','OpenFresco Quick Start'));

switch handles.Store.StopOption
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
        guidata(findobj('Tag','OpenFresco Quick Start'), handles);
        
        % execute free vibration analysis
        Response = Integrator_NewmarkExplicit(Model,GroundMotion,LastState,Analysis);
        
    case 'Save State'
        Response = LastState;
        
end