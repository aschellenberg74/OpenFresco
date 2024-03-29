function ReplayResults(step)
%REPLAYRESULTS to replay results of analysis
% ReplayResults(step)
%
% step : the current analysis step
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

if rem(step,10) ~= 0
    return
end

% initialization tasks
handles = guidata(findobj('Tag','OpenFrescoExpress'));
t = handles.Response.Time(:,1:step);
ag = handles.Response.ag(:,step);
U = handles.Response.U(:,1:step);
Udotdot = handles.Response.Udotdot(:,1:step);
Pr = handles.Response.Pr(:,1:step);
Um = handles.Response.Um(:,1:step);
error = handles.Response.error(:,1:step);
f = handles.Response.f;
MX = handles.Response.MX;
TI = handles.Response.TI(:,1:step);

switch handles.Model.Type
    % =====================================================================
    case '1 DOF'
        % structural outputs
        set(handles.Plots.SO1dplot,'Xdata',t,'YData',U(1,:));
        set(handles.Plots.SO1fplot,'Xdata',t,'YData',Pr(1,:));
        set(handles.Plots.SO1aplot,'Xdata',t,'YData',Udotdot(1,:));
        set(handles.Plots.SO1fdplot,'Xdata',U(1,:),'YData',Pr(1,:));
        set(handles.Plots.SO1fddot,'Xdata',U(1,end),'YData',Pr(1,end));
        
        % routine for tracing ag plot
        switch handles.GM.loadType
            case 'Ground Motions'
                set(handles.Plots.SO1agdot,'Xdata',t(end),'YData',ag(1));
            case 'Initial Conditions'
                if t(end) <= handles.GM.rampTime
                    set(handles.Plots.SO1agdot,'Xdata',t(end),'YData',U(end)/handles.GM.initialDisp(1)*100);
                else
                    set(handles.Plots.SO1agdot,'Xdata',t(end),'YData',0);
                end
        end
        
        % error monitors
        set(handles.Plots.EM1eplot,'Xdata',t,'YData',error(1,:));
        set(handles.Plots.EM1ffteplot,'Xdata',f,'YData',MX(1,:));
        set(handles.Plots.EM1MeasCmdplot,'Xdata',U(1,:),'YData',Um(1,:));
        set(handles.Plots.EM1MeasCmddot,'Xdata',U(1,end),'YData',Um(1,end));
        set(handles.Plots.EM1trackplot,'Xdata',t,'YData',TI(1,:));
        
        % update all the plots
        drawnow;
    % =====================================================================
    case '2 DOF A'
        % structural outputs
        set(handles.Plots.SO1dplot,'Xdata',t,'YData',U(1,:));
        set(handles.Plots.SO1fplot,'Xdata',t,'YData',Pr(1,:));
        set(handles.Plots.SO1aplot,'Xdata',t,'YData',Udotdot(1,:));
        set(handles.Plots.SO1fdplot,'Xdata',U(1,:),'YData',Pr(1,:)+Pr(2,:));
        set(handles.Plots.SO1fddot,'Xdata',U(1,end),'YData',Pr(1,end)+Pr(2,end));
        set(handles.Plots.SO1ddplot,'Xdata',U(1,:),'YData',U(2,:));
        set(handles.Plots.SO1dddot,'Xdata',U(1,end),'YData',U(2,end));
        set(handles.Plots.SO2dplot,'Xdata',t,'YData',U(2,:));
        set(handles.Plots.SO2fplot,'Xdata',t,'YData',Pr(2,:));
        set(handles.Plots.SO2aplot,'Xdata',t,'YData',Udotdot(2,:));
        set(handles.Plots.SO2fdplot,'Xdata',U(2,:)-U(1,:),'YData',Pr(2,:));
        set(handles.Plots.SO2fddot,'Xdata',U(2,end)-U(1,end),'YData',Pr(2,end));
        set(handles.Plots.SO2ffplot,'Xdata',Pr(1,:),'YData',Pr(2,:));
        set(handles.Plots.SO2ffdot,'Xdata',Pr(1,end),'YData',Pr(2,end));
        
        % routine for tracing ag plots
        switch handles.GM.loadType
            case 'Ground Motions'
                set(handles.Plots.SO1agdot,'Xdata',t(end),'YData',ag(1));
                set(handles.Plots.SO2agdot,'Xdata',t(end),'YData',ag(1));
            case 'Initial Conditions'
                if t(end) <= handles.GM.rampTime
                    y1 = U(1,:);
                    y2 = U(2,:);
                    set(handles.Plots.SO1agdot,'Xdata',t(end),'YData',y1(end)/handles.GM.initialDisp(1)*100);
                    set(handles.Plots.SO2agdot,'Xdata',t(end),'YData',y2(end)/handles.GM.initialDisp(2)*100);
                else
                    set(handles.Plots.SO1agdot,'Xdata',t(end),'YData',0);
                    set(handles.Plots.SO2agdot,'Xdata',t(end),'YData',0);
                end
        end
        
        
        % error Monitors
        set(handles.Plots.EM1eplot,'Xdata',t,'YData',error(1,:));
        set(handles.Plots.EM1ffteplot,'Xdata',f,'YData',MX(1,:));
        set(handles.Plots.EM1MeasCmdplot,'Xdata',U(1,:),'YData',Um(1,:));
        set(handles.Plots.EM1MeasCmddot,'Xdata',U(1,end),'YData',Um(1,end));
        set(handles.Plots.EM1trackplot,'Xdata',t,'YData',TI(1,:));
        set(handles.Plots.EM2eplot,'Xdata',t,'YData',error(2,:));
        set(handles.Plots.EM2ffteplot,'Xdata',f,'YData',MX(2,:));
        set(handles.Plots.EM2MeasCmdplot,'Xdata',U(2,:),'YData',Um(2,:));
        set(handles.Plots.EM2MeasCmddot,'Xdata',U(2,end),'YData',Um(2,end));
        set(handles.Plots.EM2trackplot,'Xdata',t,'YData',TI(2,:));
        
        % update all the plots
        drawnow;
    % =====================================================================
    case '2 DOF B'
        % structural outputs
        set(handles.Plots.SO1dplot,'Xdata',t,'YData',U(1,:));
        set(handles.Plots.SO1fplot,'Xdata',t,'YData',Pr(1,:));
        set(handles.Plots.SO1aplot,'Xdata',t,'YData',Udotdot(1,:));
        set(handles.Plots.SO1fdplot,'Xdata',U(1,:),'YData',Pr(1,:));
        set(handles.Plots.SO1fddot,'Xdata',U(1,end),'YData',Pr(1,end));
        set(handles.Plots.SO1ddplot,'Xdata',U(1,:),'YData',U(2,:));
        set(handles.Plots.SO1dddot,'Xdata',U(1,end),'YData',U(2,end));
        set(handles.Plots.SO2dplot,'Xdata',t,'YData',U(2,:));
        set(handles.Plots.SO2fplot,'Xdata',t,'YData',Pr(2,:));
        set(handles.Plots.SO2aplot,'Xdata',t,'YData',Udotdot(2,:));
        set(handles.Plots.SO2fdplot,'Xdata',U(2,:),'YData',Pr(2,:));
        set(handles.Plots.SO2fddot,'Xdata',U(2,end),'YData',Pr(2,end));
        set(handles.Plots.SO2ffplot,'Xdata',Pr(1,:),'YData',Pr(2,:));
        set(handles.Plots.SO2ffdot,'Xdata',Pr(1,end),'YData',Pr(2,end));
        
        % routine for tracing ag plots
        switch handles.GM.loadType
            case 'Ground Motions'
                set(handles.Plots.SO1agdot,'Xdata',t(end),'YData',ag(1));
                set(handles.Plots.SO2agdot,'Xdata',t(end),'YData',ag(2));
            case 'Initial Conditions'
                if t(end) <= handles.GM.rampTime
                    y1 = U(1,:);
                    y2 = U(2,:);
                    set(handles.Plots.SO1agdot,'Xdata',t(end),'YData',y1(end)/handles.GM.initialDisp(1)*100);
                    set(handles.Plots.SO2agdot,'Xdata',t(end),'YData',y2(end)/handles.GM.initialDisp(2)*100);
                else
                    set(handles.Plots.SO1agdot,'Xdata',t(end),'YData',0);
                    set(handles.Plots.SO2agdot,'Xdata',t(end),'YData',0);
                end
        end
        
        % error monitors
        set(handles.Plots.EM1eplot,'Xdata',t,'YData',error(1,:));
        set(handles.Plots.EM1ffteplot,'Xdata',f,'YData',MX(1,:));
        set(handles.Plots.EM1MeasCmdplot,'Xdata',U(1,:),'YData',Um(1,:));
        set(handles.Plots.EM1MeasCmddot,'Xdata',U(1,end),'YData',Um(1,end));
        set(handles.Plots.EM1trackplot,'Xdata',t,'YData',TI(1,:));
        set(handles.Plots.EM2eplot,'Xdata',t,'YData',error(2,:));
        set(handles.Plots.EM2ffteplot,'Xdata',f,'YData',MX(2,:));
        set(handles.Plots.EM2MeasCmdplot,'Xdata',U(2,:),'YData',Um(2,:));
        set(handles.Plots.EM2MeasCmddot,'Xdata',U(2,end),'YData',Um(2,end));
        set(handles.Plots.EM2trackplot,'Xdata',t,'YData',TI(2,:));
        
        % update all the plots
        drawnow;
    % =====================================================================
end
