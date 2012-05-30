function ExpControl(action,varargin)
%EXPCONTROL to handle user inputs related to the Experimental Control
% ExpControl(action,varargin)
%
% action   : selected action on the Experimental Control page
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

% initialization tasks
handles = guidata(gcbf);

% store which tab is selected
Tab_Selection = get(gcbo,'Tag');

switch action
    % =====================================================================
    case 'tab toggle'
        % switch panel display based on tab selection
        switch Tab_Selection
            % -------------------------------------------------------------
            case 'Control Point'
                set(handles.EC(2),'CData',handles.Store.CP1);
                set(handles.EC(3),'Value',0,'CData',handles.Store.Sim0);
                set(handles.EC(4),'Value',0,'CData',handles.Store.Real0);
                set(handles.EC([5 25]),'Visible','off');
                set(handles.EC([53:65 96]),'Visible','on');
                handles.ExpControl.Type = 'Real';
            % -------------------------------------------------------------
            case 'Simulation'
                set(handles.EC(2),'Value',0,'CData',handles.Store.CP0);
                set(handles.EC(3),'CData',handles.Store.Sim1);
                set(handles.EC(4),'Value',0,'CData',handles.Store.Real0);
                set(handles.EC(5:6),'Visible','on');
                set(handles.EC([25 53]),'Visible','off');
                handles.ExpControl.Type = 'Simulation';
                set(handles.EC(handles.ExpControl.store.SimActive),'Visible','on');
            % -------------------------------------------------------------
            case 'Real Controller'
                set(handles.EC(2),'Value',0,'CData',handles.Store.CP0);
                set(handles.EC(3),'Value',0,'CData',handles.Store.Sim0);
                set(handles.EC(4),'CData',handles.Store.Real1);
                set(handles.EC([5 53]),'Visible','off');
                set(handles.EC(25:27),'Visible','on');
                handles.ExpControl.Type = 'Real';
                set(handles.EC(handles.ExpControl.store.RealActive),'Visible','on');
            % -------------------------------------------------------------
        end
    % =====================================================================
    case 'CtrlDOF'
        % adjust panel display for DOF selection
        selection = get(gcbo,'Value');
        set(handles.EC(7:10),'Visible','on');
        switch selection
            % -------------------------------------------------------------
            case 1
                % no DOF selected
                handles.ExpControl.store.SimActive = 6;
                set(handles.EC(7:24),'Visible','off');
                set(handles.EC(handles.ExpControl.store.SimActive),'Visible','on');
            % -------------------------------------------------------------
            case 2
                % DOF 1 selected
                handles.ExpControl.CtrlDOF = 'DOF 1';
                set(handles.EC(12),'String',handles.ExpControl.DOF1.E);
                set(handles.EC(14),'String',handles.ExpControl.DOF1.E);
                set(handles.EC(15),'String',handles.ExpControl.DOF1.epsP);
                set(handles.EC(17),'String',handles.ExpControl.DOF1.Fy);
                set(handles.EC(18),'String',handles.ExpControl.DOF1.E);
                set(handles.EC(19),'String',handles.ExpControl.DOF1.b);
                set(handles.EC(21),'String',handles.ExpControl.DOF1.Fy);
                set(handles.EC(22),'String',handles.ExpControl.DOF1.E);
                set(handles.EC(23),'String',handles.ExpControl.DOF1.b);
                set(handles.EC(24),'String',handles.ExpControl.DOF1.R0);
                if ~isempty(handles.ExpControl.DOF1.SimMaterial)
                    index = find(strcmp(handles.ExpControl.store.MatOptions, handles.ExpControl.DOF1.SimMaterial) == 1);
                    set(handles.EC(10),'Value',index);
                    switch index
                        case 2
                            % Elastic
                            handles.ExpControl.store.SimActive = (7:12);
                            set(handles.EC(11:24),'Visible','off');
                            set(handles.EC(handles.ExpControl.store.SimActive),'Visible','on');
                        case 3
                            % EPP
                            handles.ExpControl.store.SimActive = [7:10 13:15];
                            set(handles.EC(11:24),'Visible','off');
                            set(handles.EC(handles.ExpControl.store.SimActive),'Visible','on');
                        case 4
                            % Steel Bilinear
                            handles.ExpControl.store.SimActive = [7:10 16:19];
                            set(handles.EC(11:24),'Visible','off');
                            set(handles.EC(handles.ExpControl.store.SimActive),'Visible','on');
                        case 5
                            % Steel GMP
                            handles.ExpControl.store.SimActive = [7:10 20:24];
                            set(handles.EC(11:24),'Visible','off');
                            set(handles.EC(handles.ExpControl.store.SimActive),'Visible','on');
                    end
                else
                    set(handles.EC(10),'Value',1);
                    handles.ExpControl.store.SimActive = (7:10);
                    set(handles.EC(11:24),'Visible','off');
                    set(handles.EC(handles.ExpControl.store.SimActive),'Visible','on');
                end
            % -------------------------------------------------------------
            case 3
                % DOF 2 selected
                handles.ExpControl.CtrlDOF = 'DOF 2';
                set(handles.EC(12),'String',handles.ExpControl.DOF2.E);
                set(handles.EC(14),'String',handles.ExpControl.DOF2.E);
                set(handles.EC(15),'String',handles.ExpControl.DOF2.epsP);
                set(handles.EC(17),'String',handles.ExpControl.DOF2.Fy);
                set(handles.EC(18),'String',handles.ExpControl.DOF2.E);
                set(handles.EC(19),'String',handles.ExpControl.DOF2.b);
                set(handles.EC(21),'String',handles.ExpControl.DOF2.Fy);
                set(handles.EC(22),'String',handles.ExpControl.DOF2.E);
                set(handles.EC(23),'String',handles.ExpControl.DOF2.b);
                set(handles.EC(24),'String',handles.ExpControl.DOF2.R0);
                if ~isempty(handles.ExpControl.DOF2.SimMaterial)
                    index = find(strcmp(handles.ExpControl.store.MatOptions, handles.ExpControl.DOF2.SimMaterial) == 1);
                    set(handles.EC(10),'Value',index);
                    switch index
                        case 2
                            % Elastic
                            handles.ExpControl.store.SimActive = (7:12);
                            set(handles.EC(11:24),'Visible','off');
                            set(handles.EC(handles.ExpControl.store.SimActive),'Visible','on');
                        case 3
                            % EPP
                            handles.ExpControl.store.SimActive = [7:10 13:15];
                            set(handles.EC(11:24),'Visible','off');
                            set(handles.EC(handles.ExpControl.store.SimActive),'Visible','on');
                        case 4
                            % Steel Bilinear
                            handles.ExpControl.store.SimActive = [7:10 16:19];
                            set(handles.EC(11:24),'Visible','off');
                            set(handles.EC(handles.ExpControl.store.SimActive),'Visible','on');
                        case 5
                            % Steel GMP
                            handles.ExpControl.store.SimActive = [7:10 20:24];
                            set(handles.EC(11:24),'Visible','off');
                            set(handles.EC(handles.ExpControl.store.SimActive),'Visible','on');
                    end
                else
                    set(handles.EC(10),'Value',1);
                    handles.ExpControl.store.SimActive = (7:10);
                    set(handles.EC(11:24),'Visible','off');
                    set(handles.EC(handles.ExpControl.store.SimActive),'Visible','on');
                end
            % -------------------------------------------------------------
        end
    % =====================================================================
    case 'Sim control'
        control_selection = get(gcbo,'Value');
        switch control_selection
            case 2
                handles.ExpControl.SimControl.SimType = 'SimUniaxialMaterials';
                % when SimDomain and other options are added, a case can be
                % added for each of those
        end
    % =====================================================================
    case 'Sim Material'
        if get(handles.EC(8),'Value') == 1
            msgbox('Must choose control type first!','Choose Control','error');
            set(handles.EC(10),'Value',1);
        else
            % display options for different simulation material types
            material_selection = get(gcbo,'Value');
            switch material_selection
                % ---------------------------------------------------------
                case 1  % no selection
                    handles.ExpControl.store.SimActive = (7:10);
                    set(handles.EC(11:24),'Visible','off');
                    switch handles.ExpControl.CtrlDOF
                        case 'DOF 1'
                            handles.ExpControl.DOF1.SimMaterial = [];
                        case 'DOF 2'
                            handles.ExpControl.DOF2.SimMaterial = [];
                    end
                % ---------------------------------------------------------
                case 2  % Elastic
                    handles.ExpControl.store.SimActive = (7:12);
                    set(handles.EC(11:24),'Visible','off');
                    set(handles.EC(handles.ExpControl.store.SimActive),'Visible','on');
                    switch handles.ExpControl.CtrlDOF
                        case 'DOF 1'
                            handles.ExpControl.DOF1.SimMaterial = 'Elastic';
                            set(handles.EC(12),'String',handles.ExpControl.DOF1.E);
                        case 'DOF 2'
                            handles.ExpControl.DOF2.SimMaterial = 'Elastic';
                            set(handles.EC(12),'String',handles.ExpControl.DOF2.E);
                    end
                % ---------------------------------------------------------
                case 3  % EPP
                    handles.ExpControl.store.SimActive = [7:10 13:15];
                    set(handles.EC(11:24),'Visible','off');
                    set(handles.EC(handles.ExpControl.store.SimActive),'Visible','on');
                    switch handles.ExpControl.CtrlDOF
                        case 'DOF 1'
                            handles.ExpControl.DOF1.SimMaterial = 'Elastic-Perfectly Plastic';
                            set(handles.EC(14),'String',handles.ExpControl.DOF1.E);
                            set(handles.EC(15),'String',handles.ExpControl.DOF1.epsP);
                        case 'DOF 2'
                            handles.ExpControl.DOF2.SimMaterial = 'Elastic-Perfectly Plastic';
                            set(handles.EC(14),'String',handles.ExpControl.DOF2.E);
                            set(handles.EC(15),'String',handles.ExpControl.DOF2.epsP);
                    end
                % ---------------------------------------------------------
                case 4  % Steel Bilinear
                    handles.ExpControl.store.SimActive = [7:10 16:19];
                    set(handles.EC(11:24),'Visible','off');
                    set(handles.EC(handles.ExpControl.store.SimActive),'Visible','on');
                    switch handles.ExpControl.CtrlDOF
                        case 'DOF 1'
                            handles.ExpControl.DOF1.SimMaterial = 'Steel - Bilinear';
                            set(handles.EC(17),'String',handles.ExpControl.DOF1.Fy);
                            set(handles.EC(18),'String',handles.ExpControl.DOF1.E);
                            set(handles.EC(19),'String',handles.ExpControl.DOF1.b);
                        case 'DOF 2'
                            handles.ExpControl.DOF2.SimMaterial = 'Steel - Bilinear';
                            set(handles.EC(17),'String',handles.ExpControl.DOF2.Fy);
                            set(handles.EC(18),'String',handles.ExpControl.DOF2.E);
                            set(handles.EC(19),'String',handles.ExpControl.DOF2.b);
                    end
                % ---------------------------------------------------------
                case 5  % Steel GMP
                    handles.ExpControl.store.SimActive = [7:10 20:24];
                    set(handles.EC(11:24),'Visible','off');
                    set(handles.EC(handles.ExpControl.store.SimActive),'Visible','on');
                    switch handles.ExpControl.CtrlDOF
                        case 'DOF 1'
                            handles.ExpControl.DOF1.SimMaterial = 'Steel - Giuffré-Menegotto-Pinto';
                            handles.ExpControl.DOF1.cR1 = 0.925;
                            handles.ExpControl.DOF1.cR2 = 0.15;
                            set(handles.EC(21),'String',handles.ExpControl.DOF1.Fy);
                            set(handles.EC(22),'String',handles.ExpControl.DOF1.E);
                            set(handles.EC(23),'String',handles.ExpControl.DOF1.b);
                            set(handles.EC(24),'String',handles.ExpControl.DOF1.R0);
                        case 'DOF 2'
                            handles.ExpControl.DOF2.SimMaterial = 'Steel - Giuffré-Menegotto-Pinto';
                            handles.ExpControl.DOF2.cR1 = 0.925;
                            handles.ExpControl.DOF2.cR2 = 0.15;
                            set(handles.EC(21),'String',handles.ExpControl.DOF2.Fy);
                            set(handles.EC(22),'String',handles.ExpControl.DOF2.E);
                            set(handles.EC(23),'String',handles.ExpControl.DOF2.b);
                            set(handles.EC(24),'String',handles.ExpControl.DOF2.R0);
                    end
                % ---------------------------------------------------------
            end
        end
    % =====================================================================
    case 'Real control'
        % display options for real controllers
        control_selection = get(gcbo,'Value');
        switch control_selection
            % -------------------------------------------------------------
            case 1  % no selection
                set(handles.EC(28:52),'Visible','off');
                set(handles.EC(2),'Visible','off');
            % -------------------------------------------------------------
            case 2  % LabVIEW
                msgbox('When using LabView controller, control points need to be defined','Control Point','warn');
                handles.ExpControl.RealControl.Controller = 'LabVIEW';
                handles.ExpControl.store.RealActive = (25:33);
                set(handles.EC(2),'Visible','on');
                set(handles.EC(25:52),'Visible','off');
                set(handles.EC(handles.ExpControl.store.RealActive),'Visible','on');
                set(handles.EC(29),'String',handles.ExpControl.RealControl.ipAddr);
            % -------------------------------------------------------------
            case 3  % MTSCsi
                handles.ExpControl.RealControl.Controller = 'MTSCsi';
                handles.ExpControl.store.RealActive = [25:27 34:38];
                set(handles.EC(2),'Visible','off');
                set(handles.EC(25:52),'Visible','off');
                set(handles.EC(handles.ExpControl.store.RealActive),'Visible','on');
            % -------------------------------------------------------------
            case 4  % SCRAMNet
                handles.ExpControl.RealControl.Controller = 'SCRAMNet';
                handles.ExpControl.store.RealActive = [25:27 39:41];
                set(handles.EC(2),'Visible','off');
                set(handles.EC(25:52),'Visible','off');
                set(handles.EC(handles.ExpControl.store.RealActive),'Visible','on');
            % -------------------------------------------------------------
            case 5  % dSpace
                handles.ExpControl.RealControl.Controller = 'dSpace';
                handles.ExpControl.store.RealActive = [25:27 42:44];
                set(handles.EC(2),'Visible','off');
                set(handles.EC(25:52),'Visible','off');
                set(handles.EC(handles.ExpControl.store.RealActive),'Visible','on');
            % -------------------------------------------------------------
            case 6  % xPCtarget
                handles.ExpControl.RealControl.Controller = 'xPCtarget';
                handles.ExpControl.store.RealActive = [25:27 45:52];
                set(handles.EC(2),'Visible','off');
                set(handles.EC(25:52),'Visible','off');
                set(handles.EC(handles.ExpControl.store.RealActive),'Visible','on');
                handles.ExpControl.RealControl.ipPort = get(handles.EC(49),'String');
            % -------------------------------------------------------------
        end
        
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % data input cases
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    
    % =====================================================================
    % sim controller
    % =====================================================================
    case 'NumAct'
        msgbox({'Cannot change number of actuators!','Can only change structure type!'},'Error','error');
        set(handles.EC(9),'String',handles.ExpControl.NumAct);
    % =====================================================================
    case 'E'
        switch handles.ExpControl.CtrlDOF
            case 'DOF 1'
                if strcmp(handles.Model.Type, '2 DOF A')
                    if ~strcmp(get(gcbo,'String'),num2str(handles.Model.K(1,1)-handles.Model.K(2,2)))
                        msgbox(sprintf(['Modulus does not match with structure stiffness (' num2str(handles.Model.K(1,1)-handles.Model.K(2,2)) ')\nConsider revising']),'Warning','warn');
                    end
                else
                    if ~strcmp(get(gcbo,'String'),num2str(handles.Model.K(1,1)))
                        msgbox(sprintf(['Modulus does not match with structure stiffness (' num2str(handles.Model.K(1,1)) ')\nConsider revising']),'Warning','warn');
                    end
                end
                handles.ExpControl.DOF1.E = str2num(get(gcbo,'String'));
            case 'DOF 2'
                if ~strcmp(get(gcbo,'String'),num2str(handles.Model.K(2,2)))
                    msgbox(sprintf(['Modulus does not match with structure stiffness (' num2str(handles.Model.K(2,2)) ')\nConsider revising']),'Warning','warn');
                end
                handles.ExpControl.DOF2.E = str2num(get(gcbo,'String'));
        end
    % =====================================================================
    case 'epsP'
        switch handles.ExpControl.CtrlDOF
            case 'DOF 1'
                handles.ExpControl.DOF1.epsP = str2num(get(gcbo,'String'));
            case 'DOF 2'
                handles.ExpControl.DOF2.epsP = str2num(get(gcbo,'String'));
        end
    % =====================================================================
    case 'Fy'
        switch handles.ExpControl.CtrlDOF
            case 'DOF 1'
                handles.ExpControl.DOF1.Fy = str2num(get(gcbo,'String'));
            case 'DOF 2'
                handles.ExpControl.DOF2.Fy = str2num(get(gcbo,'String'));
        end
    % =====================================================================
    case 'b'
        switch handles.ExpControl.CtrlDOF
            case 'DOF 1'
                handles.ExpControl.DOF1.b = str2num(get(gcbo,'String'));
            case 'DOF 2'
                handles.ExpControl.DOF2.b = str2num(get(gcbo,'String'));
        end
    % =====================================================================
    case 'R0'
        switch handles.ExpControl.CtrlDOF
            case 'DOF 1'
                handles.ExpControl.DOF1.R0 = str2num(get(gcbo,'String'));
            case 'DOF 2'
                handles.ExpControl.DOF2.R0 = str2num(get(gcbo,'String'));
        end
    
    % =====================================================================
    % real controller
    % =====================================================================
    case 'ipAddr'
        handles.ExpControl.RealControl.ipAddr = get(gcbo,'String');
    % =====================================================================
    case 'ipPort'
        handles.ExpControl.RealControl.ipPort = get(gcbo,'String');
    % =====================================================================
    case 'TrialCP'
        handles.ExpControl.RealControl.TrialCP = get(gcbo,'Value');
    % =====================================================================
    case 'OutputCP'
        handles.ExpControl.RealControl.OutputCP = get(gcbo,'Value');
    % =====================================================================
    case 'ConfigName'
        handles.ExpControl.RealControl.ConfigName = get(gcbo,'String');
    % =====================================================================
    case 'ConfigPath'
        handles.ExpControl.RealControl.ConfigPath = get(gcbo,'String');
    % =====================================================================
    case 'loadConfig'
        [filename, pathname] = uigetfile({'*.mtscs'});
        % break from function if load file is cancelled
        if filename == 0
            return
        % otherwise...
        else
            index = find(filename == '.');
            handles.ExpControl.RealControl.ConfigName = filename(1:index-1);
            handles.ExpControl.RealControl.ConfigType = filename(index:end);
            handles.ExpControl.RealControl.ConfigPath = pathname;
            set(handles.EC(35),...
                'String',handles.ExpControl.RealControl.ConfigName,...
                'TooltipString',handles.ExpControl.RealControl.ConfigName);
            set(handles.EC(36),...
                'String',pathname,...
                'TooltipString',pathname);
        end
    % =====================================================================
    case 'rampTime'
        handles.ExpControl.RealControl.rampTime = str2num(get(gcbo,'String'));
    % =====================================================================
    case 'memOffset'
        handles.ExpControl.RealControl.memOffset = str2num(get(gcbo,'String'));
    % =====================================================================
    case 'NumActCh'
        handles.ExpControl.RealControl.NumActCh = str2num(get(gcbo,'String'));
    % =====================================================================
    case 'PCtype'
        selection = get(gcbo,'Value');
        switch selection
            case 2
                handles.ExpControl.RealControl.PCvalue = 1;
                handles.ExpControl.RealControl.PCtype = 'Displacement';
            case 3
                handles.ExpControl.RealControl.PCvalue = 2;
                handles.ExpControl.RealControl.PCtype = 'Displacement and Velocity';
            case 4
                handles.ExpControl.RealControl.PCvalue = 3;
                handles.ExpControl.RealControl.PCtype = 'Displacement, Velocity and Acceleration';
        end
    % =====================================================================
    case 'boardName'
        selection = get(gcbo,'Value');
        switch selection
            case 2
                handles.ExpControl.RealControl.boardValue = 1;
                handles.ExpControl.RealControl.boardName = 'DS1103';
            case 3
                handles.ExpControl.RealControl.boardValue = 2;
                handles.ExpControl.RealControl.boardName = 'DS1104';
        end
    % =====================================================================
    case 'appName'
        handles.ExpControl.RealControl.appName = get(gcbo,'String');
    % =====================================================================
    case 'appPath'
        handles.ExpControl.RealControl.appPath = get(gcbo,'String');
    % =====================================================================
    case 'loadApp'
        [filename, pathname] = uigetfile({'*.dlm'});
        % break from function if load file is cancelled
        if filename == 0
            return
        % otherwise...
        else
            index = find(filename == '.');
            handles.ExpControl.RealControl.appName = filename(1:index-1);
            handles.ExpControl.RealControl.appType = filename(index:end);
            handles.ExpControl.RealControl.appPath = pathname;
            set(handles.EC(51),...
                'String',handles.ExpControl.RealControl.appName,...
                'TooltipString',handles.ExpControl.RealControl.appName);
            set(handles.EC(52),...
                'String',pathname,...
                'TooltipString',pathname);
        end
    
    % =====================================================================
    % control points
    % =====================================================================
    case 'assign limits'
        switch get(gcbo,'Tag')
            case 'Line1'
                id = 63;
            case 'Line2'
                id = 69;
            case 'Line3'
                id = 75;
            case 'Line4'
                id = 81;
            case 'Line5'
                id = 87;
            case 'Line6'
                id = 93;
        end
        checked = get(handles.EC(id),'Value');
        if checked == 1
            set(handles.EC(id+1:id+2),'BackgroundColor',[1 1 1],'Style','edit');
            set(handles.EC(id+1:id+2),'Visible','on');
        else
            set(handles.EC(id+1:id+2),'Visible','off');
        end
    % =====================================================================
    case 'nodeNum'
        if ~strcmp(handles.Model.Type,'2 DOF A')
            msgbox({'Cannot change number of nodes!','Can only change structure type!'},'Error','error');
            set(handles.EC(57),'String','1');
        end
    % =====================================================================
    case 'RQNum'
        RQ = str2num(get(gcbo,'String'));
        if isempty(RQ) || RQ <= 0
            msgbox('Must input a positive integer','Error','error');
            set(handles.EC(58),'String',num2str(handles.ExpControl.store.RQ));
        elseif RQ < 5
            % store
            handles.ExpControl.store.RQ = RQ;
            % resize to default positioning and display fields
            set(handles.EC(60),'Position',[0.31 0.56 0.1 0.15]);
            set(handles.EC(61),'Position',[0.43 0.56 0.1 0.15]);
            set(handles.EC(62),'Position',[0.56 0.59 0.08 0.1]);
            set(handles.EC(63),'Position',[0.71 0.59 0.2 0.1]);
            set(handles.EC(64),'Position',[0.77 0.59 0.08 0.1]);
            set(handles.EC(65),'Position',[0.89 0.59 0.08 0.1]);
            
            set(handles.EC(66),'Position',[0.31 0.39 0.1 0.15]);
            set(handles.EC(67),'Position',[0.43 0.39 0.1 0.15]);
            set(handles.EC(68),'Position',[0.56 0.42 0.08 0.1]);
            set(handles.EC(69),'Position',[0.71 0.42 0.2 0.1]);
            set(handles.EC(70),'Position',[0.77 0.42 0.08 0.1]);
            set(handles.EC(71),'Position',[0.89 0.42 0.08 0.1]);
            
            set(handles.EC(72),'Position',[0.31 0.22 0.1 0.15]);
            set(handles.EC(73),'Position',[0.43 0.22 0.1 0.15]);
            set(handles.EC(74),'Position',[0.56 0.25 0.08 0.1]);
            set(handles.EC(75),'Position',[0.71 0.25 0.2 0.1]);
            set(handles.EC(76),'Position',[0.77 0.25 0.08 0.1]);
            set(handles.EC(77),'Position',[0.89 0.25 0.08 0.1]);
            
            set(handles.EC(78),'Position',[0.31 0.05 0.1 0.15]);
            set(handles.EC(79),'Position',[0.43 0.05 0.1 0.15]);
            set(handles.EC(80),'Position',[0.56 0.08 0.08 0.1]);
            set(handles.EC(81),'Position',[0.71 0.08 0.2 0.1]);
            set(handles.EC(82),'Position',[0.77 0.08 0.08 0.1]);
            set(handles.EC(83),'Position',[0.89 0.08 0.08 0.1]);
            
            set(handles.EC(84),'Position',[0.31 0.05 0.1 0.15]);
            set(handles.EC(85),'Position',[0.43 0.05 0.1 0.15]);
            set(handles.EC(86),'Position',[0.56 0.08 0.08 0.1]);
            set(handles.EC(87),'Position',[0.71 0.08 0.2 0.1]);
            set(handles.EC(88),'Position',[0.77 0.08 0.08 0.1]);
            set(handles.EC(89),'Position',[0.89 0.08 0.08 0.1]);
            
            set(handles.EC(90),'Position',[0.31 0.05 0.1 0.15]);
            set(handles.EC(91),'Position',[0.43 0.05 0.1 0.15]);
            set(handles.EC(92),'Position',[0.56 0.08 0.08 0.1]);
            set(handles.EC(93),'Position',[0.71 0.08 0.2 0.1]);
            set(handles.EC(94),'Position',[0.77 0.08 0.08 0.1]);
            set(handles.EC(95),'Position',[0.89 0.08 0.08 0.1]);
            
            set(handles.EC(60:(65+6*(handles.ExpControl.store.RQ-1))),'Visible','on');
            set(handles.EC((66+6*(handles.ExpControl.store.RQ-1)):95),'Visible','off');
            handles.ExpControl.store.CPActive = (60:(65+6*(handles.ExpControl.store.RQ-1)));
            id = [63 69 75 81];
            for i = 1:4
                checked = get(handles.EC(id(i)),'Value');
                if (checked == 1) && (i <=RQ)
                    set(handles.EC(id(i)+1:id(i)+2),'BackgroundColor',[1 1 1],'Style','edit');
                    set(handles.EC(id(i)+1:id(i)+2),'Visible','on');
                else
                    set(handles.EC(id(i)+1:id(i)+2),'Visible','off');
                end
            end
            
        elseif RQ == 5
            % store
            handles.ExpControl.store.RQ = RQ;
            % resize to fit extra rows and display fields
            botDist1 = 0.67;
            botDist2 = 0.62;
            height1 = 0.06;
            height2 = 0.09;
            set(handles.EC(60),'Position',[0.31 botDist1 0.1 height1]);
            set(handles.EC(61),'Position',[0.43 botDist1 0.1 height1]);
            set(handles.EC(62),'Position',[0.56 botDist2 0.08 height2]);
            set(handles.EC(63),'Position',[0.71 botDist2 0.2 height2]);
            set(handles.EC(64),'Position',[0.77 botDist2 0.08 height2]);
            set(handles.EC(65),'Position',[0.89 botDist2 0.08 height2]);
            
            set(handles.EC(66),'Position',[0.31 botDist1-0.14 0.1 height1]);
            set(handles.EC(67),'Position',[0.43 botDist1-0.14 0.1 height1]);
            set(handles.EC(68),'Position',[0.56 botDist2-0.14 0.08 height2]);
            set(handles.EC(69),'Position',[0.71 botDist2-0.14 0.2 height2]);
            set(handles.EC(70),'Position',[0.77 botDist2-0.14 0.08 height2]);
            set(handles.EC(71),'Position',[0.89 botDist2-0.14 0.08 height2]);
            
            set(handles.EC(72),'Position',[0.31 botDist1-0.28 0.1 height1]);
            set(handles.EC(73),'Position',[0.43 botDist1-0.28 0.1 height1]);
            set(handles.EC(74),'Position',[0.56 botDist2-0.28 0.08 height2]);
            set(handles.EC(75),'Position',[0.71 botDist2-0.28 0.2 height2]);
            set(handles.EC(76),'Position',[0.77 botDist2-0.28 0.08 height2]);
            set(handles.EC(77),'Position',[0.89 botDist2-0.28 0.08 height2]);
            
            set(handles.EC(78),'Position',[0.31 botDist1-0.42 0.1 height1]);
            set(handles.EC(79),'Position',[0.43 botDist1-0.42 0.1 height1]);
            set(handles.EC(80),'Position',[0.56 botDist2-0.42 0.08 height2]);
            set(handles.EC(81),'Position',[0.71 botDist2-0.42 0.2 height2]);
            set(handles.EC(82),'Position',[0.77 botDist2-0.42 0.08 height2]);
            set(handles.EC(83),'Position',[0.89 botDist2-0.42 0.08 height2]);
            
            set(handles.EC(84),'Position',[0.31 botDist1-0.56 0.1 height1]);
            set(handles.EC(85),'Position',[0.43 botDist1-0.56 0.1 height1]);
            set(handles.EC(86),'Position',[0.56 botDist2-0.56 0.08 height2]);
            set(handles.EC(87),'Position',[0.71 botDist2-0.56 0.2 height2]);
            set(handles.EC(88),'Position',[0.77 botDist2-0.56 0.08 height2]);
            set(handles.EC(89),'Position',[0.89 botDist2-0.56 0.08 height2]);
            
            set(handles.EC(90),'Position',[0.31 0.05 0.1 height1]);
            set(handles.EC(91),'Position',[0.43 0.05 0.1 height1]);
            set(handles.EC(92),'Position',[0.56 0.08 0.08 height2]);
            set(handles.EC(93),'Position',[0.71 0.08 0.2 height2]);
            set(handles.EC(94),'Position',[0.77 0.08 0.08 height2]);
            set(handles.EC(95),'Position',[0.89 0.08 0.08 height2]);
            
            set(handles.EC(60:89),'Visible','on');
            set(handles.EC(90:95),'Visible','off');
            handles.ExpControl.store.CPActive = (60:89);
            id = [63 69 75 81 87];
            for i = 1:5
                checked = get(handles.EC(id(i)),'Value');
                if (checked == 1) && (i <=RQ)
                    set(handles.EC(id(i)+1:id(i)+2),'BackgroundColor',[1 1 1],'Style','edit');
                    set(handles.EC(id(i)+1:id(i)+2),'Visible','on');
                else
                    set(handles.EC(id(i)+1:id(i)+2),'Visible','off');
                end
            end
            
        elseif RQ == 6
            % store
            handles.ExpControl.store.RQ = RQ;
            % resize to fit extra rows an display fields
            botDist1 = 0.735;
            botDist2 = 0.67;
            height1 = 0.04;
            height2 = 0.08;
            set(handles.EC(60),'Position',[0.31 botDist1 0.1 height1]);
            set(handles.EC(61),'Position',[0.43 botDist1 0.1 height1]);
            set(handles.EC(62),'Position',[0.56 botDist2 0.08 height2]);
            set(handles.EC(63),'Position',[0.71 botDist2 0.2 height2]);
            set(handles.EC(64),'Position',[0.77 botDist2 0.08 height2]);
            set(handles.EC(65),'Position',[0.89 botDist2 0.08 height2]);
            
            set(handles.EC(66),'Position',[0.31 botDist1-0.13 0.1 height1]);
            set(handles.EC(67),'Position',[0.43 botDist1-0.13 0.1 height1]);
            set(handles.EC(68),'Position',[0.56 botDist2-0.13 0.08 height2]);
            set(handles.EC(69),'Position',[0.71 botDist2-0.13 0.2 height2]);
            set(handles.EC(70),'Position',[0.77 botDist2-0.13 0.08 height2]);
            set(handles.EC(71),'Position',[0.89 botDist2-0.13 0.08 height2]);
            
            set(handles.EC(72),'Position',[0.31 botDist1-0.26 0.1 height1]);
            set(handles.EC(73),'Position',[0.43 botDist1-0.26 0.1 height1]);
            set(handles.EC(74),'Position',[0.56 botDist2-0.26 0.08 height2]);
            set(handles.EC(75),'Position',[0.71 botDist2-0.26 0.2 height2]);
            set(handles.EC(76),'Position',[0.77 botDist2-0.26 0.08 height2]);
            set(handles.EC(77),'Position',[0.89 botDist2-0.26 0.08 height2]);
            
            set(handles.EC(78),'Position',[0.31 botDist1-0.39 0.1 height1]);
            set(handles.EC(79),'Position',[0.43 botDist1-0.39 0.1 height1]);
            set(handles.EC(80),'Position',[0.56 botDist2-0.39 0.08 height2]);
            set(handles.EC(81),'Position',[0.71 botDist2-0.39 0.2 height2]);
            set(handles.EC(82),'Position',[0.77 botDist2-0.39 0.08 height2]);
            set(handles.EC(83),'Position',[0.89 botDist2-0.39 0.08 height2]);
            
            set(handles.EC(84),'Position',[0.31 botDist1-0.52 0.1 height1]);
            set(handles.EC(85),'Position',[0.43 botDist1-0.52 0.1 height1]);
            set(handles.EC(86),'Position',[0.56 botDist2-0.52 0.08 height2]);
            set(handles.EC(87),'Position',[0.71 botDist2-0.52 0.2 height2]);
            set(handles.EC(88),'Position',[0.77 botDist2-0.52 0.08 height2]);
            set(handles.EC(89),'Position',[0.89 botDist2-0.52 0.08 height2]);
            
            set(handles.EC(90),'Position',[0.31 botDist1-0.65 0.1 height1]);
            set(handles.EC(91),'Position',[0.43 botDist1-0.65 0.1 height1]);
            set(handles.EC(92),'Position',[0.56 botDist2-0.65 0.08 height2]);
            set(handles.EC(93),'Position',[0.71 botDist2-0.65 0.2 height2]);
            set(handles.EC(94),'Position',[0.77 botDist2-0.65 0.08 height2]);
            set(handles.EC(95),'Position',[0.89 botDist2-0.65 0.08 height2]);
            
            set(handles.EC(60:95),'Visible','on');
            handles.ExpControl.store.CPActive = (60:95);
            id = [63 69 75 81 87 93];
            for i = 1:6
                checked = get(handles.EC(id(i)),'Value');
                if (checked == 1) && (i <=RQ)
                    set(handles.EC(id(i)+1:id(i)+2),'BackgroundColor',[1 1 1],'Style','edit');
                    set(handles.EC(id(i)+1:id(i)+2),'Visible','on');
                else
                    set(handles.EC(id(i)+1:id(i)+2),'Visible','off');
                end
            end
        else
            msgbox(sprintf('Defining more than six response quantities for one\ncontrol point is not supported at this time'),'Error','error');
            set(handles.EC(58),'String',num2str(handles.ExpControl.store.RQ));
        end
    % =====================================================================
    case 'SwitchCP'
        id = get(gcbo,'Value');
        if id == 1
            set(handles.EC(56),'String','');
            set(handles.EC(58),'String','1');
            handles.ExpControl.store.CPActive = (60:65);
            i = 0;
            while i < 6
                i = i+1;
                set(handles.EC(60+6*(i-1)),'Value',1);
                set(handles.EC(61+6*(i-1)),'Value',1);
                set(handles.EC(62+6*(i-1)),'String','1');
                set(handles.EC(63+6*(i-1)),'Value',0);
                set(handles.EC(64+6*(i-1)),'BackgroundColor','default','Style','text','String','');
                set(handles.EC(65+6*(i-1)),'BackgroundColor','default','Style','text','String','');
                set(handles.EC(66:95),'Visible','off');
            end
        else
            set(handles.EC(56),'String',handles.ExpControl.CP.Name{id-1});
            set(handles.EC(57),'Value',handles.ExpControl.CP.Node{id-1}+1);
            handles.ExpControl.store.RQ = handles.ExpControl.CP.NumResp{id-1};
            set(handles.EC(58),'String',handles.ExpControl.store.RQ);
            handles.ExpControl.store.CPActive = (60:(65+6*(handles.ExpControl.store.RQ-1)));
            % resize to fit extra rows and display fields
            if handles.ExpControl.store.RQ == 5
                botDist1 = 0.67;
                botDist2 = 0.62;
                height1 = 0.06;
                height2 = 0.09;
                set(handles.EC(60),'Position',[0.31 botDist1 0.1 height1]);
                set(handles.EC(61),'Position',[0.43 botDist1 0.1 height1]);
                set(handles.EC(62),'Position',[0.56 botDist2 0.08 height2]);
                set(handles.EC(63),'Position',[0.71 botDist2 0.2 height2]);
                set(handles.EC(64),'Position',[0.77 botDist2 0.08 height2]);
                set(handles.EC(65),'Position',[0.89 botDist2 0.08 height2]);
                
                set(handles.EC(66),'Position',[0.31 botDist1-0.14 0.1 height1]);
                set(handles.EC(67),'Position',[0.43 botDist1-0.14 0.1 height1]);
                set(handles.EC(68),'Position',[0.56 botDist2-0.14 0.08 height2]);
                set(handles.EC(69),'Position',[0.71 botDist2-0.14 0.2 height2]);
                set(handles.EC(70),'Position',[0.77 botDist2-0.14 0.08 height2]);
                set(handles.EC(71),'Position',[0.89 botDist2-0.14 0.08 height2]);
                
                set(handles.EC(72),'Position',[0.31 botDist1-0.28 0.1 height1]);
                set(handles.EC(73),'Position',[0.43 botDist1-0.28 0.1 height1]);
                set(handles.EC(74),'Position',[0.56 botDist2-0.28 0.08 height2]);
                set(handles.EC(75),'Position',[0.71 botDist2-0.28 0.2 height2]);
                set(handles.EC(76),'Position',[0.77 botDist2-0.28 0.08 height2]);
                set(handles.EC(77),'Position',[0.89 botDist2-0.28 0.08 height2]);
                
                set(handles.EC(78),'Position',[0.31 botDist1-0.42 0.1 height1]);
                set(handles.EC(79),'Position',[0.43 botDist1-0.42 0.1 height1]);
                set(handles.EC(80),'Position',[0.56 botDist2-0.42 0.08 height2]);
                set(handles.EC(81),'Position',[0.71 botDist2-0.42 0.2 height2]);
                set(handles.EC(82),'Position',[0.77 botDist2-0.42 0.08 height2]);
                set(handles.EC(83),'Position',[0.89 botDist2-0.42 0.08 height2]);
                
                set(handles.EC(84),'Position',[0.31 botDist1-0.56 0.1 height1]);
                set(handles.EC(85),'Position',[0.43 botDist1-0.56 0.1 height1]);
                set(handles.EC(86),'Position',[0.56 botDist2-0.56 0.08 height2]);
                set(handles.EC(87),'Position',[0.71 botDist2-0.56 0.2 height2]);
                set(handles.EC(88),'Position',[0.77 botDist2-0.56 0.08 height2]);
                set(handles.EC(89),'Position',[0.89 botDist2-0.56 0.08 height2]);
                
                set(handles.EC(90),'Position',[0.31 0.05 0.1 height1]);
                set(handles.EC(91),'Position',[0.43 0.05 0.1 height1]);
                set(handles.EC(92),'Position',[0.56 0.08 0.08 height2]);
                set(handles.EC(93),'Position',[0.71 0.08 0.2 height2]);
                set(handles.EC(94),'Position',[0.77 0.08 0.08 height2]);
                set(handles.EC(95),'Position',[0.89 0.08 0.08 height2]);
                
            elseif handles.ExpControl.store.RQ == 6
                botDist1 = 0.735;
                botDist2 = 0.67;
                height1 = 0.04;
                height2 = 0.08;
                set(handles.EC(60),'Position',[0.31 botDist1 0.1 height1]);
                set(handles.EC(61),'Position',[0.43 botDist1 0.1 height1]);
                set(handles.EC(62),'Position',[0.56 botDist2 0.08 height2]);
                set(handles.EC(63),'Position',[0.71 botDist2 0.2 height2]);
                set(handles.EC(64),'Position',[0.77 botDist2 0.08 height2]);
                set(handles.EC(65),'Position',[0.89 botDist2 0.08 height2]);
                
                set(handles.EC(66),'Position',[0.31 botDist1-0.13 0.1 height1]);
                set(handles.EC(67),'Position',[0.43 botDist1-0.13 0.1 height1]);
                set(handles.EC(68),'Position',[0.56 botDist2-0.13 0.08 height2]);
                set(handles.EC(69),'Position',[0.71 botDist2-0.13 0.2 height2]);
                set(handles.EC(70),'Position',[0.77 botDist2-0.13 0.08 height2]);
                set(handles.EC(71),'Position',[0.89 botDist2-0.13 0.08 height2]);
                
                set(handles.EC(72),'Position',[0.31 botDist1-0.26 0.1 height1]);
                set(handles.EC(73),'Position',[0.43 botDist1-0.26 0.1 height1]);
                set(handles.EC(74),'Position',[0.56 botDist2-0.26 0.08 height2]);
                set(handles.EC(75),'Position',[0.71 botDist2-0.26 0.2 height2]);
                set(handles.EC(76),'Position',[0.77 botDist2-0.26 0.08 height2]);
                set(handles.EC(77),'Position',[0.89 botDist2-0.26 0.08 height2]);
                
                set(handles.EC(78),'Position',[0.31 botDist1-0.39 0.1 height1]);
                set(handles.EC(79),'Position',[0.43 botDist1-0.39 0.1 height1]);
                set(handles.EC(80),'Position',[0.56 botDist2-0.39 0.08 height2]);
                set(handles.EC(81),'Position',[0.71 botDist2-0.39 0.2 height2]);
                set(handles.EC(82),'Position',[0.77 botDist2-0.39 0.08 height2]);
                set(handles.EC(83),'Position',[0.89 botDist2-0.39 0.08 height2]);
                
                set(handles.EC(84),'Position',[0.31 botDist1-0.52 0.1 height1]);
                set(handles.EC(85),'Position',[0.43 botDist1-0.52 0.1 height1]);
                set(handles.EC(86),'Position',[0.56 botDist2-0.52 0.08 height2]);
                set(handles.EC(87),'Position',[0.71 botDist2-0.52 0.2 height2]);
                set(handles.EC(88),'Position',[0.77 botDist2-0.52 0.08 height2]);
                set(handles.EC(89),'Position',[0.89 botDist2-0.52 0.08 height2]);
                
                set(handles.EC(90),'Position',[0.31 botDist1-0.65 0.1 height1]);
                set(handles.EC(91),'Position',[0.43 botDist1-0.65 0.1 height1]);
                set(handles.EC(92),'Position',[0.56 botDist2-0.65 0.08 height2]);
                set(handles.EC(93),'Position',[0.71 botDist2-0.65 0.2 height2]);
                set(handles.EC(94),'Position',[0.77 botDist2-0.65 0.08 height2]);
                set(handles.EC(95),'Position',[0.89 botDist2-0.65 0.08 height2]);
            end
            
            for i = 1:handles.ExpControl.CP.NumResp{id-1}
                set(handles.EC(60+6*(i-1)),'Value',find(strcmpi(get(handles.EC(60),'String'), handles.ExpControl.CP.Dir{i,id-1})),'Visible','on');
                set(handles.EC(61+6*(i-1)),'Value',find(strcmp(get(handles.EC(61),'String'), (handles.ExpControl.CP.Resp{i,id-1}))),'Visible','on');
                set(handles.EC(62+6*(i-1)),'String',num2str(handles.ExpControl.CP.F{i,id-1}),'Visible','on');
                if handles.ExpControl.CP.Lim{i,id-1} == 1
                    set(handles.EC(63+6*(i-1)),'Value',1,'Visible','on');
                    set(handles.EC(64+6*(i-1)),'BackgroundColor',[1 1 1],'Style','edit');
                    set(handles.EC(64+6*(i-1)),'String',num2str(handles.ExpControl.CP.LimL{i,id-1}),'Visible','on');
                    set(handles.EC(65+6*(i-1)),'BackgroundColor',[1 1 1],'Style','edit');
                    set(handles.EC(65+6*(i-1)),'String',num2str(handles.ExpControl.CP.LimU{i,id-1}),'Visible','on');
                else
                    set(handles.EC(63+6*(i-1)),'Value',0,'Visible','on');
                    set(handles.EC(64+6*(i-1)),'BackgroundColor','default','Style','text','String','','Visible','off');
                    set(handles.EC(65+6*(i-1)),'BackgroundColor','default','Style','text','String','','Visible','off');
                end
            end
            while i < 6
                i = i+1;
                set(handles.EC(60+6*(i-1)),'Value',1,'Visible','off');
                set(handles.EC(61+6*(i-1)),'Value',1,'Visible','off');
                set(handles.EC(62+6*(i-1)),'String','1','Visible','off');
                set(handles.EC(63+6*(i-1)),'Value',0,'Visible','off');
                set(handles.EC(64+6*(i-1)),'BackgroundColor','default','Style','text','String','','Visible','off');
                set(handles.EC(65+6*(i-1)),'BackgroundColor','default','Style','text','String','','Visible','off');
            end
        end
    % =====================================================================
    case 'saveCP'
        id = length(handles.ExpControl.store.CPOptions);
        error_flag = 0;
        NumResp = handles.ExpControl.store.RQ;
        % check if name already exists
        for k = 2:length(handles.ExpControl.store.CPOptions)
            if strcmp(get(handles.EC(56),'String'),handles.ExpControl.store.CPOptions(k))
                overwrite = questdlg(sprintf('The specified control name already exists.\nDo you want to overwrite the existing control?'),'Overwrite?','Yes','No','Yes');
                switch overwrite
                    case 'Yes'
                        error_flag = 2;
                    case 'No'
                        error_flag = 1;
                end
            end
        end
        % store selected node if 2 DOF A type
        if strcmp(handles.Model.Type, '2 DOF A');
            nn = get(handles.EC(57),'Value');
            if nn == 1
                msgbox(sprintf('Please choose a node'),'Error','error');
                error_flag = 1;
            else
                Node = nn-1;
            end
        else
            Node = str2num(get(handles.EC(57),'String'));
        end
        % save inputs/warn for missing data
        for i = 1:NumResp
            Dir = handles.ExpControl.store.DirOptions{get(handles.EC(60+6*(i-1)),'Value')};
            
            if get(handles.EC(61+6*(i-1)),'Value') == 1
                msgbox(sprintf('Please choose a response quantity on line %1.0f',i),'Error','error');
                error_flag = 1;
            else
                Resp = handles.ExpControl.store.RespOptions{get(handles.EC(61+6*(i-1)),'Value')};
            end
            
            if isempty(get(handles.EC(62+6*(i-1)),'String'))
                msgbox(sprintf('Please enter a valid factor on line %1.0f',i),'Error','error');
                error_flag = 1;
            else
                F = str2num(get(handles.EC(62+6*(i-1)),'String'));
            end
            
            if get(handles.EC(63+6*(i-1)),'Value') == 1
                Lim = 1;
                if isempty(get(handles.EC(64+6*(i-1)),'String'))
                    msgbox(sprintf('Please enter a valid lower limit on line %1.0f',i),'Error','error');
                    error_flag = 1;
                else
                    LimL = str2num(get(handles.EC(64+6*(i-1)),'String'));
                end
                if isempty(get(handles.EC(65+6*(i-1)),'String'))
                    msgbox(sprintf('Please enter a valid upper limit on line %1.0f',i),'Error','error');
                    error_flag = 1;
                elseif str2num(get(handles.EC(65+6*(i-1)),'String')) < LimL
                    msgbox(sprintf('Upper limit on line %1.0f must be greater than lower limit',i),'Error','error');
                    error_flag = 1;
                else
                    LimU = str2num(get(handles.EC(65+6*(i-1)),'String'));
                end
            else
                Lim = [];
                LimL = [];
                LimU = [];
            end
            if error_flag == 1
                % return if errors are found
                return
            elseif error_flag == 2
                % if names are identical overwrite existing CP
                id = find(strcmp(get(handles.EC(56),'String'),handles.ExpControl.store.CPOptions)) - 1;
                handles.ExpControl.CP.Name{id} = get(handles.EC(56),'String');
                handles.ExpControl.CP.NumResp{id} = NumResp;
                handles.ExpControl.CP.Node{id} = Node;
                handles.ExpControl.CP.Dir{i,id} = Dir;
                handles.ExpControl.CP.Resp{i,id} = Resp;
                handles.ExpControl.CP.F{i,id} = F;
                handles.ExpControl.CP.Lim{i,id} = Lim;
                handles.ExpControl.CP.LimL{i,id} = LimL;
                handles.ExpControl.CP.LimU{i,id} = LimU;
                set(handles.EC(54),'String',handles.ExpControl.store.CPOptions,'Value',id+1);
            else
                % otherwise, store new CP
                handles.ExpControl.CP.Name{id} = get(handles.EC(56),'String');
                handles.ExpControl.CP.NumResp{id} = NumResp;
                handles.ExpControl.CP.Node{id} = Node;
                handles.ExpControl.CP.Dir{i,id} = Dir;
                handles.ExpControl.CP.Resp{i,id} = Resp;
                handles.ExpControl.CP.F{i,id} = F;
                handles.ExpControl.CP.Lim{i,id} = Lim;
                handles.ExpControl.CP.LimL{i,id} = LimL;
                handles.ExpControl.CP.LimU{i,id} = LimU;
                
                handles.ExpControl.store.CPOptions{id+1} = handles.ExpControl.CP.Name{id};
                set(handles.EC(54),'String',handles.ExpControl.store.CPOptions,'Value',id+1);
                set(handles.EC(32),'String',handles.ExpControl.store.CPOptions(2:end));
                set(handles.EC(33),'String',handles.ExpControl.store.CPOptions(2:end));
            end
        end
    % =====================================================================
end

% update handles structure
guidata(gcbf, handles);
