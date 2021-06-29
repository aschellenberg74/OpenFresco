# File: OneBayFrame_Client1.tcl
# (use with OneBayFrame_Server1a.tcl & OneBayFrame_Server1b.tcl)
# Units: [kip,in.]
#
# Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
# Created: 11/06
# Revision: A
#
# Purpose: this file contains the tcl input to perform
# a distributed hybrid simulation of a one bay frame with
# two experimental twoNodeLink elements.
# The specimens are simulated using the SimUniaxialMaterials
# controller.


# ------------------------------
# Start of model generation
# ------------------------------
logFile "OneBayFrame_Client1.log"
defaultUnits -force kip -length in -time sec -temp F

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
node  3     0.0  54.00  -mass $mass3 $mass3
node  4   100.0  54.00  -mass $mass4 $mass4

# set the boundary conditions
# fix $tag $DX $DY
fix 1   1  1
fix 2   1  1
fix 3   0  1
fix 4   0  1

# Define materials
# ----------------
uniaxialMaterial Elastic 3 [expr 2.0*100.0/1.0]

# Define experimental site
# ------------------------
# expSite ShadowSite $tag <-setup $setupTag> $ipAddr $ipPort <-ssl> <-udp> <-dataSize $size>
expSite ShadowSite 1 "127.0.0.1" 8090
expSite ShadowSite 2 "127.0.0.1" 8091

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
test EnergyIncr 1.0e-6 10
# create the integration scheme
integrator NewmarkExplicit 0.5
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
recorder Node -file ClientNode_Dsp.out -time -node 3 4 -dof 1 disp
recorder Node -file ClientNode_Vel.out -time -node 3 4 -dof 1 vel
recorder Node -file ClientNode_Acc.out -time -node 3 4 -dof 1 accel

recorder Element -file ClientElmt_Frc.out     -time -ele 1 2 3 forces
recorder Element -file ClientElmt_ctrlDsp.out -time -ele 1 2   ctrlDisp
recorder Element -file ClientElmt_daqDsp.out  -time -ele 1 2   daqDisp

expRecorder Site -file ClientSite_trialDsp.out -time -site 1 2 trialDisp
expRecorder Site -file ClientSite_trialVel.out -time -site 1 2 trialVel
expRecorder Site -file ClientSite_trialAcc.out -time -site 1 2 trialAccel
expRecorder Site -file ClientSite_trialTme.out -time -site 1 2 trialTime
expRecorder Site -file ClientSite_outDsp.out -time -site 1 2 outDisp
expRecorder Site -file ClientSite_outVel.out -time -site 1 2 outVel
expRecorder Site -file ClientSite_outAcc.out -time -site 1 2 outAccel
expRecorder Site -file ClientSite_outFrc.out -time -site 1 2 outForce
expRecorder Site -file ClientSite_outTme.out -time -site 1 2 outTime

expRecorder Setup -file ClientSetup_trialDsp.out -time -setup 2 trialDisp
expRecorder Setup -file ClientSetup_trialVel.out -time -setup 2 trialVel
expRecorder Setup -file ClientSetup_trialAcc.out -time -setup 2 trialAccel
expRecorder Setup -file ClientSetup_trialTme.out -time -setup 2 trialTime
expRecorder Setup -file ClientSetup_outDsp.out -time -setup 2 outDisp
expRecorder Setup -file ClientSetup_outVel.out -time -setup 2 outVel
expRecorder Setup -file ClientSetup_outAcc.out -time -setup 2 outAccel
expRecorder Setup -file ClientSetup_outFrc.out -time -setup 2 outForce
expRecorder Setup -file ClientSetup_outTme.out -time -setup 2 outTime
expRecorder Setup -file ClientSetup_ctrlDsp.out -time -setup 2 ctrlDisp
expRecorder Setup -file ClientSetup_ctrlVel.out -time -setup 2 ctrlVel
expRecorder Setup -file ClientSetup_ctrlAcc.out -time -setup 2 ctrlAccel
expRecorder Setup -file ClientSetup_ctrlTme.out -time -setup 2 ctrlTime
expRecorder Setup -file ClientSetup_daqDsp.out -time -setup 2 daqDisp
expRecorder Setup -file ClientSetup_daqVel.out -time -setup 2 daqVel
expRecorder Setup -file ClientSetup_daqAcc.out -time -setup 2 daqAccel
expRecorder Setup -file ClientSetup_daqFrc.out -time -setup 2 daqForce
expRecorder Setup -file ClientSetup_daqTme.out -time -setup 2 daqTime
# --------------------------------
# End of recorder generation
# --------------------------------


# ------------------------------
# Finally perform the analysis
# ------------------------------
record
# perform an eigenvalue analysis
set pi [expr acos(-1.0)]
set lambda [eigen -fullGenLapack 2]
puts "\nEigenvalues at start of transient:"
puts "|   lambda  |   omega  |  period | frequency |"
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
wipeExp
wipe
exit
# --------------------------------
# End of analysis
# --------------------------------
