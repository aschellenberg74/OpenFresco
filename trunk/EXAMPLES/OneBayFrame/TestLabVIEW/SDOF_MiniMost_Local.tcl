# File: SDOF_MiniMost_Local.tcl
#
# $Revision$
# $Date$
# $URL$
#
# Written: Hong Kim (hong_kim@berkeley.edu)
# Created: 03/07
# Revision: A
#
# Purpose: this file contains the tcl input to perform
# a local hybrid simulation of a SDOF model with one
# experimental twoNodeLink element.
# The LabVIEW controller is used to connect to the
# MiniMost setup.


# ------------------------------
# Start of model generation
# ------------------------------
# create ModelBuilder (with two-dimensions and 2 DOF/node)
model BasicBuilder -ndm 2 -ndf 2

# Load OpenFresco package
# -----------------------
# (make sure all dlls are in the same folder as openSees.exe)
loadPackage OpenFresco

# Define geometry for model
# -------------------------
set mass2 3.0;	# N/(mm/sec^2)

# node $tag $xCrd $yCrd $mass
node  1     0.0   0.00
node  2     0.0  10.00	-mass $mass2 $mass2

# set the boundary conditions
# fix $tag $DX $DY
fix 1   1  1
fix 2   0  1

# Define materials
# ----------------
# uniaxialMaterial Steel02 $matTag $Fy $E $b $R0 $cR1 $cR2 $a1 $a2 $a3 $a4 
uniaxialMaterial Elastic 1 2453; #2420

# Define control points
# ---------------------
# expControlPoint tag nodeTag dir resp fact <dir resp fact ...>
expControlPoint 1 2 ux disp -lim -0.01 0.01
expControlPoint 2 2 ux disp ux force

# Define experimental control
# ---------------------------
# expControl LabVIEW tag ipAddr <ipPort> -trialCP cpTags -outCP cpTags
expControl LabVIEW 1 "127.0.0.1" 11997  -trialCP 1  -outCP 2;  # use with NEES-SAM
#expControl LabVIEW 1 "130.126.242.175" 44000  -trialCP 1  -outCP 2;  # use with Mini-Most at UIUC
#expControl SimUniaxialMaterials 1 1;  # use for local simulation

# Define experimental setup
# -------------------------
# expSetup OneActuator $tag <-control $ctrlTag> $dir -sizeTrialOut $t $o <-trialDispFact $f> ...
expSetup OneActuator 1 -control 1 1 -sizeTrialOut 1 1

# Define experimental site
# ------------------------
# expSite LocalSite $tag $setupTag
expSite LocalSite 1 1

# Define experimental elements
# ----------------------------
# expElement twoNodeLink $eleTag $iNode $jNode -dir $dirs -site $siteTag -initStif $Kij <-orient <$x1 $x2 $x3> $y1 $y2 $y3> <-pDelta Mratios> <-iMod> <-mass $m>
expElement twoNodeLink 1 1 2 -dir 2 -site 1 -initStif 2400

# Define dynamic loads
# --------------------
# set time series to be passed to uniform excitation
set dt 0.01
set scale 1.0
timeSeries Path 1 -filePath acc475C.txt -dt $dt -factor [expr 9.81*$scale]

# create UniformExcitation load pattern
# pattern UniformExcitation $tag $dir -accel $tsTag <-vel0 $vel0>
pattern UniformExcitation 1 1 -accel 1

# calculate the rayleigh damping factors for nodes & elements
set alphaM     1.010017396536;  # D = alphaM*M
set betaK      0.0;             # D = betaK*Kcurrent
set betaKinit  0.0;             # D = beatKinit*Kinit
set betaKcomm  0.0;             # D = betaKcomm*KlastCommit

# set the rayleigh damping 
# rayleigh $alphaM $betaK $betaKinit $betaKcomm
# ------------------------------
# End of model generation
# ------------------------------


# ------------------------------
# Start of analysis generation
# ------------------------------
# create the system of equations
system BandGeneral

# create the DOF numberer
numberer Plain

# create the constraint handler
constraints Plain

# create the convergence test
test EnergyIncr 1.0e-6 10

# create the integration scheme
integrator NewmarkExplicit 0.5
#integrator AlphaOS 0.67

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
recorder Node -file Node_Dsp.out -time -node 2 -dof 1 disp
recorder Node -file Node_Vel.out -time -node 2 -dof 1 vel
recorder Node -file Node_Acc.out -time -node 2 -dof 1 accel

recorder Element -file Elmt_Frc.out     -time -ele 1 forces
recorder Element -file Elmt_ctrlDsp.out -time -ele 1 ctrlDisp
recorder Element -file Elmt_daqDsp.out  -time -ele 1 daqDisp
# --------------------------------
# End of recorder generation
# --------------------------------


# ------------------------------
# Finally perform the analysis
# ------------------------------
# perform an eigenvalue analysis
set pi 3.14159265358979
set lambda [eigen -fullGenLapack 1]
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
    for {set i 1} {$i < 500} {incr i} {
        set t [time {analyze  1  $dt}]
        puts $outFileID $t
        puts "step $i"
        }
    }]
puts "\nElapsed Time = $tTot \n"
# close the output file
close $outFileID

wipe
# --------------------------------
# End of analysis
# --------------------------------
