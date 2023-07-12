# File: LShapedColumn_Server2b.tcl (use with LShapedColumn_Client2.tcl)
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
# The ThreeActuators experimental setup is used.


# ------------------------------
# Start of model generation
# ------------------------------
logFile "LShapedColumn_Server2b.log"
defaultUnits -force kip -length in -time sec -temp F

# create ModelBuilder (with three-dimensions and 6 DOF/node)
model BasicBuilder -ndm 3 -ndf 6

# Load OpenFresco package
# -----------------------
# (make sure all dlls are in the same folder as openSees.exe)
loadPackage OpenFresco

# Define geometry for model
# -------------------------
# node $tag $xCrd $yCrd $zCrd $mass
# node  1     0.0  0.0   0.0
# node  2     0.0  0.0  54.0
# node  3     0.0  0.0  72.0
# node  4   -36.0  0.0  72.0
# node  5    36.0  0.0  72.0

node  1     0.0  0.0   0.0
node  3     0.0  0.0  54.0
node  4   -36.0  0.0  54.0
node  5    36.0  0.0  54.0


# set the boundary conditions
# fix $tag $DX $DY $DZ $RX $RY $RZ
fix   1    1   1   1   1   1   1
#fix   2    0   1   0   1   0   1
fix   3    0   1   0   1   0   1
fix   4    0   1   0   1   0   1
fix   5    0   1   0   1   0   1

# Define materials
# ----------------
# uniaxialMaterial Steel02 $matTag $Fy $E $b $R0 $cR1 $cR2 $a1 $a2 $a3 $a4 
uniaxialMaterial Elastic 1 [expr 2.26*29000]
uniaxialMaterial Steel02 2 [expr 1.5*54] 146966.4 0.01 18.5 0.925 0.15 0.0 1.0 0.0 1.0
uniaxialMaterial Elastic 3 1E8

# Define sections
# ---------------
# section Elastic $secTag $E $A $Iz $Iy $G $J <$alphaY> <$alphaZ>
#section Elastic 1 29000 2.26 5.067806896551724 5.067806896551724 11154 1000
# section Aggregator $secTag $matTag1 $string1 $matTag2 $string2
section Aggregator 1 1 P 2 Mz 2 My 3 T

# Define coordinate transformation
# --------------------------------
# geomTransf Linear $transfTag
geomTransf Linear 1  0 -1 0

# Define element
# --------------
# first story column and rigid loading beam
# element nonlinearBeamColumn $eleTag $iNode $jNode $numIntgrPts $secTag $transfTag
element nonlinearBeamColumn 1 1 3 5 1 1
#element elasticBeamColumn 2 2 3 2260 [expr 29000*1E3] [expr 11154*1E3] 1E8 5068 5068 1
element elasticBeamColumn 3 4 3 2260 [expr 29000*1E3] [expr 11154*1E3] 1E8 5068 5068 1
element elasticBeamColumn 4 3 5 2260 [expr 29000*1E3] [expr 11154*1E3] 1E8 5068 5068 1

# Define control points
# ---------------------
# expControlPoint $tag <-node $nodeTag> $dof $rspType <-fact $f> <-lim $l $u> <-isRel> ...
expControlPoint 1 -node 4  1 disp  3 disp
expControlPoint 2 -node 5  3 disp
expControlPoint 3 -node 4  1 disp  1 force  3 disp  3 force
expControlPoint 4 -node 5  3 disp  3 force

# Define experimental control
# ---------------------------
# expControl SimDomain $tag -trialCP $cpTags -outCP $cpTags
expControl SimDomain  1  -trialCP 1 2  -outCP 3 4

# Define experimental setup
# -------------------------
# expSetup ThreeActuators2d $tag <–control $ctrlTag> $La1 $La2 $La3 $L1 $L2 <–nlGeom> <–posAct1 $pos> <–phiLocX $phi> <-trialDispFact $f> ...
#expSetup ThreeActuators2d 1 -control 1 54.0 54.0 54.0 36.0 36.0 -phiLocX 90.0
# expSetup ThreeActuators $tag <–control $ctrlTag> $dofH $dofV $dofR $sizeTrial $sizeOut $La1 $La2 $La3 $L1 $L2 $L3 <–nlGeom> <–posAct1 $pos> <-trialDispFact $f> ...
expSetup ThreeActuators 1 -control 1  2 1 3  6 6  54.0 54.0 54.0 36.0 36.0 0.0 -trialDispFact 1 -1 1 0 0 0 -outDispFact 1 -1 1 0 0 0 -outForceFact 1 -1 1 0 0 0
# expSetup ThreeActuatorsJntOff $tag <–control $ctrlTag> $dofH $dofV $dofR $sizeTrial $sizeOut $La1 $La2 $La3 $L1 $L2 $L3 $L4 $L5 $L6 <–nlGeom> <–posAct1 $pos> <-trialDispFact $f> ...
#expSetup ThreeActuatorsJntOff 1 -control 1  2 1 3  3 3  54.0 54.0 54.0 12.0 36.0 36.0 12.0 12.0 12.0 -trialDispFact 1 -1 1 -outDispFact 1 -1 1 -outForceFact 1 -1 1

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
recorder Node -file ServerBNode_Disp.out -time -node 3 -dof 1 3 5 disp

expRecorder Setup -file ServerBSetup_trialDsp.out -time -setup 1 trialDisp
expRecorder Setup -file ServerBSetup_outDsp.out -time -setup 1 outDisp
expRecorder Setup -file ServerBSetup_outFrc.out -time -setup 1 outForce
expRecorder Setup -file ServerBSetup_ctrlDsp.out -time -setup 1 ctrlDisp
expRecorder Setup -file ServerBSetup_daqDsp.out -time -setup 1 daqDisp
expRecorder Setup -file ServerBSetup_daqFrc.out -time -setup 1 daqForce
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
