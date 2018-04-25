% parameter initialization

%%%%%%%%%% (start of parameter entry) %%%%%%%%%%

%%%%%%%%%% SIGNAL COUNTS %%%%%%%%%%

nAct    = 4;			    % number of actuators
nDinp	= 4;				% no. of digital inputs written to scramnet
nDout	= 4;				% no. of digital outputs driven by scramnet
nUDPOut	= 1+7*nAct+nDinp;	% no. of outputs from simulink bridge
nUDPInp = 1+6*nAct+nDout;	% no. of inputs to simulink bridge

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
%												user		canonical
								% displ:		in			in
								% force:		kip			kip
velocCanon   	= 1;			% veloc:		in/s		in/s
lengthCanon     = 1;			% length:		in			in
areaCanon       = 1;			% area:			in^2		in^2
volumeCanon     = 231;			% volume:		gal			in^3
forceCanon      = 1;			% force:		kip			kip   
pressCanon      = 0.001;		% pressure:		psi			kip/in^2
flowCanon		= 3.85;			% flow:			gpm			in^3/s
leakCanon		= flowCanon ...
				/ pressCanon;	% leakage:		gpm/psi		in^3/s // kip/in^2
massCanon       = 0.001/386.4;	% mass:			lbm			kip//in/s^2
springCanon		= 1;			% spring:		kip/in		kip/in
damperCanon		= 1;			% damper:		kip//in/s	kip//in/s

%%%%%%%%%% HYDRAULIC PARAMETERS %%%%%%%%%%

% pump parameters
supplyPress		= 3000;			% psi
returnPress		= 200;			% psi
%pumpFlow		= 720;			% gpm
pumpFlow		= 1e10;			% (for debug purposes)

% accumulator parameters
volume			= 37.5;			% gal
precharge		= 1500;			% psi
gasConstant		= 1.8;			% nitrogen

%%%%%%%%%% CHANNEL 1 PARAMETERS %%%%%%%%%%

% hydraulic parameters
bulkModulus(1)	= 125000;		% psi

% servovalve parameters (per individual servovalve)
valveDelay(1)	= 0.005;        % sec
overlap(1)		= 0;			% percent
ratedPress(1)	= 1000;			% psi
ratedFlow(1)	= 1;			% gpm
nominFlow(1)	= 1;			% gpm

% actuator parameters
% (Note: "1" and "2" denote actuator cylinders on either side of
%  the actuator piston.  With positive valve drive, oil is ported
%  into cylinder 1, resulting in positive displacement.)
area1(1)		= 0.42;			% in^2
area2(1)		= 0.42;			% in^2
stroke1(1)		= 3.0;			% in
stroke2(1)		= 3.0;			% in
endLength1(1)	= 0.25;			% in
endLength2(1)	= 0.25;			% in
portVolume1(1)	= 0.001818		% gal
portVolume2(1)	= 0.001818		% gal
damping(1)		= 8;			% percent
leakage(1)		= 0;			% gpm/psi

% payload parameters
friction(1)		= 0;			% kip
yieldDispl(1)	= 0.1;			% in
staticForce(1)	= 0;			% kip
spring(1)		= 1/3;			% kip/in
damper(1)		= 0;			% kip//in/s
mass(1)			= 1000;			% lbm

%%%%%%%%%% CHANNEL 2 PARAMETERS %%%%%%%%%%

% hydraulic parameters
bulkModulus(2)	= 125000;		% psi

% servovalve parameters (per individual servovalve)
valveDelay(2)	= 0.005;        % sec
overlap(2)		= 0;			% percent
ratedPress(2)	= 1000;			% psi
ratedFlow(2)	= 1;			% gpm
nominFlow(2)	= 1;			% gpm

% actuator parameters
% (Note: "1" and "2" denote actuator cylinders on either side of
%  the actuator piston.  With positive valve drive, oil is ported
%  into cylinder 1, resulting in positive displacement.)
area1(2)		= 0.42;			% in^2
area2(2)		= 0.42;			% in^2
stroke1(2)		= 3.0;			% in
stroke2(2)		= 3.0;			% in
endLength1(2)	= 0.25;			% in
endLength2(2)	= 0.25;			% in
portVolume1(2)	= 0.001818		% gal
portVolume2(2)	= 0.001818		% gal
damping(2)		= 8;			% percent
leakage(2)		= 0;			% gpm/psi

% payload parameters
friction(2)		= 0;			% kip
yieldDispl(2)	= 0.1;			% in
staticForce(2)	= 0;			% kip
spring(2)		= 1/3;			% kip/in
damper(2)		= 0;			% kip//in/s
mass(2)			= 1000;			% lbm

%%%%%%%%%% CHANNEL 3 PARAMETERS %%%%%%%%%%

% hydraulic parameters
bulkModulus(3)	= 125000;		% psi

% servovalve parameters (per individual servovalve)
valveDelay(3)	= 0.005;        % sec
overlap(3)		= 0;			% percent
ratedPress(3)	= 1000;			% psi
ratedFlow(3)	= 1;			% gpm
nominFlow(3)	= 1;			% gpm

% actuator parameters
% (Note: "1" and "2" denote actuator cylinders on either side of
%  the actuator piston.  With positive valve drive, oil is ported
%  into cylinder 1, resulting in positive displacement.)
area1(3)		= 0.42;			% in^2
area2(3)		= 0.42;			% in^2
stroke1(3)		= 3.0;			% in
stroke2(3)		= 3.0;			% in
endLength1(3)	= 0.25;			% in
endLength2(3)	= 0.25;			% in
portVolume1(3)	= 0.001818		% gal
portVolume2(3)	= 0.001818		% gal
damping(3)		= 8;			% percent
leakage(3)		= 0;			% gpm/psi

% payload parameters
friction(3)		= 0;			% kip
yieldDispl(3)	= 0.1;			% in
staticForce(3)	= 0;			% kip
spring(3)		= 1/3;			% kip/in
damper(3)		= 0;			% kip//in/s
mass(3)			= 1000;			% lbm

%%%%%%%%%% CHANNEL 4 PARAMETERS %%%%%%%%%%

% hydraulic parameters
bulkModulus(4)	= 125000;		% psi

% servovalve parameters (per individual servovalve)
valveDelay(4)	= 0.005;        % sec
overlap(4)		= 0;			% percent
ratedPress(4)	= 1000;			% psi
ratedFlow(4)	= 1;			% gpm
nominFlow(4)	= 1;			% gpm

% actuator parameters
% (Note: "1" and "2" denote actuator cylinders on either side of
%  the actuator piston.  With positive valve drive, oil is ported
%  into cylinder 1, resulting in positive displacement.)
area1(4)		= 0.42;			% in^2
area2(4)		= 0.42;			% in^2
stroke1(4)		= 3.0;			% in
stroke2(4)		= 3.0;			% in
endLength1(4)	= 0.25;			% in
endLength2(4)	= 0.25;			% in
portVolume1(4)	= 0.001818		% gal
portVolume2(4)	= 0.001818		% gal
damping(4)		= 8;			% percent
leakage(4)		= 0;			% gpm/psi

% payload parameters
friction(4)		= 0;			% kip
yieldDispl(4)	= 0.1;			% in
staticForce(4)	= 0;			% kip
spring(4)		= 1/3;			% kip/in
damper(4)		= 0;			% kip//in/s
mass(4)			= 1000;			% lbm

%%%%%%%%%% (end of parameter entry) %%%%%%%%%%

%%%%%%%%%% COMPUTED QUANTITIES (do not modify!) %%%%%%%%%%

% sample period parameters
% controlPeriod	= 1/1024;		% sec (original)
controlPeriod	= 1/2048;		% sec (Updated on 10/19/17)
upsampleFactor	= 1; % (Updated on 10/17/17 from value of 2)
samplePeriod	= controlPeriod/upsampleFactor;

% other parameters
ratioPorts	= 0;			% don't ratio ports

% assemble actuator parameters
length1		= ( (stroke1 + endLength1) * lengthCanon ...
			   + portVolume1 * volumeCanon ./ (area1 * areaCanon));
length2		= ( (stroke2 + endLength2) * lengthCanon ...
			   + portVolume2 * volumeCanon ./ (area2 * areaCanon));
oilSpring	= bulkModulus * pressCanon ...
			.* (area1 * areaCanon ./ length1 + area2 * areaCanon ./ length2);
oilColumn	= sqrt(oilSpring ./ (mass * massCanon)) / (2 * pi);
oilDamping	= 2 * damping * 0.01 * 2 * pi .* oilColumn .* mass * massCanon;
dampFactor	= oilDamping / 2;
actParams	= ...
[	bulkModulus   * pressCanon;
	valveDelay;
	overlap       * 0.01;
	ratedPress    * pressCanon;
	ratedFlow     * flowCanon;
	nominFlow     * flowCanon;
	area1         * areaCanon;
	area2         * areaCanon;
	stroke1       * lengthCanon;
	stroke2       * lengthCanon;
	endLength1    * lengthCanon;
	endLength2    * lengthCanon;
	portVolume1   * volumeCanon;
	portVolume2   * volumeCanon;
	dampFactor;
	leakage       * leakCanon;
	ratioPorts    * ones(1,nAct);
	samplePeriod  * ones(1,nAct);
	1             * ones(1,nAct);
	velocCanon    * ones(1,nAct);
	1             * ones(1,nAct);
	pressCanon	  * ones(1,nAct);
	flowCanon	  * ones(1,nAct)];

% actuator envelopes
maxFlow	 = (nominFlow .* sqrt(supplyPress ./ ratedPress))';
maxDispl = ( (stroke2 + endLength2) * lengthCanon)';
minDispl = (-(stroke1 + endLength1) * lengthCanon)';
maxVeloc = ( (maxFlow' * flowCanon ./ (area1 * areaCanon)) / velocCanon)';
minVeloc = (-(maxFlow' * flowCanon ./ (area2 * areaCanon)) / velocCanon)';
maxForce = ( area1 * areaCanon * (supplyPress-returnPress) * pressCanon)';
minForce = (-area2 * areaCanon * (supplyPress-returnPress) * pressCanon)';

% sensor calibration ranges
dcal = maxDispl;
fcal = maxForce;

% accumulator parameters
discharge = volume * (1 - (precharge/supplyPress)^(1/gasConstant));

% print physical ranges
displRange = [minDispl(:) maxDispl(:)]
velocRange = [minVeloc(:) maxVeloc(:)]
forceRange = [minForce(:) maxForce(:)]
oilColumn  = oilColumn(:)
discharge
