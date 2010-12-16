function axesHelp(action, varargin)

switch action
    case 'about error'
        if ~isempty(findobj('Tag','EMHelp'))
            figure(findobj('Tag','EMHelp'));
            return
        end
        % text for error monitors help
        TEXT = ['\nDisplacement Error\n' ...
            'This plot tracks the error between the measured and command displacements over time. ' ...
            'A positive error means the measured displacements are greater than the corresponding command values ' ...
            '\n\nFourier Amplitude\n' ...
            'Indicates the values of the fourier amplitude for frequencies from 0 to 100 Hz. ' ...
            'This information can be used to identify the dominant frequencies of the system.' ...
            '\n\nMeasured versus Command Displacement\n'...
            'This plot compares the measured and command displacements, with a line at 45 degrees showing perfect tracking. ' ...
            'A line at less than 45 degrees means the response is lagging behind the command values, while more than 45 degrees means the response is leading. ' ...
            '\n\nTracking Indicator\n' ...
            'The tracking indicator is computed as the area underneath the plot of measured v. command displacement ' ...
            'A steeper line in this figure corresponds to a larger error '];
        GUI_Output(sprintf(TEXT),'About Error Monitors','About Error Monitors');

    case 'e1'
        msgbox(sprintf('Plots the error between measured\nand command displacement over time.'));
    case 'fft1'
        msgbox(sprintf('Plots the fourier amplitude and can be used to\nidentify the dominant frequencies.'));
    case 'dd1'
        msgbox(sprintf('Plots measured and command displacements.\nA line at 45 degrees indicates perfect tracking.\nLess than 45 degrees means the response is\nlagging, while more than 45 degrees means the\nresponse is leading.'));
    case 'TI1'
        msgbox(sprintf('Displays the area under the measured versus command\ndisplacement plot. A steeper slope indicates a larger error.'));
    case 'e2'
        msgbox(sprintf('Plots the error between measured\nand command displacement over time.'));
    case 'fft2'
        msgbox(sprintf('Plots the fourier amplitude and can be used to\nidentify the dominant frequencies.'));
    case 'dd2'
        msgbox(sprintf('Plots measured and command displacements.\nA line at 45 degrees indicates perfect tracking.\nLess than 45 degrees means the response is\nlagging, while more than 45 degrees means the\nresponse is leading.'));
    case 'TI2'
        msgbox(sprintf('Displays the area under the measured versus command\ndisplacement plot. A steeper slope indicates a larger error.'));
end