# File: OneBayFrame_Server1a.tcl (use with OneBayFrame_Client1.tcl)
# Units: [kip,in.]
#
# Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
# Created: 11/06
# Revision: A
#
# Purpose: this file contains the tcl input to perform
# a distributed hybrid simulation of a one bay frame
# with two experimental twoNodeLink elements.
# The specimen is simulated using the SimUniaxialMaterials
# controller.
# The experimental setups are on the server sides.


# ------------------------------
# Start of model generation
# ------------------------------
logFile "OneBayFrame_Server1a.log"
defaultUnits -force kip -length in -time sec -temp F

# create ModelBuilder (with two-dimensions and 2 DOF/node)
model BasicBuilder -ndm 2 -ndf 2

# Define materials
# ----------------
# uniaxialMaterial Steel02 $matTag $Fy $E $b $R0 $cR1 $cR2 $a1 $a2 $a3 $a4 
#uniaxialMaterial Elastic 1 2.8
uniaxialMaterial Steel02 1 1.5 2.8 0.01 18.5 0.925 0.15 0.0 1.0 0.0 1.0

# Define control points
# ---------------------
# expControlPoint $tag <-node $nodeTag> $dof $rspType <-fact $f> <-lim $l $u> ...
expControlPoint 1  1 disp
expControlPoint 2  1 disp 1 force

# Define experimental control
# ---------------------------
# expControl SimUniaxialMaterials $tag $matTags
expControl SimUniaxialMaterials 1 1
#expControl xPCtarget 1 "192.168.2.20" 22222 "D:/PredictorCorrector/RTActualTestModels/cmAPI-xPCTarget-SCRAMNet-STS/HybridControllerD2D2" -trialCP 1 -outCP 2

# Define experimental setup
# -------------------------
# expSetup OneActuator $tag <-control $ctrlTag> $dir -sizeTrialOut $t $o <-trialDispFact $f> ...
expSetup OneActuator 1 -control 1 1 -sizeTrialOut 1 1

# Define experimental site
# ------------------------
# expSite ActorSite $tag -setup $setupTag $ipPort <-ssl> <-udp>
expSite ActorSite 1 -setup 1 8090
# ------------------------------
# End of model generation
# ------------------------------


# ------------------------------
# Start of recorder generation
# ------------------------------
# create the recorder objects
expRecorder Site -file ServerASite_trialDsp.out -time -site 1 trialDisp
expRecorder Site -file ServerASite_trialVel.out -time -site 1 trialVel
expRecorder Site -file ServerASite_trialAcc.out -time -site 1 trialAccel
expRecorder Site -file ServerASite_trialTme.out -time -site 1 trialTime
expRecorder Site -file ServerASite_outDsp.out -time -site 1 outDisp
expRecorder Site -file ServerASite_outVel.out -time -site 1 outVel
expRecorder Site -file ServerASite_outAcc.out -time -site 1 outAccel
expRecorder Site -file ServerASite_outFrc.out -time -site 1 outForce
expRecorder Site -file ServerASite_outTme.out -time -site 1 outTime

expRecorder Setup -file ServerASetup_trialDsp.out -time -setup 1 trialDisp
expRecorder Setup -file ServerASetup_trialVel.out -time -setup 1 trialVel
expRecorder Setup -file ServerASetup_trialAcc.out -time -setup 1 trialAccel
expRecorder Setup -file ServerASetup_trialTme.out -time -setup 1 trialTime
expRecorder Setup -file ServerASetup_outDsp.out -time -setup 1 outDisp
expRecorder Setup -file ServerASetup_outVel.out -time -setup 1 outVel
expRecorder Setup -file ServerASetup_outAcc.out -time -setup 1 outAccel
expRecorder Setup -file ServerASetup_outFrc.out -time -setup 1 outForce
expRecorder Setup -file ServerASetup_outTme.out -time -setup 1 outTime
expRecorder Setup -file ServerASetup_ctrlDsp.out -time -setup 1 ctrlDisp
expRecorder Setup -file ServerASetup_ctrlVel.out -time -setup 1 ctrlVel
expRecorder Setup -file ServerASetup_ctrlAcc.out -time -setup 1 ctrlAccel
expRecorder Setup -file ServerASetup_ctrlTme.out -time -setup 1 ctrlTime
expRecorder Setup -file ServerASetup_daqDsp.out -time -setup 1 daqDisp
expRecorder Setup -file ServerASetup_daqVel.out -time -setup 1 daqVel
expRecorder Setup -file ServerASetup_daqAcc.out -time -setup 1 daqAccel
expRecorder Setup -file ServerASetup_daqFrc.out -time -setup 1 daqForce
expRecorder Setup -file ServerASetup_daqTme.out -time -setup 1 daqTime

expRecorder Control -file ServerAControl_ctrlDsp.out -time -control 1 ctrlDisp
expRecorder Control -file ServerAControl_ctrlVel.out -time -control 1 ctrlVel
expRecorder Control -file ServerAControl_daqDsp.out -time -control 1 daqDisp
expRecorder Control -file ServerAControl_daqVel.out -time -control 1 daqVel
expRecorder Control -file ServerAControl_daqFrc.out -time -control 1 daqForce
# --------------------------------
# End of recorder generation
# --------------------------------


# ------------------------------
# Start the server process
# ------------------------------
# startLabServer $siteTag
startLabServer  1

wipeExp
exit
# --------------------------------
# End of analysis
# --------------------------------
