function Analysis(action,varargin)
%ANALYSIS to handle user inputs related to the analysis options
% Analysis(action,varargin)
%
% action   : selected analysis action
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

% Initialization tasks
handles = guidata(findobj('Tag','OpenFrescoExpress'));

% Store which button is pressed
analysis_option = get(gcbo,'Tag');

switch action
    case 'choose option'
        %check if the analysis has already been stopped
        if handles.Model.StopFlag == 1
            %provide replay options
            if strcmp(analysis_option,'Start')
                replay = questdlg(sprintf('The experiment has ended.\nWould you like to replay\nthe results?'), ...
                    'Replay', ...
                    'Yes','No','Yes');
                switch replay
                    case 'Yes'
                        disp('Replaying...');
                        handles.Model.StopFlag = 0;
                        startCall1 = get(handles.Analysis(7),'Callback');
                        pauseCall1 = get(handles.Analysis(8),'Callback');
                        stopCall1 = get(handles.Analysis(9),'Callback');
                        startCall2 = get(handles.Sidebar(4),'Callback');
                        pauseCall2 = get(handles.Sidebar(5),'Callback');
                        stopCall2 = get(handles.Sidebar(6),'Callback');
                        set(findobj('Tag','Start'),'Value',1);
                        set(handles.Analysis(7),'Value',1);
                        set(handles.Analysis(7),'CData',handles.Store.Start1a,'Callback','');
                        set(handles.Analysis(8),'CData',handles.Store.Pause0a,'Callback','');
                        set(handles.Analysis(9),'CData',handles.Store.Stop0a,'Callback','');
                        set(handles.Sidebar(4),'CData',handles.Store.Start1b,'Callback','');
                        set(handles.Sidebar(5),'CData',handles.Store.Pause0b,'Callback','');
                        set(handles.Sidebar(6),'CData',handles.Store.Stop0b,'Callback','');
                        
                        %Update handles structure
                        guidata(findobj('Tag','OpenFrescoExpress'), handles);
                        
                        figure(findobj('Tag','ErrMon'));
                        if ~isempty(findobj('Tag','StructOutDOF2'))
                            figure(findobj('Tag','StructOutDOF2'));
                        end
                        figure(findobj('Tag','StructOutDOF1'));
                        
                        %Proceed with plotting
                        for i = 1:length(handles.Response.Time)
                            ReplayResults(i)
                        end
                        
                        set(findobj('Tag','StopControl'),'Value',1);
                        set(handles.Analysis(9),'Value',1);
                        set(handles.Analysis(7),'CData',handles.Store.Start0a,'Callback',startCall1);
                        set(handles.Analysis(8),'CData',handles.Store.Pause0a,'Callback',pauseCall1);
                        set(handles.Analysis(9),'CData',handles.Store.Stop1a,'Callback',stopCall1);
                        set(handles.Sidebar(4),'CData',handles.Store.Start0b,'Callback',startCall2);
                        set(handles.Sidebar(5),'CData',handles.Store.Pause0b,'Callback',pauseCall2);
                        set(handles.Sidebar(6),'CData',handles.Store.Stop1b,'Value',1,'Callback',stopCall2);
                        handles.Model.StopFlag = 1;
                        %Update handles structure
                        guidata(findobj('Tag','OpenFrescoExpress'), handles);
                    case 'No'
                        msgbox(sprintf('Experiment has ended.\nChoose "Run New Test" to start a new test.'),'Error','error');
                        set(handles.Sidebar(6),'Value',1);
                        set(handles.Analysis(9),'Value',1);
                        set(handles.Analysis(7),'CData',handles.Store.Start0a);
                        set(handles.Analysis(8),'CData',handles.Store.Pause0a);
                        set(handles.Analysis(9),'CData',handles.Store.Stop1a);
                        set(handles.Sidebar(4),'CData',handles.Store.Start0b);
                        set(handles.Sidebar(5),'CData',handles.Store.Pause0b);
                        set(handles.Sidebar(6),'CData',handles.Store.Stop1b);
                end
            else
                msgbox(sprintf('Experiment has ended.\nChoose "Run New Test" to start a new test.'),'Error','error');
                set(handles.Sidebar(6),'Value',1);
                set(handles.Analysis(9),'Value',1);
                set(handles.Analysis(7),'CData',handles.Store.Start0a);
                set(handles.Analysis(8),'CData',handles.Store.Pause0a);
                set(handles.Analysis(9),'CData',handles.Store.Stop1a);
                set(handles.Sidebar(4),'CData',handles.Store.Start0b);
                set(handles.Sidebar(5),'CData',handles.Store.Pause0b);
                set(handles.Sidebar(6),'CData',handles.Store.Stop1b);
            end
            
        else
            %if the analysis has NOT been previously stopped
            switch analysis_option
                %%%%%%%
                %Start%
                %%%%%%%
                case 'Start'
                    DIR = which('OpenFrescoExpress.m');
                    DIR = DIR(1:end-20);
                    %return if TCL does not exist
                    if ~exist(fullfile(DIR,'OPFAnalysis.tcl'),'file')
                        msgbox(sprintf('No .tcl file found!\nPlease write .tcl first.'),'Error','error')
                        set(handles.Analysis(7),'Value',0);
                        return
                    end
                    %initialize GUI display
                    set(handles.Analysis(7),'CData',handles.Store.Start1a);
                    handles.Store.AnalysisOption = analysis_option;
                    guidata(findobj('Tag','OpenFrescoExpress'), handles);
                    disp('Starting...');
                    %start new analysis
                    if handles.Model.firstStart
                        handles.Model.firstStart = 0;
                        guidata(findobj('Tag','OpenFrescoExpress'), handles);
                        GUI_ErrorMonitors;
                        GUI_StructuralOutput;
                        if handles.Store.Animate
                            GUI_AnimateStructure;
                        end
                        GUI_AnalysisControls;
                        handles = guidata(findobj('Tag','OpenFrescoExpress'));
                        set(handles.Sidebar(4),'CData',handles.Store.Start1b);
                        
                        %Initialize plots
                        switch handles.Model.Type
                            case '1 DOF'
                                handles.Plots.SO1dplot  = line(0,0,'LineWidth',1.0,'Parent',findobj('Tag','SO1d'));
                                handles.Plots.SO1fplot  = line(0,0,'LineWidth',1.0,'Parent',findobj('Tag','SO1f'));
                                handles.Plots.SO1aplot  = line(0,0,'LineWidth',1.0,'Parent',findobj('Tag','SO1a'));
                                handles.Plots.SO1fdplot = line(0,0,'LineWidth',1.0,'Parent',findobj('Tag','SO1fd'));
                                handles.Plots.SO1fddot  = line(0,0,'Color','red','Marker','o','LineWidth',1.0,'Parent',findobj('Tag','SO1fd'));
                                switch handles.GM.loadType
                                    case 'Ground Motions'
                                        handles.Plots.SO1agplot = line(handles.GM.scalet{1},handles.GM.scaleag{1},'LineWidth',1.0,'Parent',findobj('Tag','SO1ag'));
                                        handles.Plots.SO1agdot  = line(0,0,'Color','red','Marker','o','LineWidth',1.0,'Parent',findobj('Tag','SO1ag'));
                                    case 'Initial Conditions'
                                        handles.Plots.SO1agdot  = line(0,0,'Color','red','Marker','o','LineWidth',1.0,'Parent',findobj('Tag','SO1ag'));
                                end
                                handles.Plots.EM1eplot       = line(0,0,'LineWidth',1.0,'Parent',findobj('Tag','EM1e'));
                                handles.Plots.EM1ffteplot    = line(0,0,'LineWidth',1.0,'Parent',findobj('Tag','EM1ffte'));
                                handles.Plots.EM1MeasCmdplot = line(0,0,'LineWidth',1.0,'Parent',findobj('Tag','EM1MeasCmd'));
                                handles.Plots.EM1MeasCmddot  = line(0,0,'Color','red','Marker','o','LineWidth',1.0,'Parent',findobj('Tag','EM1MeasCmd'));
                                handles.Plots.EM1trackplot   = line(0,0,'LineWidth',1.0,'Parent',findobj('Tag','EM1track'));
                                
                            case '2 DOF A'
                                handles.Plots.SO1dplot  = line(0,0,'LineWidth',1.0,'Parent',findobj('Tag','SO1d'));
                                handles.Plots.SO1fplot  = line(0,0,'LineWidth',1.0,'Parent',findobj('Tag','SO1f'));
                                handles.Plots.SO1aplot  = line(0,0,'LineWidth',1.0,'Parent',findobj('Tag','SO1a'));
                                handles.Plots.SO1fdplot = line(0,0,'LineWidth',1.0,'Parent',findobj('Tag','SO1fd'));
                                handles.Plots.SO1fddot  = line(0,0,'Color','red','Marker','o','LineWidth',1.0,'Parent',findobj('Tag','SO1fd'));
                                switch handles.GM.loadType
                                    case 'Ground Motions'
                                        handles.Plots.SO1agplot = line(handles.GM.scalet{1},handles.GM.scaleag{1},'LineWidth',1.0,'Parent',findobj('Tag','SO1ag'));
                                        handles.Plots.SO1agdot  = line(0,0,'Color','red','Marker','o','LineWidth',1.0,'Parent',findobj('Tag','SO1ag'));
                                        handles.Plots.SO2agplot = line(handles.GM.scalet{1},handles.GM.scaleag{1},'LineWidth',1.0,'Parent',findobj('Tag','SO2ag'));
                                        handles.Plots.SO2agdot  = line(0,0,'Color','red','Marker','o','LineWidth',1.0,'Parent',findobj('Tag','SO2ag'));
                                    case 'Initial Conditions'
                                        handles.Plots.SO1agdot  = line(0,0,'Color','red','Marker','o','LineWidth',1.0,'Parent',findobj('Tag','SO1ag'));
                                        handles.Plots.SO2agdot  = line(0,0,'Color','red','Marker','o','LineWidth',1.0,'Parent',findobj('Tag','SO2ag'));
                                end
                                handles.Plots.SO1ddplot = line(0,0,'LineWidth',1.0,'Parent',findobj('Tag','SO1dd'));                                
                                handles.Plots.SO1dddot  = line(0,0,'Color','red','Marker','o','LineWidth',1.0,'Parent',findobj('Tag','SO1dd'));
                                handles.Plots.SO2dplot  = line(0,0,'LineWidth',1.0,'Parent',findobj('Tag','SO2d'));
                                handles.Plots.SO2fplot  = line(0,0,'LineWidth',1.0,'Parent',findobj('Tag','SO2f'));
                                handles.Plots.SO2aplot  = line(0,0,'LineWidth',1.0,'Parent',findobj('Tag','SO2a'));
                                handles.Plots.SO2fdplot = line(0,0,'LineWidth',1.0,'Parent',findobj('Tag','SO2fd'));
                                handles.Plots.SO2fddot  = line(0,0,'Color','red','Marker','o','LineWidth',1.0,'Parent',findobj('Tag','SO2fd'));
                                handles.Plots.SO2ffplot = line(0,0,'LineWidth',1.0,'Parent',findobj('Tag','SO2ff'));
                                handles.Plots.SO2ffdot  = line(0,0,'Color','red','Marker','o','LineWidth',1.0,'Parent',findobj('Tag','SO2ff'));
                                
                                handles.Plots.EM1eplot       = line(0,0,'LineWidth',1.0,'Parent',findobj('Tag','EM1e'));
                                handles.Plots.EM1ffteplot    = line(0,0,'LineWidth',1.0,'Parent',findobj('Tag','EM1ffte'));
                                handles.Plots.EM1MeasCmdplot = line(0,0,'LineWidth',1.0,'Parent',findobj('Tag','EM1MeasCmd'));
                                handles.Plots.EM1MeasCmddot  = line(0,0,'Color','red','Marker','o','LineWidth',1.0,'Parent',findobj('Tag','EM1MeasCmd'));
                                handles.Plots.EM1trackplot   = line(0,0,'LineWidth',1.0,'Parent',findobj('Tag','EM1track'));
                                handles.Plots.EM2eplot       = line(0,0,'LineWidth',1.0,'Parent',findobj('Tag','EM2e'));
                                handles.Plots.EM2ffteplot    = line(0,0,'LineWidth',1.0,'Parent',findobj('Tag','EM2ffte'));
                                handles.Plots.EM2MeasCmdplot = line(0,0,'LineWidth',1.0,'Parent',findobj('Tag','EM2MeasCmd'));
                                handles.Plots.EM2MeasCmddot  = line(0,0,'Color','red','Marker','o','LineWidth',1.0,'Parent',findobj('Tag','EM2MeasCmd'));
                                handles.Plots.EM2trackplot   = line(0,0,'LineWidth',1.0,'Parent',findobj('Tag','EM2track'));
                                
                            case '2 DOF B'
                                handles.Plots.SO1dplot  = line(0,0,'LineWidth',1.0,'Parent',findobj('Tag','SO1d'));
                                handles.Plots.SO1fplot  = line(0,0,'LineWidth',1.0,'Parent',findobj('Tag','SO1f'));
                                handles.Plots.SO1aplot  = line(0,0,'LineWidth',1.0,'Parent',findobj('Tag','SO1a'));
                                handles.Plots.SO1fdplot = line(0,0,'LineWidth',1.0,'Parent',findobj('Tag','SO1fd'));
                                handles.Plots.SO1fddot  = line(0,0,'Color','red','Marker','o','LineWidth',1.0,'Parent',findobj('Tag','SO1fd'));
                                switch handles.GM.loadType
                                    case 'Ground Motions'
                                        handles.Plots.SO1agplot = line(handles.GM.scalet{1},handles.GM.scaleag{1},'LineWidth',1.0,'Parent',findobj('Tag','SO1ag'));
                                        handles.Plots.SO1agdot  = line(0,0,'Color','red','Marker','o','LineWidth',1.0,'Parent',findobj('Tag','SO1ag'));
                                        handles.Plots.SO2agplot = line(handles.GM.scalet{2},handles.GM.scaleag{2},'LineWidth',1.0,'Parent',findobj('Tag','SO2ag'));
                                        handles.Plots.SO2agdot  = line(0,0,'Color','red','Marker','o','LineWidth',1.0,'Parent',findobj('Tag','SO2ag'));
                                    case 'Initial Conditions'
                                        handles.Plots.SO1agdot  = line(0,0,'Color','red','Marker','o','LineWidth',1.0,'Parent',findobj('Tag','SO1ag'));
                                        handles.Plots.SO2agdot  = line(0,0,'Color','red','Marker','o','LineWidth',1.0,'Parent',findobj('Tag','SO2ag'));
                                end
                                handles.Plots.SO1ddplot = line(0,0,'LineWidth',1.0,'Parent',findobj('Tag','SO1dd'));
                                handles.Plots.SO1dddot  = line(0,0,'Color','red','Marker','o','LineWidth',1.0,'Parent',findobj('Tag','SO1dd'));
                                handles.Plots.SO2dplot  = line(0,0,'LineWidth',1.0,'Parent',findobj('Tag','SO2d'));
                                handles.Plots.SO2fplot  = line(0,0,'LineWidth',1.0,'Parent',findobj('Tag','SO2f'));
                                handles.Plots.SO2aplot  = line(0,0,'LineWidth',1.0,'Parent',findobj('Tag','SO2a'));
                                handles.Plots.SO2fdplot = line(0,0,'LineWidth',1.0,'Parent',findobj('Tag','SO2fd'));
                                handles.Plots.SO2fddot  = line(0,0,'Color','red','Marker','o','LineWidth',1.0,'Parent',findobj('Tag','SO2fd'));
                                handles.Plots.SO2ffplot = line(0,0,'LineWidth',1.0,'Parent',findobj('Tag','SO2ff'));
                                handles.Plots.SO2ffdot  = line(0,0,'Color','red','Marker','o','LineWidth',1.0,'Parent',findobj('Tag','SO2ff'));
                                
                                handles.Plots.EM1eplot       = line(0,0,'LineWidth',1.0,'Parent',findobj('Tag','EM1e'));
                                handles.Plots.EM1ffteplot    = line(0,0,'LineWidth',1.0,'Parent',findobj('Tag','EM1ffte'));
                                handles.Plots.EM1MeasCmdplot = line(0,0,'LineWidth',1.0,'Parent',findobj('Tag','EM1MeasCmd'));
                                handles.Plots.EM1MeasCmddot  = line(0,0,'Color','red','Marker','o','LineWidth',1.0,'Parent',findobj('Tag','EM1MeasCmd'));
                                handles.Plots.EM1trackplot   = line(0,0,'LineWidth',1.0,'Parent',findobj('Tag','EM1track'));
                                handles.Plots.EM2eplot       = line(0,0,'LineWidth',1.0,'Parent',findobj('Tag','EM2e'));
                                handles.Plots.EM2ffteplot    = line(0,0,'LineWidth',1.0,'Parent',findobj('Tag','EM2ffte'));
                                handles.Plots.EM2MeasCmdplot = line(0,0,'LineWidth',1.0,'Parent',findobj('Tag','EM2MeasCmd'));
                                handles.Plots.EM2MeasCmddot  = line(0,0,'Color','red','Marker','o','LineWidth',1.0,'Parent',findobj('Tag','EM2MeasCmd'));
                                handles.Plots.EM2trackplot   = line(0,0,'LineWidth',1.0,'Parent',findobj('Tag','EM2track'));
                        end
                        set(findobj('Tag','StartControl'),'Value',1)
                        set(handles.Analysis(7),'Value',1);
                        guidata(findobj('Tag','OpenFrescoExpress'), handles);
                        DIR = which('OpenFrescoExpress.m');
                        DIR = DIR(1:end-20);
                        % load path to OpenSees.exe and OpenFresco.dll
                        if ~exist('OpenSeesPath.txt','file')
                            fprintf(1,'Using GUI-internal OpenSees/OpenFresco at:\n');
                            pathOPF = which('Pnpscr.dll');
                            pathOPF = pathOPF(1:end-11);
                            fprintf(1,'%s\n',pathOPF);
                        else
                            fprintf(1,'Using user-specified OpenSees/OpenFresco at:\n');
                            s = which('OpenSeesPath.txt');
                            if isempty(s)
                                s = fullfile(pwd,'OpenSeesPath.txt');
                            end
                            FID = fopen(s,'r');
                            pathOPF = fgetl(FID);
                            fclose(FID);
                            fprintf(1,'%s\n',pathOPF);
                        end
                        % now run OpenSees/OpenFresco
                        RunOpenFresco(pathOPF,fullfile(DIR,'OPFAnalysis.tcl'));
                        clear functions;
                        handles.Response = Integrator_NewmarkExplicit(handles.Model,handles.GM,[],handles.Analysis);
                        %set analysis to "stopped"
                        set(findobj('Tag','StopControl'),'Value',1);
                        set(handles.Analysis(9),'Value',1);
                        set(handles.Analysis(7),'CData',handles.Store.Start0a);
                        set(handles.Analysis(8),'CData',handles.Store.Pause0a);
                        set(handles.Analysis(9),'CData',handles.Store.Stop1a);
                        set(handles.Sidebar(4),'CData',handles.Store.Start0b);
                        set(handles.Sidebar(5),'CData',handles.Store.Pause0b);
                        set(handles.Sidebar(6),'CData',handles.Store.Stop1b);
                        handles.Model.StopFlag = 1;
                        handles.Store.AnalysisOption = 'Stop';
                        %save results
                        saveResults = questdlg(sprintf('Analysis complete!\nWould you like to save the test results?'),'Save?','Yes','No','Yes');
                        if strcmp(saveResults,'Yes')
                            Response = handles.Response;
                            uisave('Response');
                        end
                    %resume paused analysis
                    else
                        set(findobj('Tag','Start'),'Value',1);
                        set(handles.Analysis(7),'Value',1);
                        set(handles.Analysis(7),'CData',handles.Store.Start1a);
                        set(handles.Analysis(8),'CData',handles.Store.Pause0a);
                        set(handles.Analysis(9),'CData',handles.Store.Stop0a);
                        set(handles.Sidebar(4),'CData',handles.Store.Start1b);
                        set(handles.Sidebar(5),'CData',handles.Store.Pause0b);
                        set(handles.Sidebar(6),'CData',handles.Store.Stop0b);
                        figure(findobj('Tag','ErrMon'));
                        if ~isempty(findobj('Tag','StructOutDOF2'));
                            figure(findobj('Tag','StructOutDOF2'));
                        end
                        figure(findobj('Tag','StructOutDOF1'));
                        figure(findobj('Tag','AnalysisControls'));
                    end
                    
                %%%%%%%
                %Pause%
                %%%%%%%
                case 'Pause'
                    %return if test not yet started
                    if handles.Model.firstStart
                        msgbox('Test not yet started!','Error','error');
                        return
                    end
                    
                    handles.Store.AnalysisOption = analysis_option;
                    disp('Paused...');
                    set(findobj('Tag','Pause'),'Value',1);
                    set(handles.Analysis(8),'Value',1);
                    set(handles.Analysis(7),'CData',handles.Store.Start0a);
                    set(handles.Analysis(8),'CData',handles.Store.Pause1a);
                    set(handles.Analysis(9),'CData',handles.Store.Stop0a);
                    set(handles.Sidebar(4),'CData',handles.Store.Start0b);
                    set(handles.Sidebar(5),'CData',handles.Store.Pause1b);
                    set(handles.Sidebar(6),'CData',handles.Store.Stop0b);
                    %bring figures to front
                    figure(findobj('Tag','ErrMon'));
                    if ~isempty(findobj('Tag','StructOutDOF2'))
                        figure(findobj('Tag','StructOutDOF2'));
                    end
                    figure(findobj('Tag','StructOutDOF1'));
                    
                %%%%%%
                %Stop%
                %%%%%%
                case 'Stop'
                    %return if test not yet started
                    if handles.Model.firstStart
                        msgbox('Test not yet started!','Error','error');
                        return
                    end
                    
                    set(findobj('Tag','Stop'),'Value',1);
                    set(handles.Analysis(9),'Value',1);
                    set(handles.Analysis(7),'CData',handles.Store.Start0a);
                    set(handles.Analysis(8),'CData',handles.Store.Pause0a);
                    set(handles.Analysis(9),'CData',handles.Store.Stop1a);
                    set(handles.Sidebar(4),'CData',handles.Store.Start0b);
                    set(handles.Sidebar(5),'CData',handles.Store.Pause0b);
                    set(handles.Sidebar(6),'CData',handles.Store.Stop1b);
                    %bring figures to front
                    figure(findobj('Tag','ErrMon'));
                    if ~isempty(findobj('Tag','StructOutDOF2'))
                        figure(findobj('Tag','StructOutDOF2'));
                    end
                    figure(findobj('Tag','StructOutDOF1'));
                    %determine stopping method
                    stop_option = questdlg('How would you like to proceed?', ...
                        'Stop Test', ...
                        'Unload', 'Save State', 'Cancel', 'Unload');
                    handles.Store.StopOption = stop_option;
                    set(findobj('Tag','Stop'),'Value',1);
                    set(handles.Analysis(9),'Value',1);
                    switch stop_option
                        case 'Unload'
                            handles.Store.AnalysisOption = analysis_option;
                            disp('Unloading...');
                            handles.Model.StopFlag = 1;
                        case 'Save State'
                            handles.Store.AnalysisOption = analysis_option;
                            disp('Saving...');
                            handles.Model.StopFlag = 1;
                        case 'Cancel'
                            handles.Model.StopFlag = 0;
                            id = find(strcmp(handles.Store.AnalysisOption, get(handles.Analysis(7:8),'Tag')) == 1);
                            set(handles.Analysis(id+6),'Value',1);
                            if id == 1
                                set(findobj('Tag','Start'),'Value',1);
                                set(handles.Analysis(7),'CData',handles.Store.Start1a);
                                set(handles.Analysis(8),'CData',handles.Store.Pause0a);
                                set(handles.Analysis(9),'CData',handles.Store.Stop0a);
                                set(handles.Sidebar(4),'CData',handles.Store.Start1b,'Value',1);
                                set(handles.Sidebar(5),'CData',handles.Store.Pause0b);
                                set(handles.Sidebar(6),'CData',handles.Store.Stop0b);
                            elseif id == 2
                                set(findobj('Tag','Pause'),'Value',1)
                                set(handles.Analysis(7),'CData',handles.Store.Start0a);
                                set(handles.Analysis(8),'CData',handles.Store.Pause1a);
                                set(handles.Analysis(9),'CData',handles.Store.Stop0a);
                                set(handles.Sidebar(4),'CData',handles.Store.Start0b);
                                set(handles.Sidebar(5),'CData',handles.Store.Pause1b,'Value',1);
                                set(handles.Sidebar(6),'CData',handles.Store.Stop0b);
                            else
                                set(findobj('Tag','Stop'),'Value',1)
                                set(handles.Analysis(7),'CData',handles.Store.Start0a);
                                set(handles.Analysis(8),'CData',handles.Store.Pause0a);
                                set(handles.Analysis(9),'CData',handles.Store.Stop1a);
                                set(handles.Sidebar(4),'CData',handles.Store.Start0b);
                                set(handles.Sidebar(5),'CData',handles.Store.Pause0b);
                                set(handles.Sidebar(6),'CData',handles.Store.Stop1b,'Value',1);
                            end
                            %Bring figures to front
                            figure(findobj('Tag','ErrMon'));
                            if ~isempty(findobj('Tag','StructOutDOF2'))
                                figure(findobj('Tag','StructOutDOF2'));
                            end
                            figure(findobj('Tag','StructOutDOF1'));
                    end
            end
            guidata(findobj('Tag','OpenFrescoExpress'), handles);
        end
        
    case 'generate report'
        %display report summarizing inputs
        Report;
        DIR = which('OpenFrescoExpress.m');
        DIR = DIR(1:end-20);
        reportText = fileread(fullfile(DIR,'OPFReport.txt'));
        GUI_Output(reportText,'Input Summary','Report');
        
    case 'new test'
        handles.Model.StopFlag = 0;
        handles.Model.firstStart = 1;
        %Clear existing figures
        if ~isempty(findobj('Tag','ErrMon'))
            close(findobj('Tag','ErrMon'))
        end
        if ~isempty(findobj('Tag','StructOutDOF1'))
            close(findobj('Tag','StructOutDOF1'))
        end
        if ~isempty(findobj('Tag','StructOutDOF2'))
            close(findobj('Tag','StructOutDOF2'))
        end
        if ~isempty(findobj('Tag','StructAnim'))
            close(findobj('Tag','StructAnim'))
        end
        if ~isempty(findobj('Tag','AnalysisControls'))
            close(findobj('Tag','AnalysisControls'))
        end
        
        %Check for .tcl and report files and delete if found
        fclose('all');
        DIR = which('OpenFrescoExpress.m');
        DIR = DIR(1:end-20);
        if exist(fullfile(DIR,'OPFAnalysis.tcl'),'file')
            delete(fullfile(DIR,'OPFAnalysis.tcl'));
        end
        if exist(fullfile(DIR,'OPFReport.txt'),'file')
            delete(fullfile(DIR,'OPFReport.txt'));
        end
        
        %Return user to structure page
        tabs = get(handles.Sidebar(1),'Children');
        set(handles.Sidebar(1),'SelectedObject',tabs(5));
        set(handles.Sidebar(7),'CData',handles.Store.Structure1);
        set(handles.Sidebar(8),'CData',handles.Store.Loading0);
        set(handles.Sidebar(9),'CData',handles.Store.ExpSetup0);
        set(handles.Sidebar(10),'CData',handles.Store.ExpControl0);
        set(handles.Sidebar(11),'CData',handles.Store.Analysis0);
        set(handles.Structure,'Visible','on');
        set(handles.Structure(handles.Model.StructActive),'Visible','on');
        set(handles.Structure(handles.Model.StructInactive),'Visible','off');
        set(handles.Structure([10 18 26]),'Visible','off');
        set(handles.GroundMotions,'Visible','off');
        set(get(handles.GroundMotions(7), 'Children'), 'Visible', 'off');
        set(get(handles.GroundMotions(8), 'Children'), 'Visible', 'off');
        set(get(handles.GroundMotions(9), 'Children'), 'Visible', 'off');
        set(get(handles.GroundMotions(15), 'Children'), 'Visible', 'off');
        set(get(handles.GroundMotions(16), 'Children'), 'Visible', 'off');
        set(get(handles.GroundMotions(17), 'Children'), 'Visible', 'off');
        set(handles.ES,'Visible','off');
        set(handles.EC,'Visible','off');
        set(handles.Analysis,'Visible','off');
        set(handles.Analysis(6),'SelectedObject',[]);
        set(handles.Analysis(7),'CData',handles.Store.Start0a);
        set(handles.Analysis(8),'CData',handles.Store.Pause0a);
        set(handles.Analysis(9),'CData',handles.Store.Stop0a);
        %Update handles structure
        guidata(findobj('Tag','OpenFrescoExpress'), handles);
        
    case 'dtAnalysis'
        input_val = str2num(get(gcbo,'String'));
        if input_val > handles.Model.Maxdt
            msgbox(['Time step too large! Must be less than ' num2str(handles.Model.Maxdt)],'Error','error');
            set(handles.Analysis(3),'String',handles.GM.dtAnalysis);
        else
            handles.GM.dtAnalysis = input_val;
        end
        %Update handles structure
        guidata(findobj('Tag','OpenFrescoExpress'), handles);
        
    case 'animate'
        answer = get(gcbo,'String');
        if strcmp(answer,'Yes')
            handles.Store.Animate = 1;
        else
            handles.Store.Animate = 0;
        end
        %Update handles structure
        guidata(findobj('Tag','OpenFrescoExpress'), handles);
end
