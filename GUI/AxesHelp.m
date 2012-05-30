function AxesHelp(action,varargin)
%AXESHELP to display help messages on the analysis plots
% AxesHelp(action,varargin)
%
% action   : selected analysis plot
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

switch action
    % =====================================================================
    case 'about error'
        DIR = which('OPFE_Version.txt');
        DIR = fileparts(DIR);
        GUI_Output(fullfile(DIR,'AboutErrorMonitors.txt'),'About Error Monitors','About Error Monitors');
    % =====================================================================
    case 'e1'
        msgbox(sprintf('Plots the error between measured and command\ndisplacement over time. A positive error means\nthe measured displacements are greater than the\ncorresponding command values.\n'),'Displacement Error');
    % =====================================================================
    case 'fft1'
        msgbox(sprintf('Plots the fourier amplitude of the error and can be used to\nidentify the dominant frequencies in the error signal.\n'),'Fourier Amplitude');
    % =====================================================================
    case 'dd1'
        msgbox(sprintf('Plots measured and command displacements.\nA line at 45 degrees indicates perfect tracking.\nLess than 45 degrees means the response is\nundershooting, while more than 45 degrees\nmeans the response is overshooting. If the plot\ntraces out an ellipse counter-clockwise, the\nresponse is lagging behind the command values.\nSimilarly, a clockwise ellipse indicates a leading\nresponse.\n'),'Measured v. Command Disp');
    % =====================================================================
    case 'TI1'
        msgbox(sprintf('Displays the area under the measured vs.\ncommand displacement plot. A positive plot\nindicates added damping in the system, and a\nnegative plot means less damping. Caution\nshould be taken when the response is negative,\nas the system can become unstable. An\nincreasing line shows response lead while a\ndecreasing line shows lag.\n'),'Tracking Indicator');
    % =====================================================================
    case 'e2'
        msgbox(sprintf('Plots the error between measured and command\ndisplacement over time. A positive error means\nthe measured displacements are greater than the\ncorresponding command values.\n'),'Displacement Error');
    % =====================================================================
    case 'fft2'
        msgbox(sprintf('Plots the fourier amplitude of the error and can be used to\nidentify the dominant frequencies in the error signal.\n'),'Fourier Amplitude');
    % =====================================================================
    case 'dd2'
        msgbox(sprintf('Plots measured and command displacements.\nA line at 45 degrees indicates perfect tracking.\nLess than 45 degrees means the response is\nundershooting, while more than 45 degrees\nmeans the response is overshooting. If the plot\ntraces out an ellipse counter-clockwise, the\nresponse is lagging behind the command values.\nSimilarly, a clockwise ellipse indicates a leading\nresponse.\n'),'Measured v. Command Disp');
    % =====================================================================
    case 'TI2'
        msgbox(sprintf('Displays the area under the measured vs.\ncommand displacement plot. A positive plot\nindicates added damping in the system, and a\nnegative plot means less damping. Caution\nshould be taken when the response is negative,\nas the system can become unstable. An\nincreasing line shows response lead while a\ndecreasing line shows lag.\n'),'Tracking Indicator');
    % =====================================================================
end
