# File: OneBayFrame_Local.tcl
# Units: [kip,in.]
#
# Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
# Created: 11/06
# Revision: A
#
# Purpose: this file contains the tcl input to perform
# a local hybrid simulation of a one bay frame with
# two experimental twoNodeLink elements.
# The specimens are simulated using the SimUniaxialMaterials
# controller.


# ------------------------------
# Start of model generation
# ------------------------------
logFile "OneBayFrame_Local.log"
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
# uniaxialMaterial Steel02 $matTag $Fy $E $b $R0 $cR1 $cR2 $a1 $a2 $a3 $a4 
#uniaxialMaterial Elastic 1 2.8
uniaxialMaterial Steel02 1 1.5 2.8 0.01 18.5 0.925 0.15 0.0 1.0 0.0 1.0
uniaxialMaterial Elastic 2 5.6
#uniaxialMaterial Steel02 2 3.0 5.6 0.01 18.5 0.925 0.15 0.0 1.0 0.0 1.0 
uniaxialMaterial Elastic 3 [expr 2.0*100.0/1.0]

# Define control points
# ---------------------
# expControlPoint $tag <-node $nodeTag> $dof $rspType <-fact $f> <-lim $l $u> <-isRel> ...
expControlPoint 1  1 disp
expControlPoint 2  1 disp 1 force

# Define experimental control
# ---------------------------
# expControl SimUniaxialMaterials $tag $matTags
expControl SimUniaxialMaterials 1 1
#expControl xPCtarget 1 "192.168.2.20" 22222 "D:/PredictorCorrector/RTActualTestModels/cmAPI-xPCTarget-SCRAMNet-STS/HybridControllerD2D2" -trialCP 1 -outCP 2
#expControl SCRAMNet 1 381020 8 -trialCP 1 -outCP 2
#expControl dSpace 1 DS1104 -trialCP 1 -outCP 2
#expControl MTSCsi 1 "D:/Projects/MTS_CSI/OpenFresco/MtsCsi_Example/OneBayFrame/OpenFresco_mNEES.mtscs" 0.01 -trialCP 1 -outCP 2
#expControl GenericTCP 1 "127.0.0.1" 44000 -ctrlModes 1 0 0 0 0 -daqModes 1 0 0 1 0
expControl SimUniaxialMaterials 2 2

# Define experimental setup
# -------------------------
# expSetup OneActuator $tag <-control $ctrlTag> $dir -sizeTrialOut $t $o <-trialDispFact $f> ...
expSetup OneActuator 1 -control 1 1 -sizeTrialOut 1 1
expSetup OneActuator 2 -control 2 1 -sizeTrialOut 1 1

# Define experimental site
# ------------------------
# expSite LocalSite $tag $setupTag
expSite LocalSite 1 1
expSite LocalSite 2 2

# Define experimental elements
# ----------------------------
# left and right columns
# expElement twoNodeLink $eleTag $iNode $jNode -dir $dirs -site $siteTag -initStif $Kij <-tangStif tangStifTag> <-orient <$x1 $x2 $x3> $y1 $y2 $y3> <-pDelta Mratios> <-iMod> <-mass $m>
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
recorder Node -xml Node_Dsp.xml -time -node 3 4 -dof 1 disp
recorder Node -xml Node_Vel.xml -time -node 3 4 -dof 1 vel
recorder Node -xml Node_Acc.xml -time -node 3 4 -dof 1 accel

recorder Element -xml Elmt_Frc.xml     -time -ele 1 2 3 forces
recorder Element -xml Elmt_ctrlDsp.xml -time -ele 1 2   ctrlDisp
recorder Element -xml Elmt_daqDsp.xml  -time -ele 1 2   daqDisp

expRecorder Site -xml Site_trialDsp.xml -time -site 1 2 trialDisp
expRecorder Site -xml Site_trialVel.xml -time -site 1 2 trialVel
expRecorder Site -xml Site_trialAcc.xml -time -site 1 2 trialAccel
expRecorder Site -xml Site_trialTme.xml -time -site 1 2 trialTime
expRecorder Site -xml Site_outDsp.xml -time -site 1 2 outDisp
expRecorder Site -xml Site_outVel.xml -time -site 1 2 outVel
expRecorder Site -xml Site_outAcc.xml -time -site 1 2 outAccel
expRecorder Site -xml Site_outFrc.xml -time -site 1 2 outForce
expRecorder Site -xml Site_outTme.xml -time -site 1 2 outTime

expRecorder Setup -xml Setup_trialDsp.xml -time -setup 1 2 trialDisp
expRecorder Setup -xml Setup_trialVel.xml -time -setup 1 2 trialVel
expRecorder Setup -xml Setup_trialAcc.xml -time -setup 1 2 trialAccel
expRecorder Setup -xml Setup_trialTme.xml -time -setup 1 2 trialTime
expRecorder Setup -xml Setup_outDsp.xml -time -setup 1 2 outDisp
expRecorder Setup -xml Setup_outVel.xml -time -setup 1 2 outVel
expRecorder Setup -xml Setup_outAcc.xml -time -setup 1 2 outAccel
expRecorder Setup -xml Setup_outFrc.xml -time -setup 1 2 outForce
expRecorder Setup -xml Setup_outTme.xml -time -setup 1 2 outTime
expRecorder Setup -xml Setup_ctrlDsp.xml -time -setup 1 2 ctrlDisp
expRecorder Setup -xml Setup_ctrlVel.xml -time -setup 1 2 ctrlVel
expRecorder Setup -xml Setup_ctrlAcc.xml -time -setup 1 2 ctrlAccel
expRecorder Setup -xml Setup_ctrlTme.xml -time -setup 1 2 ctrlTime
expRecorder Setup -xml Setup_daqDsp.xml -time -setup 1 2 daqDisp
expRecorder Setup -xml Setup_daqVel.xml -time -setup 1 2 daqVel
expRecorder Setup -xml Setup_daqAcc.xml -time -setup 1 2 daqAccel
expRecorder Setup -xml Setup_daqFrc.xml -time -setup 1 2 daqForce
expRecorder Setup -xml Setup_daqTme.xml -time -setup 1 2 daqTime

expRecorder Control -xml Control_ctrlDsp.xml -time -control 1 2 ctrlDisp
expRecorder Control -xml Control_ctrlVel.xml -time -control 1 2 ctrlVel
expRecorder Control -xml Control_daqDsp.xml -time -control 1 2 daqDisp
expRecorder Control -xml Control_daqVel.xml -time -control 1 2 daqVel
expRecorder Control -xml Control_daqFrc.xml -time -control 1 2 daqForce
# --------------------------------
# End of recorder generation
# --------------------------------


# ------------------------------
# Finally perform the analysis
# ------------------------------
record
# perform an eigenvalue analysis
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
