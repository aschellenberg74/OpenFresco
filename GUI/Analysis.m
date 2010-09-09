function Analysis(action, varargin)
% Analysis stores the callbacks for the varous buttons on the analysis page

%profile on;

%  Initialization tasks
handles = guidata(findobj('Tag','OpenFresco Quick Start'));

%Store which button is pressed
analysis_option = get(gcbo,'String');

switch action
    case 'choose option'
        %If the analysis has already been stopped (StopFlag == 1)
        if handles.Model.StopFlag == 1
            msgbox(sprintf('Experiment has ended.\nChoose "Run New Test" to start a new test.'),'Error','error');
            set(handles.Sidebar(6),'Value',1);
            set(handles.Analysis(9),'Value',1);            
            set(handles.Analysis(9),'CData',handles.Store.stopButSelect1);
            set(handles.Analysis(7:8),'CData',handles.Store.analysisBut1);
            set(handles.Sidebar(6),'CData',handles.Store.stopButSelect2);
            set(handles.Sidebar(4:5),'CData',handles.Store.analysisBut2);
            %Otherwise...
        else
            switch analysis_option
                case 'Start'
                    set(handles.Analysis(7),'CData',handles.Store.startButSelect1);
                    handles.Store.AnalysisOption = analysis_option;
                    guidata(findobj('Tag','OpenFresco Quick Start'), handles);
                    disp('Starting...');
                    if handles.Model.firstStart
                        handles.Model.firstStart = 0;
                        guidata(findobj('Tag','OpenFresco Quick Start'), handles);
                        GUI_ErrorMonitors;
                        GUI_StructuralOutput;
                        GUI_AnalysisControls
                        handles = guidata(findobj('Tag','OpenFresco Quick Start'));
                        set(handles.Sidebar(4),'CData',handles.Store.startButSelect2);
                        
                        %Initialize plots
                        switch handles.Model.Type
                            case '1 DOF'
                                handles.Plots.SO1dplot  = line(0,0,'LineWidth',1.0,'Parent',findobj('Tag','SO1d'));
                                handles.Plots.SO1fplot  = line(0,0,'LineWidth',1.0,'Parent',findobj('Tag','SO1f'));
                                handles.Plots.SO1aplot  = line(0,0,'LineWidth',1.0,'Parent',findobj('Tag','SO1a'));
                                handles.Plots.SO1fdplot = line(0,0,'LineWidth',1.0,'Parent',findobj('Tag','SO1fd'));
                                handles.Plots.SO1fddot  = line(0,0,'Color','red','Marker','o','LineWidth',1.0,'Parent',findobj('Tag','SO1fd'));
                                handles.Plots.SO1agplot = line(handles.GM.t{1},handles.GM.scaleag{1},'LineWidth',1.0,'Parent',findobj('Tag','SO1ag'));
                                handles.Plots.SO1agdot  = line(0,0,'Color','red','Marker','o','LineWidth',1.0,'Parent',findobj('Tag','SO1ag'));
                                
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
                                handles.Plots.SO1agplot = line(handles.GM.t{1},handles.GM.scaleag{1},'LineWidth',1.0,'Parent',findobj('Tag','SO1ag'));
                                handles.Plots.SO1agdot  = line(0,0,'Color','red','Marker','o','LineWidth',1.0,'Parent',findobj('Tag','SO1ag'));                                
                                handles.Plots.SO1ddplot = line(0,0,'LineWidth',1.0,'Parent',findobj('Tag','SO1dd'));                                
                                handles.Plots.SO1dddot  = line(0,0,'Color','red','Marker','o','LineWidth',1.0,'Parent',findobj('Tag','SO1dd'));
                                handles.Plots.SO2dplot  = line(0,0,'LineWidth',1.0,'Parent',findobj('Tag','SO2d'));
                                handles.Plots.SO2fplot  = line(0,0,'LineWidth',1.0,'Parent',findobj('Tag','SO2f'));
                                handles.Plots.SO2aplot  = line(0,0,'LineWidth',1.0,'Parent',findobj('Tag','SO2a'));
                                handles.Plots.SO2fdplot = line(0,0,'LineWidth',1.0,'Parent',findobj('Tag','SO2fd'));
                                handles.Plots.SO2fddot  = line(0,0,'Color','red','Marker','o','LineWidth',1.0,'Parent',findobj('Tag','SO2fd'));
                                handles.Plots.SO2agplot = line(handles.GM.t{1},handles.GM.scaleag{1},'LineWidth',1.0,'Parent',findobj('Tag','SO2ag'));
                                handles.Plots.SO2agdot  = line(0,0,'Color','red','Marker','o','LineWidth',1.0,'Parent',findobj('Tag','SO2ag'));
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
                                handles.Plots.SO1agplot = line(handles.GM.t{1},handles.GM.scaleag{1},'LineWidth',1.0,'Parent',findobj('Tag','SO1ag'));
                                handles.Plots.SO1agdot  = line(0,0,'Color','red','Marker','o','LineWidth',1.0,'Parent',findobj('Tag','SO1ag'));
                                handles.Plots.SO1ddplot = line(0,0,'LineWidth',1.0,'Parent',findobj('Tag','SO1dd'));
                                handles.Plots.SO1dddot  = line(0,0,'Color','red','Marker','o','LineWidth',1.0,'Parent',findobj('Tag','SO1dd'));
                                handles.Plots.SO2dplot  = line(0,0,'LineWidth',1.0,'Parent',findobj('Tag','SO2d'));
                                handles.Plots.SO2fplot  = line(0,0,'LineWidth',1.0,'Parent',findobj('Tag','SO2f'));
                                handles.Plots.SO2aplot  = line(0,0,'LineWidth',1.0,'Parent',findobj('Tag','SO2a'));
                                handles.Plots.SO2fdplot = line(0,0,'LineWidth',1.0,'Parent',findobj('Tag','SO2fd'));
                                handles.Plots.SO2fddot  = line(0,0,'Color','red','Marker','o','LineWidth',1.0,'Parent',findobj('Tag','SO2fd'));
                                handles.Plots.SO2agplot = line(handles.GM.t{2},handles.GM.scaleag{2},'LineWidth',1.0,'Parent',findobj('Tag','SO2ag'));
                                handles.Plots.SO2agdot  = line(0,0,'Color','red','Marker','o','LineWidth',1.0,'Parent',findobj('Tag','SO2ag'));
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
                        guidata(findobj('Tag','OpenFresco Quick Start'), handles);
                        DIR = handles.Model.DIR;
                        RunOpenFresco(fullfile(DIR,'OPS & OPF'),fullfile(DIR,'OPFAnalysis.tcl'));
                        clear functions;
                        handles.Response = Integrator_NewmarkExplicit(handles.Model,handles.GM,[],handles.Analysis);
                        set(findobj('Tag','StopControl'),'Value',1);
                        set(handles.Analysis(9),'Value',1);
                        set(handles.Analysis(9),'CData',handles.Store.stopButSelect1);
                        set(handles.Analysis(7:8),'CData',handles.Store.analysisBut1);
                        set(handles.Sidebar(6),'CData',handles.Store.stopButSelect2);
                        set(handles.Sidebar(4:5),'CData',handles.Store.analysisBut2);
                        handles.Model.StopFlag = 1;
                        handles.Store.AnalysisOption = 'Stop';
                        saveResults = questdlg(sprintf('Analysis complete!\nWould you like to save the test results?'),'Save?','Yes','No','Yes');
                        switch saveResults
                            case 'Yes'
                                Response = handles.Response;
                                uisave('Response');
                            case 'No'
                        end                        
                    else
                        set(findobj('Tag','StartControl'),'Value',1);
                        set(handles.Analysis(7),'Value',1);
                        set(handles.Analysis(7),'CData',handles.Store.startButSelect1);
                        set(handles.Analysis(8:9),'CData',handles.Store.analysisBut1);
                        set(handles.Sidebar(4),'CData',handles.Store.startButSelect2);
                        set(handles.Sidebar(5:6),'CData',handles.Store.analysisBut2);
                        figure(findobj('Tag','ErrMon'));
                        if ~isempty(findobj('Tag','StructOutDOF2'));
                            figure(findobj('Tag','StructOutDOF2'));
                        end
                        figure(findobj('Tag','StructOutDOF1'));
                        figure(findobj('Tag','AnalysisControls'));
                    end
                    
                case 'Pause'
                    handles.Store.AnalysisOption = analysis_option;
                    disp('Paused...');
                    set(findobj('Tag','PauseControl'),'Value',1);
                    set(handles.Analysis(8),'Value',1);
                    set(handles.Analysis(8),'CData',handles.Store.pauseButSelect1);
                    set(handles.Analysis([7 9]),'CData',handles.Store.analysisBut1);
                    set(handles.Sidebar(5),'CData',handles.Store.pauseButSelect2);
                    set(handles.Sidebar([4 6]),'CData',handles.Store.analysisBut2);
                    figure(findobj('Tag','ErrMon'));
                    if ~isempty(findobj('Tag','StructOutDOF2'))
                        figure(findobj('Tag','StructOutDOF2'));
                    end
                    figure(findobj('Tag','StructOutDOF1'));
                    
                case 'Stop'
                    set(findobj('Tag','StopControl'),'Value',1);
                    set(handles.Analysis(9),'Value',1);
                    set(handles.Analysis(9),'CData',handles.Store.stopButSelect1);
                    set(handles.Analysis(7:8),'CData',handles.Store.analysisBut1);
                    set(handles.Sidebar(6),'CData',handles.Store.stopButSelect2);
                    set(handles.Sidebar(4:5),'CData',handles.Store.analysisBut2);
                    %Bring figures to front
                    figure(findobj('Tag','ErrMon'));
                    if ~isempty(findobj('Tag','StructOutDOF2'))
                        figure(findobj('Tag','StructOutDOF2'));
                    end
                    figure(findobj('Tag','StructOutDOF1'));
                    
                    stop_option = questdlg('How would you like to proceed?', ...
                        'Stop Test', ...
                        'Unload', 'Save State', 'Cancel', 'Unload');
                    handles.Store.StopOption = stop_option;
                    set(findobj('Tag','StopControl'),'Value',1);
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
                            id = find(strcmp(handles.Store.AnalysisOption, get(handles.Analysis(7:8),'String')) == 1);
                            set(handles.Analysis(id+6),'Value',1);
                            if id == 1
                                set(findobj('Tag','StartControl'),'Value',1);
                                set(handles.Analysis(7),'CData',handles.Store.startButSelect1);
                                set(handles.Analysis(8:9),'CData',handles.Store.analysisBut1);
                                set(handles.Sidebar(4),'CData',handles.Store.startButSelect2);
                                set(handles.Sidebar(5:6),'CData',handles.Store.analysisBut2);
                            elseif id == 2
                                set(findobj('Tag','PauseControl'),'Value',1)
                                set(handles.Analysis(8),'CData',handles.Store.pauseButSelect1);
                                set(handles.Analysis([7 9]),'CData',handles.Store.analysisBut1);
                                set(handles.Sidebar(5),'CData',handles.Store.pauseButSelect2);
                                set(handles.Sidebar([4 6]),'CData',handles.Store.analysisBut2);
                            else
                                set(findobj('Tag','StopControl'),'Value',1)
                                set(handles.Analysis(9),'CData',handles.Store.stopButSelect1);
                                set(handles.Analysis(7:8),'CData',handles.Store.analysisBut1);
                                set(handles.Sidebar(6),'CData',handles.Store.stopButSelect2);
                                set(handles.Sidebar(4:5),'CData',handles.Store.analysisBut2);
                            end
                            %Bring figures to front
                            figure(findobj('Tag','ErrMon'));
                            if ~isempty(findobj('Tag','StructOutDOF2'))
                                figure(findobj('Tag','StructOutDOF2'));
                            end
                            figure(findobj('Tag','StructOutDOF1'));
                    end
            end
            guidata(findobj('Tag','OpenFresco Quick Start'), handles);
        end
        
    case 'generate report'
        GUI_Report;
        
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
        if ~isempty(findobj('Tag','AnalysisControls'))
            close(findobj('Tag','AnalysisControls'))
        end    
        
        %Select structure tab
        tabs = get(handles.Sidebar(1),'Children');
        set(handles.Sidebar(1),'SelectedObject',tabs(5));
        set(handles.Sidebar(7),'CData',handles.Store.sideButSelect);
        set(handles.Sidebar(8:11),'CData',handles.Store.sideBut);
        %Return user to structure page using code from GUI_Template
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
        set(handles.Analysis(7:9),'CData',handles.Store.analysisBut1);
        guidata(findobj('Tag','OpenFresco Quick Start'), handles);
        
    case 'dtAnalysis'
        input_val = str2num(get(gcbo,'String'));
        if input_val > handles.Model.Maxdt
            msgbox(['Time step too large! Must be less than ' num2str(handles.Model.Maxdt)],'Error','error');
            set(handles.Analysis(3),'String',handles.GM.dtAnalysis);
        else
            handles.GM.dtAnalysis = input_val;
        end
        
        %Update handles structure
        guidata(findobj('Tag','OpenFresco Quick Start'), handles);
        
end