# File: Cantilever2D_Worker.tcl (use with Cantilever2D_Main.tcl)
# Units: [kip,in.]
#
# Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
# Created: 09/07
# Revision: A
#
# Purpose: this file contains the tcl input to perform
# a hybrid simulation of a simple cantilever column
# with one experimental beamColumn element.
# Either the actuator or adapter element is used to
# communicate with the main FE-software which is
# coordinating and executing the direct integration
# analysis.


# ------------------------------
# Start of model generation
# ------------------------------
logFile "Cantilever2D_Worker.log"
defaultUnits -force kip -length in -time sec -temp F

# create ModelBuilder (with two-dimensions and 2 DOF/node)
model BasicBuilder -ndm 2 -ndf 3

# Define geometry for model
# -------------------------
set mass2 0.04
# node $tag $xCrd $yCrd
node  1     0.0    0.0
node  2     0.0   54.0   -mass $mass2 $mass2 0.0
node  3   -10.0   54.0;  # for use with actuator element

# set the boundary conditions
# fix $tag $DX $DY $RZ
fix 1   1  1  1
fix 2   0  0  0
fix 3   1  1  1;  # for use with actuator element

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

# Define elements
# ---------------
# element forceBeamColumn $eleTag $iNode $jNode $numIntgrPts $secTag $transfTag
element forceBeamColumn 1 1 2 5 1 1

# element actuator $eleTag $iNode $jNode EA ipPort <-rho rho>
#element actuator 2 3 2 1E6 44000
#element corotActuator 2 3 2 1E6 44000

# element adapter eleTag -node Ndi Ndj ... -dof dofNdi -dof dofNdj ... -stif Kij ipPort <-mass Mij>
element adapter 2 -node 2 -dof 1 -stif 1E5 44000
# ------------------------------
# End of model generation
# ------------------------------


# ------------------------------
# Start of analysis generation
# ------------------------------
# create the system of equations
system BandGeneral
# create the DOF numberer
numberer Plain
# create the constraint handler
constraints Plain
# create the convergence test
test NormDispIncr 1.0e-8 25
#test NormUnbalance 1.0e-8 25
#test EnergyIncr 1.0e-8 25
# create the integration scheme
integrator LoadControl 1.0
#integrator Newmark 0.5 0.25
#integrator NewmarkExplicit 0.5
# create the solution algorithm
algorithm Newton
#algorithm Linear
# create the analysis object 
analysis Static
#analysis Transient
# ------------------------------
# End of analysis generation
# ------------------------------


# ------------------------------
# Start of recorder generation
# ------------------------------
# create the recorder objects
recorder Node -file Worker_Node_Dsp.out -time -node 2 -dof 1 2 3 disp
recorder Node -file Worker_Node_Vel.out -time -node 2 -dof 1 2 3 vel
recorder Node -file Worker_Node_Acc.out -time -node 2 -dof 1 2 3 accel

recorder Element -file Worker_Elmt_Frc.out     -time -ele 1 2 forces
recorder Element -file Worker_Elmt_ctrlDsp.out -time -ele   2 ctrlDisp
recorder Element -file Worker_Elmt_daqDsp.out  -time -ele   2 daqDisp
# --------------------------------
# End of recorder generation
# --------------------------------


# ------------------------------
# Finally perform the analysis
# ------------------------------
record
analyze 16000 0.02
exit
# --------------------------------
# End of analysis
# --------------------------------
