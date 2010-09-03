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
        zeta = 0.1;
        Model.alphaM = 0.0;
        Model.betaK  = 2*zeta/Model.Omega(1);
        
        tEnd = 10;
        numMotions = length(GroundMotion.dt);
        for mo=1:numMotions
            GroundMotion.scalet{mo}  = [0.0 tEnd];
            GroundMotion.scaleag{mo} = [0.0 0.0];
        end
        
        set(handles.Sidebar(4),'Value',1);
        set(handles.Analysis(7),'Value',1);
        
        Response = Integrator_NewmarkExplicit(Model,GroundMotion,LastState,Analysis);
        
    case 'Save State'
        
        
end