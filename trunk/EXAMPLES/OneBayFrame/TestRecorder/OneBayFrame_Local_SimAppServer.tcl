# File: OneBayFrame_Local_SimAppServer.tcl (use with OneBayFrame_Local_Client.tcl)
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
# a local hybrid simulation of a one bay frame
# with two experimental twoNodeLink elements.
# The specimen is simulated using the SimUniaxialMaterials
# controller.


# ------------------------------
# Start of model generation
# ------------------------------
# create ModelBuilder (with two-dimensions and 2 DOF/node)
model BasicBuilder -ndm 2 -ndf 2

# Define geometry for model
# -------------------------
# node $tag $xCrd $yCrd $mass
node  1     0.0   0.00
node  3     0.0  54.00

# Define materials
# ----------------
# uniaxialMaterial Steel02 $matTag $Fy $E $b $R0 $cR1 $cR2 $a1 $a2 $a3 $a4 
#uniaxialMaterial Elastic 1 2.8
uniaxialMaterial Steel02 1 1.5 2.8 0.01 18.5 0.925 0.15 0.0 1.0 0.0 1.0

# Define experimental control
# ---------------------------
# expControl SimUniaxialMaterials $tag $matTags
expControl SimUniaxialMaterials 1 1
#expControl xPCtarget 1 1 "192.168.2.20" 22222 HybridControllerD3D3_1Act "D:/PredictorCorrector/RTActualTestModels/cmAPI-xPCTarget-STS"

# Define experimental setup
# -------------------------
# expSetup OneActuator $tag <-control $ctrlTag> $dir -sizeTrialOut $t $o <-trialDispFact $f> ...
expSetup OneActuator 1 -control 1 1 -sizeTrialOut 1 1

# Define experimental site
# ------------------------
# expSite LocalSite $tag $setupTag
expSite LocalSite 1 1

# Define experimental element
# ---------------------------
# left column
# expElement twoNodeLink $eleTag $iNode $jNode -dir $dirs -site $siteTag -initStif $Kij <-orient <$x1 $x2 $x3> $y1 $y2 $y3> <-pDelta Mratios> <-iMod> <-mass $m>
expElement twoNodeLink 1 1 3 -dir 2 -site 1 -initStif 2.8
# ------------------------------
# End of model generation
# ------------------------------


# ------------------------------
# Start of recorder generation
# ------------------------------
# create the recorder objects
recorder Node -file ServerNode_Dsp.out -time -node 1 3 -dof 1 disp
recorder Node -file ServerNode_Vel.out -time -node 1 3 -dof 1 vel
recorder Node -file ServerNode_Acc.out -time -node 1 3 -dof 1 accel

recorder Element -file ServerElmt_Frc.out     -time -ele 1 forces
recorder Element -file ServerElmt_ctrlDsp.out -time -ele 1 ctrlDisp
recorder Element -file ServerElmt_daqDsp.out  -time -ele 1 daqDisp

expRecorder Site -file ServerSite_trialDsp.out -time -site 1 trialDisp
expRecorder Site -file ServerSite_trialVel.out -time -site 1 trialVel
expRecorder Site -file ServerSite_trialAcc.out -time -site 1 trialAccel
expRecorder Site -file ServerSite_trialTme.out -time -site 1 trialTime
expRecorder Site -file ServerSite_outDsp.out -time -site 1 outDisp
expRecorder Site -file ServerSite_outVel.out -time -site 1 outVel
expRecorder Site -file ServerSite_outAcc.out -time -site 1 outAccel
expRecorder Site -file ServerSite_outFrc.out -time -site 1 outForce
expRecorder Site -file ServerSite_outTme.out -time -site 1 outTime

expRecorder Setup -file ServerSetup_trialDsp.out -time -setup 1 trialDisp
expRecorder Setup -file ServerSetup_trialVel.out -time -setup 1 trialVel
expRecorder Setup -file ServerSetup_trialAcc.out -time -setup 1 trialAccel
expRecorder Setup -file ServerSetup_trialTme.out -time -setup 1 trialTime
expRecorder Setup -file ServerSetup_outDsp.out -time -setup 1 outDisp
expRecorder Setup -file ServerSetup_outVel.out -time -setup 1 outVel
expRecorder Setup -file ServerSetup_outAcc.out -time -setup 1 outAccel
expRecorder Setup -file ServerSetup_outFrc.out -time -setup 1 outForce
expRecorder Setup -file ServerSetup_outTme.out -time -setup 1 outTime
expRecorder Setup -file ServerSetup_ctrlDsp.out -time -setup 1 ctrlDisp
expRecorder Setup -file ServerSetup_ctrlVel.out -time -setup 1 ctrlVel
expRecorder Setup -file ServerSetup_ctrlAcc.out -time -setup 1 ctrlAccel
expRecorder Setup -file ServerSetup_ctrlTme.out -time -setup 1 ctrlTime
expRecorder Setup -file ServerSetup_daqDsp.out -time -setup 1 daqDisp
expRecorder Setup -file ServerSetup_daqVel.out -time -setup 1 daqVel
expRecorder Setup -file ServerSetup_daqAcc.out -time -setup 1 daqAccel
expRecorder Setup -file ServerSetup_daqFrc.out -time -setup 1 daqForce
expRecorder Setup -file ServerSetup_daqTme.out -time -setup 1 daqTime

expRecorder Control -file ServerControl_ctrlDsp.out -time -control 1 ctrlDisp
expRecorder Control -file ServerControl_ctrlVel.out -time -control 1 ctrlVel
expRecorder Control -file ServerControl_daqDsp.out -time -control 1 daqDisp
expRecorder Control -file ServerControl_daqVel.out -time -control 1 daqVel
expRecorder Control -file ServerControl_daqFrc.out -time -control 1 daqForce
# --------------------------------
# End of recorder generation
# --------------------------------


# ------------------------------
# Start the server process
# ------------------------------
# startSimAppElemServer $eleTag $port <-ssl>
#startSimAppElemServer 1 8090;  # use with generic client element in FEA

# startSimAppSiteServer $siteTag $port <-ssl>
startSimAppSiteServer 1 8090;  # use with experimental element in FEA
# --------------------------------
# End of analysis
# --------------------------------
