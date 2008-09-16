# File: ThreeSpanBridge_Server1b.tcl (use with ThreeSpanBridge_Client1.tcl)
#
# $Revision: $
# $Date: $
# $URL: $
#
# Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
# Created: 09/06
# Revision: A
#
# Purpose: this file contains the tcl input to perform
# a distributed hybrid simulation of a three span bridge
# with two experimental beam column elements.
# The specimen is simulated using the SimUniaxialMaterials
# controller.
# The experimental setups are on the server sides.


# ------------------------------
# Start of model generation
# ------------------------------
# create ModelBuilder (with two-dimensions and 3 DOF/node)
model BasicBuilder -ndm 2 -ndf 3

# Define materials
# ----------------
# uniaxialMaterial Steel02 matTag $Fy $E $b $R0 $cR1 $cR2 $a1 $a2 $a3 $a4 
#uniaxialMaterial Elastic 1 6.0
uniaxialMaterial Steel02 1 2.0 6.0 0.01 18.5 0.925 0.15 0.0 1.0 0.0 1.0 

# Define experimental control
# ---------------------------
# expControl SimUniaxialMaterials $tag $matTags
expControl SimUniaxialMaterials 1 1

# Define experimental setup
# -------------------------
# expSetup OneActuator $tag <-control $ctrlTag> $dir -sizeTrialOut $t $o <-trialDispFact $f> ...
expSetup OneActuator 1 -control 1 2 -sizeTrialOut 3 3

# Define experimental site
# ------------------------
# expSite ActorSite $tag -setup $setupTag $ipPort <-ssl>
expSite ActorSite 2 -setup 1 8091
# ------------------------------
# End of model generation
# ------------------------------


# ------------------------------
# Start the server process
# ------------------------------
# startLabServer $siteTag
startLabServer  2
# --------------------------------
# End of analysis
# --------------------------------
