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
    case 'about error'
        %general error monitors help
        if ~isempty(findobj('Tag','EMHelp'))
            figure(findobj('Tag','EMHelp'));
            return
        end
        % text for error monitors help
        TEXT = ['\nDisplacement Error:\n===================\n' ...
            'This plot tracks the error between the measured and command displacements over time. ' ...
            'A positive error means the measured displacements are greater than the corresponding command values. ' ...
            '\n\nFourier Amplitude:\n==================\n' ...
            'Indicates the values of the fourier amplitude of the error for frequencies from 0 to 100 Hz. ' ...
            'This information can be used to identify the dominant frequencies in the error signal. ' ...
            '\n\nMeasured versus Command Displacement:\n=====================================\n'...
            'This plot compares the measured and command displacements. A straight line at 45 degrees shows perfect tracking. ' ...
            'A line at less than 45 degrees means the response is undershooting, while more than 45 degrees means the response is overshooting. ' ...
            'If the plot traces out an ellipse counter-clockwise, the response is lagging behind the command values. ' ...
            'Similarly, a clockwise ellipse indicates a leading response. ' ...
            'A counter-clockwise ellipse at an angle under 45 degrees, indicating both lag and undershoot, is a common response. ' ...
            '\n\nTracking Indicator:\n===================\n' ...
            'The tracking indicator is computed as the area underneath the plot of measured v. command displacement. ' ...
            'A positive plot indicates added damping in the system, and a negative plot means less damping. ' ...
            'Caution should be taken when the response is negative, as the system can become unstable. ' ...
            'Meanwhile, an increasing line shows the response is leading, while a decreasing line shows lag. ' ...
            'A steeper line in this figure corresponds to a larger error.\n'];
        GUI_Output(sprintf(TEXT),'About Error Monitors','About Error Monitors');

    case 'e1'
        msgbox(sprintf('Plots the error between measured and command\ndisplacement over time. A positive error means\nthe measured displacements are greater than the\ncorresponding command values.\n'),'Displacement Error');
    case 'fft1'
        msgbox(sprintf('Plots the fourier amplitude of the error and can be used to\nidentify the dominant frequencies in the error signal.\n'),'Fourier Amplitude');
    case 'dd1'
        msgbox(sprintf('Plots measured and command displacements.\nA line at 45 degrees indicates perfect tracking.\nLess than 45 degrees means the response is\nundershooting, while more than 45 degrees\nmeans the response is overshooting. If the plot\ntraces out an ellipse counter-clockwise, the\nresponse is lagging behind the command values.\nSimilarly, a clockwise ellipse indicates a leading\nresponse.\n'),'Measured v. Command Disp');
    case 'TI1'
        msgbox(sprintf('Displays the area under the measured versus\ncommand displacement plot. A positive plot\nindicates added damping in the system, and a\nnegative plot means less damping. Caution\nshould be taken when the response is negative,\nas the system can become unstable. An\nincreasing line shows response lead while a\ndecreasing line shows lag.\n'),'Tracking Indicator');
    case 'e2'
        msgbox(sprintf('Plots the error between measured and command\ndisplacement over time. A positive error means\nthe measured displacements are greater than the\ncorresponding command values.\n'),'Displacement Error');
    case 'fft2'
        msgbox(sprintf('Plots the fourier amplitude of the error and can be used to\nidentify the dominant frequencies in the error signal.\n'),'Fourier Amplitude');
    case 'dd2'
        msgbox(sprintf('Plots measured and command displacements.\nA line at 45 degrees indicates perfect tracking.\nLess than 45 degrees means the response is\nundershooting, while more than 45 degrees\nmeans the response is overshooting. If the plot\ntraces out an ellipse counter-clockwise, the\nresponse is lagging behind the command values.\nSimilarly, a clockwise ellipse indicates a leading\nresponse.\n'),'Measured v. Command Disp');
    case 'TI2'
        msgbox(sprintf('Displays the area under the measured versus\ncommand displacement plot. A positive plot\nindicates added damping in the system, and a\nnegative plot means less damping. Caution\nshould be taken when the response is negative,\nas the system can become unstable. An\nincreasing line shows response lead while a\ndecreasing line shows lag.\n'),'Tracking Indicator');
end
