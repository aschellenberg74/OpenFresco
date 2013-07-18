function varargout = Element_ExpGeneric(action,ElmData,disp,vel)
%ELEMENT_EXPGENERIC to define an experimental generic element
% varargout = Element_ExpGeneric(action,ElmData,disp,vel)
%
% varargout : variable length output argument list
% action    : switch with following possible values
%              'initialize'         initialize internal variables
%              'setTrialDisp'       set the trial displacements
%              'getDisp'            get the current displacements
%              'getResistingForce'  get the current resisting forces
%              'getTangentStiff'    get the current tangent stiffness
%              'getInitialStiff'    get the initial stiffness
%              'commitState'        commit state of internal variables
%              'disconnect'         disconnect from experimental site
% ElmData   : data structure with element information
% disp      : trial displacement
% vel       : trial velocity
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

% socket identifier
persistent socketID;
% state variables
persistent dispT velT;
persistent rData getDaq;

% extract element properties
tag    = ElmData.tag;            % unique element tag
kInit  = ElmData.kInit;          % initial stiffness matrix
ipAddr = ElmData.ipAddr;         % ip-address of simAppSiteServer
ipPort = ElmData.ipPort;         % ip-port of simAppSiteServer
id     = ElmData.id;             % id of global element DOFs
ndf    = length(id);             % number of degrees of freedom
if isfield(ElmData,'dataSize')
    dataSize = ElmData.dataSize;  % size of send and receive vectors
else
    dataSize = 1 + 3*ndf;
end

% initialize send vector
sData = zeros(1,dataSize);

switch action
    % ======================================================================
    case 'initialize'
        if (length(socketID)<tag || socketID(tag)<=0)
            numTrials = 0;
            socketID(tag) = -1;
            while (numTrials<10 && socketID(tag)<0)
                try %#ok<TRYNC>
                    socketID(tag) = UDPSocket('openConnection',ipAddr,ipPort);
                    pause(1.0);
                    numTrials = numTrials+1;
                    if (numTrials==10)
                        continueTrial = questdlg(sprintf('The program cannot establish a connection to OpenFresco.\nContinue trying to connect?'),'Continue?','Yes','No','Yes');
                        if strcmp(continueTrial,'Yes')
                            numTrials = 0;
                        end
                    end
                end
            end
            if (socketID(tag)<0)
                error('UDPSocket:openConnection',['Unable to setup connection to ',...
                    ipAddr,' : ',num2str(ipPort)]);
            end
            
            % set the data size for the experimental site
            dataSizes = int32([ndf ndf ndf 0 0, ndf ndf 0 ndf 0, dataSize]);
            UDPSocket('sendData',socketID(tag),dataSizes,11);
            
            dispT(:,tag) = zeros(ndf,1);
            velT(:,tag) = zeros(ndf,1);
            
            % send trial response to experimental site
            sData(1) = 3;
            sData(2:ndf+1) = dispT(:,tag)';
            sData(ndf+2:2*ndf+1) = velT(:,tag)';
            UDPSocket('sendData',socketID(tag),sData,dataSize);
        end
        
        % get measured response
        sData(1) = 6;
        UDPSocket('sendData',socketID(tag),sData,dataSize);
        rData = UDPSocket('recvData',socketID(tag),dataSize);
        getDaq = 0;
        
        varargout = {rData(1:ndf)',rData(ndf+1:2*ndf)',rData(2*ndf+1:3*ndf)'};
    % ======================================================================
    case 'setTrialResp'
        dispT(:,tag) = disp;
        velT(:,tag) = vel;
        
        % send trial response to experimental site
        sData(1) = 3;
        sData(2:ndf+1) = dispT(:,tag)';
        sData(ndf+2:2*ndf+1) = velT(:,tag)';
        UDPSocket('sendData',socketID(tag),sData,dataSize);
        getDaq = 1;
        
        varargout = {0};
    % ======================================================================
    case 'getDaqResp'
        % get measured response
        if getDaq
            sData(1) = 6;
            UDPSocket('sendData',socketID(tag),sData,dataSize);
            rData = UDPSocket('recvData',socketID(tag),dataSize);
            getDaq = 0;
        end
        
        varargout = {rData(1:ndf)',rData(ndf+1:2*ndf)',rData(2*ndf+1:3*ndf)'};
    % ======================================================================
    case 'getDisp'
        % get measured displacements
        if getDaq
            sData(1) = 6;
            UDPSocket('sendData',socketID(tag),sData,dataSize);
            rData = UDPSocket('recvData',socketID(tag),dataSize);
            getDaq = 0;
        end
        
        varargout = {rData(1:ndf)'};
    % ======================================================================
    case 'getVel'
        % get measured velocities
        if getDaq
            sData(1) = 6;
            UDPSocket('sendData',socketID(tag),sData,dataSize);
            rData = UDPSocket('recvData',socketID(tag),dataSize);
            getDaq = 0;
        end
        
        varargout = {rData(ndf+1:2*ndf)'};
    % ======================================================================
    case 'getResistingForce'
        % get measured resisting forces
        if getDaq
            sData(1) = 6;
            UDPSocket('sendData',socketID(tag),sData,dataSize);
            rData = UDPSocket('recvData',socketID(tag),dataSize);
            getDaq = 0;
        end
        
        varargout = {rData(2*ndf+1:3*ndf)'};
    % ======================================================================
    case 'getTangentStiff'
        varargout = {kInit};
    % ======================================================================
    case 'getInitialStiff'
        varargout = {kInit};
    % ======================================================================
    case 'commitState'
        sData(1) = 5;
        UDPSocket('sendData',socketID(tag),sData,dataSize);
        
        varargout = {0};
    % ======================================================================
    case 'disconnect'
        if (socketID(tag)>0)
            sData(1) = 99;
            UDPSocket('sendData',socketID(tag),sData,dataSize);
            UDPSocket('closeConnection',socketID(tag));
            socketID(tag) = -1;
        end
        
        varargout = {0};
    % ======================================================================
end
