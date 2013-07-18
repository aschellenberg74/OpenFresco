%UDPSOCKET to communicate between two processes through a UDP socket
% varargout = UDPSocket(action,varargin)
% 
% varargout : variable output argument list
%  varargout = socketID  for action 'openConnection'
%  varargout = rValue    for action 'closeConnection'
%  varargout = rValue    for action 'sendData'
%  varargout = data      for action 'recvData'
%  varargout = socketID  for action 'getSocketID'
%   socketID : socket identifier, or <0 if action failed
%   rValue   : 0 if action successful and <0 if action failed
%   data     : data vector that is requested from other side
%
% action : switch with following possible values
%  'openConnection'   open a connection to an experimental site
%  'closeConnection'  close the connection to an experimental site
%  'sendData'         send data to an experimental site
%  'recvData'         receive data from an experimental site
%  'getSocketID'      get socketID of an open connection
%
% varargin : variable input argument list
%  varargin = ipAddr, ipPort                  for client action 'openConnection'
%  varargin = ipPort                          for server action 'openConnection'
%  varargin = socketID                        for action 'closeConnection'
%  varargin = socketID, data, dataSize        for action 'sendData'
%  varargin = socketID, dataSize, <dataType>  for action 'recvData'
%  varargin = ipPort, ipAddr                  for action 'getSocketID'
%   ipAddr   : ip address of server
%   ipPort   : ip port of server
%   socketID : socket identifier created by action 'openConnection'
%   data     : data vector to be sent to the experimental site
%   dataSize : size of data vector to be received
%   dataType : type of data vector to be received
%              (optional, default: double)
%   
%
% Client Example:
%  1) setting up a connection to server
%       socketID = UDPSocket('openConnection','127.0.0.1',8090);
%  2) setting the data sizes for the experimental site
%       dataSize = 2;
%       sData = zeros(1,dataSize);
%       dataSizes = int32([1 0 0 0 0, 0 0 0 1 0, dataSize]);
%       UDPSocket('sendData',socketID,dataSizes,11);
%  3) sending trial response to experimental site
%       sData(1) = 3;
%       sData(2) = db;
%       UDPSocket('sendData',socketID,sData,dataSize);
%  4) getting measured resisting forces from experimental site
%       sData(1) = 10;
%       UDPSocket('sendData',socketID,sData,dataSize);
%       rData = UDPSocket('recvData',socketID,dataSize);
%  5) disconnecting from experimental site
%       sData(1) = 99;
%       UDPSocket('sendData',socketID,sData,dataSize);
%       UDPSocket('closeConnection',socketID);
%
%
%   See also TCPSOCKET, GENERICCLIENT2D, EETRUSS, EEFRAME2D, EEFRAME3D, EETWONODELINK2D

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

% $Revision$
% $Date$
% $URL$

% Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
% Created: 02/13
% Revision: A
