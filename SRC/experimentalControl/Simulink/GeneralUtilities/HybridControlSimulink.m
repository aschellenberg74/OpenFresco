function varargout = HybridControlSimulink(action,type,appName,ctrlDsp,ctrlVel,ctrlAcc)
%HYBRIDCONTROLSIMULINK to obtain the resting forces from simulink
% varargout = HybridControlSimulink(action,type,appName,ctrlDsp,ctrlVel,ctrlAcc)
%
% varargout : output with following possible values
%                 {daqDsp,daqFrc} : for case 'acquire'
%                 {}              : for case 'initialize', 'execute' and 'stop'
% action    : switch with following possible values
%                 'initialize'   initialize the api to access the simulink model
%                 'control'      send the target displacements to predictor-corrector
%                 'acquire'      acquire displacements and resisting forces
%                 'stop'         stop the simulink model
% type      : controller type (Dsp, DspVel, DspVelAcc)
% appName   : simulink application name
% ctrlDsp   : control displacements
% ctrlVel   : control velocities
% ctrlAcc   : control accelerations
%
% Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
% Created: 11/04

persistent newTargetId switchPCId atTargetId ctrlDspId ctrlVelId ctrlAccId daqDspId daqFrcId

% set pause time
ptime = 0.0001;

switch action
    %=======================================================================
    case 'init'
        clc;
        % get signal structures
        newTargetId = [appName,'/model/simPC HC/newTarget'];
        switchPCId = get_param([appName,'/model/simPC HC/switchPC'],'RuntimeObject');
        atTargetId = get_param([appName,'/model/simPC HC/atTarget'],'RuntimeObject');
        ctrlDspId = [appName,'/model/simPC HC/targDsp'];
        if strcmp(type,'DspVel') || strcmp(type,'DspVelAcc')
            ctrlVelId = [appName,'/model/simPC HC/targVel'];
        end
        if strcmp(type,'DspVelAcc')
            ctrlAccId = [appName,'/simPC HC/targAcc'];
        end
        daqDspId = get_param([appName,'/model/simPC HC/measDsp'],'RuntimeObject');
        daqFrcId = get_param([appName,'/model/simPC HC/measFrc'],'RuntimeObject');
        
        % reset newTarget flag and control response
        set_param(newTargetId,'Value','0');
        set_param(ctrlDspId,'Value',['[',num2str(ctrlDsp),']']);
        if strcmp(type,'DspVel') || strcmp(type,'DspVelAcc')
            set_param(ctrlVelId,'Value',['[',num2str(ctrlVel),']']);
        end
        if strcmp(type,'DspVelAcc')
            set_param(ctrlAccId,'Value',['[',num2str(ctrlAcc),']']);
        end
        pause(ptime);
        
        varargout = {};
    %=======================================================================
    case 'control'
        % send ctrlDisp, ctrlVel and ctrlAccel
        set_param(ctrlDspId,'Value',['[',num2str(ctrlDsp),']']);
        if strcmp(type,'DspVel') || strcmp(type,'DspVelAcc')
            set_param(ctrlVelId,'Value',['[',num2str(ctrlVel),']']);
        end
        if strcmp(type,'DspVelAcc')
            set_param(ctrlAccId,'Value',['[',num2str(ctrlAcc),']']);
        end
        
        % set newTarget flag
        set_param(newTargetId,'Value','1');
        
        % wait until switchPC flag has changed as well
        switchPC = 0;
        while (switchPC ~= 1)
            switchPC = switchPCId.OutputPort(1).Data;
        end
        
        % reset newTarget flag
        set_param(newTargetId,'Value','0');
        
        % wait until switchPC flag has changed as well
        switchPC = 1;
        while (switchPC ~= 0)
            switchPC = switchPCId.OutputPort(1).Data;
        end
        pause(ptime);
        
        varargout = {};
    %=======================================================================
    case 'acquire'
        % wait until target is reached
        atTarget = 0;
        while (atTarget ~= 1)
            atTarget = atTargetId.OutputPort(1).Data;
        end
        
        % read displacements and resisting forces at target
        daqDsp = daqDspId.InputPort(1).Data';
        daqFrc = daqFrcId.InputPort(1).Data';
        pause(ptime);
        
        varargout = {daqDsp,daqFrc};
    %=======================================================================
    case 'stop'
        % stop simulink model
        set_param(appName,'SimulationCommand','stop');
        pause(ptime);
        
        varargout = {};
    %=======================================================================
end
