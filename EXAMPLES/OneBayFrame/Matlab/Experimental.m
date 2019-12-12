function ElementPost = Experimental(u, ElementData)
%EXPERIMENTAL one dof experimental element
% ElementPost = Experimental(u, ElementData)
%
% This is a one degree of freedom experimental element.  It takes the tip
% displacement and returns Pr, the resisting force.
%
%                           [+]-->1 DOF
%                            |
%                            |
%                            |
%                            |
%                         --[+]--
%                         ///////

%/* ****************************************************************** **
%**    OpenFRESCO - Open Framework                                     **
%**                 for Experimental Setup and Control                 **
%**                                                                    **
%**                                                                    **
%** Copyright (c) 2006, The Regents of the University of California    **
%** All Rights Reserved.                                               **
%**                                                                    **
%** Commercial use of this program without express permission of the   **
%** University of California, Berkeley, is strictly prohibited. See    **
%** file 'COPYRIGHT_UCB' in main directory for information on usage    **
%** and redistribution, and for a DISCLAIMER OF ALL WARRANTIES.        **
%**                                                                    **
%** Developed by:                                                      **
%**   Andreas Schellenberg (andreas.schellenberg@gmail.com)            **
%**   Yoshikazu Takahashi (yos@catfish.dpri.kyoto-u.ac.jp)             **
%**   Gregory L. Fenves (fenves@berkeley.edu)                          **
%**   Stephen A. Mahin (mahin@berkeley.edu)                            **
%**                                                                    **
%** ****************************************************************** */

% Written: Hong Kim (hong_kim@berkeley.edu)
% Created: 10/06
% Revision: A

% Set Parameters
socketType = ElementData.socketType;  % Socket type: UDPSocket or TCPSocket
socketID   = ElementData.socketID;    % Socket ID
dataSize   = ElementData.dataSize;    % Size of send and receive vectors

% initialize send vector
sData = zeros(1,dataSize);

% send trial response to experimental site
sData(1) = 3;
sData(2) = u;
feval(socketType,'sendData',socketID,sData,dataSize);

% get measured resisting forces
sData(1) = 10;
feval(socketType,'sendData',socketID,sData,dataSize);
rData = feval(socketType,'recvData',socketID,dataSize);

% commit state
sData(1) = 5;
feval(socketType,'sendData',socketID,sData,dataSize);

% Set resisting force
ElementPost.p_r = rData(1);

ElementPost.v_pl = 0;
ElementPost.qb   = 0;
