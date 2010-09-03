# File: 2DOFB_SimAppServer.tcl (use with OpenFrescoGUI)
#
# $Revision: $
# $Date: $
# $URL: $
#
# Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
# Created: 07/10
# Revision: A
#
# Purpose: 


# ------------------------------
# Start of model generation
# ------------------------------
# create ModelBuilder
model BasicBuilder -ndm 3 -ndf 3

# Define materials
# ----------------
# uniaxialMaterial Steel02 $matTag $Fy $E $b $R0 $cR1 $cR2 $a1 $a2 $a3 $a4 
uniaxialMaterial Elastic 1 5.6
uniaxialMaterial Elastic 2 2.8
#uniaxialMaterial Steel02 1 3.0 5.6 0.01 18.5 0.925 0.15 0.0 1.0 0.0 1.0
#uniaxialMaterial Steel02 2 1.5 2.8 0.01 18.5 0.925 0.15 0.0 1.0 0.0 1.0

# Define experimental control
# ---------------------------
# expControl SimUniaxialMaterials $tag $matTags
expControl SimUniaxialMaterials 1 1 2
#expControl xPCtarget 1 1 "192.168.2.20" 22222 HybridControllerD3D3_1Act "D:/PredictorCorrector/RTActualTestModels/cmAPI-xPCTarget-STS"

# Define experimental setup
# -------------------------
# expSetup NoTransformation $tag <-control $ctrlTag> -dir $dirs ?-sizeTrialOut t o <–ctrlDispFact $f> ...
expSetup NoTransformation 1 -control 1 -dir 1 2 -sizeTrialOut 2 2

# Define experimental site
# ------------------------
# expSite LocalSite $tag $setupTag
expSite LocalSite 1 1
# ------------------------------
# End of model generation
# ------------------------------


# ------------------------------
# Start of recorder generation
# ------------------------------
# create the recorder objects
#expRecorder Control -file Control_ctrlDsp.out -time -control 1  ctrlDisp
#expRecorder Control -file Control_daqDsp.out  -time -control 1  daqDisp
#expRecorder Control -file Control_daqFrc.out  -time -control 1  daqForce
# --------------------------------
# End of recorder generation
# --------------------------------


# ------------------------------
# Start the server process
# ------------------------------
# startSimAppSiteServer $siteTag $port <-ssl>
startSimAppSiteServer 1 8090
# --------------------------------
# End of analysis
# --------------------------------
