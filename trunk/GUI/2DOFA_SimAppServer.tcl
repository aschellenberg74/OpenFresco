# File: 2DOFA_SimAppServer.tcl (use with OpenFrescoGUI)
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
model BasicBuilder -ndm 2 -ndf 2

# Define geometry for model
# -------------------------
# node $tag $xCrd $yCrd $mass
node   1    0.0   0.0
node   2    0.0   0.0
node   3    0.0   0.0

# set the boundary conditions
# fix $tag $DX $DY
fix 1   1  1
fix 2   0  1
fix 3   0  1

# Define materials
# ----------------
# uniaxialMaterial Steel02 $matTag $Fy $E $b $R0 $cR1 $cR2 $a1 $a2 $a3 $a4 
uniaxialMaterial Elastic 1  7.0
uniaxialMaterial Elastic 2  3.0
#uniaxialMaterial Steel02 2 3.0  7.0 0.01 18.5 0.925 0.15 0.0 1.0 0.0 1.0
#uniaxialMaterial Steel02 1 1.5  3.0 0.01 18.5 0.925 0.15 0.0 1.0 0.0 1.0

# Define elements
# ---------------
# element zeroLength $eleTag $iNode $jNode -mat $matTags -dir $dirs <-orient $x1 $x2 $x3 $y1 $y2 $y3>
element zeroLength 1 1 2 -mat 1 -dir 1
element zeroLength 2 2 3 -mat 2 -dir 1

# Define control points
# ---------------------
# expControlPoint tag nodeTag dir resp <-fact f> <-lim l u> ...
expControlPoint 1 2  ux disp
expControlPoint 2 2  ux disp ux force
expControlPoint 3 3  ux disp
expControlPoint 4 3  ux disp ux force

# Define experimental control
# ---------------------------
# expControl SimDomain $tag -trialCP cpTags -outCP cpTags
expControl SimDomain  1  -trialCP 1 3  -outCP 2 4
#expControl xPCtarget 1 1 "192.168.2.20" 22222 HybridControllerD3D3_1Act "D:/PredictorCorrector/RTActualTestModels/cmAPI-xPCTarget-STS"

# Define experimental setup
# -------------------------
# expSetup NoTransformation $tag <-control $ctrlTag> -dir $dirs -sizeTrialOut t o <–ctrlDispFact $f> ...
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
