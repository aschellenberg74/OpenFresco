# File: OneBayFrame_Master.tcl (use with Run_OneBayFrame_Slave_Imp.bat)
# Units: [kip,in.]
#
# $Revision$
# $Date$
# $URL$
#
# Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
# Created: 08/08
# Revision: A
#
# Purpose: this file contains the tcl input to perform
# a hybrid simulation of a one bay frame with 
# two experimental zero length elements.
# Since the simulation of the column is performed in an
# other FE-software, the SimFEAdapter experimental control
# is used to communicate with such software.


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
set mass3 0.04
set mass4 0.02
# node $tag $xCrd $yCrd $mass
node  1     0.0   0.00
node  2   100.0   0.00
node  3     0.0  50.00  -mass $mass3 $mass3
node  4   100.0  50.00  -mass $mass4 $mass4

# set the boundary conditions
# fix $tag $DX $DY
fix 1   1  1
fix 2   1  1
fix 3   0  1
fix 4   0  1

# Define materials
# ----------------
# uniaxialMaterial Steel02 $matTag $Fy $E $b $R0 $cR1 $cR2 $a1 $a2 $a3 $a4 
#uniaxialMaterial Elastic 1 2.8
uniaxialMaterial Steel02 1 1.5 2.8 0.01 18.5 0.925 0.15 0.0 1.0 0.0 1.0
#uniaxialMaterial Elastic 2 5.6
uniaxialMaterial Steel02 2 3.0 5.6 0.01 18.5 0.925 0.15 0.0 1.0 0.0 1.0 
uniaxialMaterial Elastic 3 [expr 2.0*100.0/1.0]

# Define experimental control
# ---------------------------
# expControl SimUniaxialMaterials $tag $matTags
expControl SimUniaxialMaterials 1 1
# expControl SimFEAdapter $tag ipAddr $ipPort
expControl SimFEAdapter 2 "127.0.0.1" 44000

# Define experimental setup
# -------------------------
# expSetup OneActuator $tag <-control $ctrlTag> $dir -sizeTrialOut $t $o <-trialDispFact $f> ...
expSetup OneActuator 1 -control 1 1 -sizeTrialOut 1 1 -trialDispFact -1 -outDispFact -1 -outForceFact -1
expSetup OneActuator 2 -control 2 1 -sizeTrialOut 1 1 -trialDispFact -1 -outDispFact -1 -outForceFact -1

# Define experimental site
# ------------------------
# expSite LocalSite $tag $setupTag
expSite LocalSite 1 1
expSite LocalSite 2 2

# Define experimental elements
# ----------------------------
# left and right columns
# expElement twoNodeLink $eleTag $iNode $jNode -dir $dirs -site $siteTag -initStif $Kij <-orient <$x1 $x2 $x3> $y1 $y2 $y3> <-pDelta Mratios> <-iMod> <-mass $m>
expElement twoNodeLink 1 1 3 -dir 2 -site 1 -initStif 2.8
expElement twoNodeLink 2 2 4 -dir 2 -site 2 -initStif 5.6

# Define numerical elements
# -------------------------
# spring
# element truss $eleTag $iNode $jNode $A $matTag
element truss 3 3 4 1.0 3

# Define dynamic loads
# --------------------
# set time series to be passed to uniform excitation
set dt 0.02
set scale 1.0
timeSeries Path 1 -filePath elcentro.txt -dt $dt -factor [expr 386.1*$scale]

# create UniformExcitation load pattern
# pattern UniformExcitation $tag $dir -accel $tsTag <-vel0 $vel0>
pattern UniformExcitation 1 1 -accel 1

# calculate the Rayleigh damping factors for nodes & elements
set alphaM     1.010017396536;  # D = alphaM*M
set betaK      0.0;             # D = betaK*Kcurrent
set betaKinit  0.0;             # D = beatKinit*Kinit
set betaKcomm  0.0;             # D = betaKcomm*KlastCommit

# set the Rayleigh damping 
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
test NormDispIncr 1.0e-12 10
# create the integration scheme
#integrator Newmark 0.5 0.25
integrator NewmarkExplicit 0.5
#integrator AlphaOS 1.0
# create the solution algorithm
algorithm Linear
#algorithm Newton
# create the analysis object 
analysis Transient
# ------------------------------
# End of analysis generation
# ------------------------------


# ------------------------------
# Start of recorder generation
# ------------------------------
# create the recorder objects
recorder Node -file Master_Node_Dsp.out -time -node 3 4 -dof 1 disp
recorder Node -file Master_Node_Vel.out -time -node 3 4 -dof 1 vel
recorder Node -file Master_Node_Acc.out -time -node 3 4 -dof 1 accel

recorder Element -file Master_Elmt_Frc.out     -time -ele 1 2 3 forces
recorder Element -file Master_Elmt_ctrlDsp.out -time -ele 1 2   ctrlDisp
recorder Element -file Master_Elmt_daqDsp.out  -time -ele 1 2   daqDisp
# --------------------------------
# End of recorder generation
# --------------------------------


# ------------------------------
# Finally perform the analysis
# ------------------------------
# perform an eigenvalue analysis
set pi [expr acos(-1.0)]
set lambda [eigen -fullGenLapack 2]
puts "\nEigenvalues at start of transient:"
puts "|   lambda   |  omega   |  period | frequency |"
foreach lambda $lambda {
    set omega [expr pow($lambda,0.5)]
    set period [expr 2.0*$pi/$omega]
    set frequ [expr 1.0/$period]
    puts [format "| %5.3e | %8.4f | %7.4f | %9.4f |" $lambda $omega $period $frequ]
}

# open output file for writing
set outFileID [open elapsedTime.txt w]
# perform the transient analysis
set tTot [time {
    for {set i 1} {$i < 1600} {incr i} {
        set t [time {analyze  1  $dt}]
        puts $outFileID $t
        #puts "step $i"
    }
}]
puts "\nElapsed Time = $tTot \n"
# close the output file
close $outFileID

wipe
exit
# --------------------------------
# End of analysis
# --------------------------------
