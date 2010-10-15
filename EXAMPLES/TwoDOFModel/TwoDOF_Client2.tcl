# File: TwoDOF_Client2.tcl (use with TwoDOF_Server2.tcl)
#
# $Revision: $
# $Date: $
# $URL: $
#
# Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
# Created: 09/06
# Revision: A
#
# Purpose: this file contains the tcl input to perform
# a distributed hybrid simulation of a two degree of freedom
# system with one experimental zero length element.
# The specimen is simulated using the SimUniaxialMaterials
# controller.
# The experimental setup is on the client side.


# ------------------------------
# Start of model generation
# ------------------------------
# create ModelBuilder (with two-dimensions and 3 DOF/node)
model BasicBuilder -ndm 2 -ndf 3

# Load OpenFresco package
# -----------------------
# (make sure all dlls are in the same folder as openSees.exe)
loadPackage OpenFresco

# Define geometry for model
# -------------------------
set mass1 130000.;  # [kg]
set mass2 240000.;  # [kg]
# node $tag $xCrd $yCrd $mass
node  1     0.0   0.0
node  2     0.0   0.0  -mass $mass1 $mass1 0.0
node  3     0.0   0.0  -mass $mass2 $mass2 0.0

# set the boundary conditions
# fix $tag $DX $DY $RZ
fix 1   1  1  1
fix 2   0  1  1
fix 3   0  1  1

# Define materials
# ----------------
set E1 35.E6;    # [N/m], Stiffness of the column
set E2 50.E6;    # [N/m], Stiffness of two isolators
set Fy2 250.E3;  # [N], Yield strength of two isolators

# Define similitude
set S 0.5;                        # ratio of length from prototype to specimen
set factNtoTonf [expr 1./9.8E3];  # from [N] to [tonf]
set factMtoMM 1000.;              # from [m] to [mm]

set nIso 2;  # number of isolators

set Ee [expr $E2*$factNtoTonf/$factMtoMM*$S/$nIso];  # [tonf/mm], Stiffness of one isolator in test setup unit
set Fye [expr $Fy2*$factNtoTonf*$S*$S/$nIso];        # [tonf], Yield strength of one isolator in test setup unit

# uniaxialMaterial Steel01 $matTag $Fy $E $b
uniaxialMaterial Elastic 1 $E1
uniaxialMaterial Steel01 2 $Fye $Ee 0.1

# Define experimental setup
# -------------------------
# expSetup OneActuator $tag <-control $ctrlTag> $dir -sizeTrialOut $t $o <-trialDispFact $f> ...
expSetup OneActuator 1 1 -sizeTrialOut 1 1 -trialDispFact [expr $S*$factMtoMM] -outDispFact [expr 1.0/($S*$factMtoMM)] -outForceFact [expr 1.0/($S*$S*$factNtoTonf/$nIso)]

# Define experimental site
# ------------------------
# expSite ShadowSite $tag <-setup $setupTag> $ipAddr $ipPort <-ssl> <-dataSize $size>
expSite ShadowSite 1 -setup 1 "127.0.0.1" 8090

# Define numerical elements
# -------------------------
# element zeroLength $eleTag $iNode $jNode -mat $matTag -dir $dirs -orient $x1 $x2 $x3 $yp1 $yp2 $yp3
element zeroLength 1 1 2 -mat 1 -dir 1 -orient 1 0 0 0 1 0

# Define experimental elements
# ----------------------------
# expElement twoNodeLink $eleTag $iNode $jNode -dir $dirs -site $siteTag -initStif $Kij <-orient <$x1 $x2 $x3> $y1 $y2 $y3> <-pDelta Mratios> <-iMod> <-mass $m>
expElement twoNodeLink 2 2 3 -dir 1 -site 1 -initStif $E2 -orient 1 0 0 0 1 0

# Define dynamic loads
# --------------------
# set time series to be passed to uniform excitation
set dt 0.02
set scale 0.01
timeSeries Path 1 -filePath elc.dat -dt $dt -factor [expr 1.0*$scale]

# create UniformExcitation load pattern
# pattern UniformExcitation $tag $dir -accel $tsTag <-vel0 $vel0>
pattern UniformExcitation 1 1 -accel 1

# calculate the rayleigh damping factors for nodes & elements
set alphaM     0.0;       # D = alphaM*M
set betaK      0.0;       # D = betaK*Kcurrent
set betaKinit  0.004751;  # D = beatKinit*Kinit
set betaKcomm  0.0;       # D = betaKcomm*KlastCommit

# set the rayleigh damping 
#rayleigh $alphaM $betaK $betaKinit $betaKcomm
# ------------------------------
# End of model generation
# ------------------------------


# ------------------------------
# Start of analysis generation
# ------------------------------
# create the system of equations
system BandSPD

# create the DOF numberer
numberer Plain

# create the constraint handler
constraints Plain

# create the convergence test
test EnergyIncr 1.0e-6 10

# create the integration scheme
integrator NewmarkExplicit 0.5
#integrator NewmarkHybridSimulation 0.5 0.25
#integrator HHTExplicit 1.1
#integrator HHTHybridSimulation 1.0
#integrator AlphaOS 1.0

# create the solution algorithm
algorithm Linear

# create the analysis object 
analysis Transient
# ------------------------------
# End of analysis generation
# ------------------------------


# ------------------------------
# Start of recorder generation
# ------------------------------
# create the recorder objects
recorder Node -file Node_Dsp.out -time -node 2 3 -dof 1 disp
recorder Node -file Node_Vel.out -time -node 2 3 -dof 1 vel
recorder Node -file Node_Acc.out -time -node 2 3 -dof 1 accel

recorder Element -file Elmt_Hys.out -time -ele 1 2 deformationsANDforces
# --------------------------------
# End of recorder generation
# --------------------------------


# ------------------------------
# Finally perform the analysis
# ------------------------------
# perform an eigenvalue analysis
set pi 3.14159265358979
set lambda [eigen -fullGenLapack 2]
puts "\nEigenvalues at start of transient:"
puts "lambda         omega          period"
foreach lambda $lambda {
   set omega [expr pow($lambda,0.5)]
   set period [expr 2*$pi/pow($lambda,0.5)]
   puts "$lambda  $omega  $period"}

# open output file for writing
set outFileID [open elapsedTime.txt w]
# perform the transient analysis
set tTot [time {
    for {set i 1} {$i < 1000} {incr i} {
        set t [time {analyze  1  [expr $dt/2]}]
        puts $outFileID $t
        #puts "step $i"
    }
}]
puts "\nElapsed Time = $tTot \n"
# close the output file
close $outFileID

wipe
# --------------------------------
# End of analysis
# --------------------------------
