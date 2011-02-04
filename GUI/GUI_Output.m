function GUI_Output(text,title,figtitle,width,height)
%GUI_OUTPUT graphical user interface to display any output
% GUI_Output(text,title,figtitle,width,height)
%
% text     : text to be displayed
% title    : title (optional)
% figtitle : figure title (optional)
% width    : width of window (optional)
% height   : height of window (optional)
%
% Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
% Created: 09/10
% Revision: A
%
% $Revision: 409 $
% $Date: 2010-09-22 22:52:11 -0700 (Wed, 22 Sep 2010) $
% $URL: $ 

% set some default parameters
if nargin<2
   title    = 'Output';
   figtitle = 'Output';
   width    = 0.55;
   height   = 0.52;
elseif nargin<3
   figtitle = 'Output';
   width    = 0.55;
   height   = 0.52;
elseif nargin<4
   width    = 0.55;
   height   = 0.52;
elseif nargin<5
   width    = 0.55;
end

%%%%%%%%%%%%%%%%%%%%%%
% Figure properties
%%%%%%%%%%%%%%%%%%%%%%
SS = get(0,'screensize');
figure('Position',[0.277*SS(3) 0.215*SS(4) width*SS(3) height*SS(4)], ...
    'MenuBar','none', ...
    'NumberTitle','off', ...
    'Name',figtitle, ...
    'Color',[0.3 0.5 0.7], ...
    'Tag','GUIOutput');

% add print button
set(gcf,'Toolbar','figure');
Toolbar_handles = findall(gcf);
delete(Toolbar_handles([3:14 16:18]));
set(Toolbar_handles(15), 'TooltipString', ...
    'Print Window', 'ClickedCallback', 'printpreview(gcbf)');

% define some normalized parameters to partition the figure
dy = 0.04;  % distance between each block
dx = 0.02;
Y = 0.9;

%%%%%%%%%%%%%%%%%%%%%%
% Title text
%%%%%%%%%%%%%%%%%%%%%%
width = 1; height = 0.05;
uicontrol('Style','text', ...
   'Units','normalized', ...
   'Position',[0.5-width/2 Y+0.02 width height], ...
   'String',title, ...
   'FontUnits','normalized','FontWeight','bold','FontSize',0.6, ...
   'Horizontal','center', ...
   'BackgroundColor',[0.3 0.5 0.7], ...
   'ForegroundColor',[0 0 0]);

% generate a line to divide the frame
height = 0.001;
uicontrol('Style','frame', ...
   'Units','normalized', ...
   'Position',[0.00 Y 1.00 height], ...
   'BackgroundColor',[0 0 0], ...
   'ForegroundColor',[0 0 0]);
Y = Y - height;

%%%%%%%%%%%%%%%%%%%%%%
% Output window
%%%%%%%%%%%%%%%%%%%%%%
width = 1.0-4*dx; height = 0.7;
Control(1) = uicontrol( ...
   'Style','edit', ...
   'Units','normalized', ...
   'Position',[2*dx Y-height-dy width height], ...
   'String',text, ...
   'Max',10,'Min',1, ...
   'FontUnits','normalized','FontSize',0.04,'FontName','FixedWidth', ...
   'Horizontal','left', ...
   'BackgroundColor',[1 1 1], ...
   'ForegroundColor',[0 0 0]);

%%%%%%%%%%%%%%%%%%%%%%
% OK button
%%%%%%%%%%%%%%%%%%%%%%
width2 = 0.1; height = 0.05;
callbackStr = 'close(findobj(''Tag'',''GUIOutput'')); clc;';
uicontrol('Style','pushbutton', ...
   'Units','normalized', ...
   'Position',[0.5-width2/2 dy width2 height], ...
   'String','OK', ...
   'FontUnits','normalized','FontWeight','bold','FontSize',0.5, ...
   'Horizontal','left', ...
   'Callback',callbackStr, ...
   'BackgroundColor',[0.9 0.9 0.9], ...
   'ForegroundColor',[0 0 0]);

%%%%%%%%%%%%%%%%%%%%%%%
% Save handles 
%%%%%%%%%%%%%%%%%%%%%%%
% create the structure of handles
handles = guihandles(gcf);
handles.Control = Control;

% save the structure of handles
guidata(gcf,handles);
