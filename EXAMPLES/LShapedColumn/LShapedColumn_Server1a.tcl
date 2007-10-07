# File: LShapedColumn_Server1a.tcl (use with LShapedColumn_Client1.tcl)
#
# $Revision: $
# $Date: $
# $URL: $
#
# Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
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
# create ModelBuilder (with two-dimensions and 2 DOF/node)
model BasicBuilder -ndm 2 -ndf 3

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
# element nonlinearBeamColumn $eleTag $iNode $jNode $numIntgrPts $secTag $transfTag
element nonlinearBeamColumn 1 1 2 5 1 1

# Define control points
# ---------------------
# expControlPoint tag nodeTag dir resp <-fact f> <-lim l u> ...
expControlPoint 1 2  ux disp uy disp rz disp
expControlPoint 2 2  ux disp ux force uy disp uy force rz disp rz force

# Define experimental control
# ---------------------------
# expControl SimDomain $tag -trialCP cpTags -outCP cpTags
expControl SimDomain  1  -trialCP 1  -outCP 2

# Define experimental setup
# -------------------------
# expSetup NoTransformation $tag <–control $ctrlTag> –dir $dirs … <–ctrlDispFact $f> ...
expSetup NoTransformation 1 -control 1 -dir 2 1 3 -ctrlDispFact -1 1 1 -daqDispFact -1 1 1  -daqForceFact -1 1 1


# Define experimental site
# ------------------------
# expSite ActorSite $tag -setup $setupTag $ipPort <-ssl>
expSite ActorSite 1 -setup 1 8090
# ------------------------------
# End of model generation
# ------------------------------


# ------------------------------
# Start the server process
# ------------------------------
# startLabServer $siteTag
startLabServer  1
# --------------------------------
# End of analysis
# --------------------------------
