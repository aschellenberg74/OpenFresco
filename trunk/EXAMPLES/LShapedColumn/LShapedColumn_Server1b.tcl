# File: LShapedColumn_Server1b.tcl (use with LShapedColumn_Client1.tcl)
# Units: [kip,in.]
#
# $Revision$
# $Date$
# $URL$
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
logFile "LShapedColumn_Server1b.log"
defaultUnits -force kip -length in -time sec -temp F

# create ModelBuilder (with two-dimensions and 2 DOF/node)
model BasicBuilder -ndm 2 -ndf 3

# Load OpenFresco package
# -----------------------
# (make sure all dlls are in the same folder as openSees.exe)
loadPackage OpenFresco

# Define geometry for model
# -------------------------
# node $tag $xCrd $yCrd $mass
node  1     0.0   0.0
node  2     0.0  54.0
node  3   -36.0  54.0
node  4    36.0  54.0

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
# first story column and rigid loading beam
# element nonlinearBeamColumn $eleTag $iNode $jNode $numIntgrPts $secTag $transfTag
element nonlinearBeamColumn 1 1 2 5 1 1
element elasticBeamColumn 2 3 2 2260 29000 5068 1
element elasticBeamColumn 3 2 4 2260 29000 5068 1

# Define control points
# ---------------------
# expControlPoint $tag <-node $nodeTag> $dof $rspType <-fact $f> <-lim $l $u> <-isRel> ...
expControlPoint 1 -node 3  ux disp  uy disp
expControlPoint 2 -node 4  uy disp
expControlPoint 3 -node 3  ux disp  ux force  uy disp  uy force
expControlPoint 4 -node 4  uy disp  uy force

# Define experimental control
# ---------------------------
# expControl SimDomain $tag -trialCP $cpTags -outCP $cpTags
expControl SimDomain  1  -trialCP 1 2  -outCP 3 4

# Define experimental setup
# -------------------------
# expSetup ThreeActuators2d $tag <�control $ctrlTag> $La1 $La2 $La3 $L1 $L2 <�nlGeom> <�posAct1 $pos> <�phiLocX $phi> <-trialDispFact $f> ...
#expSetup ThreeActuators2d 1 -control 1 54.0 54.0 54.0 36.0 36.0 -phiLocX 90.0
# expSetup ThreeActuators $tag <�control $ctrlTag> $dofH $dofV $dofR $sizeTrial $sizeOut $La1 $La2 $La3 $L1 $L2 <�nlGeom> <�posAct1 $pos> <-trialDispFact $f> ...
expSetup ThreeActuators 1 -control 1 2 1 3 3 3 54.0 54.0 54.0 36.0 36.0 -trialDispFact 1 -1 1 -outDispFact 1 -1 1 -outForceFact 1 -1 1

# Define experimental site
# ------------------------
# expSite ActorSite $tag -setup $setupTag $ipPort <-ssl> <-udp>
expSite ActorSite 1 -setup 1 8090
# ------------------------------
# End of model generation
# ------------------------------


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
