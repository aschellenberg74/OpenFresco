% Plotting of the output from OpenSees

clear;
close all;
clc;

path = pwd;
%path = [pwd,'\TestLabVIEW'];
%path = [pwd,'\Results\070411_MiniMost\Run01'];
%path = [pwd,'\FastDistributed\Results\070608_NEESinc_Run001'];
%path = [pwd,'\OxfordUK\080104_Run004_Local'];
%path = 'C:\Documents and Settings\Andreas\My Documents\OpenFresco\trunk\WIN32\bin';
%path = 'C:\Documents and Settings\Andreas\My Documents\OpenFresco\trunk\EXAMPLES\OneBayFrame\OpenSees';
%path = 'C:\Documents and Settings\Andreas\My Documents\OpenFresco\trunk\EXAMPLES\OneBayFrame\TestSignalFilter';
%path = 'C:\Documents and Settings\Andreas\My Documents\OpenFresco\trunk\EXAMPLES\OneBayFrame\TestRecorder';
%path = 'C:\Documents and Settings\Andreas\My Documents\Dissertation\Force control\NewCode';
printFlag = 0;

SS = get(0,'screensize');
printID = 1;

% get ground-acceleration
ag = zeros(1599,1);
ag(1:1560) = load([pwd,'\elcentro.txt']);
ag = 386.1*ag;

% get node data
data = load([path,'\Node_Dsp.out']);
tN = data(:,1);
dN = data(:,[2,3]);
data = load([path,'\Node_Vel.out']);
vN = data(:,[2,3]);
data = load([path,'\Node_Acc.out']);
aN = data(:,[2,3]);

% plot displacement time histories
createWindow('cen',0.80*SS(4)/3*4,0.40*SS(4));
%orient tall
%subplot(3,1,1);
plot(tN,dN(:,1),'b-','LineWidth',1.0);
hold('on');
plot(tN,dN(:,2),'r-','LineWidth',1.0);
grid('on');
%axis([0,32,-2.5,2.5]);
xlabel('Time [sec]');
ylabel('Displacement [in.]');
%title('Displacement-Time-Histories');
legend('Top Left Column','Top Right Column');

% plot velocity time histories
createWindow('cen',0.80*SS(4)/3*4,0.40*SS(4));
%subplot(3,1,2);
plot(tN,vN(:,1),'b-','LineWidth',1.0);
hold('on');
plot(tN,vN(:,2),'r-','LineWidth',1.0);
grid('on');
%axis([0,32,-25,25]);
xlabel('Time [sec]');
ylabel('Velocity [in./sec]');
%title('Velocity-Time-Histories');
legend('Top Left Column','Top Right Column');

% plot acceleration time histories
createWindow('cen',0.80*SS(4)/3*4,0.40*SS(4));
%subplot(3,1,3);
plot(tN,aN(:,1),'b-','LineWidth',1.0);
hold('on');
plot(tN,aN(:,2),'r-','LineWidth',1.0);
grid('on');
%axis([0,32,-300,300]);
xlabel('Time [sec]');
ylabel('Acceleration [in./sec^2]');
%title('Acceleration-Time-Histories');
legend('Top Left Column','Top Right Column');
% print the figure
if (printFlag==1)
    print;
elseif (printFlag==2)
    print;
    close;
elseif (printFlag==3)
    print('-depsc2','-loose',['Fig',num2str(printID),'.eps']);
    printID = printID + 1;
    close;
end

if 1
    % get element data
    try %#ok<*TRYNC>
        data = load([path,'\Elmt_tangStif.out']);
        kE = data(:,2);
    end
    data = load([path,'\Elmt_ctrlDsp.out']);
    dtE = -data(:,[2,3]);
    data = load([path,'\Elmt_daqDsp.out']);
    dmE = -data(:,[2,3]);
    data = load([path,'\Elmt_Frc.out']);
    fE = data(:,[4,8]);
    %fE = data(:,[2,4]);
    tE = data(:,1);
    dE = dN;
    
    if 1
        % plot deformation time histories
        createWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
        orient landscape
        plot(tE,dtE(:,1),'c-','LineWidth',1.0);
        hold('on');
        plot(tE,dmE(:,1),'b-','LineWidth',0.5);
        plot(tE,dtE(:,2),'m-','LineWidth',1.0);
        plot(tE,dmE(:,2),'r-','LineWidth',0.5);
        grid('on');
        %axis([0,32,-2.5,2.5]);
        xlabel ('Time [sec]');
        ylabel('Deformation [in.]');
        title('Deformation-Time-Histories');
        legend('Left Column ctrl','Left Column daq','Right Column ctrl','Right Column daq');
        % print the figure
        if (printFlag==1)
            print;
        elseif (printFlag==2)
            print;
            close;
        elseif (printFlag==3)
            print('-depsc2','-loose',['Fig',num2str(printID),'.eps']);
            printID = printID + 1;
            close;
        end
    end
    
    % plot force time histories
    createWindow('cen',0.80*SS(4)/3*4,0.40*SS(4));
    orient landscape
    plot(tE,fE(:,1),'b-','LineWidth',1.0);
    hold('on');
    plot(tE,fE(:,2),'r-','LineWidth',1.0);
    grid('on');
    %axis([0,32,-5,5]);
    xlabel ('Time [sec]');
    ylabel('Resisting Force [kip]');
    title('Force-Time-Histories');
    legend('Left Column','Right Column');
    % print the figure
    if (printFlag==1)
        print;
    elseif (printFlag==2)
        print;
        close;
    elseif (printFlag==3)
        print('-depsc2','-loose',['Fig',num2str(printID),'.eps']);
        printID = printID + 1;
        close;
    end
    
    % plot hysteresis loops
    createWindow('cen',0.40*SS(4)/3*4,0.40*SS(4));
    orient landscape
    plot(dE(:,1),fE(:,1),'b-','LineWidth',1.0);
    hold('on');
    plot(dE(:,2),fE(:,2),'r-','LineWidth',1.0);
    grid('on');
    xlabel ('Deformation [in.]');
    ylabel('Resisting Force [kip]');
    title('Hysteresis-Loops of Columns');
    legend('Left Column','Right Column');
    % print the figure
    if (printFlag==1)
        print;
    elseif (printFlag==2)
        print;
        close;
    elseif (printFlag==3)
        print('-depsc2','-loose',['Fig',num2str(printID),'.eps']);
        printID = printID + 1;
        close;
    end
    
    % plot stiffness time history
    if exist('kE','var')
        createWindow('cen',0.80*SS(4)/3*4,0.40*SS(4));
        orient landscape
        plot(tE,kE(:,1),'b-','LineWidth',1.0);
        hold('on');
        grid('on');
        %axis([0,32,-5,5]);
        xlabel ('Time [sec]');
        ylabel('Tangent Stiffness [kip/in.]');
        title('Stiffness-Time-Histories');
        legend('Left Column');
        % print the figure
        if (printFlag==1)
            print;
        elseif (printFlag==2)
            print;
            close;
        elseif (printFlag==3)
            print('-depsc2','-loose',['Fig',num2str(printID),'.eps']);
            printID = printID + 1;
            close;
        end
    end
end

if 0
    % plot acceleration time histories
    vmE = zeros(1599,2);
    amE = zeros(1599,2);
    vmE(2:end,:) = 1/0.02*diff(dmE);
    amE(1:end-1,:) = 1/0.02*diff(vmE);
    createWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
    orient landscape
    plot(tN,ag,'b-');
    hold('on');
    plot(tN,aN(:,1),'r-');
    plot(tN,amE(:,1),'g-');
    grid('on');
    %axis([0,32,-2.5,2.5]);
    xlabel('Time [sec]');
    ylabel('Acceleration [in./sec^2]');
    title('Acceleration-Time-Histories');
    
    % plot inertia force time histories
    mSpecimen = 0.0493/386.1;
    createWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
    plot(tN,mSpecimen*amE(:,1),'r-');
    hold('on');
    plot(tN,0.04*aN(:,1),'b-');
    grid('on');
    %axis([0,32,-300,300]);
    xlabel('Time [sec]');
    ylabel('Inertia Force [kip]');
    title('Inertia-Force-Time-History');
    % print the figure
    if (printFlag==1)
        print;
    elseif (printFlag==2)
        print;
        close;
    elseif (printFlag==3)
        print('-depsc2','-loose',['Fig',num2str(printID),'.eps']);
        printID = printID + 1;
        close;
    end
    max(abs(mSpecimen*amE(:,1)))/max(abs(0.04*aN(:,1)))*100
end

if 0
    % plot histogram of execution times
    fid = fopen([path,'\elapsedTime.txt'],'r');
    data = textscan(fid,'%f%*[^\n]');
    time = 1E-3.*data{1};
    dtMin = min(time(3:end))
    dtMax = max(time(3:end))
    dtAvg = mean(time(3:end))
    dtStd = std(time(3:end))
    tTot  = 1E-3*sum(time(3:end))
    fclose(fid);
    
    createWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
    orient landscape
    hist(time,200);
    h = findobj(gca,'Type','patch');
    set(h,'FaceColor','b','EdgeColor','w');
    hold('on');
    n = hist(time,200);
    plot([dtAvg,dtAvg],[0,max(n)],'r-','LineWidth',0.5);
    text(dtAvg,0.9*max(n),['Average Step Duration: ',num2str(dtAvg),' msec']);
    grid('on');
    %axis([0,40,0,1600]);
    xlabel ('Integration Step Duration [msec]');
    ylabel('Number of Steps [-]');
    title('Distribution of Integration Step Duration');
    % print the figure
    if (printFlag==1)
        print;
    elseif (printFlag==2)
        print;
        close;
    elseif (printFlag==3)
        print('-depsc2','-loose',['Fig',num2str(printID),'.eps']);
        printID = printID + 1;
        close;
    end
    
    createWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
    orient landscape
    plot(tN(3:end),time(3:end),'b-','LineWidth',1.0);
    grid('on');
    %axis([0,35,0,110]);
    h = gca;
    %set(h,'YScale','log');
    xlabel ('Time [sec]')
    ylabel ('Integration Step Duration [msec]');
    title('Integration Step Duration Time-History');
    % print the figure
    if (printFlag==1)
        print;
    elseif (printFlag==2)
        print;
        close;
    elseif (printFlag==3)
        print('-depsc2','-loose',['Fig',num2str(printID),'.eps']);
        printID = printID + 1;
        close;
    end
end

% plot STS data if available
if 0
    % get STS data
    data = load([path,'\STS_data.txt']);
    f = 256;
    dt = 1/f;
    tSTS = (1:size(data,1))'*dt;
    dispCmd = -data(:,1)+data(1,1);
    dispFbk = -data(:,3)+data(1,1);
    forceFbk = -data(:,5);
    
    % plot displacement time histories
    figure(6);
    subplot(2,1,1);
    plot(tSTS,dispCmd,'b-');
    hold('on');
    plot(tSTS,dispFbk,'r-');
    grid('on');
    xlabel('Time [sec]')
    ylabel('Displacement [in.]');
    title('STS Displacement-Time-Histories of Right Column');
    legend('Right Column cmd','Right Column fbk');
    
    subplot(2,1,2);
    plot(tE,2.54*dtE(:,2),'b-');
    hold('on');
    grid('on');
    xlabel('Time [sec]')
    ylabel('Displacement [in.]');
    title('Displacement-Time-Histories of Right Column');
    %legend('Right Column cmd','Right Column fbk');
    
    % plot force time history
    figure(7);
    plot(tSTS,forceFbk,'b-');
    hold('on');
    grid('on');
    xlabel ('Time [sec]')
    ylabel('Resisting Force [kip]');
    title('STS Force-Time-History of Right Column');
    
    % plot hysteresis loop
    figure(8);
    plot(dispFbk,forceFbk,'b-');
    hold('on');
    grid('on');
    xlabel ('Deformation [in]')
    ylabel('Resisting Force [kip]');
    title('STS Hysteresis-Loop of Right Column');
end

