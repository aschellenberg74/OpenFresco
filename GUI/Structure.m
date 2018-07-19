function Structure(action,varargin)
%STRUCTURE to handle user inputs related to the structural properties of the model
% Structure(action,varargin)
%
% action   : selected action on the structure page
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
%%     Commercials use of this program without express permission of     %%
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

%#ok<*ST2NM>

% initialization tasks
handles = guidata(gcbf);

% identify chosen DOF
DOF_selection = get(get(handles.Structure(3), 'SelectedObject'),'Tag');
handles.Model.Type = DOF_selection;

switch action
    % =====================================================================
    case 'scroll'
        % switch between structural models
        % define colors and identify children plots
        active_color = [0 0 0];
        inactive_color = [0.6 0.6 0.6];
        panelDefault = [0.941176 0.941176 0.941176];
        dof1_children = get(handles.Structure(8),'Children')';
        dof2_children = get(handles.Structure(15),'Children')';
        dof3_children = get(handles.Structure(24),'Children')';
        
        % reset values
        handles.Model.M = [];
        handles.Model.K = [];
        handles.Model.Zeta = [];
        
        slider = round(get(gcbo,'Value')*10)/10;
        
        %=======
        % 1 DOF
        %=======
        if slider <= 0.3
            % initialize structure page and adjust button and field colors
            set(handles.Structure(2),'Value',0);
            structTag = findobj('Tag','1 DOF');
            set(handles.Structure(3),'SelectedObject',structTag(2));
            set(handles.Structure([4 7:13]),'Visible','on');
            set(handles.Structure([5 6 14:31]),'Visible','off');
            
            handles.Model.StructActive = [4 7:13];
            handles.Model.StructInactive = [5 6 15:31];
            set(handles.Structure(4),'CData',handles.Store.Model1A1);
            set(handles.Structure(5),'CData',handles.Store.Model2A0);
            set(handles.Structure(6),'CData',handles.Store.Model2B0);
            
            set(handles.Structure(9:10),'BackgroundColor',[1 1 1],'Style','edit','String','');
            set(handles.Structure(12),'BackgroundColor',[1 1 1]);
            set(handles.Structure(13),'BackgroundColor',[1 1 1],'Style','edit');
            
            set(handles.Structure(16:19),'BackgroundColor',panelDefault,'Style','text');
            set(handles.Structure(20),'BackgroundColor',panelDefault,'Style','text','String','Period');
            set(handles.Structure(21),'BackgroundColor',panelDefault);
            set(handles.Structure(22),'BackgroundColor',panelDefault,'Style','text','String','zeta');
            
            set(handles.Structure(25:28),'BackgroundColor',panelDefault,'Style','text');
            set(handles.Structure(29),'BackgroundColor',panelDefault,'Style','text','String','Period');
            set(handles.Structure(30),'BackgroundColor',panelDefault);
            set(handles.Structure(31),'BackgroundColor',panelDefault,'Style','text','String','zeta');
            
            set(dof1_children,'ForegroundColor',active_color);
            set(dof2_children,'ForegroundColor',inactive_color);
            set(dof3_children,'ForegroundColor',inactive_color);
            set(findobj('Tag','help1'),'CData',handles.Store.Question1);
            set(findobj('Tag','help2A'),'CData',handles.Store.Question0);
            set(findobj('Tag','help2B'),'CData',handles.Store.Question0);
            
            % remove unnecessary plots
            cla(handles.GroundMotions(13));
            cla(handles.GroundMotions(14));
            cla(handles.GroundMotions(15));
            
            % set DOF/CP options
            set(handles.EC(6),'String',{'Choose DOF...','DOF 1'},'Value',1);
            set(handles.EC(9),'String','1');
            set(handles.EC(27),'Value',1);
            set(handles.EC(54),'String',{'Existing Control Points...','Control Point 1','Control Point 2'});
            handles.ExpControl.store.CPOptions = {'Existing Control Points...','Control Point 1','Control Point 2'};
            set(handles.EC(57),'Style','edit','String','1');
            handles.ExpControl.store.NodeOptions = get(handles.EC(57),'String');
            set(handles.EC(58),'String','1');
            handles.ExpControl.store.DOFOptions = {'Choose DOF...','DOF 1'}';
            handles.ExpControl.NumAct = 1;
            handles.ExpControl.store.SimActive = 6;
            handles.ExpControl.store.RealActive = (26:27);
            
            % define element data
            handles.Model.Element{1}.tag = 1;
            handles.Model.Element{1}.type = 'Element_ExpGeneric';
            handles.Model.Element{1}.kInit = handles.Model.K;
            handles.Model.Element{1}.ipAddr = '127.0.0.1';
            handles.Model.Element{1}.ipPort = 7777;
            handles.Model.Element{1}.id = 1;
            
            % update model and ground motion parameters
            handles.Model.Type = '1 DOF';
            handles.Model.Mass_field = 9;
            handles.Model.Stiffness_field = 10;
            handles.Model.Period_field = 11;
            handles.Model.Damping_field = 13;
            set(handles.ES(3),'Value',1);
            handles.Model.ndf = 1;
            handles.Model.b = 1;
            handles.GM.AmpFact(1) = str2num(get(handles.GroundMotions(4),'String'));
            handles.GM.TimeFact(1) = str2num(get(handles.GroundMotions(5),'String'));
            
            % initialize loading page
            set(handles.GroundMotions(23),'String','Enter displacement here');
            set(handles.GroundMotions(27),'String',{'Choose Mode...','Mode 1','User Defined'},'Value',1);
            
            % initialize control points page
            set(handles.EC(54),'Value',1);
            set(handles.EC(56),'String','');
            set(handles.EC([60 61 66 67 72 73 78 79]),'Value',1);
            set(handles.EC([62 68 74 80]),'String','1');
            set(handles.EC([63 69 75 81]),'Value',0);
            set(handles.EC([64 65 70 71 76 77 82 83]),'BackgroundColor',panelDefault,'Style','text','String','');
            
            % create preset control points
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
            
        %=========
        % 2 DOF A
        %=========
        elseif slider <= 0.6
            % initialize structure page and adjust button and field colors
            set(handles.Structure(2),'Value',0.5);
            structTag = findobj('Tag','2 DOF A');
            set(handles.Structure(3),'SelectedObject',structTag(2));
            set(handles.Structure([4 6 7:13 23:31]),'Visible','off');
            set(handles.Structure([5 14:22]),'Visible','on');
            
            handles.Model.StructActive = [5 14:22];
            handles.Model.StructInactive = [4 6 7:13 23:31];
            set(handles.Structure(4),'CData',handles.Store.Model1A0);
            set(handles.Structure(5),'CData',handles.Store.Model2A1);
            set(handles.Structure(6),'CData',handles.Store.Model2B0);
            
            set(handles.Structure(9:10),'BackgroundColor',panelDefault,'Style','text');
            set(handles.Structure(11),'BackgroundColor',panelDefault,'Style','text','String','Period');
            set(handles.Structure(12),'BackgroundColor',panelDefault);
            set(handles.Structure(13),'BackgroundColor',panelDefault,'Style','text','String','zeta');
            
            set(handles.Structure(16:19),'BackgroundColor',[1 1 1],'Style','edit','String','');
            set(handles.Structure(21),'BackgroundColor',[1 1 1]);
            set(handles.Structure(22),'BackgroundColor',[1 1 1],'Style','edit');
            
            set(handles.Structure(25:28),'BackgroundColor',panelDefault,'Style','text');
            set(handles.Structure(29),'BackgroundColor',panelDefault,'Style','text','String','Period');
            set(handles.Structure(30),'BackgroundColor',panelDefault);
            set(handles.Structure(31),'BackgroundColor',panelDefault,'Style','text','String','zeta');
            
            set(dof1_children,'ForegroundColor',inactive_color);
            set(dof2_children,'ForegroundColor',active_color);
            set(dof3_children,'ForegroundColor',inactive_color);
            set(findobj('Tag','help1'),'CData',handles.Store.Question0);
            set(findobj('Tag','help2A'),'CData',handles.Store.Question1);
            set(findobj('Tag','help2B'),'CData',handles.Store.Question0);
            
            % remove unnecessary plots
            cla(handles.GroundMotions(13));
            cla(handles.GroundMotions(14));
            cla(handles.GroundMotions(15));
            
            % set DOF/CP options
            set(handles.EC(6),'String',{'Choose Story...','Story 1','Story 2'},'Value',1);
            set(handles.EC(7:10),'Visible','off');
            set(handles.EC(9),'String','2');
            set(handles.EC(27),'Value',1);
            set(handles.EC(54),'String',{'Existing Control Points...','Control Point 1','Control Point 2','Control Point 3','Control Point 4'});
            handles.ExpControl.store.CPOptions = {'Existing Control Points...','Control Point 1','Control Point 2','Control Point 3','Control Point 4'};
            set(handles.EC(57),'Style','popupmenu','String',{'Choose Node...','Node 1','Node 2'});
            handles.ExpControl.store.NodeOptions = get(handles.EC(57),'String');
            set(handles.EC(58),'String','1');
            handles.ExpControl.store.DOFOptions = {'Choose Story...','Story 1','Story 2'}';
            handles.ExpControl.NumAct = 2;
            handles.ExpControl.store.SimActive = 6;
            handles.ExpControl.store.RealActive = (26:27);
            
            % define element data
            handles.Model.Element{1}.tag = 1;
            handles.Model.Element{1}.type = 'Element_ExpGeneric';
            handles.Model.Element{1}.kInit = handles.Model.K;
            handles.Model.Element{1}.ipAddr = '127.0.0.1';
            handles.Model.Element{1}.ipPort = 7777;
            handles.Model.Element{1}.id = [1 2];
            
            % update model and ground motion parameters
            handles.Model.Type = '2 DOF A';
            handles.Model.Mass_field = 16;
            handles.Model.Stiffness_field = 18;
            handles.Model.Period_field = 20;
            handles.Model.Damping_field = 22;
            set(handles.ES(4),'Value',1);
            handles.Model.ndf = 2;
            handles.Model.b = [1;1];
            handles.GM.AmpFact(1) = str2num(get(handles.GroundMotions(4),'String'));
            handles.GM.TimeFact(1) = str2num(get(handles.GroundMotions(5),'String'));
            
            % initialize loading page
            set(handles.GroundMotions(23),'String','[U1 U2]');
            set(handles.GroundMotions(27),'String',{'Choose Mode...','Mode 1','Mode 2','User Defined'},'Value',1);
            
            % initialize control points page
            set(handles.EC(54),'Value',1);
            set(handles.EC(56),'String','');
            set(handles.EC([60 61 66 67 72 73 78 79]),'Value',1);
            set(handles.EC([62 68 74 80]),'String','1');
            set(handles.EC([63 69 75 81]),'Value',0);
            set(handles.EC([64 65 70 71 76 77 82 83]),'BackgroundColor',panelDefault,'Style','text','String','');
            
            % create preset control points
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
            
        %=========
        % 2 DOF B
        %=========
        else
            % initialize structure page and adjust button and field colors
            structTag = findobj('Tag','2 DOF B');
            set(handles.Structure(2),'Value',1);
            set(handles.Structure(3),'SelectedObject',structTag(2));
            set(handles.Structure([4 5 7:22]),'Visible','off');
            set(handles.Structure([6 23:31]),'Visible','on');
            
            handles.Model.StructActive = [6 23:31];
            handles.Model.StructInactive = [4 5 7:22];
            set(handles.Structure(4),'CData',handles.Store.Model1A0);
            set(handles.Structure(5),'CData',handles.Store.Model2A0);
            set(handles.Structure(6),'CData',handles.Store.Model2B1);
            
            set(handles.Structure(9:10),'BackgroundColor',panelDefault,'Style','text');
            set(handles.Structure(11),'BackgroundColor',panelDefault,'Style','text','String','Period');
            set(handles.Structure(12),'BackgroundColor',panelDefault);
            set(handles.Structure(13),'BackgroundColor',panelDefault,'Style','text','String','zeta');
            
            set(handles.Structure(16:19),'BackgroundColor',panelDefault,'Style','text');
            set(handles.Structure(18),'BackgroundColor',panelDefault,'Style','text');
            set(handles.Structure(20),'BackgroundColor',panelDefault,'Style','text','String','Period');
            set(handles.Structure(21),'BackgroundColor',panelDefault);
            set(handles.Structure(22),'BackgroundColor',panelDefault,'Style','text','String','zeta');
            
            set(handles.Structure(25:28),'BackgroundColor',[1 1 1],'Style','edit','String','');
            set(handles.Structure(30),'BackgroundColor',[1 1 1]);
            set(handles.Structure(31),'BackgroundColor',[1 1 1],'Style','edit');
            
            set(dof1_children,'ForegroundColor',inactive_color);
            set(dof2_children,'ForegroundColor',inactive_color);
            set(dof3_children,'ForegroundColor',active_color);
            set(findobj('Tag','help1'),'CData',handles.Store.Question0);
            set(findobj('Tag','help2A'),'CData',handles.Store.Question0);
            set(findobj('Tag','help2B'),'CData',handles.Store.Question1);
            
            % set DOF/CP options
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
            
            % define element data
            handles.Model.Element{1}.tag = 1;
            handles.Model.Element{1}.type = 'Element_ExpGeneric';
            handles.Model.Element{1}.kInit = handles.Model.K;
            handles.Model.Element{1}.ipAddr = '127.0.0.1';
            handles.Model.Element{1}.ipPort = 7777;
            handles.Model.Element{1}.id = [1 2];
            
            % update model and ground motion parameters
            handles.Model.Type = '2 DOF B';
            handles.Model.Mass_field = 25;
            handles.Model.Stiffness_field = 27;
            handles.Model.Period_field = 29;
            handles.Model.Damping_field = 31;
            set(handles.ES(5),'Value',1);
            handles.Model.ndf = 2;
            handles.Model.b = [1 0; 0 1];
            handles.GM.AmpFact(2) = str2num(get(handles.GroundMotions(12),'String'));
            handles.GM.TimeFact(2) = str2num(get(handles.GroundMotions(13),'String'));
            
            % initialize loading page
            set(handles.GroundMotions(23),'String','[U1 U2]');
            set(handles.GroundMotions(27),'String',{'Choose Mode...','Mode 1','Mode 2','User Defined'},'Value',1);
            
            % initialize control points page
            set(handles.EC(54),'Value',1);
            set(handles.EC(56),'String','');
            set(handles.EC([60 61 66 67 72 73 78 79]),'Value',1);
            set(handles.EC([62 68 74 80]),'String','1');
            set(handles.EC([63 69 75 81]),'Value',0);
            set(handles.EC([64 65 70 71 76 77 82 83]),'BackgroundColor',panelDefault,'Style','text','String','');
            
            % create preset control points
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
    % =====================================================================
    case 'choose DOF'
        active_color = [0 0 0];
        inactive_color = [0.6 0.6 0.6];
        panelDefault = [0.941176 0.941176 0.941176];
        dof1_children = get(handles.Structure(8),'Children')';
        dof2_children = get(handles.Structure(15),'Children')';
        dof3_children = get(handles.Structure(24),'Children')';
        
        % reset Values
        handles.Model.M = [];
        handles.Model.K = [];
        handles.Model.Zeta = [];
        switch DOF_selection
            % -------------------------------------------------------------
            case '1 DOF'
                handles.Model.StructActive = [4 7:13];
                handles.Model.StructInactive = [5 6 14:31];
                % adjust colors to emphasize active/inactive fields
                set(handles.Structure(4),'CData',handles.Store.Model1A1);
                set(handles.Structure(5),'CData',handles.Store.Model2A0);
                set(handles.Structure(6),'CData',handles.Store.Model2B0);
                
                set(handles.Structure(9:10),'BackgroundColor',[1 1 1],'Style','edit');
                set(handles.Structure(12),'BackgroundColor',[1 1 1]);
                set(handles.Structure(13),'BackgroundColor',[1 1 1],'Style','edit');
                
                set(handles.Structure(16:19),'BackgroundColor',panelDefault,'Style');
                set(handles.Structure(20),'BackgroundColor',panelDefault,'Style','text','String','Period');
                set(handles.Structure(21),'BackgroundColor',panelDefault);
                set(handles.Structure(22),'BackgroundColor',panelDefault,'Style','text','String','zeta');
                
                set(handles.Structure(25:28),'BackgroundColor',panelDefault,'Style');
                set(handles.Structure(29),'BackgroundColor',panelDefault,'Style','text','String','Period');
                set(handles.Structure(30),'BackgroundColor',panelDefault);
                set(handles.Structure(31),'BackgroundColor',panelDefault,'Style','text','String','zeta');
                
                set(dof1_children,'ForegroundColor',active_color);
                set(dof2_children,'ForegroundColor',inactive_color);
                set(dof3_children,'ForegroundColor',inactive_color);
                set(findobj('Tag','help1'),'CData',handles.Store.Question1);
                set(findobj('Tag','help2A'),'CData',handles.Store.Question0);
                set(findobj('Tag','help2B'),'CData',handles.Store.Question0);
                
                % remove unnecessary plots
                cla(handles.GroundMotions(15));
                cla(handles.GroundMotions(16));
                cla(handles.GroundMotions(17));
                
                % set DOF/CP options
                set(handles.EC(6),'String',{'Choose DOF...','DOF 1'},'Value',1);
                set(handles.EC(9),'String','1');
                set(handles.EC(27),'Value',1);
                set(handles.EC(54),'String',{'Existing Control Points...','Control Point 1','Control Point 2'});
                handles.ExpControl.store.CPOptions = {'Existing Control Points...','Control Point 1','Control Point 2'};
                set(handles.EC(57),'Style','edit','String','1');
                handles.ExpControl.store.NodeOptions = get(handles.EC(57),'String');
                set(handles.EC(58),'String','1');
                %handles.ExpControl.CtrlDOF = 'DOF 1';
                handles.ExpControl.store.DOFOptions = {'Choose DOF...','DOF 1'}';
                handles.ExpControl.NumAct = 1;
                handles.ExpControl.store.SimActive = 6;
                handles.ExpControl.store.RealActive = (26:27);
                
                % define element data
                handles.Model.Element{1}.tag = 1;
                handles.Model.Element{1}.type = 'Element_ExpGeneric';
                handles.Model.Element{1}.kInit = handles.Model.K;
                handles.Model.Element{1}.ipAddr = '127.0.0.1';
                handles.Model.Element{1}.ipPort = 7777;
                handles.Model.Element{1}.id = 1;
                
                % update model and ground motion parameters
                handles.Model.Type = DOF_selection;
                handles.Model.Mass_field = 9;
                handles.Model.Stiffness_field = 10;
                handles.Model.Period_field = 11;
                handles.Model.Damping_field = 13;
                set(handles.ES(3),'Value',1);
                handles.Model.ndf = 1;
                handles.Model.b = 1;
                handles.GM.AmpFact(1) = str2num(get(handles.GroundMotions(4),'String'));
                handles.GM.TimeFact(1) = str2num(get(handles.GroundMotions(5),'String'));
                
                % initialize loading page
                set(handles.GroundMotions(23),'String','Enter displacement here');
                set(handles.GroundMotions(27),'String',{'Choose Mode...','Mode 1','User Defined'},'Value',1);
                
                % initialize control points page
                set(handles.EC(54),'Value',1);
                set(handles.EC(56),'String','');
                set(handles.EC([60 61 66 67 72 73 78 79]),'Value',1);
                set(handles.EC([62 68 74 80]),'String','1');
                set(handles.EC([63 69 75 81]),'Value',0);
                set(handles.EC([64 65 70 71 76 77 82 83]),'BackgroundColor',panelDefault,'Style','text','String','');
                
                % create preset control points
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
            % -------------------------------------------------------------
            case '2 DOF A'
                handles.Model.StructActive = [5 14:22];
                handles.Model.StructInactive = [4 6 7:13 23:31];
                % adjust colors to emphasize active/inactive fields
                set(handles.Structure(4),'CData',handles.Store.Model1A0);
                set(handles.Structure(5),'CData',handles.Store.Model2A1);
                set(handles.Structure(6),'CData',handles.Store.Model2B0);
                
                set(handles.Structure(9:10),'BackgroundColor',panelDefault,'Style','text','String','Enter mass here');
                set(handles.Structure(11),'BackgroundColor',panelDefault,'Style','text','String','Period');
                set(handles.Structure(12),'BackgroundColor',panelDefault);
                set(handles.Structure(13),'BackgroundColor',panelDefault,'Style','text','String','zeta');
                
                set(handles.Structure(16:19),'BackgroundColor',[1 1 1],'Style','edit');
                set(handles.Structure(21),'BackgroundColor',[1 1 1]);
                set(handles.Structure(22),'BackgroundColor',[1 1 1],'Style','edit');
                
                set(handles.Structure(25:28),'BackgroundColor',panelDefault,'Style','text');
                set(handles.Structure(29),'BackgroundColor',panelDefault,'Style','text','String','Period');
                set(handles.Structure(30),'BackgroundColor',panelDefault);
                set(handles.Structure(31),'BackgroundColor',panelDefault,'Style','text','String','zeta');
                
                set(dof1_children,'ForegroundColor',inactive_color);
                set(dof2_children,'ForegroundColor',active_color);
                set(dof3_children,'ForegroundColor',inactive_color);
                set(findobj('Tag','help1'),'CData',handles.Store.Question0);
                set(findobj('Tag','help2A'),'CData',handles.Store.Question1);
                set(findobj('Tag','help2B'),'CData',handles.Store.Question0);
                
                % remove unnecessary plots
                cla(handles.GroundMotions(15));
                cla(handles.GroundMotions(16));
                cla(handles.GroundMotions(17));
                
                % set DOF/CP options
                set(handles.EC(6),'String',{'Choose Story...','Story 1','Story 2'},'Value',1);
                set(handles.EC(7:10),'Visible','off');
                set(handles.EC(9),'String','2');
                set(handles.EC(27),'Value',1);
                set(handles.EC(54),'String',{'Existing Control Points...','Control Point 1','Control Point 2','Control Point 3','Control Point 4'});
                handles.ExpControl.store.CPOptions = {'Existing Control Points...','Control Point 1','Control Point 2','Control Point 3','Control Point 4'};
                set(handles.EC(57),'Style','popupmenu','String',{'Choose Node...','Node 1','Node 2'});
                handles.ExpControl.store.NodeOptions = get(handles.EC(57),'String');
                set(handles.EC(58),'String','1');
                %handles.ExpControl.CtrlDOF = 'DOF 1';
                handles.ExpControl.store.DOFOptions = {'Choose Story...','Story 1','Story 2'}';
                handles.ExpControl.NumAct = 2;
                handles.ExpControl.store.SimActive = 6;
                handles.ExpControl.store.RealActive = (26:27);
                
                % define element data
                handles.Model.Element{1}.tag = 1;
                handles.Model.Element{1}.type = 'Element_ExpGeneric';
                handles.Model.Element{1}.kInit = handles.Model.K;
                handles.Model.Element{1}.ipAddr = '127.0.0.1';
                handles.Model.Element{1}.ipPort = 7777;
                handles.Model.Element{1}.id = [1 2];
                
                % update model and ground motion parameters
                handles.Model.Type = DOF_selection;
                handles.Model.Mass_field = 16;
                handles.Model.Stiffness_field = 18;
                handles.Model.Period_field = 20;
                handles.Model.Damping_field = 22;
                set(handles.ES(4),'Value',1);
                handles.Model.ndf = 2;
                handles.Model.b = [1;1];
                handles.GM.AmpFact(1) = str2num(get(handles.GroundMotions(4),'String'));
                handles.GM.TimeFact(1) = str2num(get(handles.GroundMotions(5),'String'));
                
                % initialize loading page
                set(handles.GroundMotions(23),'String','[U1 U2]');
                set(handles.GroundMotions(27),'String',{'Choose Mode...','Mode 1','Mode 2','User Defined'},'Value',1);
                
                % initialize control points page
                set(handles.EC(54),'Value',1);
                set(handles.EC(56),'String','');
                set(handles.EC([60 61 66 67 72 73 78 79]),'Value',1);
                set(handles.EC([62 68 74 80]),'String','1');
                set(handles.EC([63 69 75 81]),'Value',0);
                set(handles.EC([64 65 70 71 76 77 82 83]),'BackgroundColor',panelDefault,'Style','text','String','');
                
                % create preset control points
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
            % -------------------------------------------------------------
            case '2 DOF B'
                handles.Model.StructActive = [6 23:31];
                handles.Model.StructInactive = [4 5 7:22];
                
                % adjust colors to emphasize active/inactive fields
                set(handles.Structure(4),'CData',handles.Store.Model1A0);
                set(handles.Structure(5),'CData',handles.Store.Model2A0);
                set(handles.Structure(6),'CData',handles.Store.Model2B1);
                
                set(handles.Structure(9:10),'BackgroundColor',panelDefault,'Style','text','String','Enter mass here');
                set(handles.Structure(11),'BackgroundColor',panelDefault,'Style','text','String','Period');
                set(handles.Structure(12),'BackgroundColor',panelDefault);
                set(handles.Structure(13),'BackgroundColor',panelDefault,'Style','text','String','zeta');
                
                set(handles.Structure(16:19),'BackgroundColor',panelDefault,'Style','text');
                set(handles.Structure(20),'BackgroundColor',panelDefault,'Style','text','String','Period');
                set(handles.Structure(21),'BackgroundColor',panelDefault);
                set(handles.Structure(22),'BackgroundColor',panelDefault,'Style','text','String','zeta');
                
                set(handles.Structure(25:28),'BackgroundColor',[1 1 1],'Style','edit');
                set(handles.Structure(30),'BackgroundColor',[1 1 1]);
                set(handles.Structure(31),'BackgroundColor',[1 1 1],'Style','edit');
                
                set(dof1_children,'ForegroundColor',inactive_color);
                set(dof2_children,'ForegroundColor',inactive_color);
                set(dof3_children,'ForegroundColor',active_color);
                set(findobj('Tag','help1'),'CData',handles.Store.Question0);
                set(findobj('Tag','help2A'),'CData',handles.Store.Question0);
                set(findobj('Tag','help2B'),'CData',handles.Store.Question1);
                
                % set DOF/CP options
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
                
                % define element data
                handles.Model.Element{1}.tag = 1;
                handles.Model.Element{1}.type = 'Element_ExpGeneric';
                handles.Model.Element{1}.kInit = handles.Model.K;
                handles.Model.Element{1}.ipAddr = '127.0.0.1';
                handles.Model.Element{1}.ipPort = 7777;
                handles.Model.Element{1}.id = [1 2];
                
                % update model and ground motion parameters
                handles.Model.Type = DOF_selection;
                handles.Model.Mass_field = 25;
                handles.Model.Stiffness_field = 27;
                handles.Model.Period_field = 29;
                handles.Model.Damping_field = 31;
                set(handles.ES(5),'Value',1);
                handles.Model.ndf = 2;
                handles.Model.b = [1 0; 0 1];
                handles.GM.AmpFact(2) = str2num(get(handles.GroundMotions(12),'String'));
                handles.GM.TimeFact(2) = str2num(get(handles.GroundMotions(13),'String'));
                
                % initialize loading page
                set(handles.GroundMotions(23),'String','[U1 U2]');
                set(handles.GroundMotions(27),'String',{'Choose Mode...','Mode 1','Mode 2','User Defined'},'Value',1);
                
                % initialize control points page
                set(handles.EC(54),'Value',1);
                set(handles.EC(56),'String','');
                set(handles.EC([60 61 66 67 72 73 78 79]),'Value',1);
                set(handles.EC([62 68 74 80]),'String','1');
                set(handles.EC([63 69 75 81]),'Value',0);
                set(handles.EC([64 65 70 71 76 77 82 83]),'BackgroundColor',panelDefault,'Style','text','String','');
                
                % create preset control points
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
            % -------------------------------------------------------------
        end        
        guidata(gcbf,handles);
        
    % store input values
    % =====================================================================
    case 'mass_input'
        input_val = str2num(get(gcbo,'String'));
        
        % return if invalid mass entered
        if isempty(input_val) || length(input_val) ~= 1 || input_val <= 0
            msgbox('Must enter valid mass value(s)','Invalid Input','error');
            return
        else
            switch get(gcbo,'Tag')
                % store input value
                case 'Mass'
                    handles.Model.M = input_val;
                case 'm1A'
                    handles.Model.M(1,1) = input_val;
                case 'm2A'
                    handles.Model.M(2,2) = input_val;
                case 'm1B'
                    handles.Model.M(1,1) = input_val;
                case 'm2B'
                    handles.Model.M(2,2) = input_val;
            end
            guidata(gcbf, handles);
        end
        if isempty(handles.Model.K)
            return;
        else
            % calculate mode shapes and periods
            if strcmp(handles.Model.Type,'1 DOF')
                handles.Model.Modes = 1;
                handles.Model.Omega = sqrt(eig(handles.Model.K,handles.Model.M));
                handles.Model.minTDOF = 1;
                handles.Model.T = 2*pi./(handles.Model.Omega);
                set(handles.Structure(handles.Model.Period_field),'String',sprintf(['Period:    ' num2str(handles.Model.T')]));
            else
                if length(handles.Model.M) ~= 2 || handles.Model.M(1,1) == 0
                    return
                else
                    [Phi,OmegaSq] = eig(handles.Model.K,handles.Model.M);
                    if Phi(1,1) ~= 0
                        Mode1 = Phi(:,1)./Phi(1,1);
                    else
                        Mode1 = Phi(:,1)./Phi(2,1);
                    end
                    if Phi(2,2) ~= 0
                        Mode2 = Phi(:,2)./Phi(2,2);
                    else
                        Mode2 = Phi(:,2)./Phi(1,2);
                    end
                    handles.Model.Modes = [Mode1 Mode2];
                    handles.Model.Omega = sqrt(diag(OmegaSq));
                    if handles.Model.Omega(1) < handles.Model.Omega(2)
                        handles.Model.minTDOF = 2;
                    else
                        handles.Model.minTDOF = 1;
                    end
                    handles.Model.T = 2*pi./(handles.Model.Omega);
                    set(handles.Structure(handles.Model.Period_field),'String',sprintf(['Period:    ' num2str(handles.Model.T')]));
                end                
            end
            guidata(gcbf, handles);
        end
    % =====================================================================
    case 'stiffness_input'
        input_val = str2num(get(gcbo,'String'));
        % return if invalid stiffness entered
        if isempty(input_val) || length(input_val) ~= 1 || input_val <= 0
            msgbox('Must enter valid stiffness value','Invalid Input','error');
            return
        else
            switch get(gcbo,'Tag')
                % store/update input values
                case 'Stiffness'
                    handles.Model.K = input_val;
                    handles.ExpControl.DOF1.E = handles.Model.K;
                case 'k1A'
                    if length(handles.Model.K) == 2
                        handles.Model.K(1,1) = handles.Model.K(2,2) + input_val;
                    else
                        handles.Model.K(1,1) = input_val;
                    end
                    handles.ExpControl.DOF1.E = input_val;
                case 'k2A'
                    if isempty(handles.Model.K)
                        msgbox('Please enter k1 first','Error','error');
                        set(handles.Structure(19),'String','');
                        return
                    else
                        handles.Model.K(1,1) = str2num(get(handles.Structure(18),'String')) + input_val;
                    end
                    handles.Model.K(1,2) = -input_val;
                    handles.Model.K(2,1) = -input_val;
                    handles.Model.K(2,2) = input_val;
                    handles.ExpControl.DOF2.E = handles.Model.K(2,2);
                case 'k1B'
                    handles.Model.K(1,1) = input_val;
                    handles.ExpControl.DOF1.E = handles.Model.K(1,1);
                case 'k2B'
                    handles.Model.K(2,2) = input_val;
                    handles.ExpControl.DOF2.E = handles.Model.K(2,2);
            end
            handles.Model.Element{1}.kInit = handles.Model.K;
            guidata(gcbf, handles);
        end
        
        if isempty(handles.Model.M)
            return;
        else
            % calculate mode shapes and periods
            if strcmp(handles.Model.Type,'1 DOF')
                handles.Model.Modes = 1;
                handles.Model.Omega = sqrt(eig(handles.Model.K,handles.Model.M));
                handles.Model.minTDOF = 1;
                handles.Model.T = 2*pi./(handles.Model.Omega);
                set(handles.Structure(handles.Model.Period_field),'String',sprintf(['Period:    ' num2str(handles.Model.T')]));                
            else
                if length(handles.Model.K) ~= 2 || handles.Model.K(1,1) == 0
                    return
                else
                    [Phi,OmegaSq] = eig(handles.Model.K,handles.Model.M);
                    if Phi(1,1) ~= 0
                        Mode1 = Phi(:,1)./Phi(1,1);
                    else
                        Mode1 = Phi(:,1)./Phi(2,1);
                    end
                    if Phi(2,2) ~= 0
                        Mode2 = Phi(:,2)./Phi(2,2);
                    else
                        Mode2 = Phi(:,2)./Phi(1,2);
                    end
                    handles.Model.Modes = [Mode1 Mode2];
                    handles.Model.Omega = sqrt(diag(OmegaSq));
                    if handles.Model.Omega(1) < handles.Model.Omega(2)
                        handles.Model.minTDOF = 2;
                    else
                        handles.Model.minTDOF = 1;
                    end
                    handles.Model.T = 2*pi./(handles.Model.Omega);
                    set(handles.Structure(handles.Model.Period_field),'String',sprintf(['Period:    ' num2str(handles.Model.T')]));
                end
            end
        end
        guidata(gcbf, handles);
    % =====================================================================
    case 'choose damping'
        % return if mass or stiffness are not specified
        if ~isfield(handles.Model,'T')
            msgbox('Must specify model mass and stiffness first!','Error','error');
            set(handles.Structure([12 21 30]),'Value',1);
            return
        end
        damp_type = get(gcbo,'Value');
        switch damp_type
            % calculate damping parameters, if possible
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
                msgbox('Rayleigh damping requires two coefficients.','Rayleigh Coefficients','warn');
        end
        guidata(gcbf, handles);
    % =====================================================================
    case 'damping_input'
        % return if mass,stiffness or damping type are not specified
        if ~isfield(handles.Model,'T')
            msgbox('Must specify model mass and stiffness first!','Error','error');
            set(handles.Structure([13 22 31]),'String','zeta');
            return
        end
        if ~isfield(handles.Model,'DampType')
            msgbox('Must choose damping type!','Error','error');
            set(handles.Structure([13 22 31]),'String','zeta');
            return
        end
        input_val = str2num(get(handles.Structure(handles.Model.Damping_field),'String'));
        handles.Model.Zeta = input_val;
        guidata(gcbf, handles);
        damp_type = handles.Model.DampType;
        switch damp_type
            % calculate damping parameters
            case 'Stiffness Proportional'
                handles.Model.alphaM = 0;
                handles.Model.betaK = handles.Model.Zeta*handles.Model.T(1)/pi;
            case 'Mass Proportional'
                handles.Model.alphaM = 4*pi*handles.Model.Zeta/handles.Model.T(1);
                handles.Model.betaK = 0;
            case 'Rayleigh'
                check_matrix = length(handles.Model.Zeta);
                if check_matrix ~=2
                    msgbox('Rayleigh damping requires two coefficients.','Error','error');
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
        % update response spectra if ground motion exists
        if ~isempty(handles.GM.Spectra{1})
            handles.GM.Spectra{1} = ResponseSpectraElastic(handles.GM.scaleag{1},handles.GM.scaledt(1),1.0,handles.Model.Zeta,0.01,4.99,300);
            plot(handles.GroundMotions(7), handles.GM.scalet{1}, handles.GM.scaleag{1});
            plot(handles.GroundMotions(8), handles.GM.Spectra{1}.T, handles.GM.Spectra{1}.psdAcc);
            plot(handles.GroundMotions(9), handles.GM.Spectra{1}.T, handles.GM.Spectra{1}.dsp);
        end
        if length(handles.GM.Spectra)==2 && ~isempty(handles.GM.Spectra{2})
            handles.GM.Spectra{2} = ResponseSpectraElastic(handles.GM.scaleag{2},handles.GM.scaledt(2),1.0,handles.Model.Zeta,0.01,4.99,300);
            plot(handles.GroundMotions(15), handles.GM.scalet{2}, handles.GM.scaleag{2});
            plot(handles.GroundMotions(16), handles.GM.Spectra{2}.T, handles.GM.Spectra{2}.psdAcc);
            plot(handles.GroundMotions(17), handles.GM.Spectra{2}.T, handles.GM.Spectra{2}.dsp);
        end
        guidata(gcbf, handles);
    % =====================================================================
end

% update handles structure
guidata(gcbf,handles);
