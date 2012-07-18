# File: OneBayFrame_Server1b.tcl (use with OneBayFrame_Client1.tcl)
#
# $Revision$
# $Date$
# $URL$
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
# create ModelBuilder (with two-dimensions and 2 DOF/node)
model BasicBuilder -ndm 2 -ndf 2

# Define materials
# ----------------
# uniaxialMaterial Steel02 $matTag $Fy $E $b $R0 $cR1 $cR2 $a1 $a2 $a3 $a4
uniaxialMaterial Elastic 1  5.6;  # UC Berkeley Cantilever Column [kip/in]
#uniaxialMaterial Elastic 1 16.0;  # UBC Cantilever Column [kN/cm]
#uniaxialMaterial Elastic 1 ????;  # Kyoto University Cantilever Column [??/??]

# Define experimental control
# ---------------------------
# expControl SimUniaxialMaterials $tag $matTags
expControl SimUniaxialMaterials 1 1
#expControl xPCtarget 1 1 "192.168.2.20" 22222 HybridControllerD3D3_1Act "D:/PredictorCorrector/RTActualTestModels/cmAPI-xPCTarget-STS"

# Define experimental setup
# -------------------------
# expSetup OneActuator $tag <-control $ctrlTag> $dir -sizeTrialOut $t $o <-trialDispFact $f> ...
expSetup OneActuator 1 -control 1 1 -sizeTrialOut 1 1;  # UC Berkeley setup
#expSetup OneActuator 1 -control 1 1 -sizeTrialOut 1 1 -trialDispFact 2.54 -outDispFact [expr 1.0/2.54] -outForceFact 0.2248;  # UBC setup with units conversion
#expSetup OneActuator 1 -control 1 1 -sizeTrialOut 1 1 -trialDispFact ??? -outDispFact ??? -outForceFact ???;  # Kyoto University with units conversion

# Define experimental site
# ------------------------
# expSite ActorSite $tag -setup $setupTag $ipPort <-ssl> <-udp>
expSite ActorSite 2 -setup 1 8091
# ------------------------------
# End of model generation
# ------------------------------


# ------------------------------
# Start of recorder generation
# ------------------------------
# create the recorder objects
expRecorder Site -file ServerBSite_trialDsp.out -time -site 2 trialDisp
expRecorder Site -file ServerBSite_trialVel.out -time -site 2 trialVel
expRecorder Site -file ServerBSite_trialAcc.out -time -site 2 trialAccel
expRecorder Site -file ServerBSite_trialTme.out -time -site 2 trialTime
expRecorder Site -file ServerBSite_outDsp.out -time -site 2 outDisp
expRecorder Site -file ServerBSite_outVel.out -time -site 2 outVel
expRecorder Site -file ServerBSite_outAcc.out -time -site 2 outAccel
expRecorder Site -file ServerBSite_outFrc.out -time -site 2 outForce
expRecorder Site -file ServerBSite_outTme.out -time -site 2 outTime

expRecorder Setup -file ServerBSetup_trialDsp.out -time -setup 1 trialDisp
expRecorder Setup -file ServerBSetup_trialVel.out -time -setup 1 trialVel
expRecorder Setup -file ServerBSetup_trialAcc.out -time -setup 1 trialAccel
expRecorder Setup -file ServerBSetup_trialTme.out -time -setup 1 trialTime
expRecorder Setup -file ServerBSetup_outDsp.out -time -setup 1 outDisp
expRecorder Setup -file ServerBSetup_outVel.out -time -setup 1 outVel
expRecorder Setup -file ServerBSetup_outAcc.out -time -setup 1 outAccel
expRecorder Setup -file ServerBSetup_outFrc.out -time -setup 1 outForce
expRecorder Setup -file ServerBSetup_outTme.out -time -setup 1 outTime
expRecorder Setup -file ServerBSetup_ctrlDsp.out -time -setup 1 ctrlDisp
expRecorder Setup -file ServerBSetup_ctrlVel.out -time -setup 1 ctrlVel
expRecorder Setup -file ServerBSetup_ctrlAcc.out -time -setup 1 ctrlAccel
expRecorder Setup -file ServerBSetup_ctrlTme.out -time -setup 1 ctrlTime
expRecorder Setup -file ServerBSetup_daqDsp.out -time -setup 1 daqDisp
expRecorder Setup -file ServerBSetup_daqVel.out -time -setup 1 daqVel
expRecorder Setup -file ServerBSetup_daqAcc.out -time -setup 1 daqAccel
expRecorder Setup -file ServerBSetup_daqFrc.out -time -setup 1 daqForce
expRecorder Setup -file ServerBSetup_daqTme.out -time -setup 1 daqTime

expRecorder Control -file ServerBControl_ctrlDsp.out -time -control 1 ctrlDisp
expRecorder Control -file ServerBControl_ctrlVel.out -time -control 1 ctrlVel
expRecorder Control -file ServerBControl_daqDsp.out -time -control 1 daqDisp
expRecorder Control -file ServerBControl_daqVel.out -time -control 1 daqVel
expRecorder Control -file ServerBControl_daqFrc.out -time -control 1 daqForce
# --------------------------------
# End of recorder generation
# --------------------------------


# ------------------------------
# Start the server process
# ------------------------------
# startLabServer $siteTag
startLabServer  2
exit
# --------------------------------
# End of analysis
# --------------------------------
