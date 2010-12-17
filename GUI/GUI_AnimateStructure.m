function GUI_AnimateStructure(varargin)
% GUI_AnimateStructure provides the layout of the GUI structural animation page

% Check if window already exists
if ~isempty(findobj('Tag','StructAnim'))
    figure(findobj('Tag','StructAnim'));
    return
end

% Initialization tasks
handles = guidata(gcbf);

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
File(2) = uimenu(File(1),'Position',1,'Label','Save', ...
    'Accelerator','S','Callback','filemenufcn(gcbf,''FileSaveAs'')');
File(3) = uimenu(File(1),'Position',2,'Label','Print', ...
    'Accelerator','P','Callback','printdlg(gcbf)');
Separator(1) = uimenu('Position',2,'Label','|');
StdMenu(1) = uimenu('Position',3,'Label','MATLAB Menu');
StdMenu(2) = uimenu(StdMenu(1),'Position',1,'Label','Turn on', ...
   'Callback','set(gcf,''MenuBar'',''figure''); set(gcf,''Toolbar'',''figure'');');
StdMenu(3) = uimenu(StdMenu(1),'Position',2,'Label','Turn off', ...
   'Callback','set(gcf,''MenuBar'',''none''); set(gcf,''Toolbar'',''none'');');
% =========================================================================

% Axes
a = axes('Parent',f_StructAnim, ...
    'Visible','off', ...
    'Tag','AxesStructAnim', ...
    'Position',[0.05 0.05 0.90 0.90]);
colorbar;
colormap(hot(128));

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
        
        id = find(handles.GM.Spectra{1}.T > handles.Model.T(1));
        xlim = handles.GM.Spectra{1}.dsp(id(1),1);
        set(a,'XLim',[-3*xlim,3*xlim],'YLim',[-1,11], ...
            'CLim',[0,floor(xlim)]);
        
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
        
        id = find(handles.GM.Spectra{1}.T > handles.Model.T(1));
        xlim = handles.GM.Spectra{1}.dsp(id(1),1);
        set(a,'XLim',[-3*xlim,3*xlim],'YLim',[-1,11], ...
            'CLim',[0,floor(xlim)]);
        
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
        
        id = find(handles.GM.Spectra{1}.T > handles.Model.T(1));
        xlim = handles.GM.Spectra{1}.dsp(id(1),1);
        ylim = handles.GM.Spectra{2}.dsp(id(1),1);
        xylim = max(xlim,ylim);
        set(a,'XLim',[-3*xylim,3*xylim],'YLim',[-3*xylim,3*xylim], ...
            'ZLim',[-1,11],'CLim',[0,floor(xlim)]);
        view(3);
        
end

% save the handles
guidata(findobj('Tag','OpenFrescoExpress'), handles);
