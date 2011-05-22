# File: TwoStoryFrame_Local.tcl
#
# $Revision$
# $Date$
# $URL$
#
# Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
# Created: 11/06
# Revision: A
#
# Purpose: this file contains the tcl input to perform
# a local hybrid simulation of a two story frame model
# with two experimental beam column elements.
# The specimens are simulated using the SimUniaxialMaterials
# controller.


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

# Define materials
# ----------------
# uniaxialMaterial Steel02 $matTag $Fy $E $b $R0 $cR1 $cR2 $a1 $a2 $a3 $a4 
#uniaxialMaterial Elastic 1 2.8
uniaxialMaterial Steel02 1 1.5 2.8 0.01 18.5 0.925 0.15 0.0 1.0 0.0 1.0

# Define experimental control
# ---------------------------
# expControl SimUniaxialMaterials $tag $matTags
expControl SimUniaxialMaterials 1 1
expControl SimUniaxialMaterials 2 1

# Define experimental setup
# -------------------------
# expSetup OneActuator $tag <-control $ctrlTag> $dir -sizeTrialOut $t $o <-trialDispFact $f> ...
expSetup OneActuator 1 -control 1 2 -sizeTrialOut 3 3
expSetup OneActuator 2 -control 2 2 -sizeTrialOut 3 3

# Define experimental site
# ------------------------
# expSite LocalSite $tag $setupTag
expSite LocalSite 1 1
expSite LocalSite 2 2

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
#timeSeries Path 1 -filePath CosinePulse.txt -dt $dt -factor [expr 386.1*$scale]
#set scale 1.4
#timeSeries Path 1 -filePath SACNF01.txt -dt $dt -factor [expr 386.1*$scale]
set scale 2.5
timeSeries Path 1 -filePath ELC270.txt -dt $dt -factor [expr 386.1*$scale]

# Ccreate UniformExcitation load pattern
# pattern UniformExcitation $tag $dir -accel $tsTag <-vel0 $vel0>
pattern UniformExcitation 1 1 -accel 1

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
