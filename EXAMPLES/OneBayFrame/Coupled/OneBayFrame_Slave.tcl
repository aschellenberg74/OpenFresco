# File: OneBayFrame_Slave.tcl (use with OneBayFrame_Master.tcl)
# Units: [kip,in.]
#
# $Revision$
# $Date$
# $URL$
#
# Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
# Created: 08/08
# Revision: A
#
# Purpose: this file contains the tcl input to perform
# a hybrid simulation of a one bay frame with
# two experimental zero length elements.
# The adapter element is used to communicate with the
# main FE-software which is coordinating and executing
# the direct integration analysis.


# ------------------------------
# Start of model generation
# ------------------------------
# create ModelBuilder (with two-dimensions and 2 DOF/node)
model BasicBuilder -ndm 2 -ndf 2

# Define geometry for model
# -------------------------
set mass3 0.00
set mass4 0.00
# node $tag $xCrd $yCrd $mass
node  2   100.0   0.00
#node  3     0.0  50.00  -mass $mass3 $mass3
node  4   100.0  50.00  -mass $mass4 $mass4

# set the boundary conditions
# fix $tag $DX $DY
fix 2   1  1
#fix 3   0  1
fix 4   0  1

# Define materials
# ----------------
# uniaxialMaterial Steel02 $matTag $Fy $E $b $R0 $cR1 $cR2 $a1 $a2 $a3 $a4 
#uniaxialMaterial Elastic 1 2.8
uniaxialMaterial Steel02 1 1.5 2.8 0.01 18.5 0.925 0.15 0.0 1.0 0.0 1.0
#uniaxialMaterial Elastic 2 5.6
uniaxialMaterial Steel02 2 3.0 5.6 0.01 18.5 0.925 0.15 0.0 1.0 0.0 1.0 
uniaxialMaterial Elastic 3 [expr 2.0*100.0/1.0]

# Define elements
# ---------------
# element adapter eleTag -node Ndi Ndj ... -dof dofNdi -dof dofNdj ... -stif Kij ipPort <-mass Mij>
element adapter 1 -node 4 -dof 1 -stif 1E12 44000

# element twoNodeLink $eleTag $iNode $jNode -mat $matTags -dir $dirs <-orient <$x1 $x2 $x3> $y1 $y2 $y3> <-pDelta $Mratios> <-mass $m>
element twoNodeLink 2 2 4 -mat 2 -dir 2

# element truss $eleTag $iNode $jNode $A $matTag
#element truss 3 3 4 1.0 3

# Define dynamic loads
# --------------------
# set time series to be passed to uniform excitation
set dt 0.02
set scale 1.0
timeSeries Path 1 -filePath elcentro.txt -dt $dt -factor [expr 386.1*$scale]

# create UniformExcitation load pattern
# pattern UniformExcitation $tag $dir -accel $tsTag <-vel0 $vel0>
pattern UniformExcitation 1 1 -accel 1

# calculate the Rayleigh damping factors for nodes & elements
set alphaM     1.010017396536;  # D = alphaM*M
set betaK      0.0;             # D = betaK*Kcurrent
set betaKinit  0.0;             # D = beatKinit*Kinit
set betaKcomm  0.0;             # D = betaKcomm*KlastCommit

# set the Rayleigh damping 
rayleigh $alphaM $betaK $betaKinit $betaKcomm;
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
test NormDispIncr 1.0e-12 25
#test NormUnbalance 1.0e-12 25
#test EnergyIncr 1.0e-12 25
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
recorder Node -file Slave_Node_Dsp.out -time -node 4 -dof 1 disp
recorder Node -file Slave_Node_Vel.out -time -node 4 -dof 1 vel
recorder Node -file Slave_Node_Acc.out -time -node 4 -dof 1 accel

recorder Element -file Slave_Elmt_Frc.out     -time -ele 1 2 forces
recorder Element -file Slave_Elmt_ctrlDsp.out -time -ele 1   ctrlDisp
recorder Element -file Slave_Elmt_daqDsp.out  -time -ele 1   daqDisp
# --------------------------------
# End of recorder generation
# --------------------------------


# ------------------------------
# Finally perform the analysis
# ------------------------------
analyze 16000 0.02
exit
# --------------------------------
# End of analysis
# --------------------------------
