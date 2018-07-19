# File: OneBayFrame_LeftCol.tcl (use with Simulink control system model)
# Units: [kip,in.]
#
# $Revision$
# $Date$
# $URL$
#
# Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
# Created: 02/10
# Revision: A
#
# Purpose: this file contains the tcl input to perform
# a hybrid simulation of a one bay frame with
# two experimental zero length elements.
# The adapter element is used to communicate with the
# Simulink model representing the control system, actuator
# and hydraulic power supply.


# ------------------------------
# Start of model generation
# ------------------------------
# create ModelBuilder (with two-dimensions and 2 DOF/node)
model BasicBuilder -ndm 2 -ndf 2

# Define geometry for model
# -------------------------
set mass3 0.00
# node $tag $xCrd $yCrd $mass
node  1     0.0   0.0
node  3     0.0  54.0  -mass $mass3 $mass3

# set the boundary conditions
# fix $tag $DX $DY
fix 1   1  1
fix 3   0  1

# Define materials
# ----------------
# uniaxialMaterial Steel02 $matTag $Fy $E $b $R0 $cR1 $cR2 $a1 $a2 $a3 $a4
uniaxialMaterial Steel02 1 1.5 2.8 0.01 18.5 0.925 0.15 0.0 1.0 0.0 1.0
#uniaxialMaterial Elastic 1 2.8

# Define elements
# ---------------
# element twoNodeLink $eleTag $iNode $jNode -mat $matTags -dir $dirs <-orient <$x1 $x2 $x3> $y1 $y2 $y3> <-pDelta $Mratios> <-mass $m>
element twoNodeLink 1 1 3 -mat 1 -dir 2 -orient -1 0 0

# element adapter eleTag -node Ndi Ndj ... -dof dofNdi -dof dofNdj ... -stif Kij ipPort <-mass Mij>
element adapter 2 -node 3 -dof 1 -stif 1E12 44000

# Define dynamic loads
# --------------------
# set time series to be passed to uniform excitation
#set dt 0.02
#set scale 1.0
#timeSeries Path 1 -filePath elcentro.txt -dt $dt -factor [expr 386.1*$scale]

# create UniformExcitation load pattern
# pattern UniformExcitation $tag $dir -accel $tsTag <-vel0 $vel0>
#pattern UniformExcitation 1 1 -accel 1

# calculate the Rayleigh damping factors for nodes & elements
#set alphaM     1.010017396536;  # D = alphaM*M
#set betaK      0.0;             # D = betaK*Kcurrent
#set betaKinit  0.0;             # D = beatKinit*Kinit
#set betaKcomm  0.0;             # D = betaKcomm*KlastCommit

# set the Rayleigh damping 
#rayleigh $alphaM $betaK $betaKinit $betaKcomm
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
recorder Node -file output/Slave_Node_Dsp.out -time -node 3 -dof 1 disp
recorder Node -file output/Slave_Node_Vel.out -time -node 3 -dof 1 vel
recorder Node -file output/Slave_Node_Acc.out -time -node 3 -dof 1 accel

recorder Element -file output/Slave_Elmt_Frc.out     -time -ele 1 2 forces
recorder Element -file output/Slave_Elmt_ctrlDsp.out -time -ele 2   ctrlDisp
recorder Element -file output/Slave_Elmt_daqDsp.out  -time -ele 2   daqDisp

# recorder display "Name" $xLoc $yLoc $xPixels $yPixels <-wipe> <-dT $deltaT> 
recorder  display  "Specimen"  5  5  200  600 -wipe -dT 20
# "normal" vector to the view window
vpn 0 0 1
# "up" vector of the view window
vup 0 1 0
# Projection Reference Point (direction vector to the eye)
prp 0 0 1
# dimension of the view window (-x, x, -y, y)
viewWindow -10 10 -30 35
# center of the view window (1/2 model widths)
vrp 0 27 0
# display $elemDispOpt $nodeDispOpt $magFactor
display 1 4 3.0
# --------------------------------
# End of recorder generation
# --------------------------------


# ------------------------------
# Finally perform the analysis
# ------------------------------
# controller time step size
set dtCon [expr 1.0/1024.0]
# perform the transient analysis
for {set i 1} {$i < 1E8} {incr i} {
    analyze 1 $dtCon
    #puts "step $i"
}

wipe
exit
# --------------------------------
# End of analysis
# --------------------------------
