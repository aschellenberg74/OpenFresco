function ExpSetup(action, varargin)
% EXPSETUP handles user inputs related to the Experimental Setup 
% action     : selected action on the Experimental Setup page

%  Initialization tasks
handles = guidata(gcbf);

%Identify Chosen DOF
DOF_selection = handles.Model.Type;

switch action
    case 'choose DOF'
        %Prevent user from changing setup
        TF = strcmp(get(get(handles.ES(2), 'SelectedObject'),'Tag'), DOF_selection);
        if TF ~=1
            msgbox({'Cannot change experimental setup type!','Can only change structure type'},'Error','error');
            switch DOF_selection
                case '1 DOF'
                    set(handles.ES(3),'Value',1);
                case '2 DOF A'
                    set(handles.ES(4),'Value',1);
                case '2 DOF B'
                    set(handles.ES(5),'Value',1);
            end
        end

    case 'extra options'
        %%%This callback has not been fully checked for functionality yet
        %nonlinear options for 2 DOF B case
        if strcmp(DOF_selection, '2 DOF B')
            nonlinear = questdlg('Would you like to perform nonlinear analysis?','Nonlinear?','Yes','No','Yes');
            switch nonlinear
                case 'Yes'
                    Act_Lengths = inputdlg({['Please input actuator lengths'; 'Actuator 1                   '] 'Actuator 2'},'Actuator Lengths');
                    if ~isempty(Act_Lengths)
                        handles.Model.ActLength1 = str2num(Act_Lengths{1});
                        handles.Model.ActLength2 = str2num(Act_Lengths{2});
                    else
                        return
                    end
            end
        else
            msgbox({'Cannot change experimental setup type!','Can only change structure type'},'Error','error');
            switch DOF_selection
                case '1 DOF'
                    set(handles.ES(3),'Value',1);
                case '2 DOF A'
                    set(handles.ES(4),'Value',1);
                case '2 DOF B'
                    set(handles.ES(5),'Value',1);
            end
        end
end
            
    %Based on setup chosen, return setup type to ExpSetup.Type
    %Example: ExpSetup.Type = 'OneActuator'

%Update handles structure
guidata(gcbf, handles);

end