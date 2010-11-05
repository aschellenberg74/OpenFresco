function GUI_StructuralOutput(varargin)
% GUI_StructuralOutput provides layout of the GUI structural output page

% Check if window already exists
if ~isempty(findobj('Tag','StructOutDOF1'))
    figure(findobj('Tag','StructOutDOF1'));
    if ~isempty(findobj('Tag','StructOutDOF2'))
        figure(findobj('Tag','StructOutDOF2'));
        figure(findobj('Tag','StructOutDOF1'));
        return
    end
    return
end

%  Initialization tasks
handles = guidata(gcbf);

% find longest motion
tEnd = 0.0;
switch handles.GM.loadType
    case 'Ground Motions'
        for mo=1:length(handles.GM.dt)
            tEndi = handles.GM.scalet{mo}(end);
            if tEndi > tEnd
                tEnd = tEndi;
            end
        end
    case 'Initial Conditions'
        tEnd = handles.GM.rampTime + handles.GM.vibTime;
end

%%%%%%%%%%%%%%%%%
%Create figure 1%
%%%%%%%%%%%%%%%%%

%Main Figure
SS = get(0,'screensize');
f_StructOut1 = figure('Visible','on','Name','Structural Output',...
    'NumberTitle','off',...
    'MenuBar','none',...
    'Tag','StructOutDOF1',...
    'Color',[0.3 0.5 0.7],...
    'KeyPressFcn',@shortcutKeys,...
    'Position',[SS(3)*0.12,SS(4)*0.05,SS(3)*0.88,SS(4)*0.87]);

%Toolbar
% set(f_StructOut1,'Toolbar','figure');
% Toolbar_handles = findall(gcf);
% delete(Toolbar_handles([3:14 17:18]));
% set(Toolbar_handles(15), 'TooltipString', 'Print Window');
% set(Toolbar_handles(16), 'TooltipString', 'Save Window');
File(1) = uimenu('Position',1,'Label','File');
File(2) = uimenu(File(1),'Position',1,'Label','Save        Ctrl+S','Callback','filemenufcn(gcbf,''FileSaveAs'')');
File(3) = uimenu(File(1),'Position',2,'Label','Print        Ctrl+P','Callback','printdlg(gcbf)');
Separator(1) = uimenu('Position',2,'Label','|');
StdMenu(1) = uimenu('Position',3,'Label','MATLAB Menu');
StdMenu(2) = uimenu(StdMenu(1),'Position',1,'Label','Turn on', ...
   'Callback','set(gcf,''MenuBar'',''figure''); set(gcf,''Toolbar'',''figure'');');
StdMenu(3) = uimenu(StdMenu(1),'Position',2,'Label','Turn off', ...
   'Callback','set(gcf,''MenuBar'',''none''); set(gcf,''Toolbar'',''none'');');
% =========================================================================

%Title
st_StructuralOut1_title = uicontrol(f_StructOut1,'Style','text',...
    'String','Structural Output',...
    'FontSize',20,...
    'ForegroundColor',[1 1 1],...
    'BackgroundColor',[0.3 0.5 0.7],...
    'Units','normalized',...
    'Position',[0.3 0.86 0.4 0.1],...
    'FontName',handles.Store.font);

%DOF 1 Output
a_StructuralOutput_dX = axes('Parent',f_StructOut1,...
    'Tag','SO1d',...
    'NextPlot','replacechildren',...
    'Position',[0.06 0.69 0.56 0.15],'Box','on','XTickLabel',[],...
    'XLimMode','manual','Xlim',[0.0 tEnd]);
grid('on');
ylabel(a_StructuralOutput_dX,'Disp [L]');
a_StructuralOutput_fX = axes('Parent',f_StructOut1,...
    'Tag','SO1f',...
    'NextPlot','replacechildren',...
    'Position',[0.06 0.49 0.56 0.15],'Box','on','XTickLabel',[],...
    'XLimMode','manual','Xlim',[0.0 tEnd]);
grid('on');
ylabel(a_StructuralOutput_fX,'Force [F]');
a_StructuralOutput_aX = axes('Parent',f_StructOut1,...
    'Tag','SO1a',...
    'NextPlot','replacechildren',...
    'Position',[0.06 0.29 0.56 0.15],'Box','on','XTickLabel',[],...
    'XLimMode','manual','Xlim',[0.0 tEnd]);
grid('on');
ylabel(a_StructuralOutput_aX,'Accel [L/sec^2]');
a_StructuralOutput_agX = axes('Parent',f_StructOut1,...
    'Tag','SO1ag',...
    'NextPlot','replacechildren',...
    'Position',[0.06 0.09 0.56 0.15],'Box','on');
switch handles.GM.loadType
    case 'Ground Motions'
        line(handles.GM.scalet{1}, handles.GM.scaleag{1});
        grid('on');
        ylabel(a_StructuralOutput_agX,'Ground Accel [L/sec^2]');
        xlabel(a_StructuralOutput_agX,'Time [sec]');
        set(findobj('Tag','SO1ag'),'XLimMode','manual','Xlim',[0.0 tEnd]);
    case 'Initial Conditions'
        set(findobj('Tag','SO1ag'),'YLimMode','manual','Ylim',[-100 100],'YTick', [-100 0 100])
        xValues = [0 handles.GM.rampTime handles.GM.rampTime (handles.GM.rampTime+handles.GM.vibTime)];
        yValues = [0 100 0 0];
        line(xValues, yValues);
        grid('on');
        ylabel(a_StructuralOutput_agX,'% Total Displacement');
        xlabel(a_StructuralOutput_agX,'Time [sec]');
end

linkaxes([a_StructuralOutput_dX, a_StructuralOutput_fX, a_StructuralOutput_aX, a_StructuralOutput_agX],'x');

a_StructuralOutput_fdX = axes('Parent',f_StructOut1,...
    'Tag','SO1fd',...
    'NextPlot','replacechildren',...
    'Position',[0.68 0.09 0.3 0.75],'Box','on');
grid('on');
xlabel(a_StructuralOutput_fdX,'Disp [L]');
ylabel(a_StructuralOutput_fdX,'Force [F]');
if ~strcmp(handles.Model.Type, '1 DOF')
    set(a_StructuralOutput_fdX,'Position',[0.68 0.52 0.31 0.33]);
    a_StructuralOutput_dd = axes('Parent',f_StructOut1,...
        'Tag','SO1dd',...
        'NextPlot','replacechildren',...
        'Position',[0.68 0.08 0.31 0.33],'Box','on');
    axis(a_StructuralOutput_dd,'equal');
    grid('on');
    xlabel(a_StructuralOutput_dd,'Disp 1 [L]');
    ylabel(a_StructuralOutput_dd,'Disp 2 [L]');
end


%%%%%%%%%%%%%%%%%
%Create figure 2%
%%%%%%%%%%%%%%%%%

if ~strcmp(handles.Model.Type, '1 DOF')
    set(f_StructOut1,'Name','Structural Output DOF 1')
    set(st_StructuralOut1_title,'String','Structural Output: DOF 1');
    f_StructOut2 = figure('Visible','on','Name','Structural Output DOF 2',...
        'NumberTitle','off',...
        'MenuBar','none',...
        'Tag','StructOutDOF2',...
        'Color',[0.3 0.5 0.7],...
        'KeyPressFcn',@shortcutKeys,...
        'Position',[SS(3)*0.12,SS(4)*0.05,SS(3)*0.88,SS(4)*0.87]);
    
    %Toolbar
%     set(f_StructOut2,'Toolbar','figure');
%     Toolbar_handles = findall(gcf);
%     delete(Toolbar_handles([3:14 17:18]));
%     set(Toolbar_handles(15), 'TooltipString', 'Print Report');
%     set(Toolbar_handles(16), 'TooltipString', 'Save Report');
    File(1) = uimenu('Position',1,'Label','File');
    File(2) = uimenu(File(1),'Position',1,'Label','Save        Ctrl+S','Callback','filemenufcn(gcbf,''FileSaveAs'')');
    File(3) = uimenu(File(1),'Position',2,'Label','Print        Ctrl+P','Callback','printdlg(gcbf)');
    Separator(1) = uimenu('Position',2,'Label','|');
    StdMenu(1) = uimenu('Position',3,'Label','MATLAB Menu');
    StdMenu(2) = uimenu(StdMenu(1),'Position',1,'Label','Turn on', ...
        'Callback','set(gcf,''MenuBar'',''figure''); set(gcf,''Toolbar'',''figure'');');
    StdMenu(3) = uimenu(StdMenu(1),'Position',2,'Label','Turn off', ...
        'Callback','set(gcf,''MenuBar'',''none''); set(gcf,''Toolbar'',''none'');');
    
    %Title
    uicontrol(f_StructOut2,'Style','text',...
        'String','Structural Output: DOF 2',...
        'FontSize',20,...
        'ForegroundColor',[1 1 1],...
        'BackgroundColor',[0.3 0.5 0.7],...
        'Units','normalized',...
        'Position',[0.3 0.86 0.4 0.1],...
        'FontName',handles.Store.font);
    
    %DOF 2 Output
    a_StructuralOutput_dY = axes('Parent',f_StructOut2,...
        'Tag','SO2d',...
        'NextPlot','replacechildren',...
        'Position',[0.06 0.69 0.56 0.15],'Box','on','XTickLabel',[],...
        'XLimMode','manual','Xlim',[0.0 tEnd]);
    grid('on');
    ylabel(a_StructuralOutput_dY,'Disp [L]');
    a_StructuralOutput_fY = axes('Parent',f_StructOut2,...
        'Tag','SO2f',...
        'NextPlot','replacechildren',...
        'Position',[0.06 0.49 0.56 0.15],'Box','on','XTickLabel',[],...
        'XLimMode','manual','Xlim',[0.0 tEnd]);
    grid('on');
    ylabel(a_StructuralOutput_fY,'Force [F]');
    a_StructuralOutput_aY = axes('Parent',f_StructOut2,...
        'Tag','SO2a',...
        'NextPlot','replacechildren',...
        'Position',[0.06 0.29 0.56 0.15],'Box','on','XTickLabel',[],...
        'XLimMode','manual','Xlim',[0.0 tEnd]);
    grid('on');
    ylabel(a_StructuralOutput_aY,'Accel [L/sec^2]');
    a_StructuralOutput_agY = axes('Parent',f_StructOut2,...
        'Tag','SO2ag',...
        'NextPlot','replacechildren',...
        'Position',[0.06 0.09 0.56 0.15],'Box','on');
    switch handles.GM.loadType
        case 'Ground Motions'
            if strcmp(handles.Model.Type, '2 DOF A')
                line(handles.GM.scalet{1}, handles.GM.scaleag{1});
            elseif strcmp(handles.Model.Type, '2 DOF B')
                line(handles.GM.scalet{2}, handles.GM.scaleag{2});
            end
            grid('on');
            ylabel(a_StructuralOutput_agY,'Ground Accel [L/sec^2]');
            xlabel(a_StructuralOutput_agY,'Time [sec]');
            set(findobj('Tag','SO1ag'),'XLimMode','manual','Xlim',[0.0 tEnd]);
        case 'Initial Conditions'
            set(findobj('Tag','SO2ag'),'YLimMode','manual','Ylim',[-100 100],'YTick', [-100 0 100])
            xValues = [0 handles.GM.rampTime handles.GM.rampTime (handles.GM.rampTime+handles.GM.vibTime)];
            yValues = [0 100 0 0];
            line(xValues, yValues);
            grid('on');
            ylabel(a_StructuralOutput_agY,'% Total Displacement');
            xlabel(a_StructuralOutput_agY,'Time [sec]');
    end
    
    linkaxes([a_StructuralOutput_dY, a_StructuralOutput_fY, a_StructuralOutput_aY, a_StructuralOutput_agY],'x');
    
    a_StructuralOutput_fdY = axes('Parent',f_StructOut2,...
        'Tag','SO2fd',...
        'NextPlot','replacechildren',...
        'Position',[0.68 0.52 0.31 0.33],'Box','on');
    grid('on');
    xlabel(a_StructuralOutput_fdY,'Disp [L]');
    ylabel(a_StructuralOutput_fdY,'Force [F]');
    a_StructuralOutput_ff = axes('Parent',f_StructOut2,...
        'Tag','SO2ff',...
        'NextPlot','replacechildren',...
        'Position',[0.68 0.08 0.31 0.33],'Box','on');
    axis(a_StructuralOutput_ff,'equal');
    grid('on');
    xlabel(a_StructuralOutput_ff,'Force 1 [F]');
    ylabel(a_StructuralOutput_ff,'Force 2 [F]');
    
    %Bring DOF 1 image to the foreground
    figure(findobj('Tag','StructOutDOF1'));
end

%Callback for shortcut keys
    function shortcutKeys(source, eventdata)
        control = 0;
        for x=1:length(eventdata.Modifier)
            switch(eventdata.Modifier{x})
                case 'control'
                    control = 1;
            end
        end
        if (control == 1 && strcmp(eventdata.Key,'s'))
            filemenufcn(gcbf,'FileSaveAs');
        elseif (control == 1 && strcmp(eventdata.Key,'p'))
            printdlg(gcbf);
        end
    end

end
