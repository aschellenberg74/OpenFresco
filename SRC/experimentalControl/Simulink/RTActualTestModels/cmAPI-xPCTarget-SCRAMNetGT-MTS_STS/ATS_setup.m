% ==============================================================================================================================
% Adaptive Time Series (ATS) Compensator
% by Yunbyeong Chae, Old Dominion University
% Ref: Chae, Y., Kazemibidokhti, K., and Ricles, J.M. (2013) 
% “Adaptive time series compensator for delay compensation of servo-hydraulic actuator systems for real-time hybrid simulation”, 
% Earthquake Engineering and Structural Dynamics, 42(11), 1697-1715.
% ==============================================================================================================================

tl = 1.0;                      % duration of moving window [sec]
Nl = floor(tl/controlPeriod);  % number of samples of moving window
SN = 16;                       % skipping number
Threshold = 0.01;              % [in.] - threshold value for triggering ATS compensator 

%Par = [1 0.04 0.0008];       % initial a0, a1, a2 values
% Par values can be obtained from predefined displacement tests. However,
% the values for Par can be approximately estimated as follows:
% a0 (amplitude factor) is usually to be around 1
% a1 is about the same as the expected time delay when a0 is close to 1
% ex) if the expected time delay of an actuator is 40 msec, then a1 can be
% set to be 0.04
% a2 can be simply set to be a2=a1^2/2
if (HybridCtrlParameters.nDOF >= 1)
    Par(1,1) = 1;
    Par(2,1) = 0.020;
    Par(3,1) = 0.5*Par(2,1)^2;
end
if (HybridCtrlParameters.nDOF >= 2)
    Par(1,2) = 1;
    Par(2,2) = 0.020;
    Par(3,2) = 0.5*Par(2,2)^2;
end
if (HybridCtrlParameters.nDOF >= 3)
    Par(1,3) = 1;
    Par(2,3) = 0.020;
    Par(3,3) = 0.5*Par(2,3)^2;
end

P_range = [0.9 1.1;
           0.0 0.060;
           0.0 0.0032];
% P_range represents the minimum and maximum allowable values for a0, a1, and a2.
% Thus, the identified a0, a1, and a2 values by ATS compensator are always bounded by P_range
% See Table 1 in the paper for more details

%MRC = [2 0.05 0.001];        % maximum rate of change (can use these as default values) 
MRC = [2 0.05 0.001]./10;     % maximum rate of change (can use these as default values) 

% Butterworth filter
Filter_order = 6;
Cut_off_freq = 20;
[b,a] = butter(Filter_order,2*pi*Cut_off_freq,'s');
sys = tf(b,a);
zsys = c2d(sys,controlPeriod);
Znum = zsys.Numerator{1};
Zden = zsys.Denominator{1};

% Butterworth filter for forces
Filter_order_Frc = 4;
Cut_off_freq_Frc = 10;
[b_Frc,a_Frc] = butter(Filter_order_Frc,2*pi*Cut_off_freq_Frc,'s');
sys_Frc = tf(b_Frc,a_Frc);
zsys_Frc = c2d(sys_Frc,controlPeriod);
Znum_Frc = zsys_Frc.Numerator{1};
Zden_Frc = zsys_Frc.Denominator{1};

% % Butterworth filter for disp command
% Filter_order_Cmd = 4;
% Cut_off_freq_Cmd = 10;
% [b_Cmd,a_Cmd] = butter(Filter_order_Cmd,2*pi*Cut_off_freq_Cmd,'s');
% sys_Cmd = tf(b_Cmd,a_Cmd);
% zsys_Cmd = c2d(sys_Cmd,controlPeriod);
% Znum_Cmd = zsys_Cmd.Numerator{1};
% Zden_Cmd = zsys_Cmd.Denominator{1};
