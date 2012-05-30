function Report(varargin)
%REPORT to generate report page for overview of model definition
% Report(varargin)
%
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

% check the required input data
[error,message] = InputCheck;
if error
    msgbox(message,'Error','error');
    return
end

% get the path and open file for writing
DIR = which('OPFE_Version.txt');
DIR = fileparts(DIR);
FID = fopen(fullfile(DIR,'OPFE_Report.txt'),'w');
ver = fgetl(fopen('OPFE_Version.txt'));

% print header
fprintf(FID,'\r\n');
fprintf(FID,'===========================================================================\r\n');
fprintf(FID,'OpenFresco Express v%s                                %s\r\n',ver,datestr(now));
fprintf(FID,'===========================================================================\r\n');
fprintf(FID,' o File   : OPFE_Report.txt\r\n');
fprintf(FID,' o Purpose: Summary of inputs for use with OpenFresco Express\r\n\r\n');

switch handles.Model.Type
    % =====================================================================
    case '1 DOF'
        fprintf(FID,'Model Properties:\r\n=================\r\n');
        fprintf(FID,' o Type: 1 DOF Column\r\n');
        fprintf(FID,' o Mass [m]: %1.4f\r\n',handles.Model.M);
        fprintf(FID,' o Stiffness [F/L]: %1.4f\r\n',handles.Model.K);
        fprintf(FID,' o Period [sec]: %1.4f\r\n',handles.Model.T);
        fprintf(FID,' o Frequency [Hz]: %1.4f\r\n',1/handles.Model.T);
        fprintf(FID,' o Damping Type: %s\r\n',handles.Model.DampType);
        fprintf(FID,' o Damping Value: %1.4f\r\n',handles.Model.Zeta);
        fprintf(FID,' o Mass Proportional Damping Factor: %1.4f\r\n',handles.Model.alphaM);
        fprintf(FID,' o Stiffness Proportional Damping Factor: %1.4f\r\n\r\n',handles.Model.betaK);
        
        fprintf(FID,'Loading:\r\n========\r\n');
        switch handles.GM.loadType
            case 'Ground Motions'
                [~,gmName,gmExt] = fileparts(handles.GM.store.filepath{1});
                fprintf(FID,' o Ground Motion: %s\r\n',[gmName,gmExt]);
                fprintf(FID,' o Database Type: %s\r\n',handles.GM.databaseType{1});
                fprintf(FID,' o PGA [L/sec^2]: %3.4f\r\n',handles.GM.Spectra{1}.pga);
                fprintf(FID,' o PGV [L/sec]: %3.4f\r\n',max(abs(handles.GM.scalevg{1})));
                fprintf(FID,' o PGD [L]: %3.4f\r\n',max(abs(handles.GM.scaledg{1})));
                fprintf(FID,' o Ground Motion Time Step [sec]: %1.4f\r\n',handles.GM.dt(1));
                fprintf(FID,' o Ground Motion Time Step Limit [sec]: %1.4f\r\n',handles.Model.Maxdt);
                fprintf(FID,' o Amplitude Scale Factor: %1.4f\r\n',handles.GM.AmpFact(1));
                fprintf(FID,' o Time Scale Factor: %1.4f\r\n\r\n',handles.GM.TimeFact(1));
            case 'Initial Conditions'
                fprintf(FID,' o Initial Displacement [L]: %1.4f\r\n',handles.GM.initialDisp(1));
                fprintf(FID,' o Ramp Time [sec]: %1.4f\r\n',handles.GM.rampTime);
                fprintf(FID,' o Vibration Time [sec]: %1.4f\r\n\r\n',handles.GM.vibTime);
        end
        
        fprintf(FID,'Experimental Setup:\r\n===================\r\n');
        fprintf(FID,' o Setup: No Transformation\r\n');
        if strcmp(handles.ExpControl.Type,'Simulation')
            fprintf(FID,' o Control: %s\r\n',handles.ExpControl.SimControl.SimType);
        else
            fprintf(FID,' o Control: %s\r\n',handles.ExpControl.RealControl.Controller);
        end
        fprintf(FID,' o Direction: Global X\r\n');
        fprintf(FID,' o Size Trial: 1\r\n');
        fprintf(FID,' o Size Output: 1\r\n\r\n');
        
        fprintf(FID,'Experimental Control:\r\n=====================\r\n');
        switch handles.ExpControl.Type
            case 'Simulation'
                fprintf(FID,' o Type: %s\r\n',handles.ExpControl.Type);
                fprintf(FID,' o Controller: %s\r\n',handles.ExpControl.SimControl.SimType);
                switch handles.ExpControl.DOF1.SimMaterial
                    case 'Elastic'
                        fprintf(FID,' o Material: %s\r\n',handles.ExpControl.DOF1.SimMaterial);
                        fprintf(FID,'     E [F/L^2]: %1.4f\r\n\r\n',handles.ExpControl.DOF1.E);
                    case 'Elastic-Perfectly Plastic'
                        fprintf(FID,' o Material: %s\r\n',handles.ExpControl.DOF1.SimMaterial);
                        fprintf(FID,'     E [F/L^2]: %1.4f\r\n',handles.ExpControl.DOF1.E);
                        fprintf(FID,'     epsP: %1.4f\r\n\r\n',handles.ExpControl.DOF1.epsP);
                    case 'Steel - Bilinear'
                        fprintf(FID,' o Material: %s\r\n',handles.ExpControl.DOF1.SimMaterial);
                        fprintf(FID,'     Fy [F/L^2]: %1.4f\r\n',handles.ExpControl.DOF1.Fy);
                        fprintf(FID,'     E [F/L^2]: %1.4f\r\n',handles.ExpControl.DOF1.E);
                        fprintf(FID,'     b: %1.4f\r\n\r\n',handles.ExpControl.DOF1.b);
                    case 'Steel - Giuffré-Menegotto-Pinto'
                        fprintf(FID,' o Material: %s\r\n',handles.ExpControl.DOF1.SimMaterial);
                        fprintf(FID,'     Fy [F/L^2]: %1.4f\r\n',handles.ExpControl.DOF1.Fy);
                        fprintf(FID,'     E [F/L^2]: %1.4f\r\n',handles.ExpControl.DOF1.E);
                        fprintf(FID,'     b: %1.4f\r\n',handles.ExpControl.DOF1.b);
                        fprintf(FID,'     R0: %1.4f\r\n',handles.ExpControl.DOF1.R0);
                        fprintf(FID,'     cR1: %1.4f\r\n',handles.ExpControl.DOF1.cR1);
                        fprintf(FID,'     cR2: %1.4f\r\n\r\n',handles.ExpControl.DOF1.cR2);
                end
            case 'Real'
                fprintf(FID,' o Type: %s\r\n',handles.ExpControl.Type);
                fprintf(FID,' o Control: %s\r\n',handles.ExpControl.RealControl.Controller);
                switch handles.ExpControl.RealControl.Controller
                    case 'LabVIEW'
                        fprintf(FID,' o IP Address: %s\r\n',handles.ExpControl.RealControl.ipAddr);
                        fprintf(FID,' o IP Port: %s\r\n\r\n',handles.ExpControl.RealControl.ipPort);
                        % define experimental control points
                        %fprintf(FID,'# Define control points\r\n# ---------------------\r\n');
                        %fprintf(FID,'# expControlPoint tag nodeTag dir resp <-fact f> <-lim l u> ...\r\n');
                        %fprintf(FID,'expControlPoint 1 1  ux disp -fact 1.0 -lim -7.5 7.5\r\n');
                        %fprintf(FID,'expControlPoint 2 1  ux disp -fact 1.0 -lim -7.5 7.5  ux force -fact 1.0 -lim -12.0 12.0\r\n\r\n');
                    case 'MTSCsi'
                        fprintf(FID,' o Configuration File: %s\r\n',strcat(handles.ExpControl.RealControl.ConfigName,handles.ExpControl.RealControl.ConfigType));
                        fprintf(FID,' o Ramp Time [sec]: %1.4f\r\n\r\n',handles.ExpControl.RealControl.rampTime);
                    case 'SCRAMNet'
                        fprintf(FID,' o Memory Offset [bytes]: %1.0f\r\n',handles.ExpControl.RealControl.memOffset);
                        fprintf(FID,' o Number of Actuator Channels: %1.0f\r\n\r\n',handles.ExpControl.RealControl.NumActCh);
                    case 'dSpace'
                        fprintf(FID,' o Predictor-Corrector Type: %s\r\n',handles.ExpControl.RealControl.PCtype);
                        fprintf(FID,' o Board Name: %s\r\n\r\n',handles.ExpControl.RealControl.boardName);
                    case 'xPCtarget'
                        fprintf(FID,' o Predictor-Corrector Type: %s\r\n',handles.ExpControl.RealControl.PCtype);
                        fprintf(FID,' o IP Address: %s\r\n',handles.ExpControl.RealControl.ipAddr);
                        fprintf(FID,' o IP Port: %s\r\n',handles.ExpControl.RealControl.ipPort);
                        fprintf(FID,' o Application Name: %s\r\n\r\n',handles.ExpControl.RealControl.appName);
                end
        end
        
        fprintf(FID,'Analysis Properties:\r\n====================\r\n');
        fprintf(FID,' o Analysis Time Step [sec]: %1.4f\r\n',handles.GM.dtAnalysis);
        fprintf(FID,' o Analysis Time Step Limit [sec]: %1.4f\r\n',handles.Model.Maxdt);
    
    % =====================================================================
    case '2 DOF A'
        fprintf(FID,'Model Properties:\r\n=================\r\n');
        fprintf(FID,' o Type: 2 DOF Shear Building\r\n');
        fprintf(FID,' o Mass [M]: [%1.4f  %1.4f; %1.4f  %1.4f]\r\n',handles.Model.M(1,1),handles.Model.M(2,1),handles.Model.M(1,2),handles.Model.M(2,2));
        fprintf(FID,' o Stiffness [F/L]: [%1.4f  %1.4f; %1.4f  %1.4f]\r\n',handles.Model.K(1,1),handles.Model.K(2,1),handles.Model.K(1,2),handles.Model.K(2,2));
        fprintf(FID,' o Period [sec]: %1.4f  %1.4f\r\n',handles.Model.T(1,1),handles.Model.T(2,1));
        fprintf(FID,' o Frequency [Hz]: %1.4f %1.4f\r\n',(1/handles.Model.T(1,1)),(1/handles.Model.T(2,1)));
        fprintf(FID,' o Damping Type: %s\r\n',handles.Model.DampType);
        if strcmp(handles.Model.DampType,'Rayleigh')
            fprintf(FID,' o Damping Values: %1.4f  %1.4f\r\n',handles.Model.Zeta(1,1),handles.Model.Zeta(1,2));
        else
            fprintf(FID,' o Damping Value: %1.4f\r\n',handles.Model.Zeta);
        end
        fprintf(FID,' o Mass Proportional Damping Factor: %1.4f\r\n',handles.Model.alphaM);
        fprintf(FID,' o Stiffness Proportional Damping Factor: %1.4f\r\n\r\n',handles.Model.betaK);
        
        fprintf(FID,'Loading:\r\n========\r\n');
        switch handles.GM.loadType
            case 'Ground Motions'
                [~,gmName,gmExt] = fileparts(handles.GM.store.filepath{1});
                fprintf(FID,' o Ground Motion: %s\r\n',[gmName,gmExt]);
                fprintf(FID,' o Database Type: %s\r\n',handles.GM.databaseType{1});
                fprintf(FID,' o PGA [L/sec^2]: %3.4f\r\n',handles.GM.Spectra{1}.pga);
                fprintf(FID,' o PGV [L/sec]: %3.4f\r\n',max(abs(handles.GM.scalevg{1})));
                fprintf(FID,' o PGD [L]: %3.4f\r\n',max(abs(handles.GM.scaledg{1})));
                fprintf(FID,' o Ground Motion Time Step [sec]: %1.4f\r\n',handles.GM.dt(1));
                fprintf(FID,' o Ground Motion Time Step Limit [sec]: %1.4f\r\n',handles.Model.Maxdt);
                fprintf(FID,' o Amplitude Scale Factor: %1.4f\r\n',handles.GM.AmpFact(1));
                fprintf(FID,' o Time Scale Factor: %1.4f\r\n\r\n',handles.GM.TimeFact(1));
            case 'Initial Conditions'
                fprintf(FID,' o Initial Displacement [L]: %1.4f %1.4f\r\n',handles.GM.initialDisp(1), handles.GM.initialDisp(2));
                fprintf(FID,' o Ramp Time [sec]: %1.4f\r\n',handles.GM.rampTime);
                fprintf(FID,' o Vibration Time [sec]: %1.4f\r\n\r\n',handles.GM.vibTime);
        end
        
        fprintf(FID,'Experimental Setup:\r\n===================\r\n');
        fprintf(FID,' o Setup: No Transformation\r\n');
        if strcmp(handles.ExpControl.Type,'Simulation')
            %fprintf(FID,' o Control: %s\r\n',handles.ExpControl.SimControl.SimType);
            fprintf(FID,' o Control: SimDomain\r\n');
        else
            fprintf(FID,' o Control: %s\r\n',handles.ExpControl.RealControl.Controller);
        end
        fprintf(FID,' o Direction: Global X\r\n');
        fprintf(FID,' o Size Trial: 2\r\n');
        fprintf(FID,' o Size Output: 2\r\n\r\n');
        
        fprintf(FID,'Experimental Control:\r\n=====================\r\n');
        switch handles.ExpControl.Type
            case 'Simulation'
                fprintf(FID,' o Type: %s\r\n',handles.ExpControl.Type);
                %fprintf(FID,' o Control: %s\r\n',handles.ExpControl.SimControl.SimType);
                fprintf(FID,' o Control: SimDomain\r\n');
                switch handles.ExpControl.DOF1.SimMaterial
                    case 'Elastic'
                        fprintf(FID,' o Story 1 Material: %s\r\n',handles.ExpControl.DOF1.SimMaterial);
                        fprintf(FID,'     E [F/L^2]: %1.4f\r\n',handles.ExpControl.DOF1.E);
                    case 'Elastic-Perfectly Plastic'
                        fprintf(FID,' o Story 1 Material: %s\r\n',handles.ExpControl.DOF1.SimMaterial);
                        fprintf(FID,'     E [F/L^2]: %1.4f\r\n',handles.ExpControl.DOF1.E);
                        fprintf(FID,'     epsP: %1.4f\r\n',handles.ExpControl.DOF1.epsP);
                    case 'Steel - Bilinear'
                        fprintf(FID,' o Story 1 Material: %s\r\n',handles.ExpControl.DOF1.SimMaterial);
                        fprintf(FID,'     Fy [F/L^2]: %1.4f\r\n',handles.ExpControl.DOF1.Fy);
                        fprintf(FID,'     E [F/L^2]: %1.4f\r\n',handles.ExpControl.DOF1.E);
                        fprintf(FID,'     b: %1.4f\r\n',handles.ExpControl.DOF1.b);
                    case 'Steel - Giuffré-Menegotto-Pinto'
                        fprintf(FID,' o Story 1 Material: %s\r\n',handles.ExpControl.DOF1.SimMaterial);
                        fprintf(FID,'     Fy [F/L^2]: %1.4f\r\n',handles.ExpControl.DOF1.Fy);
                        fprintf(FID,'     E [F/L^2]: %1.4f\r\n',handles.ExpControl.DOF1.E);
                        fprintf(FID,'     b: %1.4f\r\n',handles.ExpControl.DOF1.b);
                        fprintf(FID,'     R0: %1.4f\r\n',handles.ExpControl.DOF1.R0);
                        fprintf(FID,'     cR1: %1.4f\r\n',handles.ExpControl.DOF1.cR1);
                        fprintf(FID,'     cR2: %1.4f\r\n',handles.ExpControl.DOF1.cR2);
                end
                switch handles.ExpControl.DOF2.SimMaterial
                    case 'Elastic'
                        fprintf(FID,' o Story 2 Material: %s\r\n',handles.ExpControl.DOF2.SimMaterial);
                        fprintf(FID,'     E [F/L^2]: %1.4f\r\n\r\n',handles.ExpControl.DOF2.E);
                    case 'Elastic-Perfectly Plastic'
                        fprintf(FID,' o Story 2 Material: %s\r\n',handles.ExpControl.DOF2.SimMaterial);
                        fprintf(FID,'     E [F/L^2]: %1.4f\r\n',handles.ExpControl.DOF2.E);
                        fprintf(FID,'     epsP: %1.4f\r\n\r\n',handles.ExpControl.DOF2.epsP);
                    case 'Steel - Bilinear'
                        fprintf(FID,' o Story 2 Material: %s\r\n',handles.ExpControl.DOF2.SimMaterial);
                        fprintf(FID,'     Fy [F/L^2]: %1.4f\r\n',handles.ExpControl.DOF2.Fy);
                        fprintf(FID,'     E [F/L^2]: %1.4f\r\n',handles.ExpControl.DOF2.E);
                        fprintf(FID,'     b: %1.4f\r\n\r\n',handles.ExpControl.DOF2.b);
                    case 'Steel - Giuffré-Menegotto-Pinto'
                        fprintf(FID,' o Story 2 Material: %s\r\n',handles.ExpControl.DOF2.SimMaterial);
                        fprintf(FID,'     Fy [F/L^2]: %1.4f\r\n',handles.ExpControl.DOF2.Fy);
                        fprintf(FID,'     E [F/L^2]: %1.4f\r\n',handles.ExpControl.DOF2.E);
                        fprintf(FID,'     b: %1.4f\r\n',handles.ExpControl.DOF2.b);
                        fprintf(FID,'     R0: %1.4f\r\n',handles.ExpControl.DOF2.R0);
                        fprintf(FID,'     cR1: %1.4f\r\n',handles.ExpControl.DOF2.cR1);
                        fprintf(FID,'     cR2: %1.4f\r\n\r\n',handles.ExpControl.DOF2.cR2);
                end
                % define experimental control points
                %fprintf(FID,'# Define control points\r\n# ---------------------\r\n');
                %fprintf(FID,'# expControlPoint tag nodeTag dir resp <-fact f> <-lim l u> ...\r\n');
                %fprintf(FID,'expControlPoint 1 2  ux disp\r\n');
                %fprintf(FID,'expControlPoint 2 2  ux disp  ux force\r\n');
                %fprintf(FID,'expControlPoint 3 3  ux disp\r\n');
                %fprintf(FID,'expControlPoint 4 3  ux disp  ux force\r\n\r\n');
            case 'Real'
                fprintf(FID,' o Type: %s\r\n',handles.ExpControl.Type);
                fprintf(FID,' o Control: %s\r\n',handles.ExpControl.RealControl.Controller);
                switch handles.ExpControl.RealControl.Controller
                    case 'LabVIEW'
                        fprintf(FID,' o IP Address: %s\r\n',handles.ExpControl.RealControl.ipAddr);
                        fprintf(FID,' o IP Port: %s\r\n\r\n',handles.ExpControl.RealControl.ipPort);
                        % define experimental control points
                        %fprintf(FID,'# Define control points\r\n# ---------------------\r\n');
                        %fprintf(FID,'# expControlPoint tag nodeTag dir resp <-fact f> <-lim l u> ...\r\n');
                        %fprintf(FID,'expControlPoint 1 1  ux disp -fact 1.0 -lim -7.5 7.5\r\n');
                        %fprintf(FID,'expControlPoint 2 1  ux disp -fact 1.0 -lim -7.5 7.5  ux force -fact 1.0 -lim -12.0 12.0\r\n');
                        %fprintf(FID,'expControlPoint 3 2  ux disp -fact 1.0 -lim -7.5 7.5\r\n');
                        %fprintf(FID,'expControlPoint 4 2  ux disp -fact 1.0 -lim -7.5 7.5  ux force -fact 1.0 -lim -12.0 12.0\r\n\r\n');
                    case 'MTSCsi'
                        fprintf(FID,' o Configuration File: %s\r\n',strcat(handles.ExpControl.RealControl.ConfigName,handles.ExpControl.RealControl.ConfigType));
                        fprintf(FID,' o Ramp Time [sec]: %1.4f\r\n\r\n',handles.ExpControl.RealControl.rampTime);
                    case 'SCRAMNet'
                        fprintf(FID,' o Memory Offset [bytes]: %1.0f\r\n',handles.ExpControl.RealControl.memOffset);
                        fprintf(FID,' o Number of Actuator Channels: %1.0f\r\n\r\n',handles.ExpControl.RealControl.NumActCh);
                    case 'dSpace'
                        fprintf(FID,' o Predictor-Corrector Type: %s\r\n',handles.ExpControl.RealControl.PCtype);
                        fprintf(FID,' o Board Name: %s\r\n\r\n',handles.ExpControl.RealControl.boardName);
                    case 'xPCtarget'
                        fprintf(FID,' o Predictor-Corrector Type: %s\r\n',handles.ExpControl.RealControl.PCtype);
                        fprintf(FID,' o IP Address: %s\r\n',handles.ExpControl.RealControl.ipAddr);
                        fprintf(FID,' o IP Port: %s\r\n',handles.ExpControl.RealControl.ipPort);
                        fprintf(FID,' o Application Name: %s\r\n\r\n',handles.ExpControl.RealControl.appName);
                end
        end
        
        fprintf(FID,'Analysis Properties:\r\n====================\r\n');
        fprintf(FID,' o Analysis Time Step [sec]: %1.4f\r\n',handles.GM.dtAnalysis);
        fprintf(FID,' o Analysis Time Step Limit [sec]: %1.4f\r\n',handles.Model.Maxdt);
        
    % =====================================================================
    case '2 DOF B'
        fprintf(FID,'Model Properties:\r\n=================\r\n');
        fprintf(FID,' o Type: 2 DOF Column\r\n');
        fprintf(FID,' o Mass [M]: [%1.4f  %1.4f; %1.4f  %1.4f]\r\n',handles.Model.M(1,1),handles.Model.M(2,1),handles.Model.M(1,2),handles.Model.M(2,2));
        fprintf(FID,' o Stiffness [F/L]: [%1.4f  %1.4f; %1.4f  %1.4f]\r\n',handles.Model.K(1,1),handles.Model.K(2,1),handles.Model.K(1,2),handles.Model.K(2,2));
        fprintf(FID,' o Period [sec]: %1.4f  %1.4f\r\n',handles.Model.T(1,1),handles.Model.T(2,1));
        fprintf(FID,' o Frequency [Hz]: %1.4f %1.4f\r\n',(1/handles.Model.T(1,1)),(1/handles.Model.T(2,1)));
        fprintf(FID,' o Damping Type: %s\r\n',handles.Model.DampType);
        if strcmp(handles.Model.DampType,'Rayleigh')
            fprintf(FID,' o Damping Values: %1.4f  %1.4f\r\n',handles.Model.Zeta(1,1),handles.Model.Zeta(1,2));
        else
            fprintf(FID,' o Damping Value: %1.4f\r\n',handles.Model.Zeta);
        end
        fprintf(FID,' o Mass Proportional Damping Factor: %1.4f\r\n',handles.Model.alphaM);
        fprintf(FID,' o Stiffness Proportional Damping Factor: %1.4f\r\n\r\n',handles.Model.betaK);
        
        fprintf(FID,'Loading:\r\n========\r\n');
        switch handles.GM.loadType
            case 'Ground Motions'
                fprintf(FID,' o Direction 1\r\n');
                [~,gmName,gmExt] = fileparts(handles.GM.store.filepath{1});
                fprintf(FID,' o Ground Motion: %s\r\n',[gmName,gmExt]);
                fprintf(FID,' o Database Type: %s\r\n',handles.GM.databaseType{1});
                fprintf(FID,' o PGA [L/sec^2]: %1.4f\r\n',handles.GM.Spectra{1}.pga);
                fprintf(FID,' o PGV [L/sec]: %1.4f\r\n',max(abs(handles.GM.scalevg{1})));
                fprintf(FID,' o PGD [L]: %1.4f\r\n',max(abs(handles.GM.scaledg{1})));
                fprintf(FID,' o Ground Motion Time Step [sec]: %1.4f\r\n',handles.GM.dt(1));
                fprintf(FID,' o Ground Motion Time Step Limit [sec]: %1.4f\r\n',handles.Model.Maxdt);
                fprintf(FID,' o Amplitude Scale Factor: %1.4f\r\n',handles.GM.AmpFact(1));
                fprintf(FID,' o Time Scale Factor: %1.4f\r\n\r\n',handles.GM.TimeFact(1));
                
                fprintf(FID,' o Direction 2\r\n');
                [~,gmName,gmExt] = fileparts(handles.GM.store.filepath{2});
                fprintf(FID,' o Ground Motion: %s\r\n',[gmName,gmExt]);
                fprintf(FID,' o Database Type: %s\r\n',handles.GM.databaseType{2});
                fprintf(FID,' o PGA [L/sec^2]: %1.4f\r\n',handles.GM.Spectra{2}.pga);
                fprintf(FID,' o PGV [L/sec]: %1.4f\r\n',max(abs(handles.GM.scalevg{2})));
                fprintf(FID,' o PGD [L]: %1.4f\r\n',max(abs(handles.GM.scaledg{2})));
                fprintf(FID,' o Ground Motion Time Step [sec]: %1.4f\r\n',handles.GM.dt(2));
                fprintf(FID,' o Ground Motion Time Step Limit [sec]: %1.4f\r\n',handles.Model.Maxdt);
                fprintf(FID,' o Amplitude Scale Factor: %1.4f\r\n',handles.GM.AmpFact(2));
                fprintf(FID,' o Time Scale Factor: %1.4f\r\n\r\n',handles.GM.TimeFact(2));
            case 'Initial Conditions'
                fprintf(FID,' o Initial Displacement [L]: %1.4f %1.4f\r\n',handles.GM.initialDisp(1), handles.GM.initialDisp(2));
                fprintf(FID,' o Ramp Time [sec]: %1.4f\r\n',handles.GM.rampTime);
                fprintf(FID,' o Vibration Time [sec]: %1.4f\r\n\r\n',handles.GM.vibTime);
        end
        
        fprintf(FID,'Experimental Setup:\r\n===================\r\n');
        fprintf(FID,' o Setup: No Transformation\r\n');
        if strcmp(handles.ExpControl.Type,'Simulation')
            fprintf(FID,' o Control: %s\r\n',handles.ExpControl.SimControl.SimType);
        else
            fprintf(FID,' o Control: %s\r\n',handles.ExpControl.RealControl.Controller);
        end
        fprintf(FID,' o Directions: Global X, Global Y\r\n');
        fprintf(FID,' o Size Trial: 2\r\n');
        fprintf(FID,' o Size Output: 2\r\n\r\n');
        
        fprintf(FID,'Experimental Control:\r\n=====================\r\n');
        switch handles.ExpControl.Type
            case 'Simulation'
                fprintf(FID,' o Type: %s\r\n',handles.ExpControl.Type);
                fprintf(FID,' o Control Type: %s\r\n',handles.ExpControl.SimControl.SimType);
                switch handles.ExpControl.DOF1.SimMaterial
                    case 'Elastic'
                        fprintf(FID,' o DOF 1 Material: %s\r\n',handles.ExpControl.DOF1.SimMaterial);
                        fprintf(FID,'     E [F/L^2]: %1.4f\r\n',handles.ExpControl.DOF1.E);
                    case 'Elastic-Perfectly Plastic'
                        fprintf(FID,' o DOF 1 Material: %s\r\n',handles.ExpControl.DOF1.SimMaterial);
                        fprintf(FID,'     E [F/L^2]: %1.4f\r\n',handles.ExpControl.DOF1.E);
                        fprintf(FID,'     epsP: %1.4f\r\n',handles.ExpControl.DOF1.epsP);
                    case 'Steel - Bilinear'
                        fprintf(FID,' o DOF 1 Material: %s\r\n',handles.ExpControl.DOF1.SimMaterial);
                        fprintf(FID,'     Fy [F/L^2]: %1.4f\r\n',handles.ExpControl.DOF1.Fy);
                        fprintf(FID,'     E [F/L^2]: %1.4f\r\n',handles.ExpControl.DOF1.E);
                        fprintf(FID,'     b: %1.4f\r\n',handles.ExpControl.DOF1.b);
                    case 'Steel - Giuffré-Menegotto-Pinto'
                        fprintf(FID,' o DOF 1 Material: %s\r\n',handles.ExpControl.DOF1.SimMaterial);
                        fprintf(FID,'     Fy [F/L^2]: %1.4f\r\n',handles.ExpControl.DOF1.Fy);
                        fprintf(FID,'     E [F/L^2]: %1.4f\r\n',handles.ExpControl.DOF1.E);
                        fprintf(FID,'     b: %1.4f\r\n',handles.ExpControl.DOF1.b);
                        fprintf(FID,'     R0: %1.4f\r\n',handles.ExpControl.DOF1.R0);
                        fprintf(FID,'     cR1: %1.4f\r\n',handles.ExpControl.DOF1.cR1);
                        fprintf(FID,'     cR2: %1.4f\r\n',handles.ExpControl.DOF1.cR2);
                end
                switch handles.ExpControl.DOF2.SimMaterial
                    case 'Elastic'
                        fprintf(FID,' o DOF 2 Material: %s\r\n',handles.ExpControl.DOF2.SimMaterial);
                        fprintf(FID,'     E [F/L^2]: %1.4f\r\n\r\n',handles.ExpControl.DOF2.E);
                    case 'Elastic-Perfectly Plastic'
                        fprintf(FID,' o DOF 2 Material: %s\r\n',handles.ExpControl.DOF2.SimMaterial);
                        fprintf(FID,'     E [F/L^2]: %1.4f\r\n',handles.ExpControl.DOF2.E);
                        fprintf(FID,'     epsP: %1.4f\r\n\r\n',handles.ExpControl.DOF2.epsP);
                    case 'Steel - Bilinear'
                        fprintf(FID,' o DOF 2 Material: %s\r\n',handles.ExpControl.DOF2.SimMaterial);
                        fprintf(FID,'     Fy [F/L^2]: %1.4f\r\n',handles.ExpControl.DOF2.Fy);
                        fprintf(FID,'     E [F/L^2]: %1.4f\r\n',handles.ExpControl.DOF2.E);
                        fprintf(FID,'     b: %1.4f\r\n\r\n',handles.ExpControl.DOF2.b);
                    case 'Steel - Giuffré-Menegotto-Pinto'
                        fprintf(FID,' o DOF 2 Material: %s\r\n',handles.ExpControl.DOF2.SimMaterial);
                        fprintf(FID,'     Fy [F/L^2]: %1.4f\r\n',handles.ExpControl.DOF2.Fy);
                        fprintf(FID,'     E [F/L^2]: %1.4f\r\n',handles.ExpControl.DOF2.E);
                        fprintf(FID,'     b: %1.4f\r\n',handles.ExpControl.DOF2.b);
                        fprintf(FID,'     R0: %1.4f\r\n',handles.ExpControl.DOF2.R0);
                        fprintf(FID,'     cR1: %1.4f\r\n',handles.ExpControl.DOF2.cR1);
                        fprintf(FID,'     cR2: %1.4f\r\n\r\n',handles.ExpControl.DOF2.cR2);
                end
            case 'Real'
                fprintf(FID,' o Type: %s\r\n',handles.ExpControl.Type);
                fprintf(FID,' o Control: %s\r\n',handles.ExpControl.RealControl.Controller);
                switch handles.ExpControl.RealControl.Controller
                    case 'LabVIEW'
                        fprintf(FID,' o IP Address: %s\r\n',handles.ExpControl.RealControl.ipAddr);
                        fprintf(FID,' o IP Port: %s\r\n\r\n',handles.ExpControl.RealControl.ipPort);
                        % define experimental control points
                        %fprintf(FID,'# Define control points\r\n# ---------------------\r\n');
                        %fprintf(FID,'# expControlPoint tag nodeTag dir resp <-fact f> <-lim l u> ...\r\n');
                        %fprintf(FID,'expControlPoint 1 1  ux disp -fact 1.0 -lim -7.5 7.5  uy disp -fact 1.0 -lim -7.5 7.5\r\n');
                        %fprintf(FID,'expControlPoint 2 1  ux disp -fact 1.0 -lim -7.5 7.5  uy disp -fact 1.0 -lim -7.5 7.5  ux force -fact 1.0 -lim -12.0 12.0  uy force -fact 1.0 -lim -12.0 12.0\r\n\r\n');
                    case 'MTSCsi'
                        fprintf(FID,' o Configuration File: %s\r\n',strcat(handles.ExpControl.RealControl.ConfigName,handles.ExpControl.RealControl.ConfigType));
                        fprintf(FID,' o Ramp Time [sec]: %1.4f\r\n\r\n',handles.ExpControl.RealControl.rampTime);
                    case 'SCRAMNet'
                        fprintf(FID,' o Memory Offset [bytes]: %1.0f\r\n',handles.ExpControl.RealControl.memOffset);
                        fprintf(FID,' o Number of Actuator Channels: %1.0f\r\n\r\n',handles.ExpControl.RealControl.NumActCh);
                    case 'dSpace'
                        fprintf(FID,' o Predictor-Corrector Type: %s\r\n',handles.ExpControl.RealControl.PCtype);
                        fprintf(FID,' o Board Name: %s\r\n\r\n',handles.ExpControl.RealControl.boardName);
                    case 'xPCtarget'
                        fprintf(FID,' o Predictor-Corrector Type: %s\r\n',handles.ExpControl.RealControl.PCtype);
                        fprintf(FID,' o IP Address: %s\r\n',handles.ExpControl.RealControl.ipAddr);
                        fprintf(FID,' o IP Port: %s\r\n',handles.ExpControl.RealControl.ipPort);
                        fprintf(FID,' o Application Name: %s\r\n\r\n',handles.ExpControl.RealControl.appName);
                end
        end     
        
        fprintf(FID,'Analysis Properties:\r\n====================\r\n');
        fprintf(FID,' o Analysis Time Step [sec]: %1.4f\r\n',handles.GM.dtAnalysis);
        fprintf(FID,' o Analysis Time Step Limit [sec]: %1.4f\r\n',handles.Model.Maxdt);
    % =====================================================================
end
fclose(FID);

% update handles structure
guidata(gcbf,handles);
