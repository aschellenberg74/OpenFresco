# File: OneBayFrame_Distr_Client.tcl
# (use with OneBayFrame_Distr_SimAppServer.tcl & OneBayFrame_Distr_LabServer.tcl)
#
# $Revision: $
# $Date: $
# $URL: $
#
# Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
# Created: 11/06
# Revision: A
#
# Purpose: this file contains the tcl input to perform
# a local hybrid simulation of a one bay frame with
# two experimental zero length elements.
# The specimens are simulated using the SimUniaxialMaterials
# controller.


# ------------------------------
# Start of model generation
# ------------------------------
# create ModelBuilder (with two-dimensions and 2 DOF/node)
model BasicBuilder -ndm 2 -ndf 2

# Load OpenFresco package
# -----------------------
# (make sure all dlls are in the same folder as openSees.exe)
loadPackage OpenFresco

# Define geometry for model
# -------------------------
set mass3 0.04
set mass4 0.02
# node $tag $xCrd $yCrd $mass
node  1     0.0   0.00
node  2   100.0   0.00
node  3     0.0  54.00  -mass $mass3 $mass3
node  4   100.0  54.00  -mass $mass4 $mass4

# set the boundary conditions
# fix $tag $DX $DY
fix 1   1  1
fix 2   1  1
fix 3   0  1
fix 4   0  1

# Define materials
# ----------------
# uniaxialMaterial Steel02 $matTag $Fy $E $b $R0 $cR1 $cR2 $a1 $a2 $a3 $a4 
uniaxialMaterial Elastic 2 5.6
#uniaxialMaterial Steel02 2 3.0 5.6 0.01 18.5 0.925 0.15 0.0 1.0 0.0 1.0 
uniaxialMaterial Elastic 3 [expr 2.0*100.0/1.0]

# Define experimental control
# ---------------------------
# expControl SimUniaxialMaterials $tag $matTags
expControl SimUniaxialMaterials 2 2

# Define experimental setup
# -------------------------
# expSetup OneActuator $tag <-control $ctrlTag> $dir <-ctrlDispFact $f> ...
expSetup OneActuator 2 -control 2 1

# Define experimental site
# ------------------------
# expSite LocalSite $tag $setupTag
expSite LocalSite 2 2

# Define experimental elements
# ----------------------------
# left column
# expElement genericClient $eleTag -node $Ndi -dof $dofNdi -dof $dofNdj ... -server $ipPort <$ipAddr> <-dataSize $size>
expElement genericClient 1 -node 1 3 -dof 1 2 -dof 1 2 -server 8090
#expElement zeroLength 1 1 3 -dir 2 -server 8090 -initStif 2.8 -orient 0 1 0 -1 0 0

# Define numerical elements
# -------------------------
# spring
# element truss $eleTag $iNode $jNode $A $matTag
element truss 3 3 4 1.0 3

# right column
# expElement zeroLength $eleTag $iNode $jNode -dir $dirs -site $siteTag -initStif $Kij <-orient $x1 $x2 $x3 $y1 $y2 $y3> <-iMod> <-mass $m>
expElement zeroLength 2 2 4 -dir 2 -site 2 -initStif 5.6 -orient 0 1 0 -1 0 0

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
test EnergyIncr 1.0e-6 10

# create the integration scheme
integrator NewmarkExplicit 0.5
#integrator AlphaOS 1.0

# create the solution algorithm
algorithm Linear

# create the analysis object 
analysis Transient
# ------------------------------
# End of analysis generation
# ------------------------------


# ------------------------------
# Start of recorder generation
# ------------------------------
# create the recorder objects
recorder Node -file Node_Dsp.out -time -node 3 4 -dof 1 disp
recorder Node -file Node_Vel.out -time -node 3 4 -dof 1 vel
recorder Node -file Node_Acc.out -time -node 3 4 -dof 1 accel

recorder Element -file Elmt_Frc.out  -time -ele 1 2 3 forces
recorder Element -file Elmt_tDef.out -time -ele 1 2   basicDeformations
recorder Element -file Elmt_mDef.out -time -ele 1 2   measuredDisplacements
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
        }
    }]
puts "Elapsed Time = $tTot \n"
# close the output file
close $outFileID

wipe
# --------------------------------
# End of analysis
# --------------------------------
