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
# a distributed hybrid simulation of a one bay frame
# with two experimental zero length elements.
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
#expControl xPCtarget 1 1 "192.168.2.20" "22222" "HybridControllerPoly3_1Act" "D:\\PredictorCorrector\\RTActualTestModels\\c&mAPI-xPCTarget-STS\\"

# Define experimental setup
# -------------------------
# expSetup OneActuator $tag <-control $ctrlTag> $dir <-ctrlDispFact $f> ...
expSetup OneActuator 1 -control 1 1

# Define experimental site
# ------------------------
# expSite LocalSite $tag $setupTag
expSite LocalSite 1 1

# Define experimental element
# ---------------------------
# left column
# expElement zeroLength $eleTag $iNode $jNode -dir $dirs -site $siteTag -initStif $Kij <-orient $x1 $x2 $x3 $y1 $y2 $y3> <-iMod> <-mass $m>
expElement zeroLength 1 1 3 -dir 2 -site 1 -initStif 2.8 -orient 0 1 0 -1 0 0
# ------------------------------
# End of model generation
# ------------------------------


# ------------------------------
# Start the server process
# ------------------------------
# startSimAppSiteServer $siteTag $port
startSimAppSiteServer 1 8090

# startSimAppElemServer $eleTag $port
#startSimAppElemServer 1 8090
# --------------------------------
# End of analysis
# --------------------------------
