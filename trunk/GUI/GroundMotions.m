function GroundMotions(action, varargin)
% GroundMotions contains callbacks for the objects on the Ground Motions
% page of the GUI
                 
%  Initialization tasks
handles = guidata(gcbf);

%Identify chosen ground motion direction
GM_direction = get(get(gcbo,'Parent'), 'Tag');
handles.GM.store.direction = GM_direction;

% Pre-define input values of mass and period for calculating spectra
m = 1.0;
minT = 0.01;
maxT = 4.99;
numTvalues = 300;

switch action
    case 'load'
        [filename, pathname] = uigetfile({'*.txt;*.AT2'});
        %Break from function if load file is cancelled
        if filename == 0
            return
        %Otherwise...
        else
            filepath = fullfile(pathname, filename);
            handles.GM.store.setGM = 1;            
            switch GM_direction
            %Switch based on which direction is chosen
                case 'Direction 1'
                    handles.GM.store.filepath{1} = filepath;
                    set(handles.GroundMotions(3),...
                        'String',filepath,...
                        'TooltipString',filepath);
                    
                    dt = ReadWriteTHFileNGA('readDT',filepath);
                    handles.GM.dt(1) = dt;
                    guidata(gcbf,handles);
                    
                    [t ag] = ReadWriteTHFileNGA('readTHF',filepath);
                    data = guidata(gcf);
                    handles.GM.databaseType = data.GM.databaseType;
                    handles.GM.t{1} = t;
                    handles.GM.ag{1} = ag;
                    
                    %Calculate scaled values
                    handles.GM.scaledt(1) = handles.GM.TimeFact(1).*handles.GM.dt(1);
                    handles.GM.scalet{1} = handles.GM.TimeFact(1).*handles.GM.t{1};
                    handles.GM.scaleag{1} = handles.GM.AmpFact(1).*handles.GM.ag{1};
                    
                    %Calculate spectral response quantities and plot
                    handles.GM.Spectra{1} = ResponseSpectraElastic(handles.GM.scaleag{1},handles.GM.scaledt(1),m,handles.Model.Zeta,minT,maxT,numTvalues);
                    plot(handles.GroundMotions(7), handles.GM.scalet{1}, handles.GM.scaleag{1});
                    plot(handles.GroundMotions(8), handles.GM.Spectra{1}.T, handles.GM.Spectra{1}.psdAcc);
                    plot(handles.GroundMotions(9), handles.GM.Spectra{1}.T, handles.GM.Spectra{1}.dsp);
                    
                    %Store analysis dt
                    handles.GM.dtAnalysis = min(handles.GM.dt);
                    set(handles.Analysis(3),'String',[num2str(handles.GM.dtAnalysis)]);
                    guidata(gcbf, handles);
                    
                case 'Direction 2'
                    handles.GM.store.filepath{2} = filepath;
                    set(handles.GroundMotions(11),...
                        'String',filepath,...
                        'TooltipString',filepath);
                    
                    dt = ReadWriteTHFileNGA('readDT',filepath);
                    handles.GM.dt(2) = dt;
                    guidata(gcbf,handles);
                    
                    [t ag] = ReadWriteTHFileNGA('readTHF',filepath);
                    handles.GM.t{2} = t;
                    handles.GM.ag{2} = ag;
                    
                    %Calculate scaled values
                    handles.GM.scaledt(2) = handles.GM.dt(2);
                    handles.GM.scalet{2} = handles.GM.TimeFact(2).*handles.GM.t{2};
                    handles.GM.scaleag{2} = handles.GM.AmpFact(2).*handles.GM.ag{2};
                    
                    %Calculate spectral response quantities and plot
                    handles.GM.Spectra{2} = ResponseSpectraElastic(handles.GM.scaleag{2},handles.GM.scaledt(2),m,handles.Model.Zeta,minT,maxT,numTvalues);
                    plot(handles.GroundMotions(15), handles.GM.scalet{2}, handles.GM.scaleag{2});
                    plot(handles.GroundMotions(16), handles.GM.Spectra{2}.T, handles.GM.Spectra{2}.psdAcc);
                    plot(handles.GroundMotions(17), handles.GM.Spectra{2}.T, handles.GM.Spectra{2}.dsp);
                    
                    %Store analysis dt
                    handles.GM.dtAnalysis = min(handles.GM.dt);
                    set(handles.Analysis(3),'String',[num2str(handles.GM.dtAnalysis)]);
                    guidata(gcbf, handles);
            end
        end

    case 'manual load'
        filepath = get(gcbo,'String');
        if exist(filepath) == 2
            switch GM_direction
                case 'Direction 1'
                    handles.GM.store.filepath{1} = filepath;
                    set(handles.GroundMotions(3),...
                        'String',filepath,...
                        'TooltipString',filepath);
                    
                    dt = ReadWriteTHFileNGA('readDT',filepath);
                    handles.GM.dt(1) = dt;
                    guidata(gcbf,handles);
                    
                    [t ag] = ReadWriteTHFileNGA('readTHF',filepath);
                    data = guidata(gcf);
                    handles.GM.databaseType = data.GM.databaseType;
                    handles.GM.t{1} = t;
                    handles.GM.ag{1} = ag;
                    
                    %Calculate scaled values
                    handles.GM.scaledt(1) = handles.GM.TimeFact(1).*handles.GM.dt(1);
                    handles.GM.scalet{1} = handles.GM.TimeFact(1).*handles.GM.t{1};
                    handles.GM.scaleag{1} = handles.GM.AmpFact(1).*handles.GM.ag{1};
                    
                    %Store analysis dt
                    handles.GM.dtAnalysis = min(handles.GM.dt);
                    set(handles.Analysis(3),'String',[num2str(handles.GM.dtAnalysis)]);
                    
                    %Calculate spectral response quantities and plot
                    handles.GM.Spectra{1} = ResponseSpectraElastic(handles.GM.scaleag{1},handles.GM.scaledt(1),m,handles.Model.Zeta,minT,maxT,numTvalues);
                    plot(handles.GroundMotions(7), handles.GM.scalet{1}, handles.GM.scaleag{1});
                    plot(handles.GroundMotions(8), handles.GM.Spectra{1}.T, handles.GM.Spectra{1}.psdAcc);
                    plot(handles.GroundMotions(9), handles.GM.Spectra{1}.T, handles.GM.Spectra{1}.dsp);
                    guidata(gcbf, handles);
                
                case 'Direction 2'
                    handles.GM.store.filepath{2} = filepath;
                    set(handles.GroundMotions(11),...
                        'String',filepath,...
                        'TooltipString',filepath);
                    
                    dt = ReadWriteTHFileNGA('readDT',filepath);
                    handles.GM.dt(2) = dt;
                    guidata(gcbf,handles);
                    
                    [t ag] = ReadWriteTHFileNGA('readTHF',filepath);
                    handles.GM.t{2} = t;
                    handles.GM.ag{2} = ag;
                    
                    %Calculate scaled values
                    handles.GM.scaledt(2) = handles.GM.TimeFact(2).*handles.GM.dt(2);
                    handles.GM.scalet{2} = handles.GM.TimeFact(2).*handles.GM.t{2};
                    handles.GM.scaleag{2} = handles.GM.AmpFact(2).*handles.GM.ag{2};
                    
                    %Store analysis dt
                    handles.GM.dtAnalysis = min(handles.GM.dt);
                    set(handles.Analysis(3),'String',[num2str(handles.GM.dtAnalysis)]);
                    
                    %Calculate spectral response quantities and plot
                    handles.GM.Spectra{2} = ResponseSpectraElastic(handles.GM.scaleag{2},handles.GM.scaledt(2),m,handles.Model.Zeta,minT,maxT,numTvalues);
                    plot(handles.GroundMotions(15), handles.GM.scalet{2}, handles.GM.scaleag{2});
                    plot(handles.GroundMotions(16), handles.GM.Spectra{2}.T, handles.GM.Spectra{2}.psdAcc);
                    plot(handles.GroundMotions(17), handles.GM.Spectra{2}.T, handles.GM.Spectra{2}.dsp); 
                    guidata(gcbf, handles);
            end
        else
            msgbox('Invalid file path specified','Path Not Found','warn')
            return
        end

    case 'scale'
        cbo_tag = get(gcbo,'Tag');
        switch cbo_tag
            case 'edit_amp1'
                input_val = str2num(get(gcbo,'String'));
                handles.GM.AmpFact(1) = input_val;
                guidata(gcbf, handles);
            case 'edit_time1'
                input_val = str2num(get(gcbo,'String'));
                handles.GM.TimeFact(1) = input_val;
                guidata(gcbf, handles);
            case 'scale_1'
                handles.GM.scaleag{1} = handles.GM.ag{1}.*handles.GM.AmpFact(1);
                handles.GM.scalet{1} = handles.GM.t{1}.*handles.GM.TimeFact(1);
                handles.GM.scaledt(1) = handles.GM.dt(1).*handles.GM.TimeFact(1);
                set(handles.Analysis(3),'String',[num2str(handles.GM.scaledt(1))]);
                %Calculate spectral response quantities and plot
                handles.GM.Spectra{1} = ResponseSpectraElastic(handles.GM.scaleag{1},handles.GM.scaledt(1),m,handles.Model.Zeta,minT,maxT,numTvalues);
                plot(handles.GroundMotions(7), handles.GM.scalet{1}, handles.GM.scaleag{1});
                plot(handles.GroundMotions(8), handles.GM.Spectra{1}.T, handles.GM.Spectra{1}.psdAcc);
                plot(handles.GroundMotions(9), handles.GM.Spectra{1}.T, handles.GM.Spectra{1}.dsp);
            case 'edit_amp2'
                input_val = str2num(get(gcbo,'String'));
                handles.GM.AmpFact(2) = input_val;
                guidata(gcbf, handles);
            case 'edit_time2'
                input_val = str2num(get(gcbo,'String'));
                handles.GM.TimeFact(2) = input_val;
                guidata(gcbf, handles);
            case 'scale_2'
                handles.GM.scaleag{2} = handles.GM.ag{2}.*handles.GM.AmpFact(2);
                handles.GM.scalet{2} = handles.GM.t{2}.*handles.GM.TimeFact(2);
                handles.GM.scaledt(2) = handles.GM.dt(2).*handles.GM.TimeFact(2);
                %Calculate spectral response quantities and plot
                handles.GM.Spectra{2} = ResponseSpectraElastic(handles.GM.scaleag{2},handles.GM.scaledt(2),m,handles.Model.Zeta,minT,maxT,numTvalues);
                plot(handles.GroundMotions(15), handles.GM.scalet{2}, handles.GM.scaleag{2});
                plot(handles.GroundMotions(16), handles.GM.Spectra{2}.T, handles.GM.Spectra{2}.psdAcc);
                plot(handles.GroundMotions(17), handles.GM.Spectra{2}.T, handles.GM.Spectra{2}.dsp);
        end
end

%Update handles structure
guidata(gcbf, handles);

end
