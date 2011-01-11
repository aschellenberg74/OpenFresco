function [error message] = InputCheck(varargin)
%INPUTCHECK checks inputs for completeness before writing the TCL file
% error        : returns the flag for errors found
% message      : returns the relevant error message

%  Initialization tasks
handles = guidata(gcbf);
error = 0;
message = 'Writing of the .tcl file failed!';

%check for model chosen and loading
if isempty(handles.Model.Type)
    error = 1;
    message = sprintf('Writing of the .tcl file failed!\nModel type not specified.');
    return
else
    switch handles.GM.loadType
        case 'Ground Motions'
            if handles.GM.ag{1} == 0
                error = 1;
                message = sprintf('Writing of the .tcl file failed!\nNo ground motion loaded.');
                return
            end
            if strcmp(handles.Model.Type, '2 DOF B')
                s = size(handles.GM.ag);
                if s(2) ~= 2
                    error = 1;
                    message = sprintf('Writing of the .tcl file failed!\nGround motion data missing.');
                    return
                end
            end
        case 'Initial Conditions'
            if isempty(handles.GM.initialDisp) || isempty(handles.GM.rampTime) || isempty(handles.GM.vibTime)
                error = 1;
                message = sprintf('Writing of the .tcl file failed!\nIncomplete free vibration parameters.');
                return
            end
    end
    
    switch handles.Model.Type
        %%%%%%%%%%%%
        %1 DOF Case%
        %%%%%%%%%%%%
        case '1 DOF'
            switch handles.ExpControl.Type
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
                                if isempty(handles.ExpControl.DOF1.Fy) || isempty(handles.ExpControl.DOF1.E0) || isempty(handles.ExpControl.DOF1.b)
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
            end                                    

        %%%%%%%%%%%%%%%
        %2 DOF A  Case%
        %%%%%%%%%%%%%%%
        case '2 DOF A'
            switch handles.ExpControl.Type
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
                                if isempty(handles.ExpControl.DOF1.Fy) || isempty(handles.ExpControl.DOF1.E0) || isempty(handles.ExpControl.DOF1.b)
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
                                if isempty(handles.ExpControl.DOF2.Fy) || isempty(handles.ExpControl.DOF2.E0) || isempty(handles.ExpControl.DOF2.b)
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
            end

        %%%%%%%%%%%%%%%
        %2 DOF B  Case%
        %%%%%%%%%%%%%%%
        case '2 DOF B'
            switch handles.ExpControl.Type
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
                                if isempty(handles.ExpControl.DOF1.Fy) || isempty(handles.ExpControl.DOF1.E0) || isempty(handles.ExpControl.DOF1.b)
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
                                if isempty(handles.ExpControl.DOF2.Fy) || isempty(handles.ExpControl.DOF2.E0) || isempty(handles.ExpControl.DOF2.b)
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
            end
    end
end
end