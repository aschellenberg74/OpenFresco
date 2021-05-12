function figh = createWindow(position,Width,Height,figtitle)
%CREATEWINDOW to create new window with given dimensions
% figh = createWindow(position,Width,Height,figtitle)
%
% figh     : figure handle
% position : string indicating position of window on screen (optional)
%              'cen' center
%              'ulc' upper left corner
%              'llc' lower left corner
%              'urc' upper right corner
%              'lrc' lower right corner
% Width    : window width in pixels (optional)
% Height   : window height in pixels (optional)
% figtitle : title of window (optional)
%
% Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
% Created: 04/05

% get the screen size
SS = get(0,'screensize');

if nargin < 1
    position = 'cen';
    Width = SS(3)/2;
    Height = SS(4)/2;
elseif nargin < 2
    Width = SS(3)/2;
    Height = SS(4)/2;
elseif nargin < 3
    Height = SS(4)/2;
end

% get the display position
switch position
    case 'cen'
        Position = [(SS(3)-Width)/2, (SS(4)-Height)/2, Width, Height];
    case 'ulc'
        Position = [1, SS(4)-Height, Width, Height];
    case 'llc'
        Position = [1, 1, Width, Height];
    case 'urc'
        Position = [SS(3)-Width, SS(4)-Height, Width, Height];
    case 'lrc'
        Position = [SS(3)-Width, 1, Width, Height];
end

% create figure handle
figh = figure('Units','pixel','Position',Position);

if (nargin==4)
    set(figh,'NumberTitle','off', ...
        'Name',figtitle);
end
