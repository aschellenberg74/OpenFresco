% ==============================================================================================================================
% Adaptive Time Series (ATS) Compensator
% by Yunbyeong Chae, Old Dominion University
% Ref: Chae, Y., Kazemibidokhti, K., and Ricles, J.M. (2013) 
% “Adaptive time series compensator for delay compensation of servo-hydraulic actuator systems for real-time hybrid simulation”, 
% Earthquake Engineering and Structural Dynamics, 42(11), 1697-1715.
% ==============================================================================================================================

sample = 1/2048;             % sample time step of RTHS [sec]
sample_rate = 1/sample;      % sampling rate of RTHS [Hz]
tl = 1.0;                    % duration of moving window [sec]
Nl = floor(tl*sample_rate);  % number of samples in moving window
SN = 16;                     % skipping number

% threshold value for triggering ATS compensator [in.]
if (HybridCtrlParameters.nDOF >= 1)
    Threshold(1) = 0.01;
end
if (HybridCtrlParameters.nDOF >= 2)
    Threshold(2) = 0.01;
end
if (HybridCtrlParameters.nDOF >= 3)
    Threshold(3) = 0.01;
end

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
    Par(2,1) = 0.025;
    Par(3,1) = 0.5*Par(2)^2;
end
if (HybridCtrlParameters.nDOF >= 2)
    Par(1,2) = 1;
    Par(2,2) = 0.025;
    Par(3,2) = 0.5*Par(2)^2;
end
if (HybridCtrlParameters.nDOF >= 3)
    Par(1,3) = 1;
    Par(2,3) = 0.025;
    Par(3,3) = 0.5*Par(2)^2;
end

P_range = [0.7 1.30;
           0.0 0.08;
           0.0 0.0032]; 
% P_range represents the minimum and maximum allowable values for a0, a1, and a2.
% Thus, the identified a0, a1, and a2 values by ATS compensator are always bounded by P_range
% See Table 1 in the paper for more details

MRC = [2 0.05 0.001];	% maximum rate of change (can use these as default values) 

% Butter filter
Filter_order = 6;
Cut_off_freq = 20;
[b,a] = butter(Filter_order,2*pi*Cut_off_freq,'s');
dt = 1/sample_rate;
sys = tf(b,a);
zsys = c2d(sys,dt);
ztag = get(zsys);
Znum = ztag.num{1};
Zden = ztag.den{1};
