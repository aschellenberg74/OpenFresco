%INITIALIZESIMULATION to initialize the parameters needed to build the Simulink model
%
% created by MTS
% modified by Andreas Schellenberg (andreas.schellenberg@gmx.net) 11/2004

clear;
close all;
clc;

%%%%%%%%%% HYBRID CONTROLLER PARAMETERS %%%%%%%%%%

% set time steps
HybridCtrlParameters.nAct  = 1;           % number of controlled actuators
HybridCtrlParameters.dtInt = 0.02;        % integration time step (sec)
HybridCtrlParameters.dtSim = 2^(-3);      % simulation time step (sec)
HybridCtrlParameters.dtCon = 1/1024;      % controller time step (sec)
HybridCtrlParameters.delay = 0.0;         % delay due to undershoot (sec)

% calculate max number of substeps
HybridCtrlParameters.N = round(HybridCtrlParameters.dtSim/HybridCtrlParameters.dtCon);
% update simulation time step
HybridCtrlParameters.dtSim = HybridCtrlParameters.N*HybridCtrlParameters.dtCon;

% calculate number of delay steps
HybridCtrlParameters.iDelay = round(HybridCtrlParameters.delay./HybridCtrlParameters.dtCon);

% check that finite state machine does not deadlock
delayRatio = HybridCtrlParameters.iDelay/HybridCtrlParameters.N;
if (delayRatio>0.6 && delayRatio<0.8)
    warndlg(['The delay compensation exceeds 60% of the simulation time step.', ...
        'Please consider increasing the simulation time step in order to avoid oscillations.'], ...
        'WARNING');
elseif (delayRatio>=0.8)
    errordlg(['The delay compensation exceeds 80% of the simulation time step.', ...
        'The simulation time step must be increased in order to avoid deadlock.'], ...
        'ERROR');
    return
end

% update delay time
HybridCtrlParameters.delay = HybridCtrlParameters.iDelay*HybridCtrlParameters.dtCon;

% calculate testing rate
HybridCtrlParameters.Rate = HybridCtrlParameters.dtSim/HybridCtrlParameters.dtInt;

disp('Model Properties:');
disp('=================');
disp(HybridCtrlParameters);

%%%%%%%%%% SIGNAL COUNTS %%%%%%%%%%

nAct    = 8;                                    % number of actuators
nAdcU	  = 12;                                   % number of user a/d channels
nDucU   = 8;                                    % number of user ducs
nEncU	  = 2;                                    % number of user encoders
nDinp	  = 4;                                    % no. of digital inputs written to scramnet
nDout	  = 4;                                    % no. of digital outputs driven by scramnet
nUDPOut = 1+7*nAct+nAdcU+nDucU+nEncU+nDinp;     % no. of outputs from simulink bridge
nUDPInp = 1+6*nAct+nAdcU+nDucU+nEncU+nDinp;     % no. of inputs to simulink bridge

%%%%%%%%%% UNITS %%%%%%%%%%

% Define user units and conversion factors to canonical units.
%
% User units are used for entering physical parameters and for viewing output
% signals, but internally all computations are done in "canonical" units.
% Canonical units form a consistent system of units that ultimately derive
% from the units used for displacement and force.  Canonical units should only
% contain displacement, force, and time units.  For example, if displacement is
% inches, then canonical velocity units are inches/s, acceleration units are
% inches/s^2, etc.  If you want to view acceleration in G's, then you must
% provide the conversion factor from G's to canonical acceleration units,
% inches/s^2.
%
% Below, you must enter the conversion factors from your units to canonical
% units, as follows:
% 1) Change the unit names in the "user" and "canonical" columns of the comment
%	 text.  Do this not only for documentation purposes, but also to make it
%	 clear to you what conversion factors you need to compute.  Pay particular
%	 attention to selecting linear displacement and linear force units, because
%	 they determine the canonical units of all other physical quantities.
% 2) Compute the conversion factors from user to canonical units for the
%	 variables shown (user units = conversion factor x canonical units).
%
% Note about mass user units:  Mass user units are actually force, not mass,
% because it is easier for users to think of mass in terms of weight.  The
% conversion to true mass units is done by embedding the gravity constant in
% the term "massCanon".
%                                            user        canonical
velocCanon  = 1;              % veloc:       in/s        in/s
lengthCanon = 1;              % length:      in          in
areaCanon   = 1;              % area:        in^2        in^2
volumeCanon	= 231;            % volume:      gal         in^3
forceCanon  = 1;              % force:       kip         kip
pressCanon  = 0.001;          % pressure:    psi         kip/in^2
flowCanon   = 3.85;           % flow:			gpm         in^3/s
leakCanon   = flowCanon ...
            / pressCanon;     % leakage:     gpm/psi     in^5/kip-s
massCanon   = 0.002588;       % mass:        kip         kip/in/s^2
springCanon = 1;              % spring:      kip/in      kip/in
damperCanon = 1;              % damper:      kip/in/s	   kip/in/s

%%%%%%%%%% ACTUATOR/ACCUMULATOR PARAMETERS %%%%%%%%%%

% (Note: "1" and "2" denote actuator cylinders on either side of
%  the actuator piston.  With positive valve drive, oil is ported
%  into cylinder 1, resulting in positive displacement.)

% hydraulic parameters
supplyPress = 3000;     % psi
returnPress = 50;       % psi
bulkModulus	= 1.0e05;   % psi

% dynamic short actuator parameters
ratedPressA = 1000;     % psi
ratedFlowA  = 250.0;    % gpm
nominFlowA  = 375.0;    % gpm
area1A      = 75.5;     % in^2
area2A      = 75.5;     % in^2
stroke1A	   = 20.0;     % in
stroke2A	   = 20.0;     % in
endLength1A	= 2.0;      % in
endLength2A = 2.0;      % in
leakageA    = 0;        % gpm/psi

% dynamic long actuator parameters
ratedPressB = 1000;     % psi
ratedFlowB  = 250.0;    % gpm
nominFlowB  = 375.0;    % gpm
area1B      = 55.0;     % in^2
area2B      = 55.0;     % in^2
stroke1B    = 40.0;     % in
stroke2B    = 40.0;     % in
endLength1B	= 2.0;      % in
endLength2B	= 2.0;      % in
leakageB    = 0;        % gpm/psi

% static actuator parameters
ratedPressC = 1000;     % psi
ratedFlowC  = 15.0;     % gpm
nominFlowC  = 22.5;     % gpm
area1C      = 113.1;    % in^2
area2C      = 74.6;     % in^2
stroke1C    = 72.0;     % in
stroke2C    = 72.0;     % in
endLength1C	= 0;        % in
endLength2C	= 0;        % in
leakageC    = 0;        % gpm/psi

% accumulator parameters
accumVolume = 23;       % gal
accumNumber	= 1;
precharge   = 1000;     % psi
gasConstant	= 1.8;      % nitrogen
pumpFlow    = 1e10;     % gpm

%%%%%%%%%% PAYLOAD PARAMETERS %%%%%%%%%%

% friction parameters (dynamic short actuator)
frictionA   = 0;        % kip
yieldDisplA	= 0.1;      % in

% friction parameters (dynamic long actuator)
frictionB   = 0;        % kip
yieldDisplB	= 0.1;      % in

% friction parameters (static actuator)
frictionC   = 0;        % kip
yieldDisplC	= 0.1;      % in

% fixture & specimen parameters (dynamic short actuator)
staticForceA = 0;       % kip
springA      = 2.8;     % kip/in
yieldFrcA    = 2.0;     % kip
damperA      = 0;       % kip/in/s
massA        = 4.2;     % kip

% fixture & specimen parameters (dynamic long actuator)
staticForceB = 0;       % kip
springB      = 2.8;     % kip/in
yieldFrcB    = 2.0;     % kip
damperB      = 0;       % kip/in/s
massB        = 2.5;     % kip

% fixture & specimen parameters (static actuator)
staticForceC = 0;       % kip
springC      = 2.8;     % kip/in
yieldFrcC    = 2.0;     % kip
damperC      = 0;       % kip/in/s
massC        = 2.5;     % kip

%%%%%%%%%% COMPUTED QUANTITIES (do not modify!) %%%%%%%%%%

% sample period parameters
samplePeriod = 1/1024;  % sec
T            = samplePeriod;

% other parameters
valveDelay = 0.005;     % sec
overlap    = 0.1;       % percent
damping    = 6;         % percent
ratioPorts = 0;         % don't ratio ports

% dynamic short actuator parameters
areaA       = (area1A + area2A) / 2;
halfLengthA	= (stroke1A + endLength1A + stroke2A + endLength2A) / 2;
oilColumnA  = sqrt(2 * areaA * areaCanon * bulkModulus * pressCanon ...
   / (halfLengthA * lengthCanon * massA * massCanon)) / (2 * pi);
maxFlowA    = ratedFlowA * sqrt(supplyPress / ratedPressA);
maxDisplA   =  (stroke2A + endLength2A) * lengthCanon;
minDisplA   = -(stroke1A + endLength1A) * lengthCanon;
maxVelocA   =  (maxFlowA * flowCanon / (area1A * areaCanon)) / velocCanon;
minVelocA   = -(maxFlowA * flowCanon / (area2A * areaCanon)) / velocCanon;
maxForceA   =  area1A * areaCanon * (supplyPress-returnPress) * pressCanon;
minForceA   = -area2A * areaCanon * (supplyPress-returnPress) * pressCanon;
dampFactorA	= damping * 0.01 * 2 * pi * oilColumnA * massA * massCanon;
actParamsA  = ...
   [bulkModulus * pressCanon;
   valveDelay;
   overlap      * 0.01;
   ratedPressA	 * pressCanon;
   ratedFlowA   * flowCanon;
   nominFlowA   * flowCanon;
   area1A       * areaCanon;
   area2A       * areaCanon;
   stroke1A     * lengthCanon;
   stroke2A     * lengthCanon;
   endLength1A  * lengthCanon;
   endLength2A  * lengthCanon;
   dampFactorA;
   leakageA	    * leakCanon;
   ratioPorts;
   samplePeriod;
   1;				% displCanon
   velocCanon;
   1;				% forceCanon
   pressCanon;
   flowCanon]';

% dynamic long actuator parameters
areaB	      = (area1B + area2B) / 2;
halfLengthB	= (stroke1B + endLength1B + stroke2B + endLength2B) / 2;
oilColumnB  = sqrt(2 * areaB * areaCanon * bulkModulus * pressCanon ...
   / (halfLengthB * lengthCanon * massB * massCanon)) / (2 * pi);
maxFlowB	   = ratedFlowB * sqrt(supplyPress / ratedPressB);
maxDisplB   =  (stroke2B + endLength2B) * lengthCanon;
minDisplB   = -(stroke1B + endLength1B) * lengthCanon;
maxVelocB   =  (maxFlowB * flowCanon / (area1B * areaCanon)) / velocCanon;
minVelocB   = -(maxFlowB * flowCanon / (area2B * areaCanon)) / velocCanon;
maxForceB   =  area1B * areaCanon * (supplyPress-returnPress) * pressCanon;
minForceB   = -area2B * areaCanon * (supplyPress-returnPress) * pressCanon;
dampFactorB	= damping * 0.01 * 2 * pi * oilColumnB * massB * massCanon;
actParamsB  = ...
   [bulkModulus * pressCanon;
   valveDelay;
   overlap      * 0.01;
   ratedPressB	 * pressCanon;
   ratedFlowB   * flowCanon;
   nominFlowB   * flowCanon;
   area1B       * areaCanon;
   area2B       * areaCanon;
   stroke1B	    * lengthCanon;
   stroke2B	    * lengthCanon;
   endLength1B	 * lengthCanon;
   endLength2B	 * lengthCanon
   dampFactorB;
   leakageB		 * leakCanon;
   ratioPorts;
   samplePeriod;
   1;				% displCanon
   velocCanon;
   1;				% forceCanon
   pressCanon;
   flowCanon]';

% static actuator parameters
areaC	      = (area1C + area2C) / 2;
halfLengthC	= (stroke1C + endLength1C + stroke2C + endLength2C) / 2;
oilColumnC  = sqrt(2 * areaC * areaCanon * bulkModulus * pressCanon ...
   / (halfLengthC * lengthCanon * massC * massCanon)) / (2 * pi);
maxFlowC	   = ratedFlowC * sqrt(supplyPress / ratedPressC);
maxDisplC   =  (stroke2C + endLength2C) * lengthCanon;
minDisplC   = -(stroke1C + endLength1C) * lengthCanon;
maxVelocC   =  (maxFlowC * flowCanon / (area1C * areaCanon)) / velocCanon;
minVelocC   = -(maxFlowC * flowCanon / (area2C * areaCanon)) / velocCanon;
maxForceC   =  area1C * areaCanon * (supplyPress-returnPress) * pressCanon;
minForceC   = -area2C * areaCanon * (supplyPress-returnPress) * pressCanon;
dampFactorC	= damping * 0.01 * 2 * pi * oilColumnC * massC * massCanon;
actParamsC  = ...
   [bulkModulus * pressCanon;
   valveDelay;
   overlap      * 0.01;
   ratedPressC	 * pressCanon;
   ratedFlowC   * flowCanon;
   nominFlowC   * flowCanon;
   area1C       * areaCanon;
   area2C       * areaCanon;
   stroke1C     * lengthCanon;
   stroke2C	    * lengthCanon;
   endLength1C	 * lengthCanon;
   endLength2C	 * lengthCanon;
   dampFactorC;
   leakageC	    * leakCanon;
   ratioPorts;
   samplePeriod;
   1;				% displCanon
   velocCanon;
   1;				% forceCanon
   pressCanon;
   flowCanon]';

% accumulator parameters
volume       = accumVolume * accumNumber;
maxOilVolume = volume * (1 - (precharge/supplyPress)^(1/gasConstant));

% actuator envelopes
displRange = [minDisplA maxDisplA; minDisplB maxDisplB; minDisplC maxDisplC];
velocRange = [minVelocA maxVelocA; minVelocB maxVelocB; minVelocC maxVelocC];
forceRange = [minForceA maxForceA; minForceB maxForceB; minForceC maxForceC];
oilColumn  = [oilColumnA; oilColumnB; oilColumnC];

% print physical ranges
disp('Actuator Physical Ranges:');
disp('=========================');
disp('displacement range [in.]:')
disp(displRange);
disp('velocity range [in./sec]:')
disp(velocRange);
disp('force range [kips]:')
disp(forceRange);
disp('oil column frequency [Hz]:')
disp(oilColumn);
disp('discharge [gal]:')
disp(maxOilVolume);
