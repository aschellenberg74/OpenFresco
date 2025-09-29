% File: PlotOutput.m
%
% Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
% Created: 10/07
% Revision: A
%
% Purpose: this file plots the OpenSees/OpenFresco output from
% a local hybrid simulation of a portal frame with
% two experimental beamColumn elements.
% The frame can be analyzed with or without gravity loads.
% The specimens are simulated using the SimUniaxialMaterials
% controller.

clear;
close all;
clc;

path = pwd;
printFlag = 0;

SS = get(0,'screensize');
printID = 1;

% get node data
data = load([path,'\Node_Dsp.out']);
tN = data(:,1);
%dN = data(:,2);
dN = data(:,[2,5]);
data = load([path,'\Node_Rxn.out']);
%fRxnN = -data(:,2);
fRxnN = -data(:,[2,5]);
data = load([path,'\Node_Vel.out']);
%vN = data(:,2);
vN = data(:,[2,5]);
data = load([path,'\Node_Acc.out']);
%aN = data(:,2);
aN = data(:,[2,5]);

% plot displacement histories
createWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
orient tall
subplot(3,1,1);
plot(tN,dN(:,1),'-','Color','b','LineWidth',1.0);
hold('on');
plot(tN,dN(:,2),'r-','LineWidth',1.0);
grid('on');
%axis([0,32,-2.5,2.5]);
xlabel('Time [sec]');
ylabel('Displacement [in.]');
title('Displacement-Histories');
legend('Top Left Column','Top Right Column');

% plot velocity histories
subplot(3,1,2);
plot(tN,vN(:,1),'-','Color','b','LineWidth',1.0);
hold('on');
plot(tN,vN(:,2),'r-','LineWidth',1.0);
grid('on');
%axis([0,32,-25,25]);
xlabel('Time [sec]');
ylabel('Velocity [in./sec]');
title('Velocity-Histories');
legend('Top Left Column','Top Right Column');

% plot acceleration histories
subplot(3,1,3);
plot(tN,aN(:,1),'-','Color','b','LineWidth',1.0);
hold('on');
plot(tN,aN(:,2),'r-','LineWidth',1.0);
grid('on');
%axis([0,32,-300,300]);
xlabel('Time [sec]');
ylabel('Acceleration [in./sec^2]');
title('Acceleration-Histories');
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
   data = load([path,'\Ctrl_ctrlDsp.out']);
   dtE = -data(:,[2,3]);
   data = load([path,'\Ctrl_daqDsp.out']);
   dmE = -data(:,[2,3]);
   data = load([path,'\Ctrl_daqFrc.out']);
   fbE = -data(:,[2,3]);
   data = load([path,'\Elmt_glbFrc.out']);
   fgE = data(:,[2,8]);
   tE = data(:,1);
   dE = dN;
   
   % plot deformation histories
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
   title('Deformation-Histories');
   legend('Left Column targ','Left Column meas','Right Column targ','Right Column meas');
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

   % plot force histories
   createWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
   orient landscape
   plot(tE,fgE(:,1),'b-','LineWidth',1.0);
   hold('on');
   plot(tE,fgE(:,2),'r-','LineWidth',1.0);
   grid('on');
   %axis([0,32,-5,5]);
   xlabel ('Time [sec]');
   ylabel('Resisting Force [kip]');
   title('Force-Histories');
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

   % plot hysteresis loops left
   createWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
   orient landscape
   plot(dmE(:,1),fbE(:,1),'r--','LineWidth',1.0);
   hold('on');
   plot(dN(:,1),fgE(:,1),'b-','LineWidth',1.0);
   %plot(dN(:,1),fRxnN(:,1),'g-','LineWidth',1.0);
   grid('on');
   xlabel ('Deformation [in.]');
   ylabel('Resisting Force [kip]');
   title('Hysteresis-Loops of Left Column');
   legend('Basic System','Global System');
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

   % plot hysteresis loops right
   createWindow('cen',0.80*SS(4)/3*4,0.80*SS(4));
   orient landscape
   plot(dmE(:,2),fbE(:,2),'r--','LineWidth',1.0);
   hold('on');
   plot(dN(:,2),fgE(:,2),'b-','LineWidth',1.0);
   %plot(dN(:,2),fRxnN(:,2),'g-','LineWidth',1.0);
   grid('on');
   xlabel ('Deformation [in.]');
   ylabel('Resisting Force [kip]');
   title('Hysteresis-Loops of Right Column');
   legend('Basic System','Global System');
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
    plot(tN(3:end),time(2:end),'b-','LineWidth',1.0);
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

% plot STS (controller) data if available
if 0
   % get STS data
   data = load([path,'\STS_data.txt']);
   f = 256;
   dt = 1/f;
   tSTS = (1:size(data,1))'*dt;
   dispCmd = -data(:,1)+data(1,1);
   dispFbk = -data(:,3)+data(1,1);
   forceFbk = -data(:,5);
   
   % plot displacement histories
   figure(6);
   subplot(2,1,1);
   plot(tSTS,dispCmd,'b-');
   hold('on');
   plot(tSTS,dispFbk,'r-');
   grid('on');
   xlabel('Time [sec]')
   ylabel('Displacement [in.]');
   title('STS Displacement-Histories of Right Column');
   legend('Right Column cmd','Right Column fbk');
   
   subplot(2,1,2);
   plot(tE,2.54*dtE(:,2),'b-');
   hold('on');
   grid('on');
   xlabel('Time [sec]')
   ylabel('Displacement [in.]');
   title('Displacement-Histories of Right Column');
   %legend('Right Column cmd','Right Column fbk');
   
   % plot force history
   figure(7);
   plot(tSTS,forceFbk,'b-');
   hold('on');
   grid('on');
   xlabel ('Time [sec]')
   ylabel('Resisting Force [kip]');
   title('STS Force-History of Right Column');

   % plot hysteresis loop
   figure(8);
   plot(dispFbk,forceFbk,'b-');
   hold('on');
   grid('on');
   xlabel ('Deformation [in]')
   ylabel('Resisting Force [kip]');
   title('STS Hysteresis-Loop of Right Column');
end
