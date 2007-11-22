# File: Cantilever_Main.tcl
# (use with Cantilever_Adapter.tcl)
#
# $Revision: $
# $Date: $
# $URL: $
#
# Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
# Created: 09/07
# Revision: A
#
# Purpose: this file contains the tcl input to perform
# a hybrid simulation of a simple cantilever column
# with one experimental beamColumn element.
# Since the simulation of the column is performed in an
# other FE-software, the SimFEAdapter experimental control
# is used to communicate with such software.


# ------------------------------
# Start of model generation
# ------------------------------
# create ModelBuilder (with two-dimensions and 2 DOF/node)
model BasicBuilder -ndm 2 -ndf 3

# Load OpenFresco package
# -----------------------
# (make sure all dlls are in the same folder as openSees.exe)
loadPackage OpenFresco

# Define geometry for model
# -------------------------
set mass2 0.04
# node $tag $xCrd $yCrd $mass
node  1     0.0    0.0
node  2     0.0   54.0  -mass $mass2 $mass2 0.0

# set the boundary conditions
# fix $tag $DX $DY $RZ
fix 1   1  1  1

# Define experimental control
# ---------------------------
# expControl SimFEAdapter $tag ipAddr $ipPort
expControl SimFEAdapter 1 "127.0.0.1" 44000

# Define experimental setup
# -------------------------
# expSetup OneActuator $tag <-control $ctrlTag> $dir <-ctrlDispFact $f> ...
expSetup OneActuator 1 -control 1 2 -ctrlDispFact -1.0 -daqDispFact -1.0 -daqForceFact -1.0

# Define experimental site
# ------------------------
# expSite LocalSite $tag $setupTag
expSite LocalSite 1 1

# Define coordinate transformation
# --------------------------------
# geomTransf Linear $transfTag
geomTransf Linear 1

# Define experimental element
# ---------------------------
# expElement beamColumn $eleTag $iNode $jNode $transTag –site $siteTag –initStif $Kij … <–iMod> <–rho $rho>
expElement beamColumn 1 1 2 1 -site 1 -initStif 1213 0 0 0 11.2 -302.4 0 -302.4 10886.4

# Define dynamic loads
# --------------------
# set time series to be passed to uniform excitation
set dt 0.02
set scale 1.0
set accelSeries "Path -filePath elcentro.txt -dt $dt -factor [expr 386.1*$scale]"

# create UniformExcitation load pattern
# pattern UniformExcitation $tag $dir 
pattern UniformExcitation  1 1 -accel $accelSeries

# calculate the rayleigh damping factors for nodes & elements
set alphaM     1.010017396536;  # D = alphaM*M
set betaK      0.0;             # D = betaK*Kcurrent
set betaKinit  0.0;             # D = beatKinit*Kinit
set betaKcomm  0.0;             # D = betaKcomm*KlastCommit

# set the rayleigh damping 
#rayleigh $alphaM $betaK $betaKinit $betaKcomm;
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
test EnergyIncr 1.0e-12 10

# create the integration scheme
integrator Newmark 0.5 0.25
#integrator NewmarkExplicit 0.5
#integrator AlphaOS 1.0

# create the solution algorithm
algorithm Newton

# create the analysis object 
analysis Transient
# ------------------------------
# End of analysis generation
# ------------------------------


# ------------------------------
# Start of recorder generation
# ------------------------------
# create the recorder objects
recorder Node -file Node_Dsp.out -time -node 2 -dof 1 2 3 disp
recorder Node -file Node_Vel.out -time -node 2 -dof 1 2 3 vel
recorder Node -file Node_Acc.out -time -node 2 -dof 1 2 3 accel

recorder Element -file Elmt_Frc.out  -time -ele 1 forces
recorder Element -file Elmt_tDef.out -time -ele 1 targetDisplacements
recorder Element -file Elmt_mDef.out -time -ele 1 measuredDisplacements

#recorder plot Node_Dsp.out Left_Column_Displacements 100 20 700 500 -columns 1 2

#recorder display OneBayFrame_DefoShape 200 520 500 500 -wipe
#vup 0 1 0        
#prp 0 0 10000    
#display 1 0 5
# --------------------------------
# End of recorder generation
# --------------------------------


# ------------------------------
# Finally perform the analysis
# ------------------------------
# perform an eigenvalue analysis
set pi 3.14159265358979
set lambda [eigen 1]
puts "\nEigenvalues at start of transient:"
puts "lambda         omega          period"
foreach lambda $lambda {
   set omega [expr pow($lambda,0.5)]
   set period [expr 2*$pi/pow($lambda,0.5)]
   puts "$lambda  $omega  $period \n"}

# open output file for writing
set outFileID [open elapsedTime.txt w]
# perform the transient analysis
set tTot [time {
    for {set i 1} {$i < 1600} {incr i} {
        set t [time {analyze  1  $dt}]
        puts $outFileID $t
#	    puts "step $i"
    }
}]
puts "Elapsed Time = $tTot \n"
# close the output file
close $outFileID

wipe
# --------------------------------
# End of analysis
# --------------------------------
