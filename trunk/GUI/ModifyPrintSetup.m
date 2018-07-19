function ModifyPrintSetup(hFig,varargin)
%MODIFYPRINTSETUP to modify the hidden figure print setup options
% ModifyPrintSetup(hFig,varargin)
%
% hFig     : handle to figure for which the print setup is modified
% varargin : one or more option-value pairs
%
% options that can be modified include:
%         VersionNumber: 2
%                  Name: ''
%             FrameName: ''
%           DriverColor: 1
%       AxesFreezeTicks: 0
%             tickState: {}
%      AxesFreezeLimits: 0
%              limState: {}
%                 Loose: 0
%                  CMYK: 0
%                Append: 0
%             Adobecset: 0
%               PrintUI: 1
%              Renderer: 'auto'
%        ResolutionMode: 'auto'
%                   DPI: 0
%              FileName: 'untitled'
%           Destination: 'printer'
%           PrintDriver: ''
%             DebugMode: 0
%            StyleSheet: 'default'
%              FontName: ''
%              FontSize: 0
%          FontSizeType: 'screen'
%             FontAngle: ''
%            FontWeight: ''
%             FontColor: ''
%             LineWidth: 0
%         LineWidthType: 'screen'
%          LineMinWidth: 0
%             LineStyle: ''
%             LineColor: ''
%          PrintActiveX: 0
%             GrayScale: 0
%               BkColor: 'white'
%               FigSize: [9.6250 5.4688]
%             PaperType: 'usletter'
%             PaperSize: [11 8.5000]
%      PaperOrientation: 'landscape'
%            PaperUnits: 'inches'
%         PaperPosition: [0.2500 0.2500 10.5000 8]
%     PaperPositionMode: 'manual'
%        InvertHardCopy: 'on'
%            HeaderText: ''
%      HeaderDateFormat: 'none'
%        HeaderFontName: 'Helvetica'
%        HeaderFontSize: 12
%       HeaderFontAngle: 'normal'
%      HeaderFontWeight: 'normal'
%          HeaderMargin: 72
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

% open the print preview window to initialize data
% hPre = printpreview(hFig);  % no longer working with >= R2014b
printpreview(hFig);

% retrieve the hidden print setup options
figOpts = getprinttemplate(hFig);

% modify other options if requested
for i=1:2:length(varargin)
    figOpts.(varargin{i}) = varargin{i+1};
end

% save the print setup options
setprinttemplate(hFig,figOpts);

% close the preview window
%delete(hPre);
