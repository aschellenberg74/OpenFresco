function Report(varargin)
% GUI_Analysis layout of analysis page of GUI
%       Comments displayed at the command line in response 
%       to the help command. 

%  Initialization tasks
handles = guidata(gcbf);

% get the path
DIR = handles.Model.DIR;

FID = fopen(fullfile(DIR,'OPFReport.txt'),'w');

% Print header
fprintf(FID,'##########################################################\n');
fprintf(FID,'# File: OPFReport.txt                                    #\n');
fprintf(FID,'#                                                        #\n');
fprintf(FID,'# Purpose: Summary of inputs for use with OpenFresco GUI #\n');
fprintf(FID,'#                                                        #\n');
fprintf(FID,'##########################################################\n\n\n');

switch handles.Model.Type
    %%%%%%%%%%%%
    %1 DOF Case%
    %%%%%%%%%%%%
    case '1 DOF'
        fprintf(FID,'# ------------------------------\n# Model Properties\n# ------------------------------\n');
        fprintf(FID,'Model Type: 1 DOF Column\n');
        fprintf(FID,'Mass: %1.4f\n',handles.Model.M);
        fprintf(FID,'Stiffness: %1.4f\n',handles.Model.K);
        fprintf(FID,'Period: %1.4f\n',handles.Model.T);
        fprintf(FID,'Frequency: %1.4f\n',1/handles.Model.T);
        fprintf(FID,'Damping Type: %s\n',handles.Model.DampType);
        fprintf(FID,'Damping Value: %1.4f\n',handles.Model.Zeta);
        fprintf(FID,'Mass Proportional Damping Factor: %1.4f\n',handles.Model.alphaM);
        fprintf(FID,'Stiffness Proportional Damping Factor: %1.4f\n\n',handles.Model.betaK);
        
        fprintf(FID,'# ------------------------------\n# Loading\n# ------------------------------\n');
        strLength = length(handles.GM.store.filepath{1});
        id = strfind(handles.GM.store.filepath,filesep);
        strStart = id{1}(end);
        GMName = handles.GM.store.filepath{1}(strStart+1:strLength);
        fprintf(FID,'Ground Motion: %s\n',GMName);
        fprintf(FID,'Database Type: %s\n',handles.GM.databaseType);
        fprintf(FID,'PGA: %3.4f\n',handles.GM.Spectra{1}.pga);
        fprintf(FID,'Ground Motion Time Step: %1.4f\n',handles.GM.dt);
        fprintf(FID,'Amplitude Scale Factor: %1.4f\n',handles.GM.AmpFact);
        fprintf(FID,'Time Scale Factor: %1.4f\n\n',handles.GM.TimeFact);
        
        fprintf(FID,'# ------------------------------\n# Experimental Setup\n# ------------------------------\n');
        fprintf(FID,'Setup Type: No Transformation\n');
        if strcmp(handles.ExpControl.Type,'Simulation')
            fprintf(FID,'Control: %s\n',handles.ExpControl.SimControl.SimType);
        else
            fprintf(FID,'Control: %s\n',handles.ExpControl.RealControl.Controller);
        end
        fprintf(FID,'Direction: Global X\n');
        fprintf(FID,'Size Trial: 1\n');
        fprintf(FID,'Size Output: 1\n\n');
        
        fprintf(FID,'# ------------------------------\n# Experimental Control\n# ------------------------------\n');
        switch handles.ExpControl.Type
            case 'Simulation'
                fprintf(FID,'Control Type: %s\n',handles.ExpControl.Type);
                fprintf(FID,'Simulation Type: %s\n\n',handles.ExpControl.SimControl.SimType);
                switch handles.ExpControl.DOF1.SimMaterial
                    case 'Elastic'
                        fprintf(FID,'Material: %s\n',handles.ExpControl.DOF1.SimMaterial);
                        fprintf(FID,'E: %1.4f\n\n',handles.ExpControl.DOF1.E);
                    case 'Elastic-Perfectly Plastic'
                        fprintf(FID,'Material: %s\n',handles.ExpControl.DOF1.SimMaterial);
                        fprintf(FID,'E: %1.4f\n',handles.ExpControl.DOF1.E);
                        fprintf(FID,'epsP: %1.4f\n\n',handles.ExpControl.DOF1.epsP);
                    case 'Steel - Bilinear'
                        fprintf(FID,'Material: %s\n',handles.ExpControl.DOF1.SimMaterial);
                        fprintf(FID,'Fy: %1.4f\n',handles.ExpControl.DOF1.Fy);
                        fprintf(FID,'E0: %1.4f\n',handles.ExpControl.DOF1.E0);
                        fprintf(FID,'b: %1.4f\n\n',handles.ExpControl.DOF1.b);
                    case 'Steel - Giuffré-Menegotto-Pinto'
                        fprintf(FID,'Material: %s\n',handles.ExpControl.DOF1.SimMaterial);
                        fprintf(FID,'Fy: %1.4f\n',handles.ExpControl.DOF1.Fy);
                        fprintf(FID,'E: %1.4f\n',handles.ExpControl.DOF1.E);
                        fprintf(FID,'b: %1.4f\n',handles.ExpControl.DOF1.b);
                        fprintf(FID,'R0: %1.4f\n',handles.ExpControl.DOF1.R0);
                        fprintf(FID,'cR1: %1.4f\n',handles.ExpControl.DOF1.cR1);
                        fprintf(FID,'cR2: %1.4f\n\n',handles.ExpControl.DOF1.cR2);
                end                
            case 'Real'
                fprintf(FID,'Control Type: %s\n',handles.ExpControl.Type);
                switch handles.ExpControl.RealControl.Controller
                    case 'LabVIEW'
                        fprintf(FID,'Controller: %s\n',handles.ExpControl.RealControl.Controller);
                        fprintf(FID,'IP Address: %s\n',handles.ExpControl.RealControl.ipAddr);
                        fprintf(FID,'IP Port: %s\n\n',handles.ExpControl.RealControl.ipPort);
                        
%                         %Define experimental control points
%                         fprintf(FID,'# Define control points\n# ---------------------\n');
%                         fprintf(FID,'# expControlPoint tag nodeTag dir resp <-fact f> <-lim l u> ...\n');
%                         fprintf(FID,'expControlPoint 1 1  ux disp -fact 1.0 -lim -7.5 7.5\n');
%                         fprintf(FID,'expControlPoint 2 1  ux disp -fact 1.0 -lim -7.5 7.5  ux force -fact 1.0 -lim -12.0 12.0\n\n');
                        
                    case 'MTSCsi'
                        fprintf(FID,'Controller: %s\n',handles.ExpControl.RealControl.Controller);
                        fprintf(FID,'Configuration File: %s\n',strcat(handles.ExpControl.RealControl.ConfigName,handles.ExpControl.RealControl.ConfigType));
                        fprintf(FID,'Ramp Time: %1.4f\n\n',handles.ExpControl.RealControl.rampTime);
                    case 'SCRAMNet'
                        fprintf(FID,'Controller: %s\n',handles.ExpControl.RealControl.Controller);
                        fprintf(FID,'Memory Offset (bytes): %1.0f\n',handles.ExpControl.RealControl.memOffset);
                        fprintf(FID,'Number of Actuator Channels: %1.0f\n\n',handles.ExpControl.RealControl.NumActCh);
                    case 'dSpace'
                        fprintf(FID,'Controller: %s\n',handles.ExpControl.RealControl.Controller);
                        fprintf(FID,'Predictor-Corrector Type: %s\n',handles.ExpControl.RealControl.PCtype);
                        fprintf(FID,'Board Name: %s\n\n',handles.ExpControl.RealControl.boardName);
                    case 'xPCtarget'
                        fprintf(FID,'Controller: %s\n',handles.ExpControl.RealControl.Controller);
                        fprintf(FID,'Predictor-Corrector Type: %s\n',handles.ExpControl.RealControl.PCtype);
                        fprintf(FID,'IP Address: %s\n',handles.ExpControl.RealControl.ipAddr);
                        fprintf(FID,'IP Port: %s\n',handles.ExpControl.RealControl.ipPort);
                        fprintf(FID,'Application Name: %s\n\n',handles.ExpControl.RealControl.appName);
                end
        end
        
        fprintf(FID,'# ------------------------------\n# Analysis Properties\n# ------------------------------\n');
        fprintf(FID,'Analysis Time Step: %1.4f\n',handles.GM.dtAnalysis);
        fprintf(FID,'Analysis Time Step Limit: %1.4f\n',handles.Model.Maxdt);
        
        
    %%%%%%%%%%%%%%%
    %2 DOF A  Case%
    %%%%%%%%%%%%%%%
    case '2 DOF A'
        fprintf(FID,'# ------------------------------\n# Model Properties\n# ------------------------------\n');
        fprintf(FID,'Model Type: 2 DOF Shear Building\n');
        fprintf(FID,'Mass: [%1.4f  %1.4f; %1.4f  %1.4f]\n',handles.Model.M(1,1),handles.Model.M(2,1),handles.Model.M(1,2),handles.Model.M(2,2));
        fprintf(FID,'Stiffness: [%1.4f  %1.4f; %1.4f  %1.4f]\n',handles.Model.K(1,1),handles.Model.K(2,1),handles.Model.K(1,2),handles.Model.K(2,2));
        fprintf(FID,'Period: %1.4f  %1.4f\n',handles.Model.T(1,1),handles.Model.T(2,1));
        fprintf(FID,'Frequency: %1.4f %1.4f\n',(1/handles.Model.T(1,1)),(1/handles.Model.T(2,1)));
        fprintf(FID,'Damping Type: %s\n',handles.Model.DampType);
        if strcmp(handles.Model.DampType,'Rayleigh')
            fprintf(FID,'Damping Values: %1.4f  %1.4f\n',handles.Model.Zeta(1,1),handles.Model.Zeta(1,2));
        else
            fprintf(FID,'Damping Value: %1.4f\n',handles.Model.Zeta);
        end
        fprintf(FID,'Mass Proportional Damping Factor: %1.4f\n',handles.Model.alphaM);
        fprintf(FID,'Stiffness Proportional Damping Factor: %1.4f\n\n',handles.Model.betaK);
        
        fprintf(FID,'# ------------------------------\n# Loading\n# ------------------------------\n');
        strLength = length(handles.GM.store.filepath{1});
        id = strfind(handles.GM.store.filepath,filesep);
        strStart = id{1}(end);
        GMName = handles.GM.store.filepath{1}(strStart+1:strLength);
        fprintf(FID,'Ground Motion: %s\n',GMName);
        fprintf(FID,'Database Type: %s\n',handles.GM.databaseType);
        fprintf(FID,'PGA: %3.4f\n',handles.GM.Spectra{1}.pga);
        fprintf(FID,'Analysis Time Step Limit: %1.4f\n',handles.Model.Maxdt);
        fprintf(FID,'Amplitude Scale Factor: %1.4f\n',handles.GM.AmpFact);
        fprintf(FID,'Time Scale Factor: %1.4f\n\n',handles.GM.TimeFact);
        
        fprintf(FID,'# ------------------------------\n# Experimental Setup\n# ------------------------------\n');
        fprintf(FID,'Setup Type: No Transformation\n');
        if strcmp(handles.ExpControl.Type,'Simulation')
            fprintf(FID,'Control: %s\n',handles.ExpControl.SimControl.SimType);
        else
            fprintf(FID,'Control: %s\n',handles.ExpControl.RealControl.Controller);
        end
        fprintf(FID,'Direction: Global X\n');
        fprintf(FID,'Size Trial: 2\n');
        fprintf(FID,'Size Output: 2\n\n');
        
        fprintf(FID,'# ------------------------------\n# Experimental Control\n# ------------------------------\n');
        switch handles.ExpControl.Type
            case 'Simulation'
                fprintf(FID,'Control Type: %s\n',handles.ExpControl.Type);
                fprintf(FID,'Simulation Type: %s\n\n',handles.ExpControl.SimControl.SimType);
                switch handles.ExpControl.DOF1.SimMaterial
                    case 'Elastic'
                        fprintf(FID,'DOF 1 Material: %s\n',handles.ExpControl.DOF1.SimMaterial);
                        fprintf(FID,'E: %1.4f\n\n',handles.ExpControl.DOF1.E);
                    case 'Elastic-Perfectly Plastic'
                        fprintf(FID,'DOF 1 Material: %s\n',handles.ExpControl.DOF1.SimMaterial);
                        fprintf(FID,'E: %1.4f\n',handles.ExpControl.DOF1.E);
                        fprintf(FID,'epsP: %1.4f\n\n',handles.ExpControl.DOF1.epsP);
                    case 'Steel - Bilinear'
                        fprintf(FID,'DOF 1 Material: %s\n',handles.ExpControl.DOF1.SimMaterial);
                        fprintf(FID,'Fy: %1.4f\n',handles.ExpControl.DOF1.Fy);
                        fprintf(FID,'E0: %1.4f\n',handles.ExpControl.DOF1.E0);
                        fprintf(FID,'b: %1.4f\n\n',handles.ExpControl.DOF1.b);
                    case 'Steel - Giuffré-Menegotto-Pinto'
                        fprintf(FID,'DOF 1 Material: %s\n',handles.ExpControl.DOF1.SimMaterial);
                        fprintf(FID,'Fy: %1.4f\n',handles.ExpControl.DOF1.Fy);
                        fprintf(FID,'E: %1.4f\n',handles.ExpControl.DOF1.E);
                        fprintf(FID,'b: %1.4f\n',handles.ExpControl.DOF1.b);
                        fprintf(FID,'R0: %1.4f\n',handles.ExpControl.DOF1.R0);
                        fprintf(FID,'cR1: %1.4f\n',handles.ExpControl.DOF1.cR1);
                        fprintf(FID,'cR2: %1.4f\n\n',handles.ExpControl.DOF1.cR2);
                end        
                switch handles.ExpControl.DOF2.SimMaterial
                    case 'Elastic'
                        fprintf(FID,'DOF 2 Material: %s\n',handles.ExpControl.DOF2.SimMaterial);
                        fprintf(FID,'E: %1.4f\n\n',handles.ExpControl.DOF2.E);
                    case 'Elastic-Perfectly Plastic'
                        fprintf(FID,'DOF 2 Material: %s\n',handles.ExpControl.DOF2.SimMaterial);
                        fprintf(FID,'E: %1.4f\n',handles.ExpControl.DOF2.E);
                        fprintf(FID,'epsP: %1.4f\n\n',handles.ExpControl.DOF2.epsP);
                    case 'Steel - Bilinear'
                        fprintf(FID,'DOF 2 Material: %s\n',handles.ExpControl.DOF2.SimMaterial);
                        fprintf(FID,'Fy: %1.4f\n',handles.ExpControl.DOF2.Fy);
                        fprintf(FID,'E0: %1.4f\n',handles.ExpControl.DOF2.E0);
                        fprintf(FID,'b: %1.4f\n\n',handles.ExpControl.DOF2.b);
                    case 'Steel - Giuffré-Menegotto-Pinto'
                        fprintf(FID,'DOF 2 Material: %s\n',handles.ExpControl.DOF2.SimMaterial);
                        fprintf(FID,'Fy: %1.4f\n',handles.ExpControl.DOF2.Fy);
                        fprintf(FID,'E: %1.4f\n',handles.ExpControl.DOF2.E);
                        fprintf(FID,'b: %1.4f\n',handles.ExpControl.DOF2.b);
                        fprintf(FID,'R0: %1.4f\n',handles.ExpControl.DOF2.R0);
                        fprintf(FID,'cR1: %1.4f\n',handles.ExpControl.DOF2.cR1);
                        fprintf(FID,'cR2: %1.4f\n\n',handles.ExpControl.DOF2.cR2);
                end  
                
                %                         %Define experimental control points
                %                         fprintf(FID,'# Define control points\n# ---------------------\n');
                %                         fprintf(FID,'# expControlPoint tag nodeTag dir resp <-fact f> <-lim l u> ...\n');
                %                         fprintf(FID,'expControlPoint 1 2  ux disp\n');
                %                         fprintf(FID,'expControlPoint 2 2  ux disp  ux force\n');
                %                         fprintf(FID,'expControlPoint 3 3  ux disp\n');
                %                         fprintf(FID,'expControlPoint 4 3  ux disp  ux
                %                         force\n\n');

            case 'Real'
                fprintf(FID,'Control Type: %s\n',handles.ExpControl.Type);
                switch handles.ExpControl.RealControl.Controller
                    case 'LabVIEW'
                        fprintf(FID,'Controller: %s\n',handles.ExpControl.RealControl.Controller);
                        fprintf(FID,'IP Address: %s\n',handles.ExpControl.RealControl.ipAddr);
                        fprintf(FID,'IP Port: %s\n\n',handles.ExpControl.RealControl.ipPort);
                        
%                         %Define experimental control points
%                         fprintf(FID,'# Define control points\n# ---------------------\n');
%                         fprintf(FID,'# expControlPoint tag nodeTag dir resp <-fact f> <-lim l u> ...\n');
%                         fprintf(FID,'expControlPoint 1 1  ux disp -fact 1.0 -lim -7.5 7.5\n');
%                         fprintf(FID,'expControlPoint 2 1  ux disp -fact 1.0 -lim -7.5 7.5  ux force -fact 1.0 -lim -12.0 12.0\n');
%                         fprintf(FID,'expControlPoint 3 2  ux disp -fact 1.0 -lim -7.5 7.5\n');
%                         fprintf(FID,'expControlPoint 4 2  ux disp -fact
%                         1.0 -lim -7.5 7.5  ux force -fact 1.0 -lim -12.0 12.0\n\n');


                        
                    case 'MTSCsi'
                        fprintf(FID,'Controller: %s\n',handles.ExpControl.RealControl.Controller);
                        fprintf(FID,'Configuration File: %s\n',strcat(handles.ExpControl.RealControl.ConfigName,handles.ExpControl.RealControl.ConfigType));
                        fprintf(FID,'Ramp Time: %1.4f\n\n',handles.ExpControl.RealControl.rampTime);
                    case 'SCRAMNet'
                        fprintf(FID,'Controller: %s\n',handles.ExpControl.RealControl.Controller);
                        fprintf(FID,'Memory Offset (bytes): %1.0f\n',handles.ExpControl.RealControl.memOffset);
                        fprintf(FID,'Number of Actuator Channels: %1.0f\n\n',handles.ExpControl.RealControl.NumActCh);
                    case 'dSpace'
                        fprintf(FID,'Controller: %s\n',handles.ExpControl.RealControl.Controller);
                        fprintf(FID,'Predictor-Corrector Type: %s\n',handles.ExpControl.RealControl.PCtype);
                        fprintf(FID,'Board Name: %s\n\n',handles.ExpControl.RealControl.boardName);
                    case 'xPCtarget'
                        fprintf(FID,'Controller: %s\n',handles.ExpControl.RealControl.Controller);
                        fprintf(FID,'Predictor-Corrector Type: %s\n',handles.ExpControl.RealControl.PCtype);
                        fprintf(FID,'IP Address: %s\n',handles.ExpControl.RealControl.ipAddr);
                        fprintf(FID,'IP Port: %s\n',handles.ExpControl.RealControl.ipPort);
                        fprintf(FID,'Application Name: %s\n\n',handles.ExpControl.RealControl.appName);
                end
        end
        
        fprintf(FID,'# ------------------------------\n# Analysis Properties\n# ------------------------------\n');
        fprintf(FID,'Analysis Time Step: %1.4f\n',handles.GM.dtAnalysis);
        fprintf(FID,'Analysis Time Step Limit: %1.4f\n',handles.Model.Maxdt);
        
        
    %%%%%%%%%%%%%%%
    %2 DOF B  Case%
    %%%%%%%%%%%%%%%
    case '2 DOF B'
        fprintf(FID,'# ------------------------------\n# Model Properties\n# ------------------------------\n');
        fprintf(FID,'Model Type: 2 DOF Column\n');
        fprintf(FID,'Mass: [%1.4f  %1.4f; %1.4f  %1.4f]\n',handles.Model.M(1,1),handles.Model.M(2,1),handles.Model.M(1,2),handles.Model.M(2,2));
        fprintf(FID,'Stiffness: [%1.4f  %1.4f; %1.4f  %1.4f]\n',handles.Model.K(1,1),handles.Model.K(2,1),handles.Model.K(1,2),handles.Model.K(2,2));
        fprintf(FID,'Period: %1.4f  %1.4f\n',handles.Model.T(1,1),handles.Model.T(2,1));
        fprintf(FID,'Frequency: %1.4f %1.4f\n',(1/handles.Model.T(1,1)),(1/handles.Model.T(2,1)));
        fprintf(FID,'Damping Type: %s\n',handles.Model.DampType);
        if strcmp(handles.Model.DampType,'Rayleigh')
            fprintf(FID,'Damping Values: %1.4f  %1.4f\n',handles.Model.Zeta(1,1),handles.Model.Zeta(1,2));
        else
            fprintf(FID,'Damping Value: %1.4f\n',handles.Model.Zeta);
        end
        fprintf(FID,'Mass Proportional Damping Factor: %1.4f\n',handles.Model.alphaM);
        fprintf(FID,'Stiffness Proportional Damping Factor: %1.4f\n\n',handles.Model.betaK);
        
        fprintf(FID,'# ------------------------------\n# Loading\n# ------------------------------\n');
        fprintf(FID,'Direction 1\n');
        strLength = length(handles.GM.store.filepath{1});
        id = strfind(handles.GM.store.filepath,filesep);
        strStart = id{1}(end);
        GMName = handles.GM.store.filepath{1}(strStart+1:strLength);
        fprintf(FID,'Ground Motion: %s\n',GMName);
        fprintf(FID,'Database Type: %s\n',handles.GM.databaseType);
        fprintf(FID,'PGA: %1.4f\n',handles.GM.Spectra{1}.pga);
        fprintf(FID,'Ground Motion Time Step: %1.4f\n',handles.GM.dt(1));
        fprintf(FID,'Amplitude Scale Factor: %1.4f\n',handles.GM.AmpFact(1));
        fprintf(FID,'Time Scale Factor: %1.4f\n\n',handles.GM.TimeFact(1));
        
        fprintf(FID,'Direction 2\n');
        strLength = length(handles.GM.store.filepath{2});
        id = strfind(handles.GM.store.filepath,filesep);
        strStart = id{1}(end);
        GMName = handles.GM.store.filepath{2}(strStart+1:strLength);
        fprintf(FID,'Ground Motion: %s\n',GMName);
        fprintf(FID,'Database Type: %s\n',handles.GM.databaseType);
        fprintf(FID,'PGA: %1.4f\n',handles.GM.Spectra{2}.pga);
        fprintf(FID,'Ground Motion Time Step: %1.4f\n',handles.GM.dt(2));
        fprintf(FID,'Amplitude Scale Factor: %1.4f\n',handles.GM.AmpFact(2));
        fprintf(FID,'Time Scale Factor: %1.4f\n\n',handles.GM.TimeFact(2));
        
        fprintf(FID,'# ------------------------------\n# Experimental Setup\n# ------------------------------\n');
        fprintf(FID,'Setup Type: No Transformation\n');
        if strcmp(handles.ExpControl.Type,'Simulation')
            fprintf(FID,'Control: %s\n',handles.ExpControl.SimControl.SimType);
        else
            fprintf(FID,'Control: %s\n',handles.ExpControl.RealControl.Controller);
        end
        fprintf(FID,'Directions: Global X, Global Y\n');
        fprintf(FID,'Size Trial: 2\n');
        fprintf(FID,'Size Output: 2\n\n');
        
        
        fprintf(FID,'# ------------------------------\n# Experimental Control\n# ------------------------------\n');
        switch handles.ExpControl.Type
            case 'Simulation'
                fprintf(FID,'Control Type: %s\n',handles.ExpControl.Type);
                fprintf(FID,'Simulation Type: %s\n\n',handles.ExpControl.SimControl.SimType);
                switch handles.ExpControl.DOF1.SimMaterial
                    case 'Elastic'
                        fprintf(FID,'DOF 1 Material: %s\n',handles.ExpControl.DOF1.SimMaterial);
                        fprintf(FID,'E: %1.4f\n\n',handles.ExpControl.DOF1.E);
                    case 'Elastic-Perfectly Plastic'
                        fprintf(FID,'DOF 1 Material: %s\n',handles.ExpControl.DOF1.SimMaterial);
                        fprintf(FID,'E: %1.4f\n',handles.ExpControl.DOF1.E);
                        fprintf(FID,'epsP: %1.4f\n\n',handles.ExpControl.DOF1.epsP);
                    case 'Steel - Bilinear'
                        fprintf(FID,'DOF 1 Material: %s\n',handles.ExpControl.DOF1.SimMaterial);
                        fprintf(FID,'Fy: %1.4f\n',handles.ExpControl.DOF1.Fy);
                        fprintf(FID,'E0: %1.4f\n',handles.ExpControl.DOF1.E0);
                        fprintf(FID,'b: %1.4f\n\n',handles.ExpControl.DOF1.b);
                    case 'Steel - Giuffré-Menegotto-Pinto'
                        fprintf(FID,'DOF 1 Material: %s\n',handles.ExpControl.DOF1.SimMaterial);
                        fprintf(FID,'Fy: %1.4f\n',handles.ExpControl.DOF1.Fy);
                        fprintf(FID,'E: %1.4f\n',handles.ExpControl.DOF1.E);
                        fprintf(FID,'b: %1.4f\n',handles.ExpControl.DOF1.b);
                        fprintf(FID,'R0: %1.4f\n',handles.ExpControl.DOF1.R0);
                        fprintf(FID,'cR1: %1.4f\n',handles.ExpControl.DOF1.cR1);
                        fprintf(FID,'cR2: %1.4f\n\n',handles.ExpControl.DOF1.cR2);
                end        
                switch handles.ExpControl.DOF2.SimMaterial
                    case 'Elastic'
                        fprintf(FID,'DOF 2 Material: %s\n',handles.ExpControl.DOF2.SimMaterial);
                        fprintf(FID,'E: %1.4f\n\n',handles.ExpControl.DOF2.E);
                    case 'Elastic-Perfectly Plastic'
                        fprintf(FID,'DOF 2 Material: %s\n',handles.ExpControl.DOF2.SimMaterial);
                        fprintf(FID,'E: %1.4f\n',handles.ExpControl.DOF2.E);
                        fprintf(FID,'epsP: %1.4f\n\n',handles.ExpControl.DOF2.epsP);
                    case 'Steel - Bilinear'
                        fprintf(FID,'DOF 2 Material: %s\n',handles.ExpControl.DOF2.SimMaterial);
                        fprintf(FID,'Fy: %1.4f\n',handles.ExpControl.DOF2.Fy);
                        fprintf(FID,'E0: %1.4f\n',handles.ExpControl.DOF2.E0);
                        fprintf(FID,'b: %1.4f\n\n',handles.ExpControl.DOF2.b);
                    case 'Steel - Giuffré-Menegotto-Pinto'
                        fprintf(FID,'DOF 2 Material: %s\n',handles.ExpControl.DOF2.SimMaterial);
                        fprintf(FID,'Fy: %1.4f\n',handles.ExpControl.DOF2.Fy);
                        fprintf(FID,'E: %1.4f\n',handles.ExpControl.DOF2.E);
                        fprintf(FID,'b: %1.4f\n',handles.ExpControl.DOF2.b);
                        fprintf(FID,'R0: %1.4f\n',handles.ExpControl.DOF2.R0);
                        fprintf(FID,'cR1: %1.4f\n',handles.ExpControl.DOF2.cR1);
                        fprintf(FID,'cR2: %1.4f\n\n',handles.ExpControl.DOF2.cR2);
                end

            case 'Real'
                fprintf(FID,'Control Type: %s\n',handles.ExpControl.Type);
                switch handles.ExpControl.RealControl.Controller
                    case 'LabVIEW'
                        fprintf(FID,'Controller: %s\n',handles.ExpControl.RealControl.Controller);
                        fprintf(FID,'IP Address: %s\n',handles.ExpControl.RealControl.ipAddr);
                        fprintf(FID,'IP Port: %s\n\n',handles.ExpControl.RealControl.ipPort);
                        
%                         %Define experimental control points
%                         fprintf(FID,'# Define control points\n# ---------------------\n');
%                         fprintf(FID,'# expControlPoint tag nodeTag dir resp <-fact f> <-lim l u> ...\n');
%                         fprintf(FID,'expControlPoint 1 1  ux disp -fact 1.0 -lim -7.5 7.5  uy disp -fact 1.0 -lim -7.5 7.5\n');
%                         fprintf(FID,'expControlPoint 2 1  ux disp -fact 1.0 -lim -7.5 7.5  uy disp -fact 1.0 -lim -7.5 7.5  ux force -fact 1.0 -lim -12.0 12.0  uy force -fact 1.0 -lim -12.0 12.0\n\n');
                        
                    case 'MTSCsi'
                        fprintf(FID,'Controller: %s\n',handles.ExpControl.RealControl.Controller);
                        fprintf(FID,'Configuration File: %s\n',strcat(handles.ExpControl.RealControl.ConfigName,handles.ExpControl.RealControl.ConfigType));
                        fprintf(FID,'Ramp Time: %1.4f\n\n',handles.ExpControl.RealControl.rampTime);
                    case 'SCRAMNet'
                        fprintf(FID,'Controller: %s\n',handles.ExpControl.RealControl.Controller);
                        fprintf(FID,'Memory Offset (bytes): %1.0f\n',handles.ExpControl.RealControl.memOffset);
                        fprintf(FID,'Number of Actuator Channels: %1.0f\n\n',handles.ExpControl.RealControl.NumActCh);
                    case 'dSpace'
                        fprintf(FID,'Controller: %s\n',handles.ExpControl.RealControl.Controller);
                        fprintf(FID,'Predictor-Corrector Type: %s\n',handles.ExpControl.RealControl.PCtype);
                        fprintf(FID,'Board Name: %s\n\n',handles.ExpControl.RealControl.boardName);
                    case 'xPCtarget'
                        fprintf(FID,'Controller: %s\n',handles.ExpControl.RealControl.Controller);
                        fprintf(FID,'Predictor-Corrector Type: %s\n',handles.ExpControl.RealControl.PCtype);
                        fprintf(FID,'IP Address: %s\n',handles.ExpControl.RealControl.ipAddr);
                        fprintf(FID,'IP Port: %s\n',handles.ExpControl.RealControl.ipPort);
                        fprintf(FID,'Application Name: %s\n\n',handles.ExpControl.RealControl.appName);
                end
        end
        
        fprintf(FID,'# ------------------------------\n# Analysis Properties\n# ------------------------------\n');
        fprintf(FID,'Analysis Time Step: %1.4f\n',handles.GM.dtAnalysis);
        fprintf(FID,'Analysis Time Step Limit: %1.4f\n',handles.Model.Maxdt);
end
fclose(FID);
end