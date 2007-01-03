%TCPSOCKET to communicate with an experimental site through a TCP/IP socket
% varargout = TCPSocket(action,varargin)
% 
% varargout : variable output argument list
%  varargout = socketID  for action 'openConnection'
%  varargout = socketID  for action 'getSocketID'
%  varargout = rValue    for action 'sendData'
%  varargout = data      for action 'recvData'
%  varargout = rValue    for action 'closeConnection'
%   socketID : socket identifier, or <0 if action failed
%   rValue   : 0 if action successful and <0 if action failed
%   data     : data vector that is requested from experimental site
%
% action : switch with following possible values
%  'openConnection'   open a connection to an experimental site
%  'getSocketID'      get socketID of an open connection
%  'sendData'         send data to an experimental site
%  'recvData'         receive data from an experimental site
%  'closeConnection'  close the connection to an experimental site
%
% varargin : variable input argument list
%  varargin = ipPort, <ipAddr>                for action 'openConnection'
%  varargin = ipPort, <ipAddr>                for action 'getSocketID'
%  varargin = socketID, data, dataSize        for action 'sendData'
%  varargin = socketID, dataSize, <dataType>  for action 'recvData'
%  varargin = socketID                        for action 'closeConnection'
%   ipPort   : ip port of simulation application server
%   ipAddr   : ip address of simulation application server
%              (optional, default: 127.0.0.1)
%   socketID : socket identifier created by action 'openConnection'
%   data     : data vector to be sent to the experimental site
%   dataSize : size of data vector to be received
%   dataType : type of data vector to be received
%              (optional, default: double)
%   
%
% Examples:
%  1) setting up a connection
%       socketID = TCPSocket('openConnection',8090);
%  2) setting the data sizes for the experimental site
%       dataSize = 2;
%       sData = zeros(1,dataSize);
%       dataSizes = int32([1 0 0 0 0, 0 0 0 1 0, dataSize]);
%       TCPSocket('sendData',socketID,dataSizes,11);
%  3) sending trial response to experimental site
%       sData(1) = 3;
%       sData(2) = db;
%       TCPSocket('sendData',socketID,sData,dataSize);
%  4) getting measured resisting forces from experimental site
%       sData(1) = 10;
%       TCPSocket('sendData',socketID,sData,dataSize);
%       rData = TCPSocket('recvData',socketID,dataSize);
%  5) disconnecting from experimental site
%       sData(1) = 99;
%       TCPSocket('sendData',socketID,sData,dataSize);
%       TCPSocket('closeConnection',socketID);
%
%
%   See also GENERICCLIENT2D, EETRUSS, EEFRAME2D, EEFRAME3D, EEZEROLENGTH2D

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
%**   Andreas Schellenberg (andreas.schellenberg@gmx.net)              **
%**   Yoshikazu Takahashi (yos@catfish.dpri.kyoto-u.ac.jp)             **
%**   Gregory L. Fenves (fenves@berkeley.edu)                          **
%**   Stephen A. Mahin (mahin@berkeley.edu)                            **
%**                                                                    **
%** ****************************************************************** */

% $Revision: $
% $Date: $
% $URL: $

% Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
% Created: 10/06
% Revision: A
