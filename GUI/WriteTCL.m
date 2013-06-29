function WriteTCL(varargin)
%WRITETCL to write the necessary TCL file based on user inputs
% WriteTCL(varargin)
%
% varargin : variable length input argument list
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

% initialization tasks
handles = guidata(gcbf);

% check the required input data
[error,message] = InputCheck;
if error
    msgbox(message,'Error','error');
    return
end

% get the path and open file for writing
DIR = which('OPFE_Version.txt');
DIR = fileparts(DIR);
FID = fopen(fullfile(DIR,'OPFE_Analysis.tcl'),'w');
ver = fgetl(fopen('OPFE_Version.txt'));

% print header
fprintf(FID,'#==========================================================================\n');
fprintf(FID,'# OpenFresco Express v%s                             %s\n',ver,datestr(now));
fprintf(FID,'#==========================================================================\n');
fprintf(FID,'# File   : OPFE_Analysis.tcl\n');
fprintf(FID,'# Purpose: Tcl script file for use with OpenFresco Express\n\n');

switch handles.Model.Type
    % =====================================================================
    case '1 DOF'
        % start of model generation
        fprintf(FID,'# ------------------------------\n# Start of model generation\n# ------------------------------\n# create ModelBuilder (with two-dimensions and 1 DOF/node)\n');
        fprintf(FID,'model BasicBuilder -ndm 2 -ndf 1\n\n');
        fprintf(FID,'# Load OpenFresco package\n# -----------------------\n');
        fprintf(FID,'loadPackage OpenFresco\n\n');
        % define geometry
        %fprintf(FID,'# Define geometry for model\n# -------------------------\n# node $tag $xCrd $yCrd $mass\n');
        %fprintf(FID,'node  1     0.0   0.0\nnode  2     0.0   1.0  -mass  %+1.6E  0.0\n',handles.Model.M);
        %fprintf(FID,'fix   1      1     1\nfix   2      0     1\n\n');
        
        switch handles.ExpControl.Type
            % -------------------------------------------------------------
            case 'Simulation'
                % define materials
                fprintf(FID,'# Define materials\n# ----------------\n');
                switch handles.ExpControl.DOF1.SimMaterial
                    case 'Elastic'
                        fprintf(FID,'uniaxialMaterial Elastic 1 %+1.6E\n\n',handles.ExpControl.DOF1.E);
                    case 'Elastic-Perfectly Plastic'
                        fprintf(FID,'uniaxialMaterial ElasticPP 1 %+1.6E %+1.6E\n\n',handles.ExpControl.DOF1.E,handles.ExpControl.DOF1.epsP);
                    case 'Steel - Bilinear'
                        fprintf(FID,'uniaxialMaterial Steel01 1 %+1.6E %+1.6E %+1.6E\n\n',handles.ExpControl.DOF1.Fy,handles.ExpControl.DOF1.E,handles.ExpControl.DOF1.b);
                    case 'Steel - Giuffré-Menegotto-Pinto'
                        fprintf(FID,'uniaxialMaterial Steel02 1 %+1.6E %+1.6E %+1.6E %+1.6E %+1.6E %+1.6E\n\n',handles.ExpControl.DOF1.Fy,handles.ExpControl.DOF1.E,handles.ExpControl.DOF1.b,handles.ExpControl.DOF1.R0, handles.ExpControl.DOF1.cR1, handles.ExpControl.DOF1.cR2);
                end
                % define experimental control
                fprintf(FID,'# Define experimental control\n# ---------------------------\n');
                fprintf(FID,'expControl %s 1 1\n\n',handles.ExpControl.SimControl.SimType);
            % -------------------------------------------------------------
            case 'Real'
                % define experimental control
                switch handles.ExpControl.RealControl.Controller
                    case 'LabVIEW'
                        % define experimental control points
                        fprintf(FID,'# Define control points\n# ---------------------\n');
                        fprintf(FID,'# expControlPoint tag nodeTag dir resp <-fact f> <-lim l u> ...\n');
                        j = 1;
                        for i = 1:length(handles.ExpControl.RealControl.TrialCP)
                            id1 = handles.ExpControl.RealControl.TrialCP(i);
                            id2 = handles.ExpControl.RealControl.OutputCP(i);
                            switch handles.ExpControl.CP.NumResp{id1}
                                case 1
                                    fprintf(FID,'expControlPoint %1.0f %1.0f  %s %s -fact %+1.6E -lim %+1.6E %+1.6E\n',j,handles.ExpControl.CP.Node{id1},lower(handles.ExpControl.CP.Dir{1,id1}),handles.ExpControl.CP.Resp{1,id1},handles.ExpControl.CP.F{1,id1},handles.ExpControl.CP.LimL{1,id1},handles.ExpControl.CP.LimU{1,id1});
                                case 2
                                    fprintf(FID,'expControlPoint %1.0f %1.0f  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E\n',j,handles.ExpControl.CP.Node{id1},lower(handles.ExpControl.CP.Dir{1,id1}),handles.ExpControl.CP.Resp{1,id1},handles.ExpControl.CP.F{1,id1},handles.ExpControl.CP.LimL{1,id1},handles.ExpControl.CP.LimU{1,id1},...
                                        lower(handles.ExpControl.CP.Dir{2,id1}),handles.ExpControl.CP.Resp{2,id1},handles.ExpControl.CP.F{2,id1},handles.ExpControl.CP.LimL{2,id1},handles.ExpControl.CP.LimU{2,id1});
                                case 3
                                    fprintf(FID,'expControlPoint %1.0f %1.0f  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E\n',...
                                        j,handles.ExpControl.CP.Node{id1},lower(handles.ExpControl.CP.Dir{1,id1}),handles.ExpControl.CP.Resp{1,id1},handles.ExpControl.CP.F{1,id1},handles.ExpControl.CP.LimL{1,id1},handles.ExpControl.CP.LimU{1,id1},...
                                        lower(handles.ExpControl.CP.Dir{2,id1}),handles.ExpControl.CP.Resp{2,id1},handles.ExpControl.CP.F{2,id1},handles.ExpControl.CP.LimL{2,id1},handles.ExpControl.CP.LimU{2,id1},...
                                        lower(handles.ExpControl.CP.Dir{3,id1}),handles.ExpControl.CP.Resp{3,id1},handles.ExpControl.CP.F{3,id1},handles.ExpControl.CP.LimL{3,id1},handles.ExpControl.CP.LimU{3,id1});
                                case 4
                                    fprintf(FID,'expControlPoint %1.0f %1.0f  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E\n',...
                                        j,handles.ExpControl.CP.Node{id1},lower(handles.ExpControl.CP.Dir{1,id1}),handles.ExpControl.CP.Resp{1,id1},handles.ExpControl.CP.F{1,id1},handles.ExpControl.CP.LimL{1,id1},handles.ExpControl.CP.LimU{1,id1},...
                                        lower(handles.ExpControl.CP.Dir{2,id1}),handles.ExpControl.CP.Resp{2,id1},handles.ExpControl.CP.F{2,id1},handles.ExpControl.CP.LimL{2,id1},handles.ExpControl.CP.LimU{2,id1},...
                                        lower(handles.ExpControl.CP.Dir{3,id1}),handles.ExpControl.CP.Resp{3,id1},handles.ExpControl.CP.F{3,id1},handles.ExpControl.CP.LimL{3,id1},handles.ExpControl.CP.LimU{3,id1},...
                                        lower(handles.ExpControl.CP.Dir{4,id1}),handles.ExpControl.CP.Resp{4,id1},handles.ExpControl.CP.F{4,id1},handles.ExpControl.CP.LimL{4,id1},handles.ExpControl.CP.LimU{4,id1});
                                case 5
                                    fprintf(FID,'expControlPoint %1.0f %1.0f  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E\n',...
                                        j,handles.ExpControl.CP.Node{id1},lower(handles.ExpControl.CP.Dir{1,id1}),handles.ExpControl.CP.Resp{1,id1},handles.ExpControl.CP.F{1,id1},handles.ExpControl.CP.LimL{1,id1},handles.ExpControl.CP.LimU{1,id1},...
                                        lower(handles.ExpControl.CP.Dir{2,id1}),handles.ExpControl.CP.Resp{2,id1},handles.ExpControl.CP.F{2,id1},handles.ExpControl.CP.LimL{2,id1},handles.ExpControl.CP.LimU{2,id1},...
                                        lower(handles.ExpControl.CP.Dir{3,id1}),handles.ExpControl.CP.Resp{3,id1},handles.ExpControl.CP.F{3,id1},handles.ExpControl.CP.LimL{3,id1},handles.ExpControl.CP.LimU{3,id1},...
                                        lower(handles.ExpControl.CP.Dir{4,id1}),handles.ExpControl.CP.Resp{4,id1},handles.ExpControl.CP.F{4,id1},handles.ExpControl.CP.LimL{4,id1},handles.ExpControl.CP.LimU{4,id1},...
                                        lower(handles.ExpControl.CP.Dir{5,id1}),handles.ExpControl.CP.Resp{5,id1},handles.ExpControl.CP.F{5,id1},handles.ExpControl.CP.LimL{5,id1},handles.ExpControl.CP.LimU{5,id1});
                                case 6
                                    fprintf(FID,'expControlPoint %1.0f %1.0f  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E\n',...
                                        j,handles.ExpControl.CP.Node{id1},lower(handles.ExpControl.CP.Dir{1,id1}),handles.ExpControl.CP.Resp{1,id1},handles.ExpControl.CP.F{1,id1},handles.ExpControl.CP.LimL{1,id1},handles.ExpControl.CP.LimU{1,id1},...
                                        lower(handles.ExpControl.CP.Dir{2,id1}),handles.ExpControl.CP.Resp{2,id1},handles.ExpControl.CP.F{2,id1},handles.ExpControl.CP.LimL{2,id1},handles.ExpControl.CP.LimU{2,id1},...
                                        lower(handles.ExpControl.CP.Dir{3,id1}),handles.ExpControl.CP.Resp{3,id1},handles.ExpControl.CP.F{3,id1},handles.ExpControl.CP.LimL{3,id1},handles.ExpControl.CP.LimU{3,id1},...
                                        lower(handles.ExpControl.CP.Dir{4,id1}),handles.ExpControl.CP.Resp{4,id1},handles.ExpControl.CP.F{4,id1},handles.ExpControl.CP.LimL{4,id1},handles.ExpControl.CP.LimU{4,id1},...
                                        lower(handles.ExpControl.CP.Dir{5,id1}),handles.ExpControl.CP.Resp{5,id1},handles.ExpControl.CP.F{5,id1},handles.ExpControl.CP.LimL{5,id1},handles.ExpControl.CP.LimU{5,id1},...
                                        lower(handles.ExpControl.CP.Dir{6,id1}),handles.ExpControl.CP.Resp{6,id1},handles.ExpControl.CP.F{6,id1},handles.ExpControl.CP.LimL{6,id1},handles.ExpControl.CP.LimU{6,id1});
                            end
                            j = j+1;
                            switch handles.ExpControl.CP.NumResp{id2}
                                case 1
                                    fprintf(FID,'expControlPoint %1.0f %1.0f  %s %s -fact %+1.6E -lim %+1.6E %+1.6E\n\n',j,handles.ExpControl.CP.Node{id2},lower(handles.ExpControl.CP.Dir{1,id2}),handles.ExpControl.CP.Resp{1,id2},handles.ExpControl.CP.F{1,id2},handles.ExpControl.CP.LimL{1,id2},handles.ExpControl.CP.LimU{1,id2});
                                case 2
                                    fprintf(FID,'expControlPoint %1.0f %1.0f  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E\n\n',j,handles.ExpControl.CP.Node{id2},lower(handles.ExpControl.CP.Dir{1,id2}),handles.ExpControl.CP.Resp{1,id2},handles.ExpControl.CP.F{1,id2},handles.ExpControl.CP.LimL{1,id2},handles.ExpControl.CP.LimU{1,id2},...
                                        lower(handles.ExpControl.CP.Dir{2,id2}),handles.ExpControl.CP.Resp{2,id2},handles.ExpControl.CP.F{2,id2},handles.ExpControl.CP.LimL{2,id2},handles.ExpControl.CP.LimU{2,id2});
                                case 3
                                    fprintf(FID,'expControlPoint %1.0f %1.0f  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E\n\n',...
                                        j,handles.ExpControl.CP.Node{id2},lower(handles.ExpControl.CP.Dir{1,id2}),handles.ExpControl.CP.Resp{1,id2},handles.ExpControl.CP.F{1,id2},handles.ExpControl.CP.LimL{1,id2},handles.ExpControl.CP.LimU{1,id2},...
                                        lower(handles.ExpControl.CP.Dir{2,id2}),handles.ExpControl.CP.Resp{2,id2},handles.ExpControl.CP.F{2,id2},handles.ExpControl.CP.LimL{2,id2},handles.ExpControl.CP.LimU{2,id2},...
                                        lower(handles.ExpControl.CP.Dir{3,id2}),handles.ExpControl.CP.Resp{3,id2},handles.ExpControl.CP.F{3,id2},handles.ExpControl.CP.LimL{3,id2},handles.ExpControl.CP.LimU{3,id2});
                                case 4
                                    fprintf(FID,'expControlPoint %1.0f %1.0f  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E\n\n',...
                                        j,handles.ExpControl.CP.Node{id2},lower(handles.ExpControl.CP.Dir{1,id2}),handles.ExpControl.CP.Resp{1,id2},handles.ExpControl.CP.F{1,id2},handles.ExpControl.CP.LimL{1,id2},handles.ExpControl.CP.LimU{1,id2},...
                                        lower(handles.ExpControl.CP.Dir{2,id2}),handles.ExpControl.CP.Resp{2,id2},handles.ExpControl.CP.F{2,id2},handles.ExpControl.CP.LimL{2,id2},handles.ExpControl.CP.LimU{2,id2},...
                                        lower(handles.ExpControl.CP.Dir{3,id2}),handles.ExpControl.CP.Resp{3,id2},handles.ExpControl.CP.F{3,id2},handles.ExpControl.CP.LimL{3,id2},handles.ExpControl.CP.LimU{3,id2},...
                                        lower(handles.ExpControl.CP.Dir{4,id2}),handles.ExpControl.CP.Resp{4,id2},handles.ExpControl.CP.F{4,id2},handles.ExpControl.CP.LimL{4,id2},handles.ExpControl.CP.LimU{4,id2});
                                case 5
                                    fprintf(FID,'expControlPoint %1.0f %1.0f  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E\n\n',...
                                        j,handles.ExpControl.CP.Node{id2},lower(handles.ExpControl.CP.Dir{1,id2}),handles.ExpControl.CP.Resp{1,id2},handles.ExpControl.CP.F{1,id2},handles.ExpControl.CP.LimL{1,id2},handles.ExpControl.CP.LimU{1,id2},...
                                        lower(handles.ExpControl.CP.Dir{2,id2}),handles.ExpControl.CP.Resp{2,id2},handles.ExpControl.CP.F{2,id2},handles.ExpControl.CP.LimL{2,id2},handles.ExpControl.CP.LimU{2,id2},...
                                        lower(handles.ExpControl.CP.Dir{3,id2}),handles.ExpControl.CP.Resp{3,id2},handles.ExpControl.CP.F{3,id2},handles.ExpControl.CP.LimL{3,id2},handles.ExpControl.CP.LimU{3,id2},...
                                        lower(handles.ExpControl.CP.Dir{4,id2}),handles.ExpControl.CP.Resp{4,id2},handles.ExpControl.CP.F{4,id2},handles.ExpControl.CP.LimL{4,id2},handles.ExpControl.CP.LimU{4,id2},...
                                        lower(handles.ExpControl.CP.Dir{5,id2}),handles.ExpControl.CP.Resp{5,id2},handles.ExpControl.CP.F{5,id2},handles.ExpControl.CP.LimL{5,id2},handles.ExpControl.CP.LimU{5,id2});
                                case 6
                                    fprintf(FID,'expControlPoint %1.0f %1.0f  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E\n\n',...
                                        j,handles.ExpControl.CP.Node{id2},lower(handles.ExpControl.CP.Dir{1,id2}),handles.ExpControl.CP.Resp{1,id2},handles.ExpControl.CP.F{1,id2},handles.ExpControl.CP.LimL{1,id2},handles.ExpControl.CP.LimU{1,id2},...
                                        lower(handles.ExpControl.CP.Dir{2,id2}),handles.ExpControl.CP.Resp{2,id2},handles.ExpControl.CP.F{2,id2},handles.ExpControl.CP.LimL{2,id2},handles.ExpControl.CP.LimU{2,id2},...
                                        lower(handles.ExpControl.CP.Dir{3,id2}),handles.ExpControl.CP.Resp{3,id2},handles.ExpControl.CP.F{3,id2},handles.ExpControl.CP.LimL{3,id2},handles.ExpControl.CP.LimU{3,id2},...
                                        lower(handles.ExpControl.CP.Dir{4,id2}),handles.ExpControl.CP.Resp{4,id2},handles.ExpControl.CP.F{4,id2},handles.ExpControl.CP.LimL{4,id2},handles.ExpControl.CP.LimU{4,id2},...
                                        lower(handles.ExpControl.CP.Dir{5,id2}),handles.ExpControl.CP.Resp{5,id2},handles.ExpControl.CP.F{5,id2},handles.ExpControl.CP.LimL{5,id2},handles.ExpControl.CP.LimU{5,id2},...
                                        lower(handles.ExpControl.CP.Dir{6,id2}),handles.ExpControl.CP.Resp{6,id2},handles.ExpControl.CP.F{6,id2},handles.ExpControl.CP.LimL{6,id2},handles.ExpControl.CP.LimU{6,id2});
                            end
                            j = j+1;
                        end
                        %fprintf(FID,'expControlPoint 1 1  ux disp -fact 1.0 -lim -7.5 7.5\n');
                        %fprintf(FID,'expControlPoint 2 1  ux disp -fact 1.0 -lim -7.5 7.5  ux force -fact 1.0 -lim -12.0 12.0\n\n');
                        
                        fprintf(FID,'# Define experimental control\n# ---------------------------\n');
                        fprintf(FID,'expControl %s 1 "%s" %s\n\n',handles.ExpControl.RealControl.Controller,handles.ExpControl.RealControl.ipAddr,handles.ExpControl.RealControl.ipPort);
                    case 'MTSCsi'
                        fullpath = fullfile(handles.ExpControl.RealControl.ConfigPath,strcat(handles.ExpControl.RealControl.ConfigName,handles.ExpControl.RealControl.ConfigType));
                        id = strfind(fullpath,filesep);
                        fullpath(id) = '/';
                        fprintf(FID,'# Define experimental control\n# ---------------------------\n');
                        fprintf(FID,'expControl %s 1 "%s" %4.3g\n\n',handles.ExpControl.RealControl.Controller,fullpath,handles.ExpControl.RealControl.rampTime);
                    case 'SCRAMNet'
                        fprintf(FID,'# Define experimental control\n# ---------------------------\n');
                        fprintf(FID,'expControl %s 1 %6.0f %2.0g\n\n',handles.ExpControl.RealControl.Controller,handles.ExpControl.RealControl.memOffset,handles.ExpControl.RealControl.NumActCh);
                    case 'dSpace'
                        fprintf(FID,'# Define experimental control\n# ---------------------------\n');
                        fprintf(FID,'expControl %s 1 %1.0f %s\n\n',handles.ExpControl.RealControl.Controller,handles.ExpControl.RealControl.PCvalue,handles.ExpControl.RealControl.boardName);
                    case 'xPCtarget'
                        appPath = handles.ExpControl.RealControl.appPath;
                        id = strfind(appPath,filesep);
                        appPath(id) = '/';
                        fprintf(FID,'# Define experimental control\n# ---------------------------\n');
                        fprintf(FID,'expControl %s 1 %1.0f "%s" %s %s "%s"\n\n',handles.ExpControl.RealControl.Controller,handles.ExpControl.RealControl.PCvalue,handles.ExpControl.RealControl.ipAddr,handles.ExpControl.RealControl.ipPort,handles.ExpControl.RealControl.appName,appPath(1:end-1));
                end
            % -------------------------------------------------------------
        end
        
        % define experimental setup
        fprintf(FID,'# Define experimental setup\n# -------------------------\n');
        fprintf(FID,'expSetup NoTransformation 1 -control 1 -dir 1 -sizeTrialOut 1 1\n\n');
        
        % define experimental site
        fprintf(FID,'# Define experimental site\n# ------------------------\n');
        fprintf(FID,'expSite LocalSite 1 1\n\n');
        
        % define experimental element
        %fprintf(FID,'# Define experimental element\n# ---------------------------\n');
        %fprintf(FID,'expElement generic 1 -node 2 -dof 1 -site 1 -initStif %4.0f\n',handles.Model.K);
        fprintf(FID,'# ------------------------------\n# End of model generation\n# ------------------------------\n\n\n');
        
        % start of recorder generation
        %fprintf(FID,'# ------------------------------\n# Start of recorder generation\n# ------------------------------\n# create the recorder objects\n');
        %fprintf(FID,'recorder Node -file NodeDsp.out -time -node 2 -dof 1 disp\n');
        %fprintf(FID,'recorder Node -file NodeVel.out -time -node 2 -dof 1 vel\n');
        %fprintf(FID,'recorder Node -file NodeAcc.out -time -node 2 -dof 1 accel\n\n');
        %fprintf(FID,'recorder Element -file ElmtFrc.out -time -ele 1 forces\n\n');
        %fprintf(FID,'expRecorder Control -file Control_ctrlDsp.out -time -control 1 ctrlDisp\n');
        %fprintf(FID,'expRecorder Control -file Control_DaqDsp.out -time -control 1 daqDisp\n');
        %fprintf(FID,'expRecorder Control -file Control_DaqFrc.out -time -control 1 daqForce\n');
        %fprintf(FID,'# --------------------------------\n# End of recorder generation\n# --------------------------------\n\n\n\n');
        
        % start the server process
        fprintf(FID,'# ------------------------------\n# Start the server process\n# ------------------------------\n');
        fprintf(FID,'# startSimAppSiteServer $siteTag $port <-ssl>\nstartSimAppSiteServer 1 7777\nexit\n');
        fprintf(FID,'# ------------------------------\n# End of analysis\n# ------------------------------\n');
        
    % =====================================================================
    case '2 DOF A'
        % start of model generation
        fprintf(FID,'# ------------------------------\n# Start of model generation\n# ------------------------------\n# create ModelBuilder (with two-dimensions and 2 DOF/node)\n');
        fprintf(FID,'model BasicBuilder -ndm 2 -ndf 2\n\n');
        fprintf(FID,'# Load OpenFresco package\n# -----------------------\n');
        fprintf(FID,'loadPackage OpenFresco\n\n');
        
        switch handles.ExpControl.Type
            % -------------------------------------------------------------
            case 'Simulation'
                % define geometry
                fprintf(FID,'# Define geometry for model\n# -------------------------\n# node $tag $xCrd $yCrd $mass\n');
                fprintf(FID,'node  1     0.0   0.0\nnode  2     0.0   0.0  -mass  %+1.6E  0.0\n',handles.Model.M(1,1));
                fprintf(FID,'node  3     0.0   0.0  -mass  %+1.6E  0.0\n\n',handles.Model.M(2,2));
                fprintf(FID,'# set the boundary conditions\n# fix $tag $DX $DY\n');
                fprintf(FID,'fix   1      1     1\nfix   2      0     1\nfix   3      0     1\n\n');
                
                % define materials
                fprintf(FID,'# Define materials\n# ----------------\n');
                switch handles.ExpControl.DOF1.SimMaterial
                    case 'Elastic'
                        fprintf(FID,'uniaxialMaterial Elastic 1 %+1.6E\n',handles.ExpControl.DOF1.E);
                    case 'Elastic-Perfectly Plastic'
                        fprintf(FID,'uniaxialMaterial ElasticPP 1 %+1.6E %+1.6E\n',handles.ExpControl.DOF1.E,handles.ExpControl.DOF1.epsP);
                    case 'Steel - Bilinear'
                        fprintf(FID,'uniaxialMaterial Steel01 1 %+1.6E %+1.6E %+1.6E\n',handles.ExpControl.DOF1.Fy,handles.ExpControl.DOF1.E,handles.ExpControl.DOF1.b);
                    case 'Steel - Giuffré-Menegotto-Pinto'
                        fprintf(FID,'uniaxialMaterial Steel02 1 %+1.6E %+1.6E %+1.6E %+1.6E %+1.6E %+1.6E\n',handles.ExpControl.DOF1.Fy,handles.ExpControl.DOF1.E,handles.ExpControl.DOF1.b,handles.ExpControl.DOF1.R0, handles.ExpControl.DOF1.cR1, handles.ExpControl.DOF1.cR2);
                end
                switch handles.ExpControl.DOF2.SimMaterial
                    case 'Elastic'
                        fprintf(FID,'uniaxialMaterial Elastic 2 %+1.6E\n\n',handles.ExpControl.DOF2.E);
                    case 'Elastic-Perfectly Plastic'
                        fprintf(FID,'uniaxialMaterial ElasticPP 2 %+1.6E %+1.6E\n\n',handles.ExpControl.DOF2.E,handles.ExpControl.DOF2.epsP);
                    case 'Steel - Bilinear'
                        fprintf(FID,'uniaxialMaterial Steel01 2 %+1.6E %+1.6E %+1.6E\n\n',handles.ExpControl.DOF2.Fy,handles.ExpControl.DOF2.E,handles.ExpControl.DOF2.b);
                    case 'Steel - Giuffré-Menegotto-Pinto'
                        fprintf(FID,'uniaxialMaterial Steel02 2 %+1.6E %+1.6E %+1.6E %+1.6E %+1.6E %+1.6E\n\n',handles.ExpControl.DOF2.Fy,handles.ExpControl.DOF2.E,handles.ExpControl.DOF2.b,handles.ExpControl.DOF2.R0, handles.ExpControl.DOF2.cR1, handles.ExpControl.DOF2.cR2);
                end
                
                % define elements
                fprintf(FID,'# Define elements\n# ---------------\n# element zeroLength $eleTag $iNode $jNode -mat $matTags -dir $dirs <-orient $x1 $x2 $x3 $y1 $y2 $y3>\n');
                fprintf(FID,'element zeroLength 1 1 2 -mat 1 -dir 1\nelement zeroLength 2 2 3 -mat 2 -dir 1\n\n');
                
                % define experimental control points
                fprintf(FID,'# Define control points\n# ---------------------\n');
                fprintf(FID,'# expControlPoint tag nodeTag dir resp <-fact f> <-lim l u> ...\n');
                fprintf(FID,'expControlPoint 1 2  ux disp\n');
                fprintf(FID,'expControlPoint 2 2  ux disp  ux force\n');
                fprintf(FID,'expControlPoint 3 3  ux disp\n');
                fprintf(FID,'expControlPoint 4 3  ux disp  ux force\n\n');
                
                % define experimental control
                fprintf(FID,'# Define experimental control\n# ---------------------------\n');
                %fprintf(FID,'expControl %s 1 1\n\n',handles.ExpControl.SimControl.SimType);
                % SimDomain case
                fprintf(FID,'# expControl SimDomain $tag -trialCP cpTags -outCP cpTags\n');
                fprintf(FID,'expControl SimDomain 1 -trialCP 1 3 -outCP 2 4\n\n');
            % -------------------------------------------------------------
            case 'Real'
                % define experimental control
                switch handles.ExpControl.RealControl.Controller
                    case 'LabVIEW'
                        % define experimental control points
                        fprintf(FID,'# Define control points\n# ---------------------\n');
                        fprintf(FID,'# expControlPoint tag nodeTag dir resp <-fact f> <-lim l u> ...\n');
                        j = 1;
                        for i = 1:length(handles.ExpControl.RealControl.TrialCP)
                            id1 = handles.ExpControl.RealControl.TrialCP(i);
                            id2 = handles.ExpControl.RealControl.OutputCP(i);
                            switch handles.ExpControl.CP.NumResp{id1}
                                case 1
                                    fprintf(FID,'expControlPoint %1.0f %1.0f  %s %s -fact %+1.6E -lim %+1.6E %+1.6E\n',j,handles.ExpControl.CP.Node{id1},lower(handles.ExpControl.CP.Dir{1,id1}),handles.ExpControl.CP.Resp{1,id1},handles.ExpControl.CP.F{1,id1},handles.ExpControl.CP.LimL{1,id1},handles.ExpControl.CP.LimU{1,id1});
                                case 2
                                    fprintf(FID,'expControlPoint %1.0f %1.0f  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E\n',j,handles.ExpControl.CP.Node{id1},lower(handles.ExpControl.CP.Dir{1,id1}),handles.ExpControl.CP.Resp{1,id1},handles.ExpControl.CP.F{1,id1},handles.ExpControl.CP.LimL{1,id1},handles.ExpControl.CP.LimU{1,id1},...
                                        lower(handles.ExpControl.CP.Dir{2,id1}),handles.ExpControl.CP.Resp{2,id1},handles.ExpControl.CP.F{2,id1},handles.ExpControl.CP.LimL{2,id1},handles.ExpControl.CP.LimU{2,id1});
                                case 3
                                    fprintf(FID,'expControlPoint %1.0f %1.0f  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E\n',...
                                        j,handles.ExpControl.CP.Node{id1},lower(handles.ExpControl.CP.Dir{1,id1}),handles.ExpControl.CP.Resp{1,id1},handles.ExpControl.CP.F{1,id1},handles.ExpControl.CP.LimL{1,id1},handles.ExpControl.CP.LimU{1,id1},...
                                        lower(handles.ExpControl.CP.Dir{2,id1}),handles.ExpControl.CP.Resp{2,id1},handles.ExpControl.CP.F{2,id1},handles.ExpControl.CP.LimL{2,id1},handles.ExpControl.CP.LimU{2,id1},...
                                        lower(handles.ExpControl.CP.Dir{3,id1}),handles.ExpControl.CP.Resp{3,id1},handles.ExpControl.CP.F{3,id1},handles.ExpControl.CP.LimL{3,id1},handles.ExpControl.CP.LimU{3,id1});
                                case 4
                                    fprintf(FID,'expControlPoint %1.0f %1.0f  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E\n',...
                                        j,handles.ExpControl.CP.Node{id1},lower(handles.ExpControl.CP.Dir{1,id1}),handles.ExpControl.CP.Resp{1,id1},handles.ExpControl.CP.F{1,id1},handles.ExpControl.CP.LimL{1,id1},handles.ExpControl.CP.LimU{1,id1},...
                                        lower(handles.ExpControl.CP.Dir{2,id1}),handles.ExpControl.CP.Resp{2,id1},handles.ExpControl.CP.F{2,id1},handles.ExpControl.CP.LimL{2,id1},handles.ExpControl.CP.LimU{2,id1},...
                                        lower(handles.ExpControl.CP.Dir{3,id1}),handles.ExpControl.CP.Resp{3,id1},handles.ExpControl.CP.F{3,id1},handles.ExpControl.CP.LimL{3,id1},handles.ExpControl.CP.LimU{3,id1},...
                                        lower(handles.ExpControl.CP.Dir{4,id1}),handles.ExpControl.CP.Resp{4,id1},handles.ExpControl.CP.F{4,id1},handles.ExpControl.CP.LimL{4,id1},handles.ExpControl.CP.LimU{4,id1});
                                case 5
                                    fprintf(FID,'expControlPoint %1.0f %1.0f  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E\n',...
                                        j,handles.ExpControl.CP.Node{id1},lower(handles.ExpControl.CP.Dir{1,id1}),handles.ExpControl.CP.Resp{1,id1},handles.ExpControl.CP.F{1,id1},handles.ExpControl.CP.LimL{1,id1},handles.ExpControl.CP.LimU{1,id1},...
                                        lower(handles.ExpControl.CP.Dir{2,id1}),handles.ExpControl.CP.Resp{2,id1},handles.ExpControl.CP.F{2,id1},handles.ExpControl.CP.LimL{2,id1},handles.ExpControl.CP.LimU{2,id1},...
                                        lower(handles.ExpControl.CP.Dir{3,id1}),handles.ExpControl.CP.Resp{3,id1},handles.ExpControl.CP.F{3,id1},handles.ExpControl.CP.LimL{3,id1},handles.ExpControl.CP.LimU{3,id1},...
                                        lower(handles.ExpControl.CP.Dir{4,id1}),handles.ExpControl.CP.Resp{4,id1},handles.ExpControl.CP.F{4,id1},handles.ExpControl.CP.LimL{4,id1},handles.ExpControl.CP.LimU{4,id1},...
                                        lower(handles.ExpControl.CP.Dir{5,id1}),handles.ExpControl.CP.Resp{5,id1},handles.ExpControl.CP.F{5,id1},handles.ExpControl.CP.LimL{5,id1},handles.ExpControl.CP.LimU{5,id1});
                                case 6
                                    fprintf(FID,'expControlPoint %1.0f %1.0f  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E\n',...
                                        j,handles.ExpControl.CP.Node{id1},lower(handles.ExpControl.CP.Dir{1,id1}),handles.ExpControl.CP.Resp{1,id1},handles.ExpControl.CP.F{1,id1},handles.ExpControl.CP.LimL{1,id1},handles.ExpControl.CP.LimU{1,id1},...
                                        lower(handles.ExpControl.CP.Dir{2,id1}),handles.ExpControl.CP.Resp{2,id1},handles.ExpControl.CP.F{2,id1},handles.ExpControl.CP.LimL{2,id1},handles.ExpControl.CP.LimU{2,id1},...
                                        lower(handles.ExpControl.CP.Dir{3,id1}),handles.ExpControl.CP.Resp{3,id1},handles.ExpControl.CP.F{3,id1},handles.ExpControl.CP.LimL{3,id1},handles.ExpControl.CP.LimU{3,id1},...
                                        lower(handles.ExpControl.CP.Dir{4,id1}),handles.ExpControl.CP.Resp{4,id1},handles.ExpControl.CP.F{4,id1},handles.ExpControl.CP.LimL{4,id1},handles.ExpControl.CP.LimU{4,id1},...
                                        lower(handles.ExpControl.CP.Dir{5,id1}),handles.ExpControl.CP.Resp{5,id1},handles.ExpControl.CP.F{5,id1},handles.ExpControl.CP.LimL{5,id1},handles.ExpControl.CP.LimU{5,id1},...
                                        lower(handles.ExpControl.CP.Dir{6,id1}),handles.ExpControl.CP.Resp{6,id1},handles.ExpControl.CP.F{6,id1},handles.ExpControl.CP.LimL{6,id1},handles.ExpControl.CP.LimU{6,id1});
                            end
                            j = j+1;
                            switch handles.ExpControl.CP.NumResp{id2}
                                case 1
                                    fprintf(FID,'expControlPoint %1.0f %1.0f  %s %s -fact %+1.6E -lim %+1.6E %+1.6E\n\n',j,handles.ExpControl.CP.Node{id2},lower(handles.ExpControl.CP.Dir{1,id2}),handles.ExpControl.CP.Resp{1,id2},handles.ExpControl.CP.F{1,id2},handles.ExpControl.CP.LimL{1,id2},handles.ExpControl.CP.LimU{1,id2});
                                case 2
                                    fprintf(FID,'expControlPoint %1.0f %1.0f  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E\n\n',j,handles.ExpControl.CP.Node{id2},lower(handles.ExpControl.CP.Dir{1,id2}),handles.ExpControl.CP.Resp{1,id2},handles.ExpControl.CP.F{1,id2},handles.ExpControl.CP.LimL{1,id2},handles.ExpControl.CP.LimU{1,id2},...
                                        lower(handles.ExpControl.CP.Dir{2,id2}),handles.ExpControl.CP.Resp{2,id2},handles.ExpControl.CP.F{2,id2},handles.ExpControl.CP.LimL{2,id2},handles.ExpControl.CP.LimU{2,id2});
                                case 3
                                    fprintf(FID,'expControlPoint %1.0f %1.0f  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E\n\n',...
                                        j,handles.ExpControl.CP.Node{id2},lower(handles.ExpControl.CP.Dir{1,id2}),handles.ExpControl.CP.Resp{1,id2},handles.ExpControl.CP.F{1,id2},handles.ExpControl.CP.LimL{1,id2},handles.ExpControl.CP.LimU{1,id2},...
                                        lower(handles.ExpControl.CP.Dir{2,id2}),handles.ExpControl.CP.Resp{2,id2},handles.ExpControl.CP.F{2,id2},handles.ExpControl.CP.LimL{2,id2},handles.ExpControl.CP.LimU{2,id2},...
                                        lower(handles.ExpControl.CP.Dir{3,id2}),handles.ExpControl.CP.Resp{3,id2},handles.ExpControl.CP.F{3,id2},handles.ExpControl.CP.LimL{3,id2},handles.ExpControl.CP.LimU{3,id2});
                                case 4
                                    fprintf(FID,'expControlPoint %1.0f %1.0f  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E\n\n',...
                                        j,handles.ExpControl.CP.Node{id2},lower(handles.ExpControl.CP.Dir{1,id2}),handles.ExpControl.CP.Resp{1,id2},handles.ExpControl.CP.F{1,id2},handles.ExpControl.CP.LimL{1,id2},handles.ExpControl.CP.LimU{1,id2},...
                                        lower(handles.ExpControl.CP.Dir{2,id2}),handles.ExpControl.CP.Resp{2,id2},handles.ExpControl.CP.F{2,id2},handles.ExpControl.CP.LimL{2,id2},handles.ExpControl.CP.LimU{2,id2},...
                                        lower(handles.ExpControl.CP.Dir{3,id2}),handles.ExpControl.CP.Resp{3,id2},handles.ExpControl.CP.F{3,id2},handles.ExpControl.CP.LimL{3,id2},handles.ExpControl.CP.LimU{3,id2},...
                                        lower(handles.ExpControl.CP.Dir{4,id2}),handles.ExpControl.CP.Resp{4,id2},handles.ExpControl.CP.F{4,id2},handles.ExpControl.CP.LimL{4,id2},handles.ExpControl.CP.LimU{4,id2});
                                case 5
                                    fprintf(FID,'expControlPoint %1.0f %1.0f  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E\n\n',...
                                        j,handles.ExpControl.CP.Node{id2},lower(handles.ExpControl.CP.Dir{1,id2}),handles.ExpControl.CP.Resp{1,id2},handles.ExpControl.CP.F{1,id2},handles.ExpControl.CP.LimL{1,id2},handles.ExpControl.CP.LimU{1,id2},...
                                        lower(handles.ExpControl.CP.Dir{2,id2}),handles.ExpControl.CP.Resp{2,id2},handles.ExpControl.CP.F{2,id2},handles.ExpControl.CP.LimL{2,id2},handles.ExpControl.CP.LimU{2,id2},...
                                        lower(handles.ExpControl.CP.Dir{3,id2}),handles.ExpControl.CP.Resp{3,id2},handles.ExpControl.CP.F{3,id2},handles.ExpControl.CP.LimL{3,id2},handles.ExpControl.CP.LimU{3,id2},...
                                        lower(handles.ExpControl.CP.Dir{4,id2}),handles.ExpControl.CP.Resp{4,id2},handles.ExpControl.CP.F{4,id2},handles.ExpControl.CP.LimL{4,id2},handles.ExpControl.CP.LimU{4,id2},...
                                        lower(handles.ExpControl.CP.Dir{5,id2}),handles.ExpControl.CP.Resp{5,id2},handles.ExpControl.CP.F{5,id2},handles.ExpControl.CP.LimL{5,id2},handles.ExpControl.CP.LimU{5,id2});
                                case 6
                                    fprintf(FID,'expControlPoint %1.0f %1.0f  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E\n\n',...
                                        j,handles.ExpControl.CP.Node{id2},lower(handles.ExpControl.CP.Dir{1,id2}),handles.ExpControl.CP.Resp{1,id2},handles.ExpControl.CP.F{1,id2},handles.ExpControl.CP.LimL{1,id2},handles.ExpControl.CP.LimU{1,id2},...
                                        lower(handles.ExpControl.CP.Dir{2,id2}),handles.ExpControl.CP.Resp{2,id2},handles.ExpControl.CP.F{2,id2},handles.ExpControl.CP.LimL{2,id2},handles.ExpControl.CP.LimU{2,id2},...
                                        lower(handles.ExpControl.CP.Dir{3,id2}),handles.ExpControl.CP.Resp{3,id2},handles.ExpControl.CP.F{3,id2},handles.ExpControl.CP.LimL{3,id2},handles.ExpControl.CP.LimU{3,id2},...
                                        lower(handles.ExpControl.CP.Dir{4,id2}),handles.ExpControl.CP.Resp{4,id2},handles.ExpControl.CP.F{4,id2},handles.ExpControl.CP.LimL{4,id2},handles.ExpControl.CP.LimU{4,id2},...
                                        lower(handles.ExpControl.CP.Dir{5,id2}),handles.ExpControl.CP.Resp{5,id2},handles.ExpControl.CP.F{5,id2},handles.ExpControl.CP.LimL{5,id2},handles.ExpControl.CP.LimU{5,id2},...
                                        lower(handles.ExpControl.CP.Dir{6,id2}),handles.ExpControl.CP.Resp{6,id2},handles.ExpControl.CP.F{6,id2},handles.ExpControl.CP.LimL{6,id2},handles.ExpControl.CP.LimU{6,id2});
                            end
                            j = j+1;
                        end
                        %fprintf(FID,'expControlPoint 1 1  ux disp -fact 1.0 -lim -7.5 7.5\n');
                        %fprintf(FID,'expControlPoint 2 1  ux disp -fact 1.0 -lim -7.5 7.5  ux force -fact 1.0 -lim -12.0 12.0\n');
                        %fprintf(FID,'expControlPoint 3 2  ux disp -fact 1.0 -lim -7.5 7.5\n');
                        %fprintf(FID,'expControlPoint 4 2  ux disp -fact 1.0 -lim -7.5 7.5  ux force -fact 1.0 -lim -12.0 12.0\n\n');
                        fprintf(FID,'# Define experimental control\n# ---------------------------\n');
                        fprintf(FID,'expControl %s 1 "%s" %s\n\n',handles.ExpControl.RealControl.Controller,handles.ExpControl.RealControl.ipAddr,handles.ExpControl.RealControl.ipPort);
                    case 'MTSCsi'
                        fullpath = fullfile(handles.ExpControl.RealControl.ConfigPath,strcat(handles.ExpControl.RealControl.ConfigName,handles.ExpControl.RealControl.ConfigType));
                        id = strfind(fullpath,filesep);
                        fullpath(id) = '/';
                        fprintf(FID,'# Define experimental control\n# ---------------------------\n');
                        fprintf(FID,'expControl %s 1 "%s" %4.3g\n\n',handles.ExpControl.RealControl.Controller,fullpath,handles.ExpControl.RealControl.rampTime);
                    case 'SCRAMNet'
                        fprintf(FID,'# Define experimental control\n# ---------------------------\n');
                        fprintf(FID,'expControl %s 1 %6.0f %2.0g\n\n',handles.ExpControl.RealControl.Controller,handles.ExpControl.RealControl.memOffset,handles.ExpControl.RealControl.NumActCh);
                    case 'dSpace'
                        fprintf(FID,'# Define experimental control\n# ---------------------------\n');
                        fprintf(FID,'expControl %s 1 %1.0f %s\n\n',handles.ExpControl.RealControl.Controller,handles.ExpControl.RealControl.PCvalue,handles.ExpControl.RealControl.boardName);
                    case 'xPCtarget'
                        appPath = handles.ExpControl.RealControl.appPath;
                        id = strfind(appPath,filesep);
                        appPath(id) = '/';
                        fprintf(FID,'# Define experimental control\n# ---------------------------\n');
                        fprintf(FID,'expControl %s 1 %1.0f "%s" %s %s "%s"\n\n',handles.ExpControl.RealControl.Controller,handles.ExpControl.RealControl.PCvalue,handles.ExpControl.RealControl.ipAddr,handles.ExpControl.RealControl.ipPort,handles.ExpControl.RealControl.appName,appPath(1:end-1));
                end
            % -------------------------------------------------------------
        end
        
        % define experimental setup
        fprintf(FID,'# Define experimental setup\n# -------------------------\n');
        fprintf(FID,'expSetup NoTransformation 1 -control 1 -dir 1 2 -sizeTrialOut 2 2\n\n');
        
        % define experimental site
        fprintf(FID,'# Define experimental site\n# ------------------------\n');
        fprintf(FID,'expSite LocalSite 1 1\n\n');
        
        % define experimental element
        %fprintf(FID,'# Define experimental element\n# ---------------------------\n');
        %fprintf(FID,'expElement generic 1 -node 2 3 -dof 1 -dof 1 -site 1 -initStif %4.0f %4.0f %4.0f %4.0f\n',handles.Model.K(1,1),handles.Model.K(1,2),handles.Model.K(2,1),handles.Model.K(2,2));
        fprintf(FID,'# ------------------------------\n# End of model generation\n# ------------------------------\n\n\n');
        
        % start of recorder generation
        %fprintf(FID,'# ------------------------------\n# Start of recorder generation\n# ------------------------------\n# create the recorder objects\n');
        %fprintf(FID,'recorder Node -file NodeDsp.out -time -node 2 -dof 1 disp\n');
        %fprintf(FID,'recorder Node -file NodeVel.out -time -node 2 -dof 1 vel\n');
        %fprintf(FID,'recorder Node -file NodeAcc.out -time -node 2 -dof 1 accel\n');
        %fprintf(FID,'recorder Node -file NodeDsp.out -time -node 3 -dof 1 disp\n');
        %fprintf(FID,'recorder Node -file NodeVel.out -time -node 3 -dof 1 vel\n');
        %fprintf(FID,'recorder Node -file NodeAcc.out -time -node 3 -dof 1 accel\n\n');
        %fprintf(FID,'recorder Element -file ElmtFrc.out -time -ele 1 forces\n\n');
        %fprintf(FID,'expRecorder Control -file Control_ctrlDsp.out -time -control 1 ctrlDisp\n');
        %fprintf(FID,'expRecorder Control -file Control_DaqDsp.out -time -control 1 daqDisp\n');
        %fprintf(FID,'expRecorder Control -file Control_DaqFrc.out -time -control 1 daqForce\n');
        %fprintf(FID,'# --------------------------------\n# End of recorder generation\n# --------------------------------\n\n\n\n');
        
        % start the server process
        fprintf(FID,'# ------------------------------\n# Start the server process\n# ------------------------------\n');
        fprintf(FID,'# startSimAppSiteServer $siteTag $port <-ssl>\nstartSimAppSiteServer 1 7777\nexit\n');
        fprintf(FID,'# ------------------------------\n# End of analysis\n# ------------------------------\n');
        
    % =====================================================================
    case '2 DOF B'
        % start of model generation
        fprintf(FID,'# ------------------------------\n# Start of model generation\n# ------------------------------\n# create ModelBuilder (with three-dimensions and 3 DOF/node)\n');
        fprintf(FID,'model BasicBuilder -ndm 3 -ndf 3\n\n');
        fprintf(FID,'# Load OpenFresco package\n# -----------------------\n');
        fprintf(FID,'loadPackage OpenFresco\n\n');
        % define geometry
        %fprintf(FID,'# Define geometry for model\n# -------------------------\n# node $tag $xCrd $yCrd $mass\n');
        %fprintf(FID,'node  1     0.0   0.0   0.0\nnode  2     0.0   0.0   1.0  -mass  %+1.6E  %+1.6E   0.0\n',handles.Model.M(1,1),handles.Model.M(2,2));
        %fprintf(FID,'fix   1      1     1     1\nfix   2      0     0     1\n\n');
        
        switch handles.ExpControl.Type
            % -------------------------------------------------------------
            case 'Simulation'
                % define materials
                fprintf(FID,'# Define materials\n# ----------------\n');
                switch handles.ExpControl.DOF1.SimMaterial
                    case 'Elastic'
                        fprintf(FID,'uniaxialMaterial Elastic 1 %+1.6E\n',handles.ExpControl.DOF1.E);
                    case 'Elastic-Perfectly Plastic'
                        fprintf(FID,'uniaxialMaterial ElasticPP 1 %+1.6E %+1.6E\n',handles.ExpControl.DOF1.E,handles.ExpControl.DOF1.epsP);
                    case 'Steel - Bilinear'
                        fprintf(FID,'uniaxialMaterial Steel01 1 %+1.6E %+1.6E %+1.6E\n',handles.ExpControl.DOF1.Fy,handles.ExpControl.DOF1.E,handles.ExpControl.DOF1.b);
                    case 'Steel - Giuffré-Menegotto-Pinto'
                        fprintf(FID,'uniaxialMaterial Steel02 1 %+1.6E %+1.6E %+1.6E %+1.6E %+1.6E %+1.6E\n',handles.ExpControl.DOF1.Fy,handles.ExpControl.DOF1.E,handles.ExpControl.DOF1.b,handles.ExpControl.DOF1.R0, handles.ExpControl.DOF1.cR1, handles.ExpControl.DOF1.cR2);
                end
                switch handles.ExpControl.DOF2.SimMaterial
                    case 'Elastic'
                        fprintf(FID,'uniaxialMaterial Elastic 2 %+1.6E\n\n',handles.ExpControl.DOF2.E);
                    case 'Elastic-Perfectly Plastic'
                        fprintf(FID,'uniaxialMaterial ElasticPP 2 %+1.6E %+1.6E\n\n',handles.ExpControl.DOF2.E,handles.ExpControl.DOF2.epsP);
                    case 'Steel - Bilinear'
                        fprintf(FID,'uniaxialMaterial Steel01 2 %+1.6E %+1.6E %+1.6E\n\n',handles.ExpControl.DOF2.Fy,handles.ExpControl.DOF2.E,handles.ExpControl.DOF2.b);
                    case 'Steel - Giuffré-Menegotto-Pinto'
                        fprintf(FID,'uniaxialMaterial Steel02 2 %+1.6E %+1.6E %+1.6E %+1.6E %+1.6E %+1.6E\n\n',handles.ExpControl.DOF2.Fy,handles.ExpControl.DOF2.E,handles.ExpControl.DOF2.b,handles.ExpControl.DOF2.R0, handles.ExpControl.DOF2.cR1, handles.ExpControl.DOF2.cR2);
                end
                
                % define experimental control
                fprintf(FID,'# Define experimental control\n# ---------------------------\n');
                fprintf(FID,'expControl %s 1 1 2\n\n',handles.ExpControl.SimControl.SimType);
                % SimDomain case
                %fprintf(FID,'# expControl SimDomain $tag -trialCP cpTags -outCP cpTags');
                %fprintf(FID,'expControl %s 1 -trialCP 1 -outCP 2\n\n',handles.ExpControl.SimControl.SimType);
            % -------------------------------------------------------------
            case 'Real'
                % define experimental control
                switch handles.ExpControl.RealControl.Controller
                    case 'LabVIEW'
                        % define experimental control points
                        fprintf(FID,'# Define control points\n# ---------------------\n');
                        fprintf(FID,'# expControlPoint tag nodeTag dir resp <-fact f> <-lim l u> ...\n');
                        j = 1;
                        for i = 1:length(handles.ExpControl.RealControl.TrialCP)
                            id1 = handles.ExpControl.RealControl.TrialCP(i);
                            id2 = handles.ExpControl.RealControl.OutputCP(i);
                            switch handles.ExpControl.CP.NumResp{id1}
                                case 1
                                    fprintf(FID,'expControlPoint %1.0f %1.0f  %s %s -fact %+1.6E -lim %+1.6E %+1.6E\n',j,handles.ExpControl.CP.Node{id1},lower(handles.ExpControl.CP.Dir{1,id1}),handles.ExpControl.CP.Resp{1,id1},handles.ExpControl.CP.F{1,id1},handles.ExpControl.CP.LimL{1,id1},handles.ExpControl.CP.LimU{1,id1});
                                case 2
                                    fprintf(FID,'expControlPoint %1.0f %1.0f  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E\n',j,handles.ExpControl.CP.Node{id1},lower(handles.ExpControl.CP.Dir{1,id1}),handles.ExpControl.CP.Resp{1,id1},handles.ExpControl.CP.F{1,id1},handles.ExpControl.CP.LimL{1,id1},handles.ExpControl.CP.LimU{1,id1},...
                                        lower(handles.ExpControl.CP.Dir{2,id1}),handles.ExpControl.CP.Resp{2,id1},handles.ExpControl.CP.F{2,id1},handles.ExpControl.CP.LimL{2,id1},handles.ExpControl.CP.LimU{2,id1});
                                case 3
                                    fprintf(FID,'expControlPoint %1.0f %1.0f  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E\n',...
                                        j,handles.ExpControl.CP.Node{id1},lower(handles.ExpControl.CP.Dir{1,id1}),handles.ExpControl.CP.Resp{1,id1},handles.ExpControl.CP.F{1,id1},handles.ExpControl.CP.LimL{1,id1},handles.ExpControl.CP.LimU{1,id1},...
                                        lower(handles.ExpControl.CP.Dir{2,id1}),handles.ExpControl.CP.Resp{2,id1},handles.ExpControl.CP.F{2,id1},handles.ExpControl.CP.LimL{2,id1},handles.ExpControl.CP.LimU{2,id1},...
                                        lower(handles.ExpControl.CP.Dir{3,id1}),handles.ExpControl.CP.Resp{3,id1},handles.ExpControl.CP.F{3,id1},handles.ExpControl.CP.LimL{3,id1},handles.ExpControl.CP.LimU{3,id1});
                                case 4
                                    fprintf(FID,'expControlPoint %1.0f %1.0f  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E\n',...
                                        j,handles.ExpControl.CP.Node{id1},lower(handles.ExpControl.CP.Dir{1,id1}),handles.ExpControl.CP.Resp{1,id1},handles.ExpControl.CP.F{1,id1},handles.ExpControl.CP.LimL{1,id1},handles.ExpControl.CP.LimU{1,id1},...
                                        lower(handles.ExpControl.CP.Dir{2,id1}),handles.ExpControl.CP.Resp{2,id1},handles.ExpControl.CP.F{2,id1},handles.ExpControl.CP.LimL{2,id1},handles.ExpControl.CP.LimU{2,id1},...
                                        lower(handles.ExpControl.CP.Dir{3,id1}),handles.ExpControl.CP.Resp{3,id1},handles.ExpControl.CP.F{3,id1},handles.ExpControl.CP.LimL{3,id1},handles.ExpControl.CP.LimU{3,id1},...
                                        lower(handles.ExpControl.CP.Dir{4,id1}),handles.ExpControl.CP.Resp{4,id1},handles.ExpControl.CP.F{4,id1},handles.ExpControl.CP.LimL{4,id1},handles.ExpControl.CP.LimU{4,id1});
                                case 5
                                    fprintf(FID,'expControlPoint %1.0f %1.0f  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E\n',...
                                        j,handles.ExpControl.CP.Node{id1},lower(handles.ExpControl.CP.Dir{1,id1}),handles.ExpControl.CP.Resp{1,id1},handles.ExpControl.CP.F{1,id1},handles.ExpControl.CP.LimL{1,id1},handles.ExpControl.CP.LimU{1,id1},...
                                        lower(handles.ExpControl.CP.Dir{2,id1}),handles.ExpControl.CP.Resp{2,id1},handles.ExpControl.CP.F{2,id1},handles.ExpControl.CP.LimL{2,id1},handles.ExpControl.CP.LimU{2,id1},...
                                        lower(handles.ExpControl.CP.Dir{3,id1}),handles.ExpControl.CP.Resp{3,id1},handles.ExpControl.CP.F{3,id1},handles.ExpControl.CP.LimL{3,id1},handles.ExpControl.CP.LimU{3,id1},...
                                        lower(handles.ExpControl.CP.Dir{4,id1}),handles.ExpControl.CP.Resp{4,id1},handles.ExpControl.CP.F{4,id1},handles.ExpControl.CP.LimL{4,id1},handles.ExpControl.CP.LimU{4,id1},...
                                        lower(handles.ExpControl.CP.Dir{5,id1}),handles.ExpControl.CP.Resp{5,id1},handles.ExpControl.CP.F{5,id1},handles.ExpControl.CP.LimL{5,id1},handles.ExpControl.CP.LimU{5,id1});
                                case 6
                                    fprintf(FID,'expControlPoint %1.0f %1.0f  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E\n',...
                                        j,handles.ExpControl.CP.Node{id1},lower(handles.ExpControl.CP.Dir{1,id1}),handles.ExpControl.CP.Resp{1,id1},handles.ExpControl.CP.F{1,id1},handles.ExpControl.CP.LimL{1,id1},handles.ExpControl.CP.LimU{1,id1},...
                                        lower(handles.ExpControl.CP.Dir{2,id1}),handles.ExpControl.CP.Resp{2,id1},handles.ExpControl.CP.F{2,id1},handles.ExpControl.CP.LimL{2,id1},handles.ExpControl.CP.LimU{2,id1},...
                                        lower(handles.ExpControl.CP.Dir{3,id1}),handles.ExpControl.CP.Resp{3,id1},handles.ExpControl.CP.F{3,id1},handles.ExpControl.CP.LimL{3,id1},handles.ExpControl.CP.LimU{3,id1},...
                                        lower(handles.ExpControl.CP.Dir{4,id1}),handles.ExpControl.CP.Resp{4,id1},handles.ExpControl.CP.F{4,id1},handles.ExpControl.CP.LimL{4,id1},handles.ExpControl.CP.LimU{4,id1},...
                                        lower(handles.ExpControl.CP.Dir{5,id1}),handles.ExpControl.CP.Resp{5,id1},handles.ExpControl.CP.F{5,id1},handles.ExpControl.CP.LimL{5,id1},handles.ExpControl.CP.LimU{5,id1},...
                                        lower(handles.ExpControl.CP.Dir{6,id1}),handles.ExpControl.CP.Resp{6,id1},handles.ExpControl.CP.F{6,id1},handles.ExpControl.CP.LimL{6,id1},handles.ExpControl.CP.LimU{6,id1});
                            end
                            j = j+1;
                            switch handles.ExpControl.CP.NumResp{id2}
                                case 1
                                    fprintf(FID,'expControlPoint %1.0f %1.0f  %s %s -fact %+1.6E -lim %+1.6E %+1.6E\n\n',j,handles.ExpControl.CP.Node{id2},lower(handles.ExpControl.CP.Dir{1,id2}),handles.ExpControl.CP.Resp{1,id2},handles.ExpControl.CP.F{1,id2},handles.ExpControl.CP.LimL{1,id2},handles.ExpControl.CP.LimU{1,id2});
                                case 2
                                    fprintf(FID,'expControlPoint %1.0f %1.0f  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E\n\n',j,handles.ExpControl.CP.Node{id2},lower(handles.ExpControl.CP.Dir{1,id2}),handles.ExpControl.CP.Resp{1,id2},handles.ExpControl.CP.F{1,id2},handles.ExpControl.CP.LimL{1,id2},handles.ExpControl.CP.LimU{1,id2},...
                                        lower(handles.ExpControl.CP.Dir{2,id2}),handles.ExpControl.CP.Resp{2,id2},handles.ExpControl.CP.F{2,id2},handles.ExpControl.CP.LimL{2,id2},handles.ExpControl.CP.LimU{2,id2});
                                case 3
                                    fprintf(FID,'expControlPoint %1.0f %1.0f  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E\n\n',...
                                        j,handles.ExpControl.CP.Node{id2},lower(handles.ExpControl.CP.Dir{1,id2}),handles.ExpControl.CP.Resp{1,id2},handles.ExpControl.CP.F{1,id2},handles.ExpControl.CP.LimL{1,id2},handles.ExpControl.CP.LimU{1,id2},...
                                        lower(handles.ExpControl.CP.Dir{2,id2}),handles.ExpControl.CP.Resp{2,id2},handles.ExpControl.CP.F{2,id2},handles.ExpControl.CP.LimL{2,id2},handles.ExpControl.CP.LimU{2,id2},...
                                        lower(handles.ExpControl.CP.Dir{3,id2}),handles.ExpControl.CP.Resp{3,id2},handles.ExpControl.CP.F{3,id2},handles.ExpControl.CP.LimL{3,id2},handles.ExpControl.CP.LimU{3,id2});
                                case 4
                                    fprintf(FID,'expControlPoint %1.0f %1.0f  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E\n\n',...
                                        j,handles.ExpControl.CP.Node{id2},lower(handles.ExpControl.CP.Dir{1,id2}),handles.ExpControl.CP.Resp{1,id2},handles.ExpControl.CP.F{1,id2},handles.ExpControl.CP.LimL{1,id2},handles.ExpControl.CP.LimU{1,id2},...
                                        lower(handles.ExpControl.CP.Dir{2,id2}),handles.ExpControl.CP.Resp{2,id2},handles.ExpControl.CP.F{2,id2},handles.ExpControl.CP.LimL{2,id2},handles.ExpControl.CP.LimU{2,id2},...
                                        lower(handles.ExpControl.CP.Dir{3,id2}),handles.ExpControl.CP.Resp{3,id2},handles.ExpControl.CP.F{3,id2},handles.ExpControl.CP.LimL{3,id2},handles.ExpControl.CP.LimU{3,id2},...
                                        lower(handles.ExpControl.CP.Dir{4,id2}),handles.ExpControl.CP.Resp{4,id2},handles.ExpControl.CP.F{4,id2},handles.ExpControl.CP.LimL{4,id2},handles.ExpControl.CP.LimU{4,id2});
                                case 5
                                    fprintf(FID,'expControlPoint %1.0f %1.0f  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E\n\n',...
                                        j,handles.ExpControl.CP.Node{id2},lower(handles.ExpControl.CP.Dir{1,id2}),handles.ExpControl.CP.Resp{1,id2},handles.ExpControl.CP.F{1,id2},handles.ExpControl.CP.LimL{1,id2},handles.ExpControl.CP.LimU{1,id2},...
                                        lower(handles.ExpControl.CP.Dir{2,id2}),handles.ExpControl.CP.Resp{2,id2},handles.ExpControl.CP.F{2,id2},handles.ExpControl.CP.LimL{2,id2},handles.ExpControl.CP.LimU{2,id2},...
                                        lower(handles.ExpControl.CP.Dir{3,id2}),handles.ExpControl.CP.Resp{3,id2},handles.ExpControl.CP.F{3,id2},handles.ExpControl.CP.LimL{3,id2},handles.ExpControl.CP.LimU{3,id2},...
                                        lower(handles.ExpControl.CP.Dir{4,id2}),handles.ExpControl.CP.Resp{4,id2},handles.ExpControl.CP.F{4,id2},handles.ExpControl.CP.LimL{4,id2},handles.ExpControl.CP.LimU{4,id2},...
                                        lower(handles.ExpControl.CP.Dir{5,id2}),handles.ExpControl.CP.Resp{5,id2},handles.ExpControl.CP.F{5,id2},handles.ExpControl.CP.LimL{5,id2},handles.ExpControl.CP.LimU{5,id2});
                                case 6
                                    fprintf(FID,'expControlPoint %1.0f %1.0f  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E  %s %s -fact %+1.6E -lim %+1.6E %+1.6E\n\n',...
                                        j,handles.ExpControl.CP.Node{id2},lower(handles.ExpControl.CP.Dir{1,id2}),handles.ExpControl.CP.Resp{1,id2},handles.ExpControl.CP.F{1,id2},handles.ExpControl.CP.LimL{1,id2},handles.ExpControl.CP.LimU{1,id2},...
                                        lower(handles.ExpControl.CP.Dir{2,id2}),handles.ExpControl.CP.Resp{2,id2},handles.ExpControl.CP.F{2,id2},handles.ExpControl.CP.LimL{2,id2},handles.ExpControl.CP.LimU{2,id2},...
                                        lower(handles.ExpControl.CP.Dir{3,id2}),handles.ExpControl.CP.Resp{3,id2},handles.ExpControl.CP.F{3,id2},handles.ExpControl.CP.LimL{3,id2},handles.ExpControl.CP.LimU{3,id2},...
                                        lower(handles.ExpControl.CP.Dir{4,id2}),handles.ExpControl.CP.Resp{4,id2},handles.ExpControl.CP.F{4,id2},handles.ExpControl.CP.LimL{4,id2},handles.ExpControl.CP.LimU{4,id2},...
                                        lower(handles.ExpControl.CP.Dir{5,id2}),handles.ExpControl.CP.Resp{5,id2},handles.ExpControl.CP.F{5,id2},handles.ExpControl.CP.LimL{5,id2},handles.ExpControl.CP.LimU{5,id2},...
                                        lower(handles.ExpControl.CP.Dir{6,id2}),handles.ExpControl.CP.Resp{6,id2},handles.ExpControl.CP.F{6,id2},handles.ExpControl.CP.LimL{6,id2},handles.ExpControl.CP.LimU{6,id2});
                            end
                            j = j+1;
                        end
                        %fprintf(FID,'expControlPoint 1 1  ux disp -fact 1.0 -lim -7.5 7.5  uy disp -fact 1.0 -lim -7.5 7.5\n');
                        %fprintf(FID,'expControlPoint 2 1  ux disp -fact 1.0 -lim -7.5 7.5  uy disp -fact 1.0 -lim -7.5 7.5  ux force -fact 1.0 -lim -12.0 12.0  uy force -fact 1.0 -lim -12.0 12.0\n\n');
                        fprintf(FID,'# Define experimental control\n# ---------------------------\n');
                        fprintf(FID,'expControl %s 1 "%s" %s\n\n',handles.ExpControl.RealControl.Controller,handles.ExpControl.RealControl.ipAddr,handles.ExpControl.RealControl.ipPort);
                    case 'MTSCsi'
                        fullpath = fullfile(handles.ExpControl.RealControl.ConfigPath,strcat(handles.ExpControl.RealControl.ConfigName,handles.ExpControl.RealControl.ConfigType));
                        id = strfind(fullpath,filesep);
                        fullpath(id) = '/';
                        fprintf(FID,'# Define experimental control\n# ---------------------------\n');
                        fprintf(FID,'expControl %s 1 "%s" %4.3g\n\n',handles.ExpControl.RealControl.Controller,fullpath,handles.ExpControl.RealControl.rampTime);
                    case 'SCRAMNet'
                        fprintf(FID,'# Define experimental control\n# ---------------------------\n');
                        fprintf(FID,'expControl %s 1 %6.0f %2.0g\n\n',handles.ExpControl.RealControl.Controller,handles.ExpControl.RealControl.memOffset,handles.ExpControl.RealControl.NumActCh);
                    case 'dSpace'
                        fprintf(FID,'# Define experimental control\n# ---------------------------\n');
                        fprintf(FID,'expControl %s 1 %1.0f %s\n\n',handles.ExpControl.RealControl.Controller,handles.ExpControl.RealControl.PCvalue,handles.ExpControl.RealControl.boardName);
                    case 'xPCtarget'
                        appPath = handles.ExpControl.RealControl.appPath;
                        id = strfind(appPath,filesep);
                        appPath(id) = '/';
                        fprintf(FID,'# Define experimental control\n# ---------------------------\n');
                        fprintf(FID,'expControl %s 1 %1.0f "%s" %s %s "%s"\n\n',handles.ExpControl.RealControl.Controller,handles.ExpControl.RealControl.PCvalue,handles.ExpControl.RealControl.ipAddr,handles.ExpControl.RealControl.ipPort,handles.ExpControl.RealControl.appName,appPath(1:end-1));
                end
            % -------------------------------------------------------------
        end
        
        % define experimental setup
        fprintf(FID,'# Define experimental setup\n# -------------------------\n');
        fprintf(FID,'expSetup NoTransformation 1 -control 1 -dir 1 2 -sizeTrialOut 2 2\n\n');
        
        % define experimental site
        fprintf(FID,'# Define experimental site\n# ------------------------\n');
        fprintf(FID,'expSite LocalSite 1 1\n\n');
        
        % define experimental element
        %fprintf(FID,'# Define experimental element\n# ---------------------------\n');
        %fprintf(FID,'expElement generic 1 -node 2 -dof 1 2 -site 1 -initStif %+1.6E %+1.6E %+1.6E %+1.6E\n',handles.Model.K(1,1),handles.Model.K(1,2),handles.Model.K(2,1),handles.Model.K(2,2));
        fprintf(FID,'# ------------------------------\n# End of model generation\n# ------------------------------\n\n\n');
        
        % start of recorder generation
        %fprintf(FID,'# ------------------------------\n# Start of recorder generation\n# ------------------------------\n# create the recorder objects\n');
        %fprintf(FID,'recorder Node -file NodeDsp.out -time -node 2 -dof 1 disp\n');
        %fprintf(FID,'recorder Node -file NodeVel.out -time -node 2 -dof 1 vel\n');
        %fprintf(FID,'recorder Node -file NodeAcc.out -time -node 2 -dof 1 accel\n');
        %fprintf(FID,'recorder Node -file NodeDsp.out -time -node 2 -dof 2 disp\n');
        %fprintf(FID,'recorder Node -file NodeVel.out -time -node 2 -dof 2 vel\n');
        %fprintf(FID,'recorder Node -file NodeAcc.out -time -node 2 -dof 2 accel\n\n');
        %fprintf(FID,'recorder Element -file ElmtFrc.out -time -ele 1 forces\n\n');
        %fprintf(FID,'expRecorder Control -file Control_ctrlDsp.out -time -control 1 ctrlDisp\n');
        %fprintf(FID,'expRecorder Control -file Control_DaqDsp.out -time -control 1 daqDisp\n');
        %fprintf(FID,'expRecorder Control -file Control_DaqFrc.out -time -control 1 daqForce\n');
        %fprintf(FID,'# --------------------------------\n# End of recorder generation\n# --------------------------------\n\n\n\n');
        
        % start the server process
        fprintf(FID,'# ------------------------------\n# Start the server process\n# ------------------------------\n');
        fprintf(FID,'# startSimAppSiteServer $siteTag $port <-ssl>\nstartSimAppSiteServer 1 7777\nexit\n');
        fprintf(FID,'# ------------------------------\n# End of analysis\n# ------------------------------\n');
    % =====================================================================
end

msgbox('The .tcl file was written successfully!','','none');
fclose(FID);

% update handles structure
guidata(gcbf,handles);
