# File: LShapedColumn_Server1a.tcl (use with LShapedColumn_Client1.tcl)
# Units: [kip,in.]
#
# Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
# Created: 07/07
# Revision: A
#
# Purpose: this file contains the tcl input to perform
# a distributed hybrid simulation of a L-shaped column
# with one experimental beamColumn element.
# Since the shear and moment DOF are coupled, the
# specimen is simulated using the SimDomain controller.
# The NoTransformation experimental setup is used.


# ------------------------------
# Start of model generation
# ------------------------------
logFile "LShapedColumn_Server1a.log"
defaultUnits -force kip -length in -time sec -temp F

# create ModelBuilder (with two-dimensions and 2 DOF/node)
model BasicBuilder -ndm 2 -ndf 3

# Load OpenFresco package
# -----------------------
# (make sure all dlls are in the same folder as OpenFrescoTcl)
loadPackage OpenFrescoTcl

# Define geometry for model
# -------------------------
# node $tag $xCrd $yCrd $mass
node  1     0.0   0.0
node  2     0.0  54.0

# set the boundary conditions
# fix tag DX DY RZ
fix  1   1  1  1

# Define materials
# ----------------
# uniaxialMaterial Steel02 $matTag $Fy $E $b $R0 $cR1 $cR2 $a1 $a2 $a3 $a4 
uniaxialMaterial Elastic 1 [expr 2.26*29000]
uniaxialMaterial Steel02 2 [expr 1.5*54] 146966.4 0.01 18.5 0.925 0.15 0.0 1.0 0.0 1.0

# Define sections
# ---------------
# section Elastic $secTag $E $A $Iz
#section Elastic 1 29000 2.26 5.067806896551724
# section Aggregator $secTag $matTag1 $string1 $matTag2 $string2
section Aggregator 1 1 P 2 Mz

# Define coordinate transformation
# --------------------------------
# geomTransf Linear $transfTag
geomTransf Linear 1

# Define element
# --------------
# element forceBeamColumn $eleTag $iNode $jNode $numIntgrPts $secTag $transfTag
element forceBeamColumn 1 1 2 5 1 1

# Define control points
# ---------------------
# expControlPoint $tag <-node $nodeTag> $dof $rspType <-fact $f> <-lim $l $u> <-isRel> ...
expControlPoint 1 -node 2  1 disp  2 disp  3 disp
expControlPoint 2 -node 2  1 disp  2 disp  3 disp 1 force  2 force  3 force

# Define experimental control
# ---------------------------
# expControl SimDomain $tag -trialCP $cpTags -outCP $cpTags
expControl SimDomain  1  -trialCP 1  -outCP 2

# Define experimental setup
# -------------------------
# expSetup NoTransformation $tag <�control $ctrlTag> �dof $DOFs -sizeTrialOut $t $o <�trialDispFact $f> ...
expSetup NoTransformation 1 -control 1 -dof 2 1 3 -sizeTrialOut 3 3 -trialDispFact 1 -1 1 -outDispFact 1 -1 1 -outForceFact 1 -1 1

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
recorder Node -file ServerANode_Disp.out -time -node 2 -dof 1 2 3 disp

expRecorder Site -file ServerASite_trialDsp.out -time -site 1 trialDisp
expRecorder Site -file ServerASite_outDsp.out  -time -site 1 daqDisp
expRecorder Site -file ServerASite_outFrc.out  -time -site 1 daqForce
expRecorder Control -file ServerACtrl_ctrlDsp.out -time -control 1 ctrlDisp
expRecorder Control -file ServerACtrl_daqDsp.out  -time -control 1 daqDisp
expRecorder Control -file ServerACtrl_daqFrc.out  -time -control 1 daqForce
# --------------------------------
# End of recorder generation
# --------------------------------


# ------------------------------
# Start the server process
# ------------------------------
record

# startLabServer $siteTag
startLabServer  1

wipeExp
exit
# --------------------------------
# End of analysis
# --------------------------------
