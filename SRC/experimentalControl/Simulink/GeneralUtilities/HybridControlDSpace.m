function varargout = HybridControlDSpace(action,type,boardName,targDsp,targVel,targAcc)
%HYBRIDCONTROLDSPACE to obtain the resting forces from dSpace board
% varargout = HybridControlDSpace(action,type,boardName,targDsp,targVel,targAcc)
%
% varargout : output with following possible values
%                 {measDisp,measForc} : for case 'acquire'
%                 {}                  : for case 'initialize', 'execute' and 'stop'
% action    : switch with following possible values
%                 'initialize'   initialize the api to access the simulink model
%                 'execute'      send the target displacement to predictor-corrector
%                 'acquire'      acquire resisting force and displacement
%                 'stop'         stop the simulink model
% type      : controller type (Dsp, DspVel, DspVelAcc)
% boardName : name of dSpace board (DS1103, DS1104, ...)
% targDsp   : controller target displacements
% targVel   : controller target velocities
% targAcc   : controller target accelerations
%
% Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
% Created: 11/04

persistent simStateId updateFlagId targetFlagId targDspId targVelId targAccId measDspId measFrcId

% set pause time
ptime = 0.0001;

switch action
    %=======================================================================
    case 'init'
        clc;
        % initialize dSPACE MLIB
        mlib('SelectBoard',boardName);
        fprintf('*****************************************\n');
        fprintf('* The board %s has been initialized *\n',boardName);
        fprintf('*****************************************\n\n');
        
        % check if the real-time processor application is running
        if ~mlib('IsApplRunning')
            fprintf('No rtp application is loaded on board %s - error\n\n',boardName);
            return
        else
            % check state of simulation
            simStateId = mlib('GetTrcVar','simState');
            simState = mlib('Read',simStateId);
            if simState~=0
                mlib('Write',simStateId,'Data',{0});
            end
            fprintf('******************************************************\n');
            fprintf('* The rtp application has been loaded and is stopped *\n');
            fprintf('******************************************************\n\n');
        end
        
        % get descriptors for requested variables (use '\n' for new line)
        updateFlagId = mlib('GetTrcVar','Tunable Parameters/updateFlag');
        targetFlagId = mlib('GetTrcVar','Labels/targetFlag');
        targDspId   = mlib('GetTrcVar','Tunable Parameters/targDsp');
        if strcmp(type,'DspVel') | strcmp(type,'DspVelAcc')
            targVelId   = mlib('GetTrcVar','Tunable Parameters/targVel');
        end
        if strcmp(type,'DspVelAcc')
            targAccId   = mlib('GetTrcVar','Tunable Parameters/targAcc');
        end
        measDspId   = mlib('GetTrcVar','Labels/measDsp');
        measFrcId   = mlib('GetTrcVar','Labels/measFrc');
        
        fprintf('**************************************************************\n');
        fprintf('* Make sure that offset values of controller are set to ZERO *\n');
        fprintf('*                                                            *\n');
        fprintf('* Hit any key to proceed the initialization                  *\n');
        fprintf('**************************************************************\n\n');
        pause;
        
        % pause the rtp application
        mlib('Write',simStateId,'Data',{1});
        
        % get displacements and resisting forces from board
        measDsp = mlib('Read',measDspId);
        measFrc = mlib('Read',measFrcId);
        
        fprintf('*********************************\n');
        fprintf('* Initial values of DAQ are:    *\n');
        fprintf('*                               *\n');
        fprintf('* measDisp = %+1.6E     *\n',measDsp);
        fprintf('* measForc = %+1.6E     *\n',measFrc);
        fprintf('*                               *\n');
        fprintf('* If okay, hit any key to start *\n');
        fprintf('*********************************\n\n');
        pause;
        
        % start the rtp application
        mlib('Write',simStateId,'Data',{2});
        
        % reset updateFlag and targDisp
        mlib('Write',updateFlagId,'Data',{-1});
        mlib('Write',targDspId,'Data',{targDsp});
        if strcmp(type,'DspVel') || strcmp(type,'DspVelAcc')
            mlib('Write',targVelId,'Data',{targVel});
        end
        if strcmp(type,'DspVelAcc')
            mlib('Write',targAccId,'Data',{targAcc});
        end
        
        fprintf('*****************\n');
        fprintf('* Running...... *\n');
        fprintf('*****************\n\n');
        
        varargout = {};
    %=======================================================================
    case 'execute'
        % send target displacements and set updateFlag
        mlib('Write',targDspId,'Data',{targDsp});
        if strcmp(type,'DspVel') || strcmp(type,'DspVelAcc')
            mlib('Write',targVelId,'Data',{targVel});
        end
        if strcmp(type,'DspVelAcc')
            mlib('Write',targAccId,'Data',{targAcc});
        end
        mlib('Write',updateFlagId,'Data',{1});
        
        varargout = {};
    %=======================================================================
    case 'acquire'
        targetFlag = 1;
        while targetFlag
            % get targetFlag
            pause(ptime);
            targetFlag = mlib('Read',targetFlagId);
        end
        
        % get displacements and resisting forces at target
        measDsp = mlib('Read',measDspId);
        measFrc = mlib('Read',measFrcId);
        
        % reset updateFlag
        mlib('Write',updateFlagId,'Data',{0});
        pause(ptime);
        
        varargout = {measDsp,measFrc};
    %=======================================================================
    case 'stop'
        % stop the rtp application
        mlib('Write',simStateId,'Data',{0});
        
        fprintf('****************************************\n');
        fprintf('* The rtp application has been stopped *\n');
        fprintf('****************************************\n\n');
        
        varargout = {};
    %=======================================================================
end
