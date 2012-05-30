function DisplayAxes(action,varargin)
%DISPLAYAXES to expand certain plots for better visibility
% DisplayAxes(action,varargin)
%
% action   : selected plot to expand
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

% check if window already exists
tagFig = ['Display',upper(action)];
tagAxis = ['GM_',upper(action)];
hFig = findobj('Tag',tagFig);
if ~isempty(hFig)
    figure(hFig);
    return
end

% initialization tasks
handles = guidata(findobj('Tag','OpenFrescoExpress'));
SS = handles.Store.SS;

% main figure
hFig = figure('Name',['Display ',upper(action)],...
    'Visible','on',...
    'NumberTitle','off',...
    'MenuBar','none',...
    'Tag',tagFig,...
    'Color',[0.3 0.5 0.7],...
    'Position',[0.277*SS(3) 0.215*SS(4) 0.55*SS(3) 0.5*SS(4)]);
orient(hFig,'landscape');
ModifyPrintSetup(hFig,'PrintUI',0);

% toolbar
File(1) = uimenu('Position',1,'Label','File');
uimenu(File(1),'Position',1,'Label','Save As FIG','Accelerator','M', ...
    'Callback',['PrintWithHeader(''fig'',''',tagAxis,''',[-0.12,0.07,0.07,0.07])']);
uimenu(File(1),'Position',2,'Label','Save As PDF','Accelerator','S', ...
    'Callback',['PrintWithHeader(''pdf'',''',tagAxis,''',[-0.12,0.07,0.07,0.07])']);
uimenu(File(1),'Position',3,'Label','Copy','Accelerator','C', ...
    'Callback',['PrintWithHeader(''clipboard'',''',tagAxis,''',[-0.12,0.07,0.07,0.07])']);
uimenu(File(1),'Position',4,'Label','Print','Accelerator','P', ...
    'Callback',['PrintWithHeader(''printer'',''',tagAxis,''',[-0.12,0.07,0.07,0.07])']);
uimenu('Position',2,'Label','|');
StdMenu(1) = uimenu('Position',3,'Label','MATLAB Menu');
uimenu(StdMenu(1),'Position',1,'Label','Turn on',...
    'Callback','set(gcf,''MenuBar'',''figure''); set(gcf,''Toolbar'',''figure'');');
uimenu(StdMenu(1),'Position',2,'Label','Turn off',...
    'Callback','set(gcf,''MenuBar'',''none''); set(gcf,''Toolbar'',''none'');');

switch action
    % =====================================================================
    case 'ag1'
        if isempty(handles.GM.scalet{1}) || isempty(handles.GM.scaleag{1})
            close(findobj('Tag',tagFig));
            return
        else
            % add pga, pgv and pgd labels
            pga = handles.GM.Spectra{1}.pga;
            pgv = max(abs(handles.GM.scalevg{1}));
            pgd = max(abs(handles.GM.scaledg{1}));
            plot(handles.GM.scalet{1}, handles.GM.scaleag{1}, 'LineWidth', 1.0);
            grid('on');
            xlabel('Time [sec]');
            ylabel('ag [L/sec^2]');
            [~,fileName,fileExt] = fileparts(handles.GM.store.filepath{1});
            title(['Ground Motion File: ',fileName,fileExt]);
            text(0.82*handles.GM.scalet{1}(end),0.85*pga,...
                sprintf('PGA = %1.2f\nPGV = %1.3f\nPGD = %1.4f',pga,pgv,pgd),...
                'BackgroundColor',[1 1 1]);
            set(gca,'Tag',tagAxis, ...
                'XLim',[0,handles.GM.scalet{1}(end)], ...
                'YLim',[-1.1*pga,1.1*pga]);
        end
    % =====================================================================
    case 'Sa1'
        if isempty(handles.GM.Spectra{1})
            close(findobj('Tag',tagFig));
            return
        else
            if length(handles.Model.Zeta) == 1
                plot(handles.GM.Spectra{1}.T, handles.GM.Spectra{1}.psdAcc(:,1), 'LineWidth', 1.0);
            else
                plot(handles.GM.Spectra{1}.T, handles.GM.Spectra{1}.psdAcc, 'LineWidth', 1.0);
            end
            grid('on');
            xlabel('Period [sec]');
            ylabel('Sa [L/sec^2]');
            [~,fileName,fileExt] = fileparts(handles.GM.store.filepath{1});
            title(['Ground Motion File: ',fileName,fileExt]);
            % label Sa values
            if length(handles.Model.T) == 1
                psdAcc = interp1(handles.GM.Spectra{1}.T,handles.GM.Spectra{1}.psdAcc(:,1),handles.Model.T);
                text(handles.Model.T*1.2,psdAcc*1.1,sprintf('T = %1.4f\nSa = %1.2f',handles.Model.T,psdAcc),'BackgroundColor',[1 1 1]);
                hold('on');
                plot(handles.Model.T,psdAcc,'ro', 'LineWidth', 1.0);
            else
                if length(handles.Model.Zeta) == 1
                    psdAcc1 = interp1(handles.GM.Spectra{1}.T,handles.GM.Spectra{1}.psdAcc(:,1),handles.Model.T(1));
                    psdAcc2 = interp1(handles.GM.Spectra{1}.T,handles.GM.Spectra{1}.psdAcc(:,1),handles.Model.T(2));
                else
                    psdAcc1 = interp1(handles.GM.Spectra{1}.T,handles.GM.Spectra{1}.psdAcc(:,1),handles.Model.T(1));
                    psdAcc2 = interp1(handles.GM.Spectra{1}.T,handles.GM.Spectra{1}.psdAcc(:,2),handles.Model.T(2));
                    legend(sprintf([num2str(handles.Model.Zeta(1)) '%% Damping']),sprintf([num2str(handles.Model.Zeta(2)) '%% Damping']),'Location','NorthEast');
                end
                if psdAcc1 > psdAcc2
                    text(handles.Model.T(1)*1.1,psdAcc1*1.1,sprintf('T_1 = %1.4f\nSa_1 = %1.2f',handles.Model.T(1),psdAcc1),'BackgroundColor',[1 1 1]);
                    text(handles.Model.T(2)*0.9,psdAcc2*0.8,sprintf('T_2 = %1.4f\nSa_2 = %1.2f',handles.Model.T(2),psdAcc2),'BackgroundColor',[1 1 1]);
                else
                    text(handles.Model.T(1)*1.1,psdAcc1*0.8,sprintf('T_1 = %1.4f\nSa_1 = %1.2f',handles.Model.T(1),psdAcc1),'BackgroundColor',[1 1 1]);
                    text(handles.Model.T(2)*0.9,psdAcc2*1.1,sprintf('T_2 = %1.4f\nSa_2 = %1.2f',handles.Model.T(2),psdAcc2),'BackgroundColor',[1 1 1]);
                end
                hold('on');
                plot(handles.Model.T(1),psdAcc1,'ro', 'LineWidth', 1.0);
                plot(handles.Model.T(2),psdAcc2,'ro', 'LineWidth', 1.0);
            end
            set(gca,'Tag',tagAxis);
        end
    % =====================================================================
    case 'Sd1'
        if isempty(handles.GM.Spectra{1})
            close(findobj('Tag',tagFig));
            return
        else
            if length(handles.Model.Zeta) == 1
                plot(handles.GM.Spectra{1}.T, handles.GM.Spectra{1}.dsp(:,1), 'LineWidth', 1.0);
            else
                plot(handles.GM.Spectra{1}.T, handles.GM.Spectra{1}.dsp, 'LineWidth', 1.0);
            end
            grid('on');
            xlabel('Period [sec]');
            ylabel('Sd [L]');
            [~,fileName,fileExt] = fileparts(handles.GM.store.filepath{1});
            title(['Ground Motion File: ',fileName,fileExt]);
            % label Sd values
            if length(handles.Model.T) == 1
                dsp = interp1(handles.GM.Spectra{1}.T,handles.GM.Spectra{1}.dsp(:,1),handles.Model.T);
                text(handles.Model.T*1.2,dsp*1.1,sprintf('T = %1.4f\nSd = %1.4f',handles.Model.T,dsp),'BackgroundColor',[1 1 1]);
                hold('on');
                plot(handles.Model.T,dsp,'ro', 'LineWidth', 1.0);
            else
                if length(handles.Model.Zeta) == 1
                    dsp1 = interp1(handles.GM.Spectra{1}.T,handles.GM.Spectra{1}.dsp(:,1),handles.Model.T(1));
                    dsp2 = interp1(handles.GM.Spectra{1}.T,handles.GM.Spectra{1}.dsp(:,1),handles.Model.T(2));
                else
                    dsp1 = interp1(handles.GM.Spectra{1}.T,handles.GM.Spectra{1}.dsp(:,1),handles.Model.T(1));
                    dsp2 = interp1(handles.GM.Spectra{1}.T,handles.GM.Spectra{1}.dsp(:,2),handles.Model.T(2));
                    legend(sprintf([num2str(handles.Model.Zeta(1)) '%% Damping']),sprintf([num2str(handles.Model.Zeta(2)) '%% Damping']),'Location','NorthWest');
                end
                text(handles.Model.T(1)*1.1,dsp1*1.1,sprintf('T_1 = %1.4f\nSd_1 = %1.4f',handles.Model.T(1),dsp1),'BackgroundColor',[1 1 1]);
                text(handles.Model.T(2)*1.3,dsp2*1.25,sprintf('T_2 = %1.4f\nSd_2 = %1.4f',handles.Model.T(2),dsp2),'BackgroundColor',[1 1 1]);
                hold('on');
                plot(handles.Model.T(1),dsp1,'ro', 'LineWidth', 1.0);
                plot(handles.Model.T(2),dsp2,'ro', 'LineWidth', 1.0);
            end
            set(gca,'Tag',tagAxis);
        end
    % =====================================================================
    case 'ag2'
        if isempty(handles.GM.scalet{2}) || isempty(handles.GM.scaleag{2})
            close(findobj('Tag',tagFig));
            return
        else
            % add pga, pgv and pgd labels
            pga = handles.GM.Spectra{2}.pga;
            pgv = max(abs(handles.GM.scalevg{2}));
            pgd = max(abs(handles.GM.scaledg{2}));
            plot(handles.GM.scalet{2}, handles.GM.scaleag{2}, 'LineWidth', 1.0);
            grid('on');
            xlabel('Time [sec]');
            ylabel('ag [L/sec^2]');
            [~,fileName,fileExt] = fileparts(handles.GM.store.filepath{2});
            title(['Ground Motion File: ',fileName,fileExt]);
            text(0.82*handles.GM.scalet{2}(end),0.85*pga,...
                sprintf('PGA = %1.2f\nPGV = %1.3f\nPGD = %1.4f',pga,pgv,pgd),...
                'BackgroundColor',[1 1 1]);
            set(gca,'Tag',tagAxis, ...
                'XLim',[0,handles.GM.scalet{2}(end)], ...
                'YLim',[-1.1*pga,1.1*pga]);
        end
    % =====================================================================
    case 'Sa2'
        if isempty(handles.GM.Spectra{2})
            close(findobj('Tag',tagFig));
            return
        else
            if length(handles.Model.Zeta) == 1
                plot(handles.GM.Spectra{2}.T, handles.GM.Spectra{2}.psdAcc(:,1), 'LineWidth', 1.0);
            else
                plot(handles.GM.Spectra{2}.T, handles.GM.Spectra{2}.psdAcc, 'LineWidth', 1.0);
            end
            grid('on');
            xlabel('Period [sec]');
            ylabel('Sa [L/sec^2]');
            [~,fileName,fileExt] = fileparts(handles.GM.store.filepath{2});
            title(['Ground Motion File: ',fileName,fileExt]);
            % label Sa values
            if length(handles.Model.T) == 1
                psdAcc = interp1(handles.GM.Spectra{2}.T,handles.GM.Spectra{2}.psdAcc(:,1),handles.Model.T);
                text(handles.Model.T*1.2,psdAcc*1.1,sprintf('T = %1.4f\nSa = %1.2f',handles.Model.T,psdAcc),'BackgroundColor',[1 1 1]);
                hold('on');
                plot(handles.Model.T,psdAcc,'ro', 'LineWidth', 1.0);
            else
                if length(handles.Model.Zeta) == 1
                    psdAcc1 = interp1(handles.GM.Spectra{2}.T,handles.GM.Spectra{2}.psdAcc(:,1),handles.Model.T(1));
                    psdAcc2 = interp1(handles.GM.Spectra{2}.T,handles.GM.Spectra{2}.psdAcc(:,1),handles.Model.T(2));
                else
                    psdAcc1 = interp1(handles.GM.Spectra{2}.T,handles.GM.Spectra{2}.psdAcc(:,1),handles.Model.T(1));
                    psdAcc2 = interp1(handles.GM.Spectra{2}.T,handles.GM.Spectra{2}.psdAcc(:,2),handles.Model.T(2));
                    legend(sprintf([num2str(handles.Model.Zeta(1)) '%% Damping']),sprintf([num2str(handles.Model.Zeta(2)) '%% Damping']),'Location','NorthEast');
                end
                if psdAcc1 > psdAcc2
                    text(handles.Model.T(1)*1.1,psdAcc1*1.1,sprintf('T_1 = %1.4f\nSa_1 = %1.2f',handles.Model.T(1),psdAcc1),'BackgroundColor',[1 1 1]);
                    text(handles.Model.T(2)*0.9,psdAcc2*0.8,sprintf('T_2 = %1.4f\nSa_2 = %1.2f',handles.Model.T(2),psdAcc2),'BackgroundColor',[1 1 1]);
                else
                    text(handles.Model.T(1)*1.1,psdAcc1*0.95,sprintf('T_1 = %1.4f\nSa_1 = %1.2f',handles.Model.T(1),psdAcc1),'BackgroundColor',[1 1 1]);
                    text(handles.Model.T(2)*1.1,psdAcc2*1.1,sprintf('T_2 = %1.4f\nSa_2 = %1.2f',handles.Model.T(2),psdAcc2),'BackgroundColor',[1 1 1]);
                end
                hold('on');
                plot(handles.Model.T(1),psdAcc1,'ro', 'LineWidth', 1.0);
                plot(handles.Model.T(2),psdAcc2,'ro', 'LineWidth', 1.0);
            end
            set(gca,'Tag',tagAxis);
        end
    % =====================================================================
    case 'Sd2'
        if isempty(handles.GM.Spectra{2})
            close(findobj('Tag',tagFig));
            return
        else
            if length(handles.Model.Zeta) == 1
                plot(handles.GM.Spectra{2}.T, handles.GM.Spectra{2}.dsp(:,1), 'LineWidth', 1.0);
            else
                plot(handles.GM.Spectra{2}.T, handles.GM.Spectra{2}.dsp, 'LineWidth', 1.0);
            end
            grid('on');
            xlabel('Period [sec]');
            ylabel('Sd [L]');
            [~,fileName,fileExt] = fileparts(handles.GM.store.filepath{2});
            title(['Ground Motion File: ',fileName,fileExt]);
            % label Sd values
            if length(handles.Model.T) == 1
                dsp = interp1(handles.GM.Spectra{2}.T,handles.GM.Spectra{2}.dsp(:,1),handles.Model.T);
                text(handles.Model.T*1.2,dsp*1.1,sprintf('T = %1.4f\nSa = %1.4f',handles.Model.T,dsp),'BackgroundColor',[1 1 1]);
                hold('on');
                plot(handles.Model.T,dsp,'ro', 'LineWidth', 1.0);
            else
                if length(handles.Model.Zeta) == 1
                    dsp1 = interp1(handles.GM.Spectra{2}.T,handles.GM.Spectra{2}.dsp(:,1),handles.Model.T(1));
                    dsp2 = interp1(handles.GM.Spectra{2}.T,handles.GM.Spectra{2}.dsp(:,1),handles.Model.T(2));
                else
                    dsp1 = interp1(handles.GM.Spectra{2}.T,handles.GM.Spectra{2}.dsp(:,1),handles.Model.T(1));
                    dsp2 = interp1(handles.GM.Spectra{2}.T,handles.GM.Spectra{2}.dsp(:,2),handles.Model.T(2));
                    legend(sprintf([num2str(handles.Model.Zeta(1)) '%% Damping']),sprintf([num2str(handles.Model.Zeta(2)) '%% Damping']),'Location','NorthWest');
                end
                text(handles.Model.T(1)*1.1,dsp1*1.1,sprintf('T_1 = %1.4f\nSd_1 = %1.4f',handles.Model.T(1),dsp1),'BackgroundColor',[1 1 1]);
                text(handles.Model.T(2)*1.3,dsp2*1.25,sprintf('T_2 = %1.4f\nSd_2 = %1.4f',handles.Model.T(2),dsp2),'BackgroundColor',[1 1 1]);
                hold('on');
                plot(handles.Model.T(1),dsp1,'ro', 'LineWidth', 1.0);
                plot(handles.Model.T(2),dsp2,'ro', 'LineWidth', 1.0);
            end
            set(gca,'Tag',tagAxis);
        end
    % =====================================================================
end
