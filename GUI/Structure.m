function Structure(action, varargin)

%  Initialization tasks
handles = guidata(gcbf);

%Identify chosen DOF
DOF_selection = get(get(handles.Structure(2), 'SelectedObject'),'Tag');
handles.Model.Type = DOF_selection;

switch action
    case 'scroll'
        slider = round(get(gcbo,'Value')*10)/10;
        if slider <= 0.3
            set(handles.Structure([3 6:13]),'Visible','on');
            set(handles.Structure([4 14:21]),'Visible','off');
            set(handles.Structure([5 22:29]),'Visible','off');
            set(handles.Structure([10 18 26]),'Visible','off');
        elseif slider <= 0.6
            set(handles.Structure([3 6:13]),'Visible','off');
            set(handles.Structure([4 14:21]),'Visible','on');
            set(handles.Structure([5 22:29]),'Visible','off');
            set(handles.Structure([10 18 26]),'Visible','off');
        else
            set(handles.Structure([3 6:13]),'Visible','off');
            set(handles.Structure([4 14:21]),'Visible','off');
            set(handles.Structure([5 22:29]),'Visible','on');
            set(handles.Structure([10 18 26]),'Visible','off');
        end
    case 'choose DOF'
        active_color = [0 0 0];
        inactive_color = [0.6 0.6 0.6];
        dof1_children = [get(handles.Structure(6),'Children')' get(handles.Structure(8),'Children')'];
        dof2_children = [get(handles.Structure(14),'Children')' get(handles.Structure(16),'Children')'];
        dof3_children = [get(handles.Structure(22),'Children')' get(handles.Structure(24),'Children')'];
        
        %Reset Values
        handles.Model.M = [];
        handles.Model.K = [];
        handles.Model.Zeta = [];
        switch DOF_selection;
            case '1 DOF'
                handles.Model.StructActive = [3 6:13];
                handles.Model.StructInactive = [4 5 14:29];
                %Adjust colors to emphasize active/inactive fields
                set(handles.Structure(7),'BackgroundColor',[1 1 1],'Style','edit');
                set(handles.Structure(9),'BackgroundColor',[1 1 1],'Style','edit');
                set(handles.Structure(12),'BackgroundColor',[1 1 1]);
                set(handles.Structure(13),'BackgroundColor',[1 1 1],'Style','edit');
                set(handles.Structure(15),'BackgroundColor','default','Style','text','String','[m1 0; 0 m2]');
                set(handles.Structure(17),'BackgroundColor','default','Style','text','String','[k11 k12; k21 k22]');
                set(handles.Structure(19),'BackgroundColor','default','Style','text','String','Period');                
                set(handles.Structure(20),'BackgroundColor','default');
                set(handles.Structure(21),'BackgroundColor','default','Style','text','String','zeta');
                set(handles.Structure(23),'BackgroundColor','default','Style','text','String','[m1 0; 0 m2]');
                set(handles.Structure(25),'BackgroundColor','default','Style','text','String','[k11 k12; k21 k22]');
                set(handles.Structure(27),'BackgroundColor','default','Style','text','String','Period');                
                set(handles.Structure(28),'BackgroundColor','default');
                set(handles.Structure(29),'BackgroundColor','default','Style','text','String','zeta');
                set(dof1_children,'ForegroundColor',active_color);
                set(dof2_children,'ForegroundColor',inactive_color);
                set(dof3_children,'ForegroundColor',inactive_color);
                
                %Remove unnecessary plots
                cla(handles.GroundMotions(15));
                cla(handles.GroundMotions(16));
                cla(handles.GroundMotions(17));
                
                %Set DOF/CP options
                set(handles.EC(6),'String',{'Choose DOF...','DOF 1'},'Value',1);
                set(handles.EC(9),'String','1');
                set(handles.EC(27),'Value',1);
                set(handles.EC(54),'String',{'Existing Control Points...','Control Point 1','Control Point 2'});
                handles.ExpControl.store.CPOptions = {'Existing Control Points...','Control Point 1','Control Point 2'};
                set(handles.EC(57),'Style','edit','String','1');
                handles.ExpControl.store.NodeOptions = get(handles.EC(57),'String');
                set(handles.EC(58),'String','1');
%                 handles.ExpControl.CtrlDOF = 'DOF 1';
                handles.ExpControl.store.DOFOptions = {'Choose DOF...','DOF 1'}';
                handles.ExpControl.NumAct = 1;
                handles.ExpControl.store.SimActive = 6;
                handles.ExpControl.store.RealActive = (26:27);
                
                %Define element data
                handles.Model.Element{1}.tag = 1;
                handles.Model.Element{1}.type = 'Element_ExpGeneric';
                handles.Model.Element{1}.kInit = handles.Model.K;
                handles.Model.Element{1}.ipAddr = '127.0.0.1';
                handles.Model.Element{1}.ipPort = 8090;
                handles.Model.Element{1}.id = 1;
                
                %Update model and ground motion parameters
                handles.Model.Type = DOF_selection;
                handles.Model.Mass_field = 7;
                handles.Model.Stiffness_field = 9;
                handles.Model.Period_field = 11;
                handles.Model.Damping_field = 13;
                set(handles.ES(3),'Value',1);
                handles.Model.ndf = 1;
                handles.Model.b = 1;
                handles.GM.AmpFact = handles.GM.AmpFact(1);
                handles.GM.TimeFact = handles.GM.TimeFact(1);
                handles.GM.dt = handles.GM.dt(1);
                handles.GM.t = handles.GM.t(1);
                handles.GM.ag = handles.GM.ag(1);
                handles.GM.scaledt = handles.GM.scaledt(1);
                handles.GM.scalet = handles.GM.scalet(1);
                handles.GM.scaleag = handles.GM.scaleag(1);
                handles.GM.Spectra = handles.GM.Spectra(1);
                
                %Initialize Control Points Page
                set(handles.EC(54),'Value',1);
                set(handles.EC(56),'String','');
                set(handles.EC([60 61 66 67 72 73 78 79]),'Value',1);
                set(handles.EC([62 68 74 80]),'String','1');
                set(handles.EC([63 69 75 81]),'Value',0);
                set(handles.EC([64 65 70 71 76 77 82 83]),'BackgroundColor','default','Style','text','String','');
                
                %Create Preset Control Points
                handles.ExpControl.CP = {};
                handles.ExpControl.CP.Name{1} = 'Control Point 1';
                handles.ExpControl.CP.NumResp{1} = 1;
                handles.ExpControl.CP.Node{1} = 1;
                handles.ExpControl.CP.Dir{1,1} = 'UX';
                handles.ExpControl.CP.Resp{1,1} = 'disp';
                handles.ExpControl.CP.F{1,1} = 1;
                handles.ExpControl.CP.Lim{1,1} = 1;
                handles.ExpControl.CP.LimL{1,1} = -7.5;
                handles.ExpControl.CP.LimU{1,1} = 7.5;
                
                handles.ExpControl.CP.Name{2} = 'Control Point 2';
                handles.ExpControl.CP.NumResp{2} = 2;
                handles.ExpControl.CP.Node{2} = 1;
                handles.ExpControl.CP.Dir{1,2} = 'UX';
                handles.ExpControl.CP.Resp{1,2} = 'disp';
                handles.ExpControl.CP.F{1,2} = 1;
                handles.ExpControl.CP.Lim{1,2} = 1;
                handles.ExpControl.CP.LimL{1,2} = -7.5;
                handles.ExpControl.CP.LimU{1,2} = 7.5;
                handles.ExpControl.CP.Dir{2,2} = 'UX';
                handles.ExpControl.CP.Resp{2,2} = 'force';
                handles.ExpControl.CP.F{2,2} = 1;
                handles.ExpControl.CP.Lim{2,2} = 1;
                handles.ExpControl.CP.LimL{2,2} = -12;
                handles.ExpControl.CP.LimU{2,2} = 12;
                
                set(handles.EC(32),'String',handles.ExpControl.store.CPOptions(2:end));
                set(handles.EC(33),'String',handles.ExpControl.store.CPOptions(2:end));
                
            case '2 DOF A'
                handles.Model.StructActive = [4 14:21];
                handles.Model.StructInactive = [3 5 6:13 22:29];
                %Adjust colors to emphasize active/inactive fields
                set(handles.Structure(7),'BackgroundColor','default','Style','text','String','Enter mass here');
                set(handles.Structure(9),'BackgroundColor','default','Style','text','String','Enter stiffness here');
                set(handles.Structure(11),'BackgroundColor','default','Style','text','String','Period');
                set(handles.Structure(12),'BackgroundColor','default');
                set(handles.Structure(13),'BackgroundColor','default','Style','text','String','zeta');
                set(handles.Structure(15),'BackgroundColor',[1 1 1],'Style','edit');
                set(handles.Structure(17),'BackgroundColor',[1 1 1],'Style','edit');
                set(handles.Structure(20),'BackgroundColor',[1 1 1]);
                set(handles.Structure(21),'BackgroundColor',[1 1 1],'Style','edit');
                set(handles.Structure(23),'BackgroundColor','default','Style','text','String','[m1 0; 0 m2]');
                set(handles.Structure(25),'BackgroundColor','default','Style','text','String','[k11 k12; k21 k22]');
                set(handles.Structure(27),'BackgroundColor','default','Style','text','String','Period');                
                set(handles.Structure(28),'BackgroundColor','default');
                set(handles.Structure(29),'BackgroundColor','default','Style','text','String','zeta');
                set(dof1_children,'ForegroundColor',inactive_color);
                set(dof2_children,'ForegroundColor',active_color);
                set(dof3_children,'ForegroundColor',inactive_color);
                
                %Remove unnecessary plots
                cla(handles.GroundMotions(15));
                cla(handles.GroundMotions(16));
                cla(handles.GroundMotions(17));
                
                %Set DOF/CP options
                set(handles.EC(6),'String',{'Choose Story...','Story 1','Story 2'},'Value',1);
                set(handles.EC(7:10),'Visible','off');
                set(handles.EC(9),'String','2');
                set(handles.EC(27),'Value',1);
                set(handles.EC(54),'String',{'Existing Control Points...','Control Point 1','Control Point 2','Control Point 3','Control Point 4'});
                handles.ExpControl.store.CPOptions = {'Existing Control Points...','Control Point 1','Control Point 2','Control Point 3','Control Point 4'};
                set(handles.EC(57),'Style','popupmenu','String',{'Choose Node...','Node 1','Node 2'});
                handles.ExpControl.store.NodeOptions = get(handles.EC(57),'String');
                set(handles.EC(58),'String','1');
%                 handles.ExpControl.CtrlDOF = 'DOF 1';
                handles.ExpControl.store.DOFOptions = {'Choose Story...','Story 1','Story 2'}';
                handles.ExpControl.NumAct = 2;
                handles.ExpControl.store.SimActive = 6;
                handles.ExpControl.store.RealActive = (26:27);
                
                %Define element data
                handles.Model.Element{1}.tag = 1;
                handles.Model.Element{1}.type = 'Element_ExpGeneric';
                handles.Model.Element{1}.kInit = handles.Model.K;
                handles.Model.Element{1}.ipAddr = '127.0.0.1';
                handles.Model.Element{1}.ipPort = 8090;
                handles.Model.Element{1}.id = [1 2];

                %Update model and ground motion parameters
                handles.Model.Type = DOF_selection;
                handles.Model.Mass_field = 15;
                handles.Model.Stiffness_field = 17;
                handles.Model.Period_field = 19;
                handles.Model.Damping_field = 21;
                set(handles.ES(4),'Value',1);
                handles.Model.ndf = 2;
                handles.Model.b = [1;1];
                handles.GM.AmpFact = handles.GM.AmpFact(1);
                handles.GM.TimeFact = handles.GM.TimeFact(1);
                handles.GM.dt = handles.GM.dt(1);
                handles.GM.t = handles.GM.t(1);
                handles.GM.ag = handles.GM.ag(1);
                handles.GM.scaledt = handles.GM.scaledt(1);
                handles.GM.scalet = handles.GM.scalet(1);
                handles.GM.scaleag = handles.GM.scaleag(1);
                handles.GM.Spectra = handles.GM.Spectra(1);
                
                %Initialize Control Points Page
                set(handles.EC(54),'Value',1);
                set(handles.EC(56),'String','');
                set(handles.EC([60 61 66 67 72 73 78 79]),'Value',1);
                set(handles.EC([62 68 74 80]),'String','1');
                set(handles.EC([63 69 75 81]),'Value',0);
                set(handles.EC([64 65 70 71 76 77 82 83]),'BackgroundColor','default','Style','text','String','');
                
                %Create Preset Control Points
                handles.ExpControl.CP = {};
                handles.ExpControl.CP.Name{1} = 'Control Point 1';
                handles.ExpControl.CP.NumResp{1} = 1;
                handles.ExpControl.CP.Node{1} = 1;
                handles.ExpControl.CP.Dir{1,1} = 'UX';
                handles.ExpControl.CP.Resp{1,1} = 'disp';
                handles.ExpControl.CP.F{1,1} = 1;
                handles.ExpControl.CP.Lim{1,1} = 1;
                handles.ExpControl.CP.LimL{1,1} = -7.5;
                handles.ExpControl.CP.LimU{1,1} = 7.5;
                
                handles.ExpControl.CP.Name{2} = 'Control Point 2';
                handles.ExpControl.CP.NumResp{2} = 2;
                handles.ExpControl.CP.Node{2} = 1;
                handles.ExpControl.CP.Dir{1,2} = 'UX';
                handles.ExpControl.CP.Resp{1,2} = 'disp';
                handles.ExpControl.CP.F{1,2} = 1;
                handles.ExpControl.CP.Lim{1,2} = 1;
                handles.ExpControl.CP.LimL{1,2} = -7.5;
                handles.ExpControl.CP.LimU{1,2} = 7.5;
                handles.ExpControl.CP.Dir{2,2} = 'UX';
                handles.ExpControl.CP.Resp{2,2} = 'force';
                handles.ExpControl.CP.F{2,2} = 1;
                handles.ExpControl.CP.Lim{2,2} = 1;
                handles.ExpControl.CP.LimL{2,2} = -12;
                handles.ExpControl.CP.LimU{2,2} = 12;
                
                handles.ExpControl.CP.Name{3} = 'Control Point 3';
                handles.ExpControl.CP.NumResp{3} = 1;
                handles.ExpControl.CP.Node{3} = 2;
                handles.ExpControl.CP.Dir{1,3} = 'UX';
                handles.ExpControl.CP.Resp{1,3} = 'disp';
                handles.ExpControl.CP.F{1,3} = 1;
                handles.ExpControl.CP.Lim{1,3} = 1;
                handles.ExpControl.CP.LimL{1,3} = -7.5;
                handles.ExpControl.CP.LimU{1,3} = 7.5;
                
                handles.ExpControl.CP.Name{4} = 'Control Point 4';
                handles.ExpControl.CP.NumResp{4} = 2;
                handles.ExpControl.CP.Node{4} = 2;
                handles.ExpControl.CP.Dir{1,4} = 'UX';
                handles.ExpControl.CP.Resp{1,4} = 'disp';
                handles.ExpControl.CP.F{1,4} = 1;
                handles.ExpControl.CP.Lim{1,4} = 1;
                handles.ExpControl.CP.LimL{1,4} = -7.5;
                handles.ExpControl.CP.LimU{1,4} = 7.5;
                handles.ExpControl.CP.Dir{2,4} = 'UX';
                handles.ExpControl.CP.Resp{2,4} = 'force';
                handles.ExpControl.CP.F{2,4} = 1;
                handles.ExpControl.CP.Lim{2,4} = 1;
                handles.ExpControl.CP.LimL{2,4} = -12;
                handles.ExpControl.CP.LimU{2,4} = 12;
                
                set(handles.EC(32),'String',handles.ExpControl.store.CPOptions(2:end));
                set(handles.EC(33),'String',handles.ExpControl.store.CPOptions(2:end));
                
                
            case '2 DOF B'
                handles.Model.StructActive = [5 22:29];
                handles.Model.StructInactive = [3 4 6:21];
                
                %Adjust colors to emphasize active/inactive fields
                set(handles.Structure(7),'BackgroundColor','default','Style','text','String','Enter mass here');
                set(handles.Structure(9),'BackgroundColor','default','Style','text','String','Enter stiffness here');
                set(handles.Structure(11),'BackgroundColor','default','Style','text','String','Period');                
                set(handles.Structure(12),'BackgroundColor','default');
                set(handles.Structure(13),'BackgroundColor','default','Style','text','String','zeta');
                set(handles.Structure(15),'BackgroundColor','default','Style','text','String','[m1 0; 0 m2]');
                set(handles.Structure(17),'BackgroundColor','default','Style','text','String','[k11 k12; k21 k22]');
                set(handles.Structure(19),'BackgroundColor','default','Style','text','String','Period');                
                set(handles.Structure(20),'BackgroundColor','default');
                set(handles.Structure(21),'BackgroundColor','default','Style','text','String','zeta');
                set(handles.Structure(23),'BackgroundColor',[1 1 1],'Style','edit');
                set(handles.Structure(25),'BackgroundColor',[1 1 1],'Style','edit');
                set(handles.Structure(28),'BackgroundColor',[1 1 1]);
                set(handles.Structure(29),'BackgroundColor',[1 1 1],'Style','edit');
                set(dof1_children,'ForegroundColor',inactive_color);
                set(dof2_children,'ForegroundColor',inactive_color);
                set(dof3_children,'ForegroundColor',active_color);
                
                %Set DOF/CP options
                set(handles.EC(6),'String',{'Choose DOF...','DOF 1','DOF 2'},'Value',1);
                set(handles.EC(7:10),'Visible','off');
                set(handles.EC(9),'String','2');
                set(handles.EC(27),'Value',1);
                set(handles.EC(54),'String',{'Existing Control Points...','Control Point 1','Control Point 2'});
                handles.ExpControl.store.CPOptions = {'Existing Control Points...','Control Point 1','Control Point 2'};
                set(handles.EC(57),'Style','edit','String','1');
                handles.ExpControl.store.NodeOptions = get(handles.EC(57),'String');
                set(handles.EC(58),'String','2');
                handles.ExpControl.store.DOFOptions = {'Choose DOF...','DOF 1','DOF 2'}';
                handles.ExpControl.NumAct = 2;
                handles.ExpControl.store.SimActive = 6;
                handles.ExpControl.store.RealActive = (26:27);
                
                %Define element data
                handles.Model.Element{1}.tag = 1;
                handles.Model.Element{1}.type = 'Element_ExpGeneric';
                handles.Model.Element{1}.kInit = handles.Model.K;
                handles.Model.Element{1}.ipAddr = '127.0.0.1';
                handles.Model.Element{1}.ipPort = 8090;
                handles.Model.Element{1}.id = [1 2];

                %Update model and ground motion parameters
                handles.Model.Type = DOF_selection;
                handles.Model.Mass_field = 23;
                handles.Model.Stiffness_field = 25;
                handles.Model.Period_field = 27;
                handles.Model.Damping_field = 29;
                set(handles.ES(5),'Value',1);
                handles.Model.ndf = 2;
                handles.Model.b = [1 0; 0 1];
                handles.GM.AmpFact(2) = str2num(get(handles.GroundMotions(12),'String'));
                handles.GM.TimeFact(2) = str2num(get(handles.GroundMotions(13),'String'));
                
                %Initialize Control Points Page
                set(handles.EC(54),'Value',1);
                set(handles.EC(56),'String','');
                set(handles.EC([60 61 66 67 72 73 78 79]),'Value',1);
                set(handles.EC([62 68 74 80]),'String','1');
                set(handles.EC([63 69 75 81]),'Value',0);
                set(handles.EC([64 65 70 71 76 77 82 83]),'BackgroundColor','default','Style','text','String','');
                
                %Create Preset Control Points
                handles.ExpControl.CP = {};
                handles.ExpControl.CP.Name{1} = 'Control Point 1';
                handles.ExpControl.CP.NumResp{1} = 2;
                handles.ExpControl.CP.Node{1} = 1;
                handles.ExpControl.CP.Dir{1,1} = 'UX';
                handles.ExpControl.CP.Resp{1,1} = 'disp';
                handles.ExpControl.CP.F{1,1} = 1;
                handles.ExpControl.CP.Lim{1,1} = 1;
                handles.ExpControl.CP.LimL{1,1} = -7.5;
                handles.ExpControl.CP.LimU{1,1} = 7.5;
                handles.ExpControl.CP.Dir{2,1} = 'UY';
                handles.ExpControl.CP.Resp{2,1} = 'disp';
                handles.ExpControl.CP.F{2,1} = 1;
                handles.ExpControl.CP.Lim{2,1} = 1;
                handles.ExpControl.CP.LimL{2,1} = -7.5;
                handles.ExpControl.CP.LimU{2,1} = 7.5;
                
                handles.ExpControl.CP.Name{2} = 'Control Point 2';
                handles.ExpControl.CP.NumResp{2} = 4;
                handles.ExpControl.CP.Node{2} = 1;
                handles.ExpControl.CP.Dir{1,2} = 'UX';
                handles.ExpControl.CP.Resp{1,2} = 'disp';
                handles.ExpControl.CP.F{1,2} = 1;
                handles.ExpControl.CP.Lim{1,2} = 1;
                handles.ExpControl.CP.LimL{1,2} = -7.5;
                handles.ExpControl.CP.LimU{1,2} = 7.5;
                handles.ExpControl.CP.Dir{2,2} = 'UY';
                handles.ExpControl.CP.Resp{2,2} = 'disp';
                handles.ExpControl.CP.F{2,2} = 1;
                handles.ExpControl.CP.Lim{2,2} = 1;
                handles.ExpControl.CP.LimL{2,2} = -7.5;
                handles.ExpControl.CP.LimU{2,2} = 7.5;
                handles.ExpControl.CP.Dir{3,2} = 'UX';
                handles.ExpControl.CP.Resp{3,2} = 'force';
                handles.ExpControl.CP.F{3,2} = 1;
                handles.ExpControl.CP.Lim{3,2} = 1;
                handles.ExpControl.CP.LimL{3,2} = -12;
                handles.ExpControl.CP.LimU{3,2} = 12;
                handles.ExpControl.CP.Dir{4,2} = 'UY';
                handles.ExpControl.CP.Resp{4,2} = 'force';
                handles.ExpControl.CP.F{4,2} = 1;
                handles.ExpControl.CP.Lim{4,2} = 1;
                handles.ExpControl.CP.LimL{4,2} = -12;
                handles.ExpControl.CP.LimU{4,2} = 12;    
                
                set(handles.EC(32),'String',handles.ExpControl.store.CPOptions(2:end));
                set(handles.EC(33),'String',handles.ExpControl.store.CPOptions(2:end));
        end
        
        guidata(gcbf, handles);
    
    %Store input values
    case 'mass_input'
        input_val = str2num(get(handles.Structure(handles.Model.Mass_field),'String'));
        if isempty(input_val)
            msgbox('Must enter valid mass value(s)','Invalid Input','error');
        else
            if strcmp(DOF_selection, '1 DOF')
                if size(input_val) ~= [1 1]
                    msgbox('Must enter valid mass value(s)','Invalid Input','error');
                else
                    handles.Model.M = input_val;
                    guidata(gcbf, handles);
                end
            else
                if size(input_val) ~= [2 2]
                    msgbox('Must enter valid mass value(s)','Invalid Input','error');
                elseif input_val(1,2) ~= 0 | input_val(2,1) ~= 0
                    msgbox('Mass matrix must be diagonal','Invalid Input','error');
                else
                    handles.Model.M = input_val;
                    guidata(gcbf, handles);
                end
            end
        end
        if isempty(handles.Model.K)
            return;
        else
            handles.Model.Omega = sqrt(eig(handles.Model.K, handles.Model.M));
            if size(handles.Model.Omega) == [2 1];
                if handles.Model.Omega(1) < handles.Model.Omega(2)
                    handles.Model.minTDOF = 2;
                else
                    handles.Model.minTDOF = 1;
                end
            else
                handles.Model.minTDOF = 1;
            end
            handles.Model.T = sort(2*pi./(handles.Model.Omega));
            set(handles.Structure(handles.Model.Period_field),'String',sprintf(['Period:    ' num2str(handles.Model.T')]));
        end

    case 'stiffness_input'
        input_val = str2num(get(handles.Structure(handles.Model.Stiffness_field),'String'));
        if isempty(input_val)
            msgbox('Must enter valid stiffness value(s)','Invalid Input','error');
        else
            if strcmp(DOF_selection, '1 DOF')
                if size(input_val) ~= [1 1]
                    msgbox('Must enter valid stiffness value(s)','Invalid Input','error');
                else
                    handles.Model.K = input_val;
                    handles.Model.Element{1}.kInit = handles.Model.K;
                    guidata(gcbf, handles);
                end
            else
                if size(input_val) ~= [2 2]
                    msgbox('Must enter valid stiffness value(s)','Invalid Input','error');
                elseif input_val(1,2) > 0 | input_val(2,1) > 0
                    msgbox('Must enter valid stiffness value(s)','Invalid Input','error');
                elseif input_val(1,2) ~= input_val(2,1)
                    msgbox('Stiffness matrix must be symmetric','Invalid Input','error');
                else
                    handles.Model.K = input_val;
                    handles.Model.Element{1}.kInit = handles.Model.K;
                    guidata(gcbf, handles);
                end
            end
        end
        if isempty(handles.Model.M)
            return;
        else
            handles.Model.Omega = sqrt(eig(handles.Model.K, handles.Model.M));
            if size(handles.Model.Omega) == [2 1];
                if handles.Model.Omega(1) < handles.Model.Omega(2)
                    handles.Model.minTDOF = 2;
                else
                    handles.Model.minTDOF = 1;
                end
            else
                handles.Model.minTDOF = 1;
            end
            handles.Model.T = sort(2*pi./(handles.Model.Omega));
            set(handles.Structure(handles.Model.Period_field),'String',sprintf(['Period:    ' num2str(handles.Model.T')]));
        end
        guidata(gcbf, handles);
        
%     case 'period_calc'
%         if isempty(handles.Model.M)
%             msgbox('Must enter valid mass value(s)','Invalid Input','error');
%         elseif isempty(handles.Model.K)
%             msgbox('Must enter valid stiffness value(s)','Invalid Input','error');
%         else
%             handles.Model.Omega = sqrt(eig(handles.Model.K, handles.Model.M));
%             if size(handles.Model.Omega) == [2 1];
%                 if handles.Model.Omega(1) < handles.Model.Omega(2)
%                     handles.Model.minTDOF = 2;
%                 else
%                     handles.Model.minTDOF = 1;
%                 end
%             else
%                 handles.Model.minTDOF = 1;
%             end
%             handles.Model.T = sort(2*pi./(handles.Model.Omega));
%             set(handles.Structure(handles.Model.Period_field),'String',num2str(handles.Model.T'));
%             guidata(gcbf, handles);
%         end

    case 'choose damping'
        damp_type = get(gcbo,'Value');
        switch damp_type
            case 2
                handles.Model.DampType = 'Stiffness Proportional';
                if ~isempty(handles.Model.Zeta)
                    handles.Model.alphaM = 0;
                    handles.Model.betaK = handles.Model.Zeta*handles.Model.T(1)/pi;
                end
            case 3
                handles.Model.DampType = 'Mass Proportional';
                if ~isempty(handles.Model.Zeta)
                    handles.Model.alphaM = 4*pi*handles.Model.Zeta/handles.Model.T(1);
                    handles.Model.betaK = 0;
                end
            case 4
                handles.Model.DampType = 'Rayleigh';
                msgbox('Rayleigh damping requires two coefficients','Rayleigh Coefficients','warn');
        end
        guidata(gcbf, handles);

    case 'damping_input'
        input_val = str2num(get(handles.Structure(handles.Model.Damping_field),'String'));
        handles.Model.Zeta = input_val;
        guidata(gcbf, handles);
        damp_type = handles.Model.DampType;
        switch damp_type
            case 'Stiffness Proportional'
                handles.Model.alphaM = 0;
                handles.Model.betaK = handles.Model.Zeta*handles.Model.T(1)/pi;
            case 'Mass Proportional'
                handles.Model.alphaM = 4*pi*handles.Model.Zeta/handles.Model.T(1);
                handles.Model.betaK = 0;
            case 'Rayleigh'
                check_matrix = length(handles.Model.Zeta);
                if check_matrix ~=2
                    msgbox('Rayleigh damping requires two coefficients','Error','error');
                else
                      wi = handles.Model.Omega(1);
                      wj = handles.Model.Omega(2);
                      zetai = handles.Model.Zeta(1);
                      zetaj = handles.Model.Zeta(2);
                      Damping = [1/wi wi; 1/wj wj]\[zetai; zetaj]*2;
                      handles.Model.alphaM = Damping(1);
                      handles.Model.betaK = Damping(2);
                end
                guidata(gcbf, handles);
        end
        guidata(gcbf, handles);
end

%Update handles structure
guidata(gcbf, handles);
end