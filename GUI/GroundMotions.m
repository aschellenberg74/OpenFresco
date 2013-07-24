function GroundMotions(action,varargin)
%GROUNDMOTIONS to handle user inputs related to the ground motions
% GroundMotions(action,varargin)
%
% action   : selected action on the ground motions page
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

% identify chosen ground motion direction
GM_direction = get(get(gcbo,'Parent'), 'Tag');
handles.GM.store.direction = GM_direction;

% pre-define input values of mass and period range for calculating spectra
m = 1.0;
minT = 0.01;
maxT = 4.99;
numTvalues = 300;

switch action
    % =====================================================================
    case 'tab toggle'
        % switch panel display based on tab selection
        Tab_Selection = get(gcbo,'Tag');
        switch Tab_Selection
            % -------------------------------------------------------------
            case 'Ground Motions'
                handles.GM.loadType = 'Ground Motions';
                set(handles.GroundMotions(19),'Value',1,'CData',handles.Store.GM1);
                set(handles.GroundMotions(20),'Value',0,'CData',handles.Store.IC0);
                set(handles.GroundMotions([2:9 28]),'Visible','on');
                set(handles.GroundMotions(21:27),'Visible','off');
                set(get(handles.GroundMotions(7), 'Children'), 'Visible', 'on');
                set(get(handles.GroundMotions(8), 'Children'), 'Visible', 'on');
                set(get(handles.GroundMotions(9), 'Children'), 'Visible', 'on');
                set(get(handles.GroundMotions(15), 'Children'), 'Visible', 'on');
                set(get(handles.GroundMotions(16), 'Children'), 'Visible', 'on');
                set(get(handles.GroundMotions(17), 'Children'), 'Visible', 'on');
                set(get(handles.GroundMotions(26), 'Children'), 'Visible', 'off');
                if strcmp(handles.Model.Type,'2 DOF B')
                    set(handles.GroundMotions(10:17),'Visible','on');
                    set(handles.GroundMotions(2),'Position',[0.12 0.64 0.35 0.33]);
                    set(handles.GroundMotions(7),'Position',[0.12 0.47 0.35 0.15]);
                    set(handles.GroundMotions(8),'Position',[0.12 0.25 0.35 0.15]);
                    set(handles.GroundMotions(9),'Position',[0.12 0.08 0.35 0.15]);
                    set(findobj('Tag','GMhelp'),'Position',[0.85 0.65 0.083 0.132]);
                    set(findobj('Tag','Scalehelp'),'Position',[0.85 0.35 0.083 0.132]);
                end
                handles.GM.dtAnalysis = min(handles.GM.scaledt);
                set(handles.Analysis(3),'String',num2str(handles.GM.dtAnalysis));
            % -------------------------------------------------------------
            case 'Initial Conditions'
                if isempty(handles.Model.M) || isempty(handles.Model.K)
                    msgbox(sprintf('Please define structural properties before\n setting initial conditions.'),'Error','error');
                end
                handles.GM.loadType = 'Initial Conditions';
                handles.GM.dtAnalysis = 0.005;
                set(handles.Analysis(3),'String',num2str(handles.GM.dtAnalysis));
                set(handles.GroundMotions(19),'Value',0,'CData',handles.Store.GM0);
                set(handles.GroundMotions(20),'Value',1,'CData',handles.Store.IC1);
                set(handles.GroundMotions(2:17),'Visible','off');
                set(handles.GroundMotions(21:27),'Visible','on');
                
                if ~isfield(handles.GM, 'initialDisp') || isempty(handles.GM.initialDisp)
                    if strcmp(handles.Model.Type, '1 DOF')
                        set(handles.GroundMotions(23),'String','Enter displacement here');
                    else
                        set(handles.GroundMotions(23),'String','[U1 U2]');
                    end
                end
                if ~isfield(handles.GM, 'rampTime') || isempty(handles.GM.rampTime)
                    set(handles.GroundMotions(24),'String','');
                end
                if ~isfield(handles.GM, 'vibTime') || isempty(handles.GM.vibTime)
                    set(handles.GroundMotions(25),'String','');
                end
                set(get(handles.GroundMotions(7), 'Children'), 'Visible', 'off');
                set(get(handles.GroundMotions(8), 'Children'), 'Visible', 'off');
                set(get(handles.GroundMotions(9), 'Children'), 'Visible', 'off');
                set(get(handles.GroundMotions(15), 'Children'), 'Visible', 'off');
                set(get(handles.GroundMotions(16), 'Children'), 'Visible', 'off');
                set(get(handles.GroundMotions(17), 'Children'), 'Visible', 'off');
                set(get(handles.GroundMotions(26), 'Children'), 'Visible', 'on');
            % -------------------------------------------------------------
        end
    % =====================================================================
    case 'load'
        [filename, pathname] = uigetfile({'*.txt;*.AT2'});
        % return from function if load file is canceled
        if filename == 0
            return
        % check that damping has been selected
        elseif isempty(handles.Model.Zeta)
            msgbox(sprintf('Must specify damping values before\nloading ground motion!'),'Error','error');
            set(handles.GroundMotions([3 11]),'String','...');
            return
        else
            filepath = fullfile(pathname, filename);
            handles.GM.store.setGM = 1;
            % switch based on which direction is chosen
            switch GM_direction
                % ---------------------------------------------------------
                case 'Direction 1'
                    handles.GM.store.filepath{1} = filepath;
                    set(handles.GroundMotions(3),...
                        'String',filepath,...
                        'TooltipString',filepath);
                    
                    [dt databaseType] = ReadWriteTHFile('readDT',filepath);
                    [t ag] = ReadWriteTHFile('readTHF',filepath);
                    handles.GM.databaseType{1} = databaseType;
                    handles.GM.dt(1) = dt;
                    handles.GM.t{1} = t;
                    handles.GM.ag{1} = ag;
                    set(handles.GroundMotions(28),'String',num2str(dt));
                    if strcmp(databaseType,'UNKNOWN')
                        set(handles.GroundMotions(28),'Style','edit','BackgroundColor',[1 1 1]);
                        handles.GM.t{1} = dt*t;
                        handles.GM.AmpFact(1) = 1.0;
                    else
                        set(handles.GroundMotions(28),'Style','text','BackgroundColor',[0.941176 0.941176 0.941176]);
                        handles.GM.AmpFact(1) = 32.174*12;
                    end
                    set(handles.GroundMotions(4),'String',handles.GM.AmpFact(1));
                    
                    % calculate scaled values
                    handles.GM.scaledt(1) = handles.GM.TimeFact(1).*handles.GM.dt(1);
                    handles.GM.scalet{1}  = handles.GM.TimeFact(1).*handles.GM.t{1};
                    handles.GM.scaleag{1} = handles.GM.AmpFact(1).*handles.GM.ag{1};
                    handles.GM.scalevg{1} = handles.GM.scaledt(1)*cumtrapz(handles.GM.scaleag{1});
                    handles.GM.scaledg{1} = handles.GM.scaledt(1)*cumtrapz(handles.GM.scalevg{1});
                    
                    % calculate spectral response quantities and plot
                    handles.GM.Spectra{1} = ResponseSpectraElastic(handles.GM.scaleag{1},handles.GM.scaledt(1),m,handles.Model.Zeta,minT,maxT,numTvalues);
                    plot(handles.GroundMotions(7), handles.GM.scalet{1}, handles.GM.scaleag{1});
                    plot(handles.GroundMotions(8), handles.GM.Spectra{1}.T, handles.GM.Spectra{1}.psdAcc);
                    plot(handles.GroundMotions(9), handles.GM.Spectra{1}.T, handles.GM.Spectra{1}.dsp);
                    
                    % store analysis dt
                    handles.GM.dtAnalysis = min(handles.GM.scaledt);
                    set(handles.Analysis(3),'String',num2str(handles.GM.dtAnalysis));
                    guidata(gcbf,handles);
                % ---------------------------------------------------------
                case 'Direction 2'
                    handles.GM.store.filepath{2} = filepath;
                    set(handles.GroundMotions(11),...
                        'String',filepath,...
                        'TooltipString',filepath);
                    
                    [dt databaseType] = ReadWriteTHFile('readDT',filepath);
                    [t ag] = ReadWriteTHFile('readTHF',filepath);
                    handles.GM.databaseType{2} = databaseType;
                    handles.GM.dt(2) = dt;
                    handles.GM.t{2} = t;
                    handles.GM.ag{2} = ag;
                    set(handles.GroundMotions(29),'String',num2str(dt));
                    if strcmp(databaseType,'UNKNOWN')
                        set(handles.GroundMotions(29),'Style','edit','BackgroundColor',[1 1 1]);
                        handles.GM.t{2} = dt*t;
                        handles.GM.AmpFact(2) = 1.0;
                    else
                        set(handles.GroundMotions(29),'Style','text','BackgroundColor',[0.941176 0.941176 0.941176]);
                        handles.GM.AmpFact(2) = 32.174*12;
                    end
                    set(handles.GroundMotions(12),'String',handles.GM.AmpFact(2));
                    
                    % calculate scaled values
                    handles.GM.scaledt(2) = handles.GM.dt(2);
                    handles.GM.scalet{2}  = handles.GM.TimeFact(2).*handles.GM.t{2};
                    handles.GM.scaleag{2} = handles.GM.AmpFact(2).*handles.GM.ag{2};
                    handles.GM.scalevg{2} = handles.GM.scaledt(2)*cumtrapz(handles.GM.scaleag{2});
                    handles.GM.scaledg{2} = handles.GM.scaledt(2)*cumtrapz(handles.GM.scalevg{2});
                    
                    % calculate spectral response quantities and plot
                    handles.GM.Spectra{2} = ResponseSpectraElastic(handles.GM.scaleag{2},handles.GM.scaledt(2),m,handles.Model.Zeta,minT,maxT,numTvalues);
                    plot(handles.GroundMotions(15), handles.GM.scalet{2}, handles.GM.scaleag{2});
                    plot(handles.GroundMotions(16), handles.GM.Spectra{2}.T, handles.GM.Spectra{2}.psdAcc);
                    plot(handles.GroundMotions(17), handles.GM.Spectra{2}.T, handles.GM.Spectra{2}.dsp);
                    
                    % store analysis dt
                    handles.GM.dtAnalysis = min(handles.GM.scaledt);
                    set(handles.Analysis(3),'String',num2str(handles.GM.dtAnalysis));
                    guidata(gcbf, handles);
                % ---------------------------------------------------------
            end
        end
    % =====================================================================
    case 'manual load'
        filepath = get(gcbo,'String');
        if exist(filepath,'file')
            % check that damping has been selected
            if isempty(handles.Model.Zeta)
                msgbox(sprintf('Must specify damping values before\nloading ground motion!'),'Error','error');
                set(handles.GroundMotions([3 11]),'String','...');
                return
            end
            % switch based on which direction is chosen
            switch GM_direction
                % ---------------------------------------------------------
                case 'Direction 1'
                    handles.GM.store.filepath{1} = filepath;
                    set(handles.GroundMotions(3),...
                        'String',filepath,...
                        'TooltipString',filepath);
                    
                    [dt databaseType] = ReadWriteTHFile('readDT',filepath);
                    [t ag] = ReadWriteTHFile('readTHF',filepath);
                    handles.GM.databaseType = databaseType;
                    handles.GM.dt(1) = dt;
                    handles.GM.t{1} = t;
                    handles.GM.ag{1} = ag;
                    set(handles.GroundMotions(28),'String',num2str(dt));
                    if strcmp(databaseType,'UNKNOWN')
                        set(handles.GroundMotions(28),'Style','edit','BackgroundColor',[1 1 1]);
                        handles.GM.t{1} = dt*t;
                    else
                        set(handles.GroundMotions(28),'Style','text','BackgroundColor',[0.941176 0.941176 0.941176]);
                    end
                    
                    % calculate scaled values
                    handles.GM.scaledt(1) = handles.GM.TimeFact(1).*handles.GM.dt(1);
                    handles.GM.scalet{1}  = handles.GM.TimeFact(1).*handles.GM.t{1};
                    handles.GM.scaleag{1} = handles.GM.AmpFact(1).*handles.GM.ag{1};
                    handles.GM.scalevg{1} = handles.GM.scaledt(1)*cumtrapz(handles.GM.scaleag{1});
                    handles.GM.scaledg{1} = handles.GM.scaledt(1)*cumtrapz(handles.GM.scalevg{1});
                    
                    % store analysis dt
                    handles.GM.dtAnalysis = min(handles.GM.dt);
                    set(handles.Analysis(3),'String',num2str(handles.GM.dtAnalysis));
                    
                    % calculate spectral response quantities and plot
                    handles.GM.Spectra{1} = ResponseSpectraElastic(handles.GM.scaleag{1},handles.GM.scaledt(1),m,handles.Model.Zeta,minT,maxT,numTvalues);
                    plot(handles.GroundMotions(7), handles.GM.scalet{1}, handles.GM.scaleag{1});
                    plot(handles.GroundMotions(8), handles.GM.Spectra{1}.T, handles.GM.Spectra{1}.psdAcc);
                    plot(handles.GroundMotions(9), handles.GM.Spectra{1}.T, handles.GM.Spectra{1}.dsp);
                    guidata(gcbf, handles);
                % ---------------------------------------------------------
                case 'Direction 2'
                    handles.GM.store.filepath{2} = filepath;
                    set(handles.GroundMotions(11),...
                        'String',filepath,...
                        'TooltipString',filepath);
                    
                    [dt databaseType] = ReadWriteTHFile('readDT',filepath);
                    [t ag] = ReadWriteTHFile('readTHF',filepath);
                    handles.GM.databaseType{2} = databaseType;
                    handles.GM.dt(2) = dt;
                    handles.GM.t{2} = t;
                    handles.GM.ag{2} = ag;
                    set(handles.GroundMotions(29),'String',num2str(dt));
                    if strcmp(databaseType,'Unknown')
                        set(handles.GroundMotions(29),'Style','edit','BackgroundColor',[1 1 1]);
                        handles.GM.t{2} = dt*t;
                    else
                        set(handles.GroundMotions(29),'Style','text','BackgroundColor',[0.941176 0.941176 0.941176]);
                    end
                    
                    % calculate scaled values
                    handles.GM.scaledt(2) = handles.GM.TimeFact(2).*handles.GM.dt(2);
                    handles.GM.scalet{2}  = handles.GM.TimeFact(2).*handles.GM.t{2};
                    handles.GM.scaleag{2} = handles.GM.AmpFact(2).*handles.GM.ag{2};
                    handles.GM.scalevg{2} = handles.GM.scaledt(2)*cumtrapz(handles.GM.scaleag{2});
                    handles.GM.scaledg{2} = handles.GM.scaledt(2)*cumtrapz(handles.GM.scalevg{2});
                    
                    % store analysis dt
                    handles.GM.dtAnalysis = min(handles.GM.dt);
                    set(handles.Analysis(3),'String',num2str(handles.GM.dtAnalysis));
                    
                    % calculate spectral response quantities and plot
                    handles.GM.Spectra{2} = ResponseSpectraElastic(handles.GM.scaleag{2},handles.GM.scaledt(2),m,handles.Model.Zeta,minT,maxT,numTvalues);
                    plot(handles.GroundMotions(15), handles.GM.scalet{2}, handles.GM.scaleag{2});
                    plot(handles.GroundMotions(16), handles.GM.Spectra{2}.T, handles.GM.Spectra{2}.psdAcc);
                    plot(handles.GroundMotions(17), handles.GM.Spectra{2}.T, handles.GM.Spectra{2}.dsp);
                    guidata(gcbf, handles);
                % ---------------------------------------------------------
            end
        else
            % return if filepath is invalid
            msgbox('Invalid file path specified','Path Not Found','warn')
            return
        end
    % =====================================================================
    case 'manual dt'
        cbo_tag = get(gcbo,'Tag');
        switch cbo_tag
            % -------------------------------------------------------------
            case 'dt1'
                % adjust dt for GM 1
                if str2num(get(gcbo,'String')) <= 0
                    msgbox('Time step must be positive!','Error','error');
                    set(handles.GroundMotions(28),'String',num2str(handles.GM.dt(1)));
                    return
                end
                handles.GM.dt(1) = str2num(get(gcbo,'String'));
                guidata(gcbf,handles);
                
                t = (0:length(handles.GM.ag{1})-1)'*handles.GM.dt(1);
                handles.GM.t{1} = t;
                
                % calculate scaled values
                handles.GM.scaledt(1) = handles.GM.TimeFact(1).*handles.GM.dt(1);
                handles.GM.scalet{1}  = handles.GM.TimeFact(1).*handles.GM.t{1};
                handles.GM.scaleag{1} = handles.GM.AmpFact(1).*handles.GM.ag{1};
                handles.GM.scalevg{1} = handles.GM.scaledt(1)*cumtrapz(handles.GM.scaleag{1});
                handles.GM.scaledg{1} = handles.GM.scaledt(1)*cumtrapz(handles.GM.scalevg{1});
                
                % calculate spectral response quantities and plot
                handles.GM.Spectra{1} = ResponseSpectraElastic(handles.GM.scaleag{1},handles.GM.scaledt(1),m,handles.Model.Zeta,minT,maxT,numTvalues);
                plot(handles.GroundMotions(7), handles.GM.scalet{1}, handles.GM.scaleag{1});
                plot(handles.GroundMotions(8), handles.GM.Spectra{1}.T, handles.GM.Spectra{1}.psdAcc);
                plot(handles.GroundMotions(9), handles.GM.Spectra{1}.T, handles.GM.Spectra{1}.dsp);
                
                % store analysis dt
                handles.GM.dtAnalysis = min(handles.GM.dt);
                set(handles.Analysis(3),'String',num2str(handles.GM.dtAnalysis));
                guidata(gcbf, handles);
            % -------------------------------------------------------------
            case 'dt2'
                % adjust dt for GM 2
                if str2num(get(gcbo,'String')) <= 0
                    msgbox('Time step must be positive!','Error','error');
                    set(handles.GroundMotions(29),'String',num2str(handles.GM.dt(1)));
                    return
                end
                handles.GM.dt(2) = str2num(get(gcbo,'String'));
                guidata(gcbf,handles);
                
                t = (0:length(handles.GM.ag{2})-1)'*handles.GM.dt(2);
                handles.GM.t{2} = t;
                
                % calculate scaled values
                handles.GM.scaledt(2) = handles.GM.TimeFact(2).*handles.GM.dt(2);
                handles.GM.scalet{2}  = handles.GM.TimeFact(2).*handles.GM.t{2};
                handles.GM.scaleag{2} = handles.GM.AmpFact(2).*handles.GM.ag{2};
                handles.GM.scalevg{2} = handles.GM.scaledt(2)*cumtrapz(handles.GM.scaleag{2});
                handles.GM.scaledg{2} = handles.GM.scaledt(2)*cumtrapz(handles.GM.scalevg{2});
                
                % calculate spectral response quantities and plot
                handles.GM.Spectra{2} = ResponseSpectraElastic(handles.GM.scaleag{2},handles.GM.scaledt(2),m,handles.Model.Zeta,minT,maxT,numTvalues);
                plot(handles.GroundMotions(15), handles.GM.scalet{2}, handles.GM.scaleag{2});
                plot(handles.GroundMotions(16), handles.GM.Spectra{2}.T, handles.GM.Spectra{2}.psdAcc);
                plot(handles.GroundMotions(17), handles.GM.Spectra{2}.T, handles.GM.Spectra{2}.dsp);
                
                % store analysis dt
                handles.GM.dtAnalysis = min(handles.GM.dt);
                set(handles.Analysis(3),'String',num2str(handles.GM.dtAnalysis));
                guidata(gcbf, handles);
            % -------------------------------------------------------------
        end
    % =====================================================================
    case 'scale'
        % check for necessary ground motions
        if strcmp(GM_direction,'Direction 1')
            if length(handles.GM.t{1}) == 1
                msgbox('Please load a ground motion first!','Error','error');
                return
            end
        elseif strcmp(GM_direction,'Direction 2')
            if length(handles.GM.t) == 1
                msgbox('Please load a ground motion first!','Error','error');
                return
            end
        end
        cbo_tag = get(gcbo,'Tag');
        switch cbo_tag
            % -------------------------------------------------------------
            case 'edit_amp1'
                input_val = str2num(get(gcbo,'String'));
                handles.GM.AmpFact(1) = input_val;
                handles.GM.scaleag{1} = handles.GM.ag{1}.*handles.GM.AmpFact(1);
                handles.GM.scalevg{1} = handles.GM.scaledt(1)*cumtrapz(handles.GM.scaleag{1});
                handles.GM.scaledg{1} = handles.GM.scaledt(1)*cumtrapz(handles.GM.scalevg{1});
                % calculate spectral response quantities and plot
                handles.GM.Spectra{1} = ResponseSpectraElastic(handles.GM.scaleag{1},handles.GM.scaledt(1),m,handles.Model.Zeta,minT,maxT,numTvalues);
                plot(handles.GroundMotions(7), handles.GM.scalet{1}, handles.GM.scaleag{1});
                plot(handles.GroundMotions(8), handles.GM.Spectra{1}.T, handles.GM.Spectra{1}.psdAcc);
                plot(handles.GroundMotions(9), handles.GM.Spectra{1}.T, handles.GM.Spectra{1}.dsp);
                guidata(gcbf, handles);
            % -------------------------------------------------------------
            case 'edit_time1'
                input_val = str2num(get(gcbo,'String'));
                handles.GM.TimeFact(1) = input_val;
                handles.GM.scalet{1}  = handles.GM.t{1}.*handles.GM.TimeFact(1);
                handles.GM.scaledt(1) = handles.GM.dt(1).*handles.GM.TimeFact(1);
                handles.GM.scalevg{1} = handles.GM.scaledt(1)*cumtrapz(handles.GM.scaleag{1});
                handles.GM.scaledg{1} = handles.GM.scaledt(1)*cumtrapz(handles.GM.scalevg{1});
                handles.GM.dtAnalysis = min(handles.GM.scaledt);
                set(handles.Analysis(3),'String',num2str(handles.GM.dtAnalysis));
                % calculate spectral response quantities and plot
                handles.GM.Spectra{1} = ResponseSpectraElastic(handles.GM.scaleag{1},handles.GM.scaledt(1),m,handles.Model.Zeta,minT,maxT,numTvalues);
                plot(handles.GroundMotions(7), handles.GM.scalet{1}, handles.GM.scaleag{1});
                plot(handles.GroundMotions(8), handles.GM.Spectra{1}.T, handles.GM.Spectra{1}.psdAcc);
                plot(handles.GroundMotions(9), handles.GM.Spectra{1}.T, handles.GM.Spectra{1}.dsp);
                guidata(gcbf, handles);
            % -------------------------------------------------------------
            case 'edit_amp2'
                input_val = str2num(get(gcbo,'String'));
                handles.GM.AmpFact(2) = input_val;
                handles.GM.scaleag{2} = handles.GM.ag{2}.*handles.GM.AmpFact(2);
                handles.GM.scalevg{2} = handles.GM.scaledt(2)*cumtrapz(handles.GM.scaleag{2});
                handles.GM.scaledg{2} = handles.GM.scaledt(2)*cumtrapz(handles.GM.scalevg{2});
                % calculate spectral response quantities and plot
                handles.GM.Spectra{2} = ResponseSpectraElastic(handles.GM.scaleag{2},handles.GM.scaledt(2),m,handles.Model.Zeta,minT,maxT,numTvalues);
                plot(handles.GroundMotions(15), handles.GM.scalet{2}, handles.GM.scaleag{2});
                plot(handles.GroundMotions(16), handles.GM.Spectra{2}.T, handles.GM.Spectra{2}.psdAcc);
                plot(handles.GroundMotions(17), handles.GM.Spectra{2}.T, handles.GM.Spectra{2}.dsp);
                guidata(gcbf, handles);
            % -------------------------------------------------------------
            case 'edit_time2'
                input_val = str2num(get(gcbo,'String'));
                handles.GM.TimeFact(2) = input_val;
                handles.GM.scalet{2}  = handles.GM.t{2}.*handles.GM.TimeFact(2);
                handles.GM.scaledt(2) = handles.GM.dt(2).*handles.GM.TimeFact(2);
                handles.GM.scalevg{2} = handles.GM.scaledt(2)*cumtrapz(handles.GM.scaleag{2});
                handles.GM.scaledg{2} = handles.GM.scaledt(2)*cumtrapz(handles.GM.scalevg{2});
                handles.GM.dtAnalysis = min(handles.GM.scaledt);
                set(handles.Analysis(3),'String',num2str(handles.GM.dtAnalysis));
                % calculate spectral response quantities and plot
                handles.GM.Spectra{2} = ResponseSpectraElastic(handles.GM.scaleag{2},handles.GM.scaledt(2),m,handles.Model.Zeta,minT,maxT,numTvalues);
                plot(handles.GroundMotions(15), handles.GM.scalet{2}, handles.GM.scaleag{2});
                plot(handles.GroundMotions(16), handles.GM.Spectra{2}.T, handles.GM.Spectra{2}.psdAcc);
                plot(handles.GroundMotions(17), handles.GM.Spectra{2}.T, handles.GM.Spectra{2}.dsp);
                guidata(gcbf, handles);
            % -------------------------------------------------------------
        end
    % =====================================================================
    case 'initialDispType'
        % adjust display of initial displacement options
        options = get(gcbo,'String');
        modeSelection = options(get(gcbo,'Value'));
        switch modeSelection{1}
            % -------------------------------------------------------------
            case 'Choose Mode...'
                if strcmp(handles.Model.Type, '1 DOF')
                    set(handles.GroundMotions(23),'String','Enter displacement here');
                else
                    set(handles.GroundMotions(23),'String','[U1 U2]');
                end
                handles.GM.initialDisp = [];
            % -------------------------------------------------------------
            case 'Mode 1'
                set(handles.GroundMotions(23),'String',num2str(handles.Model.Modes(:,1)'));
                handles.GM.initialDisp = handles.Model.Modes(:,1)';
            % -------------------------------------------------------------
            case 'Mode 2'
                set(handles.GroundMotions(23),'String',num2str(handles.Model.Modes(:,2)'));
                handles.GM.initialDisp = handles.Model.Modes(:,2)';
            % -------------------------------------------------------------
            case 'User Defined'
                if strcmp(handles.Model.Type, '1 DOF')
                    set(handles.GroundMotions(23),'String','Enter displacement here');
                else
                    set(handles.GroundMotions(23),'String','[U1 U2]');
                end
                handles.GM.initialDisp = [];
            % -------------------------------------------------------------
        end
    % =====================================================================
    case 'initialDisp'
        input_val = str2num(get(gcbo,'String'));
        if strcmp(handles.Model.Type, '1 DOF')
            if length(input_val) ~= 1
                msgbox('Please specify a single displacement value','Error','error');
            elseif isempty(input_val) || (input_val==0)
                msgbox('Invalid initial displacement specified!','Error','error');
                handles.GM.initialDisp = [];
            else
                handles.GM.initialDisp = input_val;
            end
        else
            if length(input_val) ~= 2
                msgbox('Please specify two displacement values','Error','error');
            elseif isempty(input_val) || (input_val(1)==0) || (input_val(2)==0)
                msgbox('Invalid initial displacement specified!','Error','error');
                handles.GM.initialDisp = [];
            else
                handles.GM.initialDisp = input_val;
            end
        end
    % =====================================================================
    case 'rampTime'
        input_val = str2num(get(gcbo,'String'));
        if length(input_val) ~= 1
            msgbox('Please specify a single ramp time value','Error','error');
        elseif isempty(input_val) || (input_val<=0)
            msgbox('Invalid ramp time specified!','Error','error');
            handles.GM.rampTime = [];
        else
            handles.GM.rampTime = input_val;
        end
    % =====================================================================
    case 'vibTime'
        input_val = str2num(get(gcbo,'String'));
        if length(input_val) ~= 1
            msgbox('Please specify a single vibration time value','Error','error');
        elseif isempty(input_val) || (input_val<=0)
            msgbox('Invalid vibration time specified!','Error','error');
            handles.GM.vibTime = [];
        else
            handles.GM.vibTime = input_val;
        end
    % =====================================================================
end

% update handles structure
guidata(gcbf,handles);
