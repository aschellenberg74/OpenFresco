function varargout = HybridControlSimulink(action,type,appName,targDsp,targVel,targAcc)
%HYBRIDCONTROLSIMULINK to obtain the resting forces from simulink
% varargout = HybridControlSimulink(action,type,appName,targDsp,targVel,targAcc)
%
% varargout : output with following possible values
%                 {measDsp,measFrc} : for case 'acquire'
%                 {}                : for case 'initialize', 'execute' and 'stop'
% action    : switch with following possible values
%                 'initialize'   initialize the api to access the simulink model
%                 'execute'      send the target displacements to predictor-corrector
%                 'acquire'      acquire resisting forces and displacements 
%                 'stop'         stop the simulink model
% type      : controller type (Dsp, DspVel, DspVelAcc)
% appName   : simulink application name
% targDsp   : controller target displacements
% targVel   : controller target velocities
% targAcc   : controller target accelerations
%
% Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
% Created: 11/04

persistent updateFlagId targetFlagId targDspId targVelId targAccId measDspId measFrcId

% set pause time
ptime = 0.0001;

switch action
   %=======================================================================
   case 'init'
      clc;
      % get signal structures
      updateFlagId = [appName,'/simPC HC/updateFlag'];
      targetFlagId = get_param([appName,'/simPC HC/targetFlag'],'RuntimeObject');
      targDspId  = [appName,'/simPC HC/targDsp'];
      if strcmp(type,'DspVel') | strcmp(type,'DspVelAcc')
         targVelId  = [appName,'/simPC HC/targVel'];
      end
      if strcmp(type,'DspVelAcc')
         targAccId  = [appName,'/simPC HC/targAcc'];
      end
      measDspId    = get_param([appName,'/simPC HC/measDsp'],'RuntimeObject');
      measFrcId    = get_param([appName,'/simPC HC/measFrc'],'RuntimeObject');

      % reset updateFlag and targDsp
      set_param(updateFlagId,'Value','-1');
      set_param(targDspId,'Value',['[',num2str(targDsp),']']);
      if strcmp(type,'DspVel') | strcmp(type,'DspVelAcc')
         set_param(targVelId,'Value',['[',num2str(targVel),']']);
      end
      if strcmp(type,'DspVelAcc')
         set_param(targAccId,'Value',['[',num2str(targAcc),']']);
      end      
      pause(ptime);
      
      varargout = {};
   %=======================================================================
   case 'execute'
      % send target displacements and set updateFlag
      set_param(targDspId,'Value',['[',num2str(targDsp),']']);
      if strcmp(type,'DspVel') | strcmp(type,'DspVelAcc')
         set_param(targVelId,'Value',['[',num2str(targVel),']']);
      end
      if strcmp(type,'DspVelAcc')
         set_param(targAccId,'Value',['[',num2str(targAcc),']']);
      end            
      set_param(updateFlagId,'Value','1');
      pause(ptime);
      
      varargout = {};
   %=======================================================================
   case 'acquire'
      targetFlag = 1;
      while targetFlag
         % get targetFlag
         pause(ptime);
         targetFlag = targetFlagId.OutputPort(1).Data;
      end
      
      % get displacement and resisting force at target
      pause(ptime);
      measDsp = measDspId.InputPort(1).Data';
      measFrc = measFrcId.InputPort(1).Data';
      
      % reset updateFlag
      set_param(updateFlagId,'Value','0');
      pause(ptime);

      varargout = {measDsp,measFrc};
   %=======================================================================
   case 'stop'
      % stop simulink model
      set_param(appName,'SimulationCommand','stop');
      pause(ptime);
      
      varargout = {};
   %=======================================================================
end