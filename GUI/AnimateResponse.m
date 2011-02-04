function [error, f, MX, TI] = AnimateResponse(Type,t,ag,U,Udotdot,Pr,Um)
%ANIMATERESPONSE switches over the model type to choose which plots to
%populate with the structural response and error monitoring data
% error        : displacement error
% f            : error frequency vector
% MX           : fourier amplitude of the error
% TI           : error tracking indicator
%
% Type         : structural model type
% t            : vector of time steps for analysis
% ag           : vector of ground motion values
% U            : command displacement values
% Udotdot      : acceleration values
% Pr           : resisting force
% Um           : measured displacement values

% Initialization tasks
handles = guidata(findobj('Tag','OpenFrescoExpress'));

switch Type
    case '1 DOF'
        %Structural Outputs
        set(handles.Plots.SO1dplot,'Xdata',t,'YData',U(1,:));
        set(handles.Plots.SO1fplot,'Xdata',t,'YData',Pr(1,:));
        set(handles.Plots.SO1aplot,'Xdata',t,'YData',Udotdot(1,:));
        set(handles.Plots.SO1fdplot,'Xdata',U(1,:),'YData',Pr(1,:));
        set(handles.Plots.SO1fddot,'Xdata',U(1,end),'YData',Pr(1,end));
        
        %Routine for tracing ag plot
        switch handles.GM.loadType
            case 'Ground Motions'
                set(handles.Plots.SO1agdot,'Xdata',t(end),'YData',ag(1));
            case 'Initial Conditions'
                if t(end) <= handles.GM.rampTime
                    set(handles.Plots.SO1agdot,'Xdata',t(end), ...
                        'YData',U(1,end)/handles.GM.initialDisp(1)*100);
                else
                    set(handles.Plots.SO1agdot,'Xdata',t(end),'YData',0);
                end
        end
        
        %Error Monitors
        error = Um-U;
        [f,MX] = GetFFT(error',t(2)-t(1));
        TI(1,:) = 0.5*(cumtrapz(U(1,:),Um(1,:)) - cumtrapz(Um(1,:),U(1,:)));
        set(handles.Plots.EM1eplot,'Xdata',t,'YData',error(1,:));
        set(handles.Plots.EM1ffteplot,'Xdata',f,'YData',MX(:,1));
        set(handles.Plots.EM1MeasCmdplot,'Xdata',U(1,:),'YData',Um(1,:));
        set(handles.Plots.EM1MeasCmddot,'Xdata',U(1,end),'YData',Um(1,end));
        set(handles.Plots.EM1trackplot,'Xdata',t,'YData',TI(1,:));
        
        %Animate Structure
        if findobj('Tag','StructAnim')
            set(handles.Plots.StructAnim, ...
                'Vertices',[0.0 0.0; U(1,end) 10.0], ...
                'FaceVertexCData',[0; abs(U(1,end))]);
        end
        
        % update all the plots
        drawnow;
        
    case '2 DOF A'
        %Structural Outputs
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
        
        %Routine for tracing ag plots
        switch handles.GM.loadType
            case 'Ground Motions'
                set(handles.Plots.SO1agdot,'Xdata',t(end),'YData',ag(1));
                set(handles.Plots.SO2agdot,'Xdata',t(end),'YData',ag(1));
            case 'Initial Conditions'
                if t(end) <= handles.GM.rampTime
                    set(handles.Plots.SO1agdot,'Xdata',t(end), ...
                        'YData',U(1,end)/handles.GM.initialDisp(1)*100);
                    set(handles.Plots.SO2agdot,'Xdata',t(end), ...
                        'YData',U(2,end)/handles.GM.initialDisp(2)*100);
                else
                    set(handles.Plots.SO1agdot,'Xdata',t(end),'YData',0);
                    set(handles.Plots.SO2agdot,'Xdata',t(end),'YData',0);
                end
        end
        
        %Error Monitors
        error = Um-U;
        [f,MX] = GetFFT(error',t(2)-t(1));
        TI(1,:) = 0.5*(cumtrapz(U(1,:),Um(1,:)) - cumtrapz(Um(1,:),U(1,:)));
        TI(2,:) = 0.5*(cumtrapz(U(2,:),Um(2,:)) - cumtrapz(Um(2,:),U(2,:)));
        set(handles.Plots.EM1eplot,'Xdata',t,'YData',error(1,:));
        set(handles.Plots.EM1ffteplot,'Xdata',f,'YData',MX(:,1));
        set(handles.Plots.EM1MeasCmdplot,'Xdata',U(1,:),'YData',Um(1,:));
        set(handles.Plots.EM1MeasCmddot,'Xdata',U(1,end),'YData',Um(1,end));
        set(handles.Plots.EM1trackplot,'Xdata',t,'YData',TI(1,:));
        set(handles.Plots.EM2eplot,'Xdata',t,'YData',error(2,:));
        set(handles.Plots.EM2ffteplot,'Xdata',f,'YData',MX(:,2));
        set(handles.Plots.EM2MeasCmdplot,'Xdata',U(2,:),'YData',Um(2,:));
        set(handles.Plots.EM2MeasCmddot,'Xdata',U(2,end),'YData',Um(2,end));
        set(handles.Plots.EM2trackplot,'Xdata',t,'YData',TI(2,:));
        
        %Animate Structure
        if findobj('Tag','StructAnim')
            set(handles.Plots.StructAnim, ...
                'Vertices',[0.0 0.0; U(1,end) 5.0; U(2,end) 10.0], ...
                'FaceVertexCData',[0; abs(U(:,end))]);
        end
        
        % update all the plots
        drawnow;
        
    case '2 DOF B'
        %Structural Outputs
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
        
        %Routine for tracing ag plots
        switch handles.GM.loadType
            case 'Ground Motions'
                set(handles.Plots.SO1agdot,'Xdata',t(end),'YData',ag(1));
                set(handles.Plots.SO2agdot,'Xdata',t(end),'YData',ag(2));
            case 'Initial Conditions'
                if t(end) <= handles.GM.rampTime
                    set(handles.Plots.SO1agdot,'Xdata',t(end), ...
                        'YData',U(1,end)/handles.GM.initialDisp(1)*100);
                    set(handles.Plots.SO2agdot,'Xdata',t(end), ...
                        'YData',U(2,end)/handles.GM.initialDisp(2)*100);
                else
                    set(handles.Plots.SO1agdot,'Xdata',t(end),'YData',0);
                    set(handles.Plots.SO2agdot,'Xdata',t(end),'YData',0);
                end
        end
        
        %Error Monitors
        error = Um-U;
        [f,MX] = GetFFT(error',t(2)-t(1));
        TI(1,:) = 0.5*(cumtrapz(U(1,:),Um(1,:)) - cumtrapz(Um(1,:),U(1,:)));
        TI(2,:) = 0.5*(cumtrapz(U(2,:),Um(2,:)) - cumtrapz(Um(2,:),U(2,:)));
        set(handles.Plots.EM1eplot,'Xdata',t,'YData',error(1,:));
        set(handles.Plots.EM1ffteplot,'Xdata',f,'YData',MX(:,1));
        set(handles.Plots.EM1MeasCmdplot,'Xdata',U(1,:),'YData',Um(1,:));
        set(handles.Plots.EM1MeasCmddot,'Xdata',U(1,end),'YData',Um(1,end));
        set(handles.Plots.EM1trackplot,'Xdata',t,'YData',TI(1,:));
        set(handles.Plots.EM2eplot,'Xdata',t,'YData',error(2,:));
        set(handles.Plots.EM2ffteplot,'Xdata',f,'YData',MX(:,2));
        set(handles.Plots.EM2MeasCmdplot,'Xdata',U(2,:),'YData',Um(2,:));
        set(handles.Plots.EM2MeasCmddot,'Xdata',U(2,end),'YData',Um(2,end));
        set(handles.Plots.EM2trackplot,'Xdata',t,'YData',TI(2,:));
        
        %Animate Structure
        if findobj('Tag','StructAnim')
            set(handles.Plots.StructAnim, ...
                'Vertices',[0.0 0.0 0.0; U(1,end) U(2,end) 10.0], ...
                'FaceVertexCData',[0; norm(U(:,end))]);
        end
        
        % update all the plots
        drawnow;
end
