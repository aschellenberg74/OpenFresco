function errorCode = RunOpenFresco(opfPath,tclFile)
%RUNOPENFRESCO to run OpenFresco and source in the provided tcl file
% errorCode = RunOpenFresco(opfPath,tclFile)
%
% errorCode : error code returned by dos or system command
% opfPath   : path to OpenFresco.exe executable file
% tclFile   : tcl file (including path) to be sourced

% Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
% Created: 07/10
% Revision: A

% write the command line for execution
if ispc
   command = ['@ "',fullfile(opfPath,'openSees.exe'),'" "',tclFile,'"'];
elseif ismac || isunix
   command = ['"',fullfile(opfPath,'OpenSees'),'" "',tclFile,'"'];
end

% run OpenSees in console (this will load OpenFresco.dll) 
if ispc
   errorCode = dos([command,' & exit &']);
elseif ismac || isunix
   errorCode = system(['xterm -e ',command,' &']);
end

% print error code to Matlab command prompt
if (errorCode ~= 0)
   fprintf(1,'error code = %d\n',errorCode);
end
