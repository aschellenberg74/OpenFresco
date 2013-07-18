function errorCode = RunOpenFresco(opfPath,tclFile,console)
%RUNOPENFRESCO to run OpenFresco and source the provided tcl file
% errorCode = RunOpenFresco(opfPath,tclFile,console)
%
% errorCode : error code returned by dos or system command
% opfPath   : path to OpenFresco.exe executable file
% tclFile   : tcl file (including path) to be sourced
% console   : flag to run iconically or in console
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

% write the command line for execution
if ispc
    command = ['@ "',fullfile(opfPath,'openSees.exe'),'" "',tclFile,'"'];
elseif ismac || isunix
    command = ['"',fullfile(opfPath,'OpenSees'),'" "',tclFile,'"'];
end

if (console == 1)
    % run OpenSees in console (this will load OpenFresco.dll)
    if ispc
        errorCode = dos([command,' & exit &']);
    elseif ismac || isunix
        errorCode = system(['xterm -e ',command,' &']);
    end
else
    % run OpenSees iconically (this will load OpenFresco.dll)
    if ispc
        errorCode = dos(command,'-echo');
    elseif ismac || isunix
        errorCode = system(['xterm -e ',command]);
    end    
end

% print error code to Matlab command prompt
if (errorCode ~= 0)
    fprintf(1,'error code = %d\n',errorCode);
end
