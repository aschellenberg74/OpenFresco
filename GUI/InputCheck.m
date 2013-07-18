function [error,message] = InputCheck(varargin)
%INPUTCHECK to check inputs for completeness before writing the TCL file
% [error,message] = InputCheck(varargin)
%
% error    : returns the flag for errors found
% message  : returns the relevant error message
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
error = 0;
message = 'Writing of the .tcl file failed!';

% check for model chosen and loading
if isempty(handles.Model.Type)
    error = 1;
    message = sprintf('Writing of the .tcl file failed!\nModel type not specified.');
    return
else
    switch handles.GM.loadType
        % =================================================================
        case 'Ground Motions'
            if length(handles.GM.ag{1}) < 2
                error = 1;
                message = sprintf('Writing of the .tcl file failed!\nNo ground motion loaded.');
                return
            end
            if strcmp(handles.Model.Type,'2 DOF B') && length(handles.GM.ag{2}) < 2
                error = 1;
                message = sprintf('Writing of the .tcl file failed!\nGround motion data missing.');
                return
            end
        % =================================================================
        case 'Initial Conditions'
            if isempty(handles.GM.initialDisp) || isempty(handles.GM.rampTime) || isempty(handles.GM.vibTime)
                error = 1;
                message = sprintf('Writing of the .tcl file failed!\nIncomplete free vibration parameters.');
                return
            end
        % =================================================================
    end
    
    switch handles.ExpSite.Type
        % =================================================================
        case 'Distributed'
            error = 1;
            message = sprintf('Writing of the .tcl file failed!\nDistributed site type not specified.');
            return
        % =================================================================
        case 'Shadow'
            if isempty(handles.ExpSite.ipAddr) || isempty(handles.ExpSite.ipPort) || isempty(handles.ExpSite.protocol) || isempty(handles.ExpSite.dataSize)
                error = 1;
                message = sprintf('Writing of the .tcl file failed!\nShadow Site inputs are incomplete.');
                return
            end
            % check if IP address has valid format
            octets = textscan(handles.ExpSite.ipAddr,'%s %s %s %s','delimiter','.');
            for i=1:4
                if (length(octets{i}) ~= 1) || (str2double(octets{i}{1}) < 0) || (str2double(octets{i}{1}) > 255)
                    error = 1;
                    message = sprintf('Writing of the .tcl file failed!\nShadow Site has invalid IP Address specified.');
                    return
                end
            end
        % =================================================================
        case 'Actor'
            if isempty(handles.ExpSite.ipPort) || isempty(handles.ExpSite.protocol)
                error = 1;
                message = sprintf('Writing of the .tcl file failed!\nActor Site inputs are incomplete.');
                return
            end
        % =================================================================
    end
    
    switch handles.Model.Type
        % =================================================================
        case '1 DOF'
            switch handles.ExpControl.Type
                % ---------------------------------------------------------
                case 'Simulation'
                    if isempty(handles.ExpControl.DOF1.SimMaterial)
                        error = 1;
                        message = sprintf('Writing of the .tcl file failed!\nDOF 1 material not specified.');
                        return
                    else
                        switch handles.ExpControl.DOF1.SimMaterial
                            case 'Elastic'
                                if isempty(handles.ExpControl.DOF1.E)
                                    error = 1;
                                    message = sprintf('Writing of the .tcl file failed!\nDOF 1 material is missing inputs.');
                                    return
                                end
                            case 'Elastic-Perfectly Plastic'
                                if isempty(handles.ExpControl.DOF1.E) || isempty(handles.ExpControl.DOF1.epsP)
                                    error = 1;
                                    message = sprintf('Writing of the .tcl file failed!\nDOF 1 material is missing inputs.');
                                    return
                                end
                            case 'Steel - Bilinear'
                                if isempty(handles.ExpControl.DOF1.Fy) || isempty(handles.ExpControl.DOF1.E) || isempty(handles.ExpControl.DOF1.b)
                                    error = 1;
                                    message = sprintf('Writing of the .tcl file failed!\nDOF 1 material is missing inputs.');
                                    return
                                end
                            case 'Steel - Giuffré-Menegotto-Pinto'
                                if isempty(handles.ExpControl.DOF1.Fy) || isempty(handles.ExpControl.DOF1.E) || isempty(handles.ExpControl.DOF1.b) || isempty(handles.ExpControl.DOF1.R0) || isempty(handles.ExpControl.DOF1.cR1) || isempty(handles.ExpControl.DOF1.cR2)
                                    error = 1;
                                    message = sprintf('Writing of the .tcl file failed!\nDOF 1 material is missing inputs.');
                                    return
                                end
                        end
                    end
                    
                    if isempty(handles.ExpControl.SimControl.SimType)
                        error = 1;
                        message = sprintf('Writing of the .tcl file failed!\nSimulation control type not specified.');
                        return
                    end
                % ---------------------------------------------------------
                case 'Real'
                    if isempty(handles.ExpControl.RealControl.Controller)
                        error = 1;
                        message = sprintf('Writing of the .tcl file failed!\nReal controller not specified.');
                        return
                    else
                        switch handles.ExpControl.RealControl.Controller
                            case 'LabVIEW'
                                if isempty(handles.ExpControl.RealControl.ipAddr) || isempty(handles.ExpControl.RealControl.ipPort)
                                    error = 1;
                                    message = sprintf('Writing of the .tcl file failed!\nLabVIEW inputs are incomplete.');
                                    return
                                end
                            case 'MTSCsi'
                                if isempty(handles.ExpControl.RealControl.ConfigPath) || isempty(handles.ExpControl.RealControl.ConfigName) || isempty(handles.ExpControl.RealControl.ConfigType) || isempty(handles.ExpControl.RealControl.rampTime)
                                    error = 1;
                                    message = sprintf('Writing of the .tcl file failed!\nMTSCsi inputs are incomplete.');
                                    return
                                end
                            case 'SCRAMNet'
                                if isempty(handles.ExpControl.RealControl.memOffset) || isempty(handles.ExpControl.RealControl.NumActCh)
                                    error = 1;
                                    message = sprintf('Writing of the .tcl file failed!\nSCRAMNet inputs are incomplete.');
                                    return
                                end
                            case 'dSpace'
                                if isempty(handles.ExpControl.RealControl.PCvalue) || isempty(handles.ExpControl.RealControl.boardName)
                                    error = 1;
                                    message = sprintf('Writing of the .tcl file failed!\ndSpace inputs are incomplete.');
                                    return
                                end
                            case 'xPCtarget'
                                if isempty(handles.ExpControl.RealControl.appPath) || isempty(handles.ExpControl.RealControl.PCvalue) || isempty(handles.ExpControl.RealControl.ipAddr) || isempty(handles.ExpControl.RealControl.ipPort) || isempty(handles.ExpControl.RealControl.appName)
                                    error = 1;
                                    message = sprintf('Writing of the .tcl file failed!\nxPCtarget inputs are incomplete.');
                                    return
                                end
                        end
                    end
                % ---------------------------------------------------------
            end
        % =================================================================
        case '2 DOF A'
            switch handles.ExpControl.Type
                % ---------------------------------------------------------
                case 'Simulation'
                    if isempty(handles.Model.M)
                        error = 1;
                        message = sprintf('Writing of the .tcl file failed!\nModel mass not specified.');
                        return
                    end
                    if isempty(handles.ExpControl.DOF1.SimMaterial)
                        error = 1;
                        message = sprintf('Writing of the .tcl file failed!\nDOF 1 material not specified.');
                        return
                    else
                        switch handles.ExpControl.DOF1.SimMaterial
                            case 'Elastic'
                                if isempty(handles.ExpControl.DOF1.E)
                                    error = 1;
                                    message = sprintf('Writing of the .tcl file failed!\nDOF 1 material is missing inputs.');
                                    return
                                end
                            case 'Elastic-Perfectly Plastic'
                                if isempty(handles.ExpControl.DOF1.E) || isempty(handles.ExpControl.DOF1.epsP)
                                    error = 1;
                                    message = sprintf('Writing of the .tcl file failed!\nDOF 1 material is missing inputs.');
                                    return
                                end
                            case 'Steel - Bilinear'
                                if isempty(handles.ExpControl.DOF1.Fy) || isempty(handles.ExpControl.DOF1.E) || isempty(handles.ExpControl.DOF1.b)
                                    error = 1;
                                    message = sprintf('Writing of the .tcl file failed!\nDOF 1 material is missing inputs.');
                                    return
                                end
                            case 'Steel - Giuffré-Menegotto-Pinto'
                                if isempty(handles.ExpControl.DOF1.Fy) || isempty(handles.ExpControl.DOF1.E) || isempty(handles.ExpControl.DOF1.b) || isempty(handles.ExpControl.DOF1.R0) || isempty(handles.ExpControl.DOF1.cR1) || isempty(handles.ExpControl.DOF1.cR2)
                                    error = 1;
                                    message = sprintf('Writing of the .tcl file failed!\nDOF 1 material is missing inputs.');
                                    return
                                end
                        end
                    end
                    
                    if isempty(handles.ExpControl.DOF2.SimMaterial)
                        error = 1;
                        message = sprintf('Writing of the .tcl file failed!\nDOF 2 material not specified.');
                        return
                    else
                        switch handles.ExpControl.DOF2.SimMaterial
                            case 'Elastic'
                                if isempty(handles.ExpControl.DOF2.E)
                                    error = 1;
                                    message = sprintf('Writing of the .tcl file failed!\nDOF 2 material is missing inputs.');
                                    return
                                end
                            case 'Elastic-Perfectly Plastic'
                                if isempty(handles.ExpControl.DOF2.E) || isempty(handles.ExpControl.DOF2.epsP)
                                    error = 1;
                                    message = sprintf('Writing of the .tcl file failed!\nDOF 2 material is missing inputs.');
                                    return
                                end
                            case 'Steel - Bilinear'
                                if isempty(handles.ExpControl.DOF2.Fy) || isempty(handles.ExpControl.DOF2.E) || isempty(handles.ExpControl.DOF2.b)
                                    error = 1;
                                    message = sprintf('Writing of the .tcl file failed!\nDOF 2 material is missing inputs.');
                                    return
                                end
                            case 'Steel - Giuffré-Menegotto-Pinto'
                                if isempty(handles.ExpControl.DOF2.Fy) || isempty(handles.ExpControl.DOF2.E) || isempty(handles.ExpControl.DOF2.b) || isempty(handles.ExpControl.DOF2.R0) || isempty(handles.ExpControl.DOF2.cR1) || isempty(handles.ExpControl.DOF2.cR2)
                                    error = 1;
                                    message = sprintf('Writing of the .tcl file failed!\nDOF 2 material is missing inputs.');
                                    return
                                end
                        end
                    end
                % ---------------------------------------------------------
                case 'Real'
                    if isempty(handles.ExpControl.RealControl.Controller)
                        error = 1;
                        message = sprintf('Writing of the .tcl file failed!\nReal controller not specified.');
                        return
                    else
                        switch handles.ExpControl.RealControl.Controller
                            case 'LabVIEW'
                                if isempty(handles.ExpControl.RealControl.ipAddr) || isempty(handles.ExpControl.RealControl.ipPort)
                                    error = 1;
                                    message = sprintf('Writing of the .tcl file failed!\nLabVIEW inputs are incomplete.');
                                    return
                                end
                            case 'MTSCsi'
                                if isempty(handles.ExpControl.RealControl.ConfigPath) || isempty(handles.ExpControl.RealControl.ConfigName) || isempty(handles.ExpControl.RealControl.ConfigType) || isempty(handles.ExpControl.RealControl.rampTime)
                                    error = 1;
                                    message = sprintf('Writing of the .tcl file failed!\nMTSCsi inputs are incomplete.');
                                    return
                                end
                            case 'SCRAMNet'
                                if isempty(handles.ExpControl.RealControl.memOffset) || isempty(handles.ExpControl.RealControl.NumActCh)
                                    error = 1;
                                    message = sprintf('Writing of the .tcl file failed!\nSCRAMNet inputs are incomplete.');
                                    return
                                end
                            case 'dSpace'
                                if isempty(handles.ExpControl.RealControl.PCvalue) || isempty(handles.ExpControl.RealControl.boardName)
                                    error = 1;
                                    message = sprintf('Writing of the .tcl file failed!\ndSpace inputs are incomplete.');
                                    return
                                end
                            case 'xPCtarget'
                                if isempty(handles.ExpControl.RealControl.appPath) || isempty(handles.ExpControl.RealControl.PCvalue) || isempty(handles.ExpControl.RealControl.ipAddr) || isempty(handles.ExpControl.RealControl.ipPort) || isempty(handles.ExpControl.RealControl.appName)
                                    error = 1;
                                    message = sprintf('Writing of the .tcl file failed!\nxPCtarget inputs are incomplete.');
                                    return
                                end
                        end
                    end
                % ---------------------------------------------------------
            end
        % =================================================================
        case '2 DOF B'
            switch handles.ExpControl.Type
                % ---------------------------------------------------------
                case 'Simulation'
                    if isempty(handles.ExpControl.DOF1.SimMaterial)
                        error = 1;
                        message = sprintf('Writing of the .tcl file failed!\nDOF 1 material not specified.');
                        return
                    else
                        switch handles.ExpControl.DOF1.SimMaterial
                            case 'Elastic'
                                if isempty(handles.ExpControl.DOF1.E)
                                    error = 1;
                                    message = sprintf('Writing of the .tcl file failed!\nDOF 1 material is missing inputs.');
                                    return
                                end
                            case 'Elastic-Perfectly Plastic'
                                if isempty(handles.ExpControl.DOF1.E) || isempty(handles.ExpControl.DOF1.epsP)
                                    error = 1;
                                    message = sprintf('Writing of the .tcl file failed!\nDOF 1 material is missing inputs.');
                                    return
                                end
                            case 'Steel - Bilinear'
                                if isempty(handles.ExpControl.DOF1.Fy) || isempty(handles.ExpControl.DOF1.E) || isempty(handles.ExpControl.DOF1.b)
                                    error = 1;
                                    message = sprintf('Writing of the .tcl file failed!\nDOF 1 material is missing inputs.');
                                    return
                                end
                            case 'Steel - Giuffré-Menegotto-Pinto'
                                if isempty(handles.ExpControl.DOF1.Fy) || isempty(handles.ExpControl.DOF1.E) || isempty(handles.ExpControl.DOF1.b) || isempty(handles.ExpControl.DOF1.R0) || isempty(handles.ExpControl.DOF1.cR1) || isempty(handles.ExpControl.DOF1.cR2)
                                    error = 1;
                                    message = sprintf('Writing of the .tcl file failed!\nDOF 1 material is missing inputs.');
                                    return
                                end
                        end
                    end
                    
                    if isempty(handles.ExpControl.DOF2.SimMaterial)
                        error = 1;
                        message = sprintf('Writing of the .tcl file failed!\nDOF 2 material not specified.');
                        return
                    else
                        switch handles.ExpControl.DOF2.SimMaterial
                            case 'Elastic'
                                if isempty(handles.ExpControl.DOF2.E)
                                    error = 1;
                                    message = sprintf('Writing of the .tcl file failed!\nDOF 2 material is missing inputs.');
                                    return
                                end
                            case 'Elastic-Perfectly Plastic'
                                if isempty(handles.ExpControl.DOF2.E) || isempty(handles.ExpControl.DOF2.epsP)
                                    error = 1;
                                    message = sprintf('Writing of the .tcl file failed!\nDOF 2 material is missing inputs.');
                                    return
                                end
                            case 'Steel - Bilinear'
                                if isempty(handles.ExpControl.DOF2.Fy) || isempty(handles.ExpControl.DOF2.E) || isempty(handles.ExpControl.DOF2.b)
                                    error = 1;
                                    message = sprintf('Writing of the .tcl file failed!\nDOF 2 material is missing inputs.');
                                    return
                                end
                            case 'Steel - Giuffré-Menegotto-Pinto'
                                if isempty(handles.ExpControl.DOF2.Fy) || isempty(handles.ExpControl.DOF2.E) || isempty(handles.ExpControl.DOF2.b) || isempty(handles.ExpControl.DOF2.R0) || isempty(handles.ExpControl.DOF2.cR1) || isempty(handles.ExpControl.DOF2.cR2)
                                    error = 1;
                                    message = sprintf('Writing of the .tcl file failed!\nDOF 2 material is missing inputs.');
                                    return
                                end
                        end
                    end
                    
                    if isempty(handles.ExpControl.SimControl.SimType)
                        error = 1;
                        message = sprintf('Writing of the .tcl file failed!\nSimulation control type not specified.');
                        return
                    end
                % ---------------------------------------------------------
                case 'Real'
                    if isempty(handles.ExpControl.RealControl.Controller)
                        error = 1;
                        message = sprintf('Writing of the .tcl file failed!\nReal controller not specified.');
                        return
                    else
                        switch handles.ExpControl.RealControl.Controller
                            case 'LabVIEW'
                                if isempty(handles.ExpControl.RealControl.ipAddr) || isempty(handles.ExpControl.RealControl.ipPort)
                                    error = 1;
                                    message = sprintf('Writing of the .tcl file failed!\nLabVIEW inputs are incomplete.');
                                    return
                                end
                            case 'MTSCsi'
                                if isempty(handles.ExpControl.RealControl.ConfigPath) || isempty(handles.ExpControl.RealControl.ConfigName) || isempty(handles.ExpControl.RealControl.ConfigType) || isempty(handles.ExpControl.RealControl.rampTime)
                                    error = 1;
                                    message = sprintf('Writing of the .tcl file failed!\nMTSCsi inputs are incomplete.');
                                    return
                                end
                            case 'SCRAMNet'
                                if isempty(handles.ExpControl.RealControl.memOffset) || isempty(handles.ExpControl.RealControl.NumActCh)
                                    error = 1;
                                    message = sprintf('Writing of the .tcl file failed!\nSCRAMNet inputs are incomplete.');
                                    return
                                end
                            case 'dSpace'
                                if isempty(handles.ExpControl.RealControl.PCvalue) || isempty(handles.ExpControl.RealControl.boardName)
                                    error = 1;
                                    message = sprintf('Writing of the .tcl file failed!\ndSpace inputs are incomplete.');
                                    return
                                end
                            case 'xPCtarget'
                                if isempty(handles.ExpControl.RealControl.appPath) || isempty(handles.ExpControl.RealControl.PCvalue) || isempty(handles.ExpControl.RealControl.ipAddr) || isempty(handles.ExpControl.RealControl.ipPort) || isempty(handles.ExpControl.RealControl.appName)
                                    error = 1;
                                    message = sprintf('Writing of the .tcl file failed!\nxPCtarget inputs are incomplete.');
                                    return
                                end
                        end
                    end
                % ---------------------------------------------------------
            end
        % =================================================================
    end
end
