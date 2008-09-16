# File: TwoStoryFrame_Client1.tcl
# (use with TwoStoryFrame_Server1a.tcl & TwoStoryFrame_Server1b.tcl)
#
# $Revision: $
# $Date: $
# $URL: $
#
# Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
# Created: 11/06
# Revision: A
#
# Purpose: this file contains the tcl input to perform
# a distributed hybrid simulation of a two story frame
# model with two experimental beam column elements.
# The specimens are simulated using the SimUniaxialMaterials
# controller.
# The experimental setups are on the server sides.


# ------------------------------
# Start of model generation
# ------------------------------
# Create ModelBuilder (with two-dimensions and 3 DOF/node)
model BasicBuilder -ndm 2 -ndf 3

# Load OpenFresco package
# -----------------------
# (make sure all dlls are in the same folder as openSees.exe)
loadPackage OpenFresco

# Define geometry for model
# -------------------------
set mass [expr 1/386.1]
# node $tag $xCrd $yCrd $mass
node  1     0.0   0.0  
node  2     0.0  54.0 -mass $mass $mass 0.0
node  3    54.0  54.0 -mass $mass $mass 0.0
node  4     0.0 108.0 -mass $mass $mass 0.0
node  5    54.0 108.0 -mass $mass $mass 0.0

# set the boundary conditions
# fix $tag $DX $DY $RZ
fix 1   1  1  0
fix 3   0  1  0
fix 5   0  1  0

# Define experimental site
# ------------------------
# expSite ShadowSite $tag <-setup $setupTag> $ipAddr $ipPort <-ssl> <-dataSize $size>
expSite ShadowSite 1 "127.0.0.1" 8090
expSite ShadowSite 2 "127.0.0.1" 8091

# Define elments
# --------------
# geometric transformation
# geomTransf type tag 
geomTransf Linear 1
geomTransf Corotational 2

# columns of first story W6x12
# element elasticBeamColumn $eleTag $iNode $jNode $A $E $Iz $transTag
element elasticBeamColumn 1 1 2 3.55 29000 22.10 1    
# girder on first floor S4x7.7
# expElement beamColumn $eleTag $iNode $jNode $transTag -site $siteTag -initStif $Kij <-iMod> <-rho $rho>
expElement beamColumn 2 2 3 1 -site 1 -initStif 1213 0 0 0 11.2 -302.4 0 -302.4 10886.4
# columns of second story W6x12
# element elasticBeamColumn $eleTag $iNode $jNode $A $E $Iz $transTag
element elasticBeamColumn 3 2 4 3.55 29000 22.10 1
# girder on second floor S4x7.7
# expElement beamColumn $eleTag $iNode $jNode $transTag -site $siteTag -initStif $Kij <-iMod> <-rho $rho>
expElement beamColumn 4 4 5 1 -site 2 -initStif 1213 0 0 0 11.2 -302.4 0 -302.4 10886.4

# Define dynamic loads
# --------------------
# set time series to be passed to uniform excitation
set dt 0.01
#set scale 1.0
#set accelSeries "Path -filePath CosinePulse.txt -dt $dt -factor [expr 386.1*$scale]"
#set scale 1.4
#set accelSeries "Path -filePath SACNF01.txt -dt $dt -factor [expr 386.1*$scale]"
set scale 2.5
set accelSeries "Path -filePath ELC270.txt -dt $dt -factor [expr 386.1*$scale]"

# Ccreate UniformExcitation load pattern
# pattern UniformExcitation $tag $dir 
pattern UniformExcitation  1 1 -accel $accelSeries

# calculate the rayleigh damping factors for nodes & elements
set alphaM     0.0;       # D = alphaM*M
set betaK      0.0;       # D = betaK*Kcurrent
set betaKinit  0.003082;  # D = beatKinit*Kinit
set betaKcomm  0.0;       # D = betaKcomm*KlastCommit

# set the rayleigh damping 
rayleigh $alphaM $betaK $betaKinit $betaKcomm
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
#integrator NewmarkExplicit 0.5
#integrator HHTHybridSimulation 0.667
integrator AlphaOS 0.6667

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
recorder Node -file Node_Dsp.out -time -node 2 4 -dof 1 disp
recorder Node -file Node_Vel.out -time -node 2 4 -dof 1 vel
recorder Node -file Node_Acc.out -time -node 2 4 -dof 1 accel

recorder Element -file Elmt_Frc.out -time -ele 2 4 basicForces
recorder Element -file Elmt_Def.out -time -ele 2 4 basicDeformations
# --------------------------------
# End of recorder generation
# --------------------------------


# ------------------------------
# Finally perform the analysis
# ------------------------------
# perform an eigenvalue analysis
set pi 3.14159265358979
set lambda [eigen -fullGenLapack 6]
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
    for {set i 1} {$i < 3000} {incr i} {
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
