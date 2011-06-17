function GUI_AnimateStructure(varargin)
% GUI_AnimateStructure provides the layout of the GUI structural animation page

% Initialization tasks
handles = guidata(findobj('Tag','OpenFrescoExpress'));

% update the axis limits and the color bar
if (nargin > 0 && strcmp(varargin{1},'update'))
    xlim = str2num(get(handles.Plots.ColorBarMax,'String'));
    if (xlim > 0)
        switch handles.Model.Type
            case {'1 DOF','2 DOF A'}
                set(gca,'XLim',[-3*xlim,3*xlim],'YLim',[-1,11], ...
                    'CLim',[0,xlim]);
            case '2 DOF B'
                set(gca,'XLim',[-3*xlim,3*xlim],'YLim',[-3*xlim,3*xlim], ...
                    'ZLim',[-1,11],'CLim',[0,xlim]);
        end
    end
end

% Check if window already exists
if ~isempty(findobj('Tag','StructAnim'))
    figure(findobj('Tag','StructAnim'));
    return
end

% Main Figure
SS = get(0,'screensize');
f_StructAnim = figure('Visible','on', ...
    'Name','Animate Structure', ...
    'NumberTitle','off', ...
    'MenuBar','none', ...
    'Tag','StructAnim', ...
    'Renderer','zbuffer', ...
    'Position',[0.3*SS(3),0.25*SS(4),0.5*SS(3),0.5*SS(4)]);
imagesc(imread(which('BlueGradient.png')));
set(gca,'Visible','off','Position',[0,0,1,1]);

% Toolbar
File(1) = uimenu('Position',1,'Label','File');
uimenu(File(1),'Position',1,'Label','Save', ...
    'Accelerator','S','Callback','filemenufcn(gcbf,''FileSaveAs'')');
uimenu(File(1),'Position',2,'Label','Print', ...
    'Accelerator','P','Callback','printdlg(gcbf)');
uimenu('Position',2,'Label','|');
StdMenu(1) = uimenu('Position',3,'Label','MATLAB Menu');
uimenu(StdMenu(1),'Position',1,'Label','Turn on', ...
   'Callback','set(gcf,''MenuBar'',''figure''); set(gcf,''Toolbar'',''figure'');');
uimenu(StdMenu(1),'Position',2,'Label','Turn off', ...
   'Callback','set(gcf,''MenuBar'',''none''); set(gcf,''Toolbar'',''none'');');
% =========================================================================

% Axes
a = axes('Parent',f_StructAnim, ...
    'Visible','off', ...
    'Tag','AxesStructAnim', ...
    'Position',[0.05 0.05 0.90 0.90]);
colorbar('Position',[0.8935 0.0467 0.036 0.863]);
%colorbar('Position',[0.8935 0.0467 0.0317 0.835]);
set(a,'Position',[0.05 0.05 0.81746 0.9]);
colormap(hot(128));

% Colorbar Maximum
handles.Plots.ColorBarMax = uicontrol(f_StructAnim,'Style','edit',...
    'Units','normalized',...
    'FontSize',10,...
    'String','1.0',...
    'BackgroundColor',[1 1 1],...
    'Position',[0.8935 0.908 0.038 0.05],...
    'Tag','ColorBarMax',...
    'Callback','GUI_AnimateStructure(''update'')');

% Plot Model
switch handles.Model.Type
    case '1 DOF'
        xyzNodes(1,:) = [0.0  0.0];  % node 1
        xyzNodes(2,:) = [0.0 10.0];  % node 2
        elemCon(1,:)  = [1 1 2 2];   % element 1
        cdata = [0 0]';
        
        handles.Plots.StructAnim = patch( ...
            'Faces',elemCon,'Vertices',xyzNodes, ...
            'FaceVertexCData',cdata, ...
            'EdgeColor','interp','LineWidth',5.0, ...
            'Marker','s','MarkerSize',12, ...
            'MarkerFaceColor','k','MarkerEdgeColor','none');

        if strcmp(handles.GM.loadType,'Ground Motions')
            id = find(handles.GM.Spectra{1}.T > handles.Model.T(1));
            xlim = handles.GM.Spectra{1}.dsp(id(1),1);
        elseif strcmp(handles.GM.loadType,'Initial Conditions')
            xlim = handles.GM.initialDisp;
        end
        set(a,'XLim',[-3*xlim,3*xlim],'YLim',[-1,11], ...
            'CLim',[0,xlim]);
        set(handles.Plots.ColorBarMax,'String',num2str(xlim,3));
        
    case '2 DOF A'
        xyzNodes(1,:) = [0.0  0.0];  % node 1
        xyzNodes(2,:) = [0.0  5.0];  % node 2
        xyzNodes(3,:) = [0.0 10.0];  % node 3
        elemCon(1,:)  = [1 1 2 2];   % element 1
        elemCon(2,:)  = [2 2 3 3];   % element 2
        cdata = [0 0 0]';
        
        handles.Plots.StructAnim = patch( ...
            'Faces',elemCon,'Vertices',xyzNodes, ...
            'FaceVertexCData',cdata, ...
            'EdgeColor','interp','LineWidth',5.0, ...
            'Marker','s','MarkerSize',12, ...
            'MarkerFaceColor','k','MarkerEdgeColor','none');
        
        if strcmp(handles.GM.loadType,'Ground Motions')
            id = find(handles.GM.Spectra{1}.T > handles.Model.T(1));
            xlim = handles.GM.Spectra{1}.dsp(id(1),1);
        elseif strcmp(handles.GM.loadType,'Initial Conditions')
            xlim = handles.GM.initialDisp(2);
        end
        set(a,'XLim',[-3*xlim,3*xlim],'YLim',[-1,11], ...
            'CLim',[0,xlim]);
        set(handles.Plots.ColorBarMax,'String',num2str(xlim,3));
        
    case '2 DOF B'
        xyzNodes(1,:) = [0.0  0.0  0.0];  % node 1
        xyzNodes(2,:) = [0.0  0.0 10.0];  % node 2
        elemCon(1,:)  = [1 1 2 2];        % element 1
        cdata = [0 0]';
        
        handles.Plots.StructAnim = patch( ...
            'Faces',elemCon,'Vertices',xyzNodes, ...
            'FaceVertexCData',cdata, ...
            'EdgeColor','interp','LineWidth',5.0, ...
            'Marker','s','MarkerSize',12, ...
            'MarkerFaceColor','k','MarkerEdgeColor','none');
        
        if strcmp(handles.GM.loadType,'Ground Motions')
            T = 2*pi./sqrt(handles.Model.K(1,1)/handles.Model.M(1,1));
            id = find(handles.GM.Spectra{1}.T > T);
            xlim = handles.GM.Spectra{1}.dsp(id(1),1);
            T = 2*pi./sqrt(handles.Model.K(2,2)/handles.Model.M(2,2));
            id = find(handles.GM.Spectra{2}.T > T);
            ylim = handles.GM.Spectra{2}.dsp(id(1),2);
        elseif strcmp(handles.GM.loadType,'Initial Conditions')
            xlim = handles.GM.initialDisp(1);
            ylim = handles.GM.initialDisp(2);
        end
        xylim = norm([xlim,ylim]);
        set(a,'XLim',[-3*xylim,3*xylim],'YLim',[-3*xylim,3*xylim], ...
            'ZLim',[-1,11],'CLim',[0,xylim]);
        set(handles.Plots.ColorBarMax,'String',num2str(xylim,3));
        view(3);
end

% save the handles
guidata(findobj('Tag','OpenFrescoExpress'), handles);
