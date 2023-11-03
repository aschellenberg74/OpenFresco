# File: Truss_Local.tcl
# Units: [kip,in.]
#
# Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
# Created: 11/06
# Revision: A
#
# Purpose: this file contains the tcl input to perform
# a hybrid simulation of a truss model with one experimental
# truss element.
# The specimen is simulated using the SimUniaxialMaterials
# controller.


# ------------------------------
# Start of model generation
# ------------------------------
logFile "Truss_Local.log"
defaultUnits -force kip -length in -time sec -temp F

# create ModelBuilder (with two-dimensions and 2 DOF/node)
model BasicBuilder -ndm 2 -ndf 2

# Load OpenFresco package
# -----------------------
# (make sure all dlls are in the same folder as openSees.exe)
loadPackage OpenFresco

# Define geometry for model
# -------------------------
# node $tag $xCrd $yCrd $mass
node 1   0.0  0.0
node 2 144.0  0.0
node 3 168.0  0.0
node 4  72.0 96.0

# set the boundary conditions
# fix $tag $DX $DY
fix 1 1 1 
fix 2 1 1
fix 3 1 1

# Define materials
# ----------------
set E 3000.0
set A3 5.0
set L3 [expr sqrt(pow(168.0-72.0,2.0) + pow(96.0,2.0))]
set kInit [expr $E*$A3/$L3]
# uniaxialMaterial Elastic $matTag $E
uniaxialMaterial Elastic 1 $E
#uniaxialMaterial Elastic 2 $kInit
uniaxialMaterial Steel01 2 50.0 $kInit 0.1

# Define experimental control
# ---------------------------
# expControl SimUniaxialMaterials $tag $matTags
expControl SimUniaxialMaterials 1 2

# Define experimental setup
# -------------------------
# expSetup OneActuator $tag <-control $ctrlTag> $dir -sizeTrialOut $t $o <-trialDispFact $f> ...
expSetup OneActuator 1 -control 1 1 -sizeTrialOut 1 1

# Define experimental site
# ------------------------
# expSite LocalSite $tag $setupTag
expSite LocalSite 1 1

# Define experimental tangent stiffness
# -------------------------------------
# expTangentStiff Broyden $tag
expTangentStiff Broyden 1
# expTangentStiff BFGS $tag <-eps $value>
#expTangentStiff BFGS 1
# expTangentStiff Transpose $tag $numCols
#expTangentStiff Transpose 1 1

# Define numerical elements
# -------------------------
# element truss $eleTag $iNode $jNode $A $matTag
element truss 1 1 4 10.0 1
element truss 2 2 4  5.0 1
#element corotTruss 1 1 4 10.0 1
#element corotTruss 2 2 4  5.0 1

# Define experimental element
# ---------------------------
# expElement truss $eleTag $iNode $jNode -site $siteTag -initStif $Kij <-tangStif tangStifTag> <-iMod> <-rho $rho> 
expElement truss 3 3 4 -site 1 -initStif $kInit -tangStif 1
#expElement corotTruss 3 3 4 -site 1 -initStif $kInit -tangStif 1

# Define static loads
# -------------------
# create a Plain load pattern with a linear TimeSeries
pattern Plain 1 "Linear" {
    # load $nodeTag $xForce $yForce
    load 4 100 -50
}
# ------------------------------
# End of model generation
# ------------------------------


# ------------------------------
# Start of analysis generation
# ------------------------------
# create the system of equation
system BandSPD
# create the DOF numberer
numberer RCM
# create the constraint handler
constraints Plain
# create the convergence test
test EnergyIncr 1.0e-12 25 1
# create the integration scheme
integrator LoadControl 0.1
# create the solution algorithm
algorithm Newton
# create the analysis object 
analysis Static
# ------------------------------
# End of analysis generation
# ------------------------------


# ------------------------------
# Start of recorder generation
# ------------------------------
# create the recorder objects
recorder Node -file Node_Dsp.out -time -node 4 -dof 1 2 disp
recorder Element -file Elmt_tangStif.out -time -ele 3 tangStif stif
# --------------------------------
# End of recorder generation
# ---------------------------------


# ------------------------------
# Finally perform the analysis
# ------------------------------
record
# perform the static analysis
analyze 20

# print the current state at node 4 and at all elements
print node 4
print ele
wipeExp
wipe
exit
# --------------------------------
# End of analysis
# --------------------------------
