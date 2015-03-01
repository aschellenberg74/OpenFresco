# File: OneBayFrame_Server1b.tcl (use with OneBayFrame_Client1.tcl)
# Units: [kip,in.]
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
uniaxialMaterial Elastic 1  5.6

# Define control points
# ---------------------
# expControlPoint $tag <-node $nodeTag> $dof $rspType <-fact $f> <-lim $l $u> ...
expControlPoint 1  1 disp
expControlPoint 2  1 disp 1 force

# Define experimental control
# ---------------------------
# expControl SimUniaxialMaterials $tag $matTags
expControl SimUniaxialMaterials 2 1
#expControl xPCtarget 2 "192.168.2.20" 22222 "D:/PredictorCorrector/RTActualTestModels/cmAPI-xPCTarget-SCRAMNet-STS/HybridControllerD2D2" -trialCP 1 -outCP 2

# Define experimental site
# ------------------------
# expSite ActorSite $tag -control $ctrlTag $ipPort <-ssl> <-udp>
expSite ActorSite 2 -control 2 8091

# setupLabServer $siteTag
setupLabServer  2
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

expRecorder Control -file ServerBControl_ctrlDsp.out -time -control 2 ctrlDisp
expRecorder Control -file ServerBControl_ctrlVel.out -time -control 2 ctrlVel
expRecorder Control -file ServerBControl_daqDsp.out -time -control 2 daqDisp
expRecorder Control -file ServerBControl_daqVel.out -time -control 2 daqVel
expRecorder Control -file ServerBControl_daqFrc.out -time -control 2 daqForce
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
