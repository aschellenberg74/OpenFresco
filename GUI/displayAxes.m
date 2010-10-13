function displayAxes(action, varargin)

handles = guidata(gcf);

switch action
    case 'ag1'
        if length(handles.GM.scalet{1}) == 1 || length(handles.GM.scaleag{1}) == 1
            return
        else
            pga = handles.GM.Spectra{1}.pga;
            figure;
            plot(handles.GM.scalet{1}, handles.GM.scaleag{1});
            grid('on');
            xlabel('Time [sec]');
            ylabel('ag [L/sec^2]');
            text(0.7*handles.GM.scalet{1}(end),0.94*pga,sprintf('PGA: %1.4E',pga));
        end
        
    case 'Sa1'
        if ~isfield(handles.GM.Spectra{1}, 'T') || ~isfield(handles.GM.Spectra{1}, 'psdAcc');
            return
        else
            figure;
            if length(handles.Model.Zeta) == 1
                plot(handles.GM.Spectra{1}.T, handles.GM.Spectra{1}.psdAcc(:,1));
            else
                plot(handles.GM.Spectra{1}.T, handles.GM.Spectra{1}.psdAcc);
            end
            grid('on');
            xlabel('Period [sec]');
            ylabel('Sa [L/sec^2]');
            id1 = [];
            id2 = [];
            tol = 0.01;
            if length(handles.Model.T) == 1
                while isempty(id1)
                    id1 = find(abs(handles.GM.Spectra{1}.T-handles.Model.T) < tol);
                    tol = tol + 0.01;
                end
                psdAcc = handles.GM.Spectra{1}.psdAcc(id1(1),1);
                text(handles.Model.T*1.2,psdAcc*1.1,sprintf('T = %1.4E\nSa = %1.4E',handles.Model.T,psdAcc));
                hold on
                plot(handles.Model.T,psdAcc,'ro');
                hold off
            else
                while isempty(id1)
                    id1 = find(abs(handles.GM.Spectra{1}.T-handles.Model.T(1)) < tol);
                    tol = tol + 0.01;
                end
                while isempty(id2)
                    id2 = find(abs(handles.GM.Spectra{1}.T-handles.Model.T(2)) < tol);
                    tol = tol + 0.01;
                end
                if length(handles.Model.Zeta) == 1
                    psdAcc1 = handles.GM.Spectra{1}.psdAcc(id1(1),1);
                    psdAcc2 = handles.GM.Spectra{1}.psdAcc(id2(1),1);
                else
                    psdAcc1 = handles.GM.Spectra{1}.psdAcc(id1(1),1);
                    psdAcc2 = handles.GM.Spectra{1}.psdAcc(id2(1),2);
                end
                if psdAcc1 > psdAcc2
                    text(handles.Model.T(1)*1.1,psdAcc1*1.1,sprintf('T = %1.4E\nSa = %1.4E',handles.Model.T(1),psdAcc1));
                    text(handles.Model.T(2)*0.9,psdAcc2*0.8,sprintf('T = %1.4E\nSa = %1.4E',handles.Model.T(2),psdAcc2));
                else
                    text(handles.Model.T(1)*1.1,psdAcc1*0.8,sprintf('T = %1.4E\nSa = %1.4E',handles.Model.T(1),psdAcc1));
                    text(handles.Model.T(2)*0.9,psdAcc2*1.1,sprintf('T = %1.4E\nSa = %1.4E',handles.Model.T(2),psdAcc2));
                end
                hold on
                plot(handles.Model.T(1),psdAcc1,'ro');
                plot(handles.Model.T(2),psdAcc2,'ro');
                hold off
            end
        end
        
    case 'Sd1'
        if ~isfield(handles.GM.Spectra{1}, 'T') || ~isfield(handles.GM.Spectra{1}, 'dsp');
            return
        else
            figure;
            if length(handles.Model.Zeta) == 1
                plot(handles.GM.Spectra{1}.T, handles.GM.Spectra{1}.dsp(:,1));
            else
                plot(handles.GM.Spectra{1}.T, handles.GM.Spectra{1}.dsp);
            end
            grid('on');
            xlabel('Period [sec]');
            ylabel('Sd [L]');
            id1 = [];
            id2 = [];
            tol = 0.01;
            if length(handles.Model.T) == 1
                while isempty(id1)
                    id1 = find(abs(handles.GM.Spectra{1}.T-handles.Model.T) < tol);
                    tol = tol + 0.01;
                end
                dsp = handles.GM.Spectra{1}.dsp(id1(1),1);
                text(handles.Model.T*1.2,dsp*1.1,sprintf('T = %1.4E\nSa = %1.4E',handles.Model.T,dsp));
                hold on
                plot(handles.Model.T,dsp,'ro');
                hold off
            else
                while isempty(id1)
                    id1 = find(abs(handles.GM.Spectra{1}.T-handles.Model.T(1)) < tol);
                    tol = tol + 0.01;
                end
                while isempty(id2)
                    id2 = find(abs(handles.GM.Spectra{1}.T-handles.Model.T(2)) < tol);
                    tol = tol + 0.01;
                end
                if length(handles.Model.Zeta) == 1
                    dsp1 = handles.GM.Spectra{1}.dsp(id1(1),1);
                    dsp2 = handles.GM.Spectra{1}.dsp(id2(1),1);
                else
                    dsp1 = handles.GM.Spectra{1}.dsp(id1(1),1);
                    dsp2 = handles.GM.Spectra{1}.dsp(id2(1),2);
                end
                text(handles.Model.T(1)*1.1,dsp1*1.1,sprintf('T = %1.4E\nSd = %1.4E',handles.Model.T(1),dsp1));
                text(handles.Model.T(2)*1.3,dsp2,sprintf('T = %1.4E\nSd = %1.4E',handles.Model.T(2),dsp2));
                hold on
                plot(handles.Model.T(1),dsp1,'ro');
                plot(handles.Model.T(2),dsp2,'ro');
                hold off
            end
        end
        
    case 'ag2'
        if length(handles.GM.scalet) == 1 || length(handles.GM.scaleag) == 1
            return
        else
            pga = handles.GM.Spectra{2}.pga;
            figure;
            plot(handles.GM.scalet{2}, handles.GM.scaleag{2});
            grid('on');
            xlabel('Time [sec]');
            ylabel('ag [L/sec^2]');
            text(0.7*handles.GM.scalet{2}(end),0.94*pga,sprintf('PGA: %1.4E',pga));
        end
        
    case 'Sa2'
        if length(handles.GM.Spectra) == 1
            return
        else
            figure;
            if length(handles.Model.Zeta) == 1
                plot(handles.GM.Spectra{2}.T, handles.GM.Spectra{2}.psdAcc(:,1));
            else
                plot(handles.GM.Spectra{2}.T, handles.GM.Spectra{2}.psdAcc);
            end
            grid('on');
            xlabel('Period [sec]');
            ylabel('Sa [L/sec^2]');
            id1 = [];
            id2 = [];
            tol = 0.01;
            if length(handles.Model.T) == 1
                while isempty(id1)
                    id1 = find(abs(handles.GM.Spectra{2}.T-handles.Model.T) < tol);
                    tol = tol + 0.01;
                end
                psdAcc = handles.GM.Spectra{2}.psdAcc(id1(1),1);
                text(handles.Model.T*1.2,psdAcc*1.1,sprintf('T = %1.4E\nSa = %1.4E',handles.Model.T,psdAcc));
                hold on
                plot(handles.Model.T,psdAcc,'ro');
                hold off
            else
                while isempty(id1)
                    id1 = find(abs(handles.GM.Spectra{2}.T-handles.Model.T(1)) < tol);
                    tol = tol + 0.01;
                end
                while isempty(id2)
                    id2 = find(abs(handles.GM.Spectra{2}.T-handles.Model.T(2)) < tol);
                    tol = tol + 0.01;
                end
                if length(handles.Model.Zeta) == 1
                    psdAcc1 = handles.GM.Spectra{2}.psdAcc(id1(1),1);
                    psdAcc2 = handles.GM.Spectra{2}.psdAcc(id2(1),1);
                else
                    psdAcc1 = handles.GM.Spectra{2}.psdAcc(id1(1),1);
                    psdAcc2 = handles.GM.Spectra{2}.psdAcc(id2(1),2);
                end
                if psdAcc1 > psdAcc2
                    text(handles.Model.T(1)*1.1,psdAcc1*1.1,sprintf('T = %1.4E\nSa = %1.4E',handles.Model.T(1),psdAcc1));
                    text(handles.Model.T(2)*0.9,psdAcc2*0.8,sprintf('T = %1.4E\nSa = %1.4E',handles.Model.T(2),psdAcc2));
                else
                    text(handles.Model.T(1)*1.1,psdAcc1*0.95,sprintf('T = %1.4E\nSa = %1.4E',handles.Model.T(1),psdAcc1));
                    text(handles.Model.T(2)*1.1,psdAcc2*1.1,sprintf('T = %1.4E\nSa = %1.4E',handles.Model.T(2),psdAcc2));
                end
                hold on
                plot(handles.Model.T(1),psdAcc1,'ro');
                plot(handles.Model.T(2),psdAcc2,'ro');
                hold off
            end
        end
        
    case 'Sd2'
        if length(handles.GM.Spectra) == 1
            return
        else
            figure;
            if length(handles.Model.Zeta) == 1
                plot(handles.GM.Spectra{2}.T, handles.GM.Spectra{2}.dsp(:,1));
            else
                plot(handles.GM.Spectra{2}.T, handles.GM.Spectra{2}.dsp);
            end
            grid('on');
            xlabel('Period [sec]');
            ylabel('Sd [L]');
            id1 = [];
            id2 = [];
            tol = 0.01;
            if length(handles.Model.T) == 1
                while isempty(id1)
                    id1 = find(abs(handles.GM.Spectra{2}.T-handles.Model.T) < tol);
                    tol = tol + 0.01;
                end
                dsp = handles.GM.Spectra{2}.dsp(id1(1),1);
                text(handles.Model.T*1.2,dsp*1.1,sprintf('T = %1.4E\nSa = %1.4E',handles.Model.T,dsp));
                hold on
                plot(handles.Model.T,dsp,'ro');
                hold off
            else
                while isempty(id1)
                    id1 = find(abs(handles.GM.Spectra{2}.T-handles.Model.T(1)) < tol);
                    tol = tol + 0.01;
                end
                while isempty(id2)
                    id2 = find(abs(handles.GM.Spectra{2}.T-handles.Model.T(2)) < tol);
                    tol = tol + 0.01;
                end
                if length(handles.Model.Zeta) == 1
                    dsp1 = handles.GM.Spectra{2}.dsp(id1(1),1);
                    dsp2 = handles.GM.Spectra{2}.dsp(id2(1),1);
                else
                    dsp1 = handles.GM.Spectra{2}.dsp(id1(1),1);
                    dsp2 = handles.GM.Spectra{2}.dsp(id2(1),2);
                end
                text(handles.Model.T(1)*1.1,dsp1*1.1,sprintf('T = %1.4E\nSd = %1.4E',handles.Model.T(1),dsp1));
                text(handles.Model.T(2)*1.3,dsp2,sprintf('T = %1.4E\nSd = %1.4E',handles.Model.T(2),dsp2));
                hold on
                plot(handles.Model.T(1),dsp1,'ro');
                plot(handles.Model.T(2),dsp2,'ro');
                hold off
            end
        end
end
end




