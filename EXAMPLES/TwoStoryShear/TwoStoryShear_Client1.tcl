# File: TwoStoryShear_Client1.tcl
# (use with TwoStoryShear_Server1a.tcl & TwoStoryShear_Server1b.tcl)
#
# $Revision$
# $Date$
# $URL$
#
# Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
# Created: 09/06
# Revision: A
#
# Purpose: this file contains the tcl input to perform
# a distributed hybrid simulation of a two story shear
# model with two experimental beam column elements.
# The specimens are simulated using the SimUniaxialMaterials
# controller.
# The experimental setups are on the server sides.


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
set mass [expr 4/386.1]
# node $tag $xCrd $yCrd $mass
node  1   0.0   0.0  
node  2   0.0 108.0 -mass $mass $mass
node  3   0.0 216.0 -mass $mass $mass

# set the boundary conditions
# fix $tag $DX $DY $RZ
fix 1   1  1

# Define experimental site
# ------------------------
# expSite ShadowSite $tag <-setup $setupTag> $ipAddr $ipPort <-ssl> <-dataSize $size>
expSite ShadowSite 1 "127.0.0.1" 8090
expSite ShadowSite 2 "127.0.0.1" 8091

# Define elments
# --------------
# columns of first story S4x7.7
# expElement twoNodeLink $eleTag $iNode $jNode -dir $dirs -site $siteTag -initStif $Kij <-orient <$x1 $x2 $x3> $y1 $y2 $y3> <-pDelta Mratios> <-iMod> <-mass $m>
expElement twoNodeLink 1 1 2 -dir 1 2 -site 1 -initStif 1213 0 0 2.8
# columns of second story S4x7.7                                  
expElement twoNodeLink 2 2 3 -dir 1 2 -site 2 -initStif 1213 0 0 2.8

# Define dynamic loads
# --------------------
# set time series to be passed to uniform excitation
set dt 0.01
#set scale 1.0
#timeSeries Path 1 -filePath CosinePulse.txt -dt $dt -factor [expr 386.1*$scale]
#set scale 1.0
#timeSeries Path 1 -filePath SACNF01.txt -dt $dt -factor [expr 386.1*$scale]
set scale 2.0
timeSeries Path 1 -filePath ELC270.txt -dt $dt -factor [expr 386.1*$scale]

# create UniformExcitation load pattern
# pattern UniformExcitation $tag $dir -accel $tsTag <-vel0 $vel0>
pattern UniformExcitation 1 1 -accel 1

# calculate the rayleigh damping factors for nodes & elements
set alphaM     0.0;       # D = alphaM*M
set betaK      0.0;       # D = betaK*Kcurrent
set betaKinit  0.003937;  # D = beatKinit*Kinit
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
system BandGeneral

# create the DOF numberer
numberer Plain

# create the constraint handler
constraints Plain

# create the convergence test 
test EnergyIncr 1.0e-6  10
#test FixedNumIter 2

# create the integration scheme
integrator NewmarkExplicit 0.5
#integrator HHTHybridSimulation 0.667
#integrator AlphaOS 0.6667

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

recorder Element -file Elmt_Frc.out -time -ele 1 2 basicForces
recorder Element -file Elmt_Def.out -time -ele 1 2 basicDeformations
# --------------------------------
# End of recorder generation
# --------------------------------


# ------------------------------
# Finally perform the analysis
# ------------------------------
# perform an eigenvalue analysis
set pi 3.14159265358979
set lambda [eigen -fullGenLapack 4]
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
    for {set i 1} {$i < 4000} {incr i} {
        set t [time {analyze  1  $dt}]
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
