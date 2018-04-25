timelog = tg.TimeLog;
output = tg.OutputLog;

close all
%%
i_MV = i_MV+1;
%%
data_collected = [timelog output];

filename = ['20171115_Data_' num2str(i_MV) '_MV'];
save(filename,'data_collected')    

figure
plot(data_collected(:,1),data_collected(:,11),data_collected(:,1),data_collected(:,9),data_collected(:,1),data_collected(:,15),data_collected(:,1),data_collected(:,12))
xlabel 'Time'
ylabel 'Displacement fbk'
title 'Displacement fbk Time History'
legend 'reference' 'disp compensated' 'disp target' 'displ fbk (measured)'

[timeShift_RMS,timeShift_CC] = getTimeShift(data_collected(:,1),data_collected(:,11),data_collected(:,1),data_collected(:,12),0.5)

[timeShift_RMS1,timeShift_CC1] = getTimeShift(data_collected(:,1),data_collected(:,15),data_collected(:,1),data_collected(:,12),0.5)

error_target_measured = data_collected(:,12)-data_collected(:,15);
sig_Range = max(data_collected(:,15))-min(data_collected(:,15));

figure
getFFT(error_target_measured.*(100/sig_Range),1/2048,'Error between Measured and Target Displacements');
xlim ([0 120])

ref = data_collected(:,11);
orig = data_collected(:,9);
targ = data_collected(:,15);
disp_measured = data_collected(:,12);
vel_measured = data_collected(:,13);

[lref, cref] = size(ref);
vel_ref = ref(1:lref-1) - ref(2:lref);

[lorig, corig] = size(orig);
vel_orig = orig(1:lorig-1) - orig(2:lorig);

[ltarg, ctarg] = size(targ);
vel_targ = (targ(1:ltarg-1) - targ(2:ltarg));

figure
plot(data_collected(2:end,1),vel_ref,data_collected(2:end,1),vel_orig,data_collected(2:end,1),vel_targ)
xlabel 'Time'
ylabel 'Velocity'
title 'Velocity Time History'
legend 'reference' 'compansated' 'target'

figure
plot(data_collected(:,1),data_collected(:,13))
xlabel 'Time'
ylabel 'Velocity fbk'
title 'Velocity fbk (measured) Time History'

figure
subplot(4,1,1)
plot(data_collected(:,1),data_collected(:,16))
title 'ATS Parameter 1'
subplot(4,1,2)
plot(data_collected(:,1),data_collected(:,17))
title 'ATS Parameter 2'
subplot(4,1,3)
plot(data_collected(:,1),data_collected(:,18))
title 'ATS Parameter 3'
subplot(4,1,4)
plot(data_collected(:,1),data_collected(:,19))
title 'ATS Parameter 4'

figure
plot(data_collected(:,1),data_collected(:,5),data_collected(:,1),data_collected(:,24))
xlabel 'Time'
ylabel 'Force'
title 'Computed Force vs. Measured Force Time History (4 load cells)'
legend 'computed (simulated)' 'filtered fbk (measured)' 

figure
plot(data_collected(:,1),data_collected(:,20),data_collected(:,1),data_collected(:,21),data_collected(:,1),data_collected(:,22),data_collected(:,1),data_collected(:,23))
xlabel 'Time'
ylabel 'Force'
title 'Filtered Load Cells Shear Time History'
legend '1' '2' '3' '4' 




