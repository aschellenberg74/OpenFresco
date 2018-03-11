function varargout = HybridControlxPCtarget(action,nCtrl,nDaq,type,appName,targDsp,targVel,targAcc)
%HYBRIDCONTROLXPCTARGET to obtain the resting forces from xPCtarget
% varargout = HybridControlxPCtarget(action,nCtrl,nDaq,type,appName,targDsp,targVel,targAcc)
%
% varargout : output with following possible values
%                 {measDsp,measFrc} : for case 'acquire'
%                 {}                : for case 'initialize', 'execute' and 'stop'
% action    : switch with following possible values
%                 'initialize'   initialize the api to access the simulink model
%                 'execute'      send the target displacements to predictor-corrector
%                 'acquire'      acquire resisting forces and displacements
%                 'stop'         stop the simulink model
% nCtrl     : size of control vector
% nDaq      : size of data acquisition vector
% type      : controller type (Dsp, DspVel, DspVelAcc)
% appName   : simulink application name
% targDsp   : controller target displacements
% targVel   : controller target velocities
% targAcc   : controller target accelerations
%
% Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
% Created: 11/04
%
% $Revision$
% $Date$
% $URL$

persistent target updateFlagId targetFlagId targDspId targVelId targAccId measDspId measFrcId

% set pause time
ptime = 0.0001;

switch action
    %=======================================================================
    case 'init'
        clc;
        % initialize xPC target object
        target = xpc;
        
        % load the compiled target application to the target
        load(target,appName);
        
        % stop the target application on the xPC Target
        start(target);
        pause(1.0);
        stop(target);
        
        fprintf('**************************************************************************\n');
        fprintf(['* The application "',appName,'" has been loaded and is stopped \n']);
        fprintf(['* sample time = ',num2str(get(target,'SampleTime')), ', stop time = ',num2str(get(target,'StopTime')),'\n']);
        fprintf('**************************************************************************\n\n');
        
        % get addresses of the controlled variables on the xPC Target
        updateFlagId = getparamid (target,'xPC HC/updateFlag','Value');
        targetFlagId = getsignalid(target,'xPC HC/targetFlag');
        targDspId    = getparamid (target,'xPC HC/targDsp','Value');
        if strcmp(type,'DspVel') || strcmp(type,'DspVelAcc')
            targVelId = getparamid(target,'xPC HC/targVel','Value');
        end
        if strcmp(type,'DspVelAcc')
            targAccId = getparamid(target,'xPC HC/targAcc','Value');
        end
        if (nDaq==1)
            measDspId = getsignalid(target,'xPC HC/measDsp');
            measFrcId = getsignalid(target,'xPC HC/measFrc');
        else
            for i=1:nDaq
                measDspId(i) = getsignalid(target,['xPC HC/measDsp/s',int2str(i)]);
                measFrcId(i) = getsignalid(target,['xPC HC/measFrc/s',int2str(i)]);
            end
        end
        
        fprintf('**************************************************************\n');
        fprintf('* Make sure that offset values of controller are set to ZERO *\n');
        fprintf('*                                                            *\n');
        fprintf('* Hit any key to proceed the initialization                  *\n');
        fprintf('**************************************************************\n\n');
        pause;
        
        % start the target application on the xPC Target
        start(target);
        
        % reset updateFlag and targDsp
        setparam(target,updateFlagId,0);
        setparam(target,targDspId,targDsp);
        if strcmp(type,'DspVel') || strcmp(type,'DspVelAcc')
            setparam(target,targVelId,targVel);
        end
        if strcmp(type,'DspVelAcc')
            setparam(target,targAccId,targAcc);
        end
        setparam(target,updateFlagId,1);
        
        targetFlag = 1;
        while targetFlag
            % get targetFlag
            pause(ptime);
            targetFlag = getsignal(target,targetFlagId);
        end
        
        % get displacement and resisting force from board
        for i=1:nDaq
            measDsp(i) = getsignal(target,measDspId(i));
            measFrc(i) = getsignal(target,measFrcId(i));
        end
        
        % reset updateFlag
        setparam(target,updateFlagId,0);
        
        % stop the target application on the xPC Target
        stop(target);
        
        fprintf('*********************************\n');
        fprintf('* Initial values of DAQ are:    *\n');
        fprintf('*                               *\n');
        fprintf('* measDsp = %+1.6E      *\n',measDsp);
        fprintf('* measFrc = %+1.6E      *\n',measFrc);
        fprintf('*                               *\n');
        fprintf('* If okay, hit any key to start *\n');
        fprintf('*********************************\n\n');
        pause;
        
        % start the target application on the xPC Target
        start(target);
        
        fprintf('******************\n');
        fprintf('* Running......  *\n');
        fprintf('******************\n\n');
        
        varargout = {};
    %=======================================================================
    case 'execute'
        % send target displacement and set updateFlag
        setparam(target,targDspId,targDsp);
        if strcmp(type,'DspVel') || strcmp(type,'DspVelAcc')
            setparam(target,targVelId,targVel);
        end
        if strcmp(type,'DspVelAcc')
            setparam(target,targAccId,targAcc);
        end
        setparam(target,updateFlagId,1);
        
        varargout = {};
        %=======================================================================
    case 'acquire'
        targetFlag = 1;
        while targetFlag
            % get targetFlag
            pause(ptime);
            targetFlag = getsignal(target,targetFlagId);
        end
        
        % get displacement and resisting force at target
        for i=1:nDaq
            measDsp(i) = getsignal(target,measDspId(i));
            measFrc(i) = getsignal(target,measFrcId(i));
        end
        
        % reset updateFlag
        setparam(target,updateFlagId,0);
        pause(ptime);
        
        varargout = {measDsp,measFrc};
    %=======================================================================
    case 'stop'
        % stop the target application on the xPC Target
        stop(target);
        
        fprintf('****************************************\n');
        fprintf('* The rtp application has been stopped *\n');
        fprintf('****************************************\n\n');
        
        varargout = {};
    %=======================================================================
end