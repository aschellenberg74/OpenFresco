function PrintWithHeader(format,axisTag,loc)
%PRINTWITHHEADER to print a figure with header
% PrintWithHeader(format,axisTag,loc)
%
% format  : format to print to 'fig', 'pdf', 'clipboard' or 'printer'
% axisTag : tag of axis above which to put the header
% loc     : location in terms of normalized axis size [x1,y1,x2,y2]
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

% get the OpenFresco version
ver = fgetl(fopen('OPFE_Version.txt'));

% get handle to the axis above which to put the header
hAxis = findobj('Tag',axisTag);

% setup the headers
xLim = get(hAxis,'XLim');
yLim = get(hAxis,'YLim');
hHeaderLeft = text('Parent',hAxis, ...
    'Position',[xLim(1)+loc(1)*diff(xLim),yLim(2)+loc(2)*diff(yLim),0], ...
    'String',sprintf('OpenFresco Express v%s',ver), ...
    'HorizontalAlignment','left', ...
    'FontName','Helvetica','FontSize',10);
hHeaderRight = text('Parent',hAxis, ...
    'Position',[xLim(2)+loc(3)*diff(xLim),yLim(2)+loc(4)*diff(yLim),0], ...
    'String',datestr(now), ...
    'HorizontalAlignment','right', ...
    'FontName','Helvetica','FontSize',10);

switch lower(format)
    case 'fig'
        filemenufcn(gcbf,'FileSaveAs');
    case 'pdf'
        [name,path] = uiputfile('*.pdf','Save As PDF');
        print(gcbf,fullfile(path,name),'-dpdf');
    case 'clipboard'
        print(gcbf,'-dmeta');
    case 'printer'
        printdlg(gcbf);
end

% delete the headers
delete(hHeaderLeft);
delete(hHeaderRight);
