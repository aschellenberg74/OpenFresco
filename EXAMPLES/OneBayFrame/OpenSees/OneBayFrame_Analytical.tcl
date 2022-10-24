# File: OneBayFrame_Analytical.tcl
# Units: [kip,in.]
#
# Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
# Created: 11/06
# Revision: A
#
# Purpose: this file contains the tcl input to perform
# a purely numerical simulation of a one bay frame.


# ------------------------------
# Start of model generation
# ------------------------------
logFile "OneBayFrame_Analytical.log"
defaultUnits -force kip -length in -time sec -temp F

# create ModelBuilder (with two-dimensions and 2 DOF/node)
model BasicBuilder -ndm 2 -ndf 2

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
#uniaxialMaterial Elastic 1 2.8
uniaxialMaterial Steel02 1 1.5 2.8 0.01 18.5 0.925 0.15 0.0 1.0 0.0 1.0
uniaxialMaterial Elastic 2 5.6
#uniaxialMaterial Steel02 2 3.0 5.6 0.01 18.5 0.925 0.15 0.0 1.0 0.0 1.0 
uniaxialMaterial Elastic 3 [expr 2.0*100.0/1.0]

# Define elements
# ---------------
# left and right columns
# element twoNodeLink $eleTag $iNode $jNode -mat $matTags -dir $dirs <-orient <$x1 $x2 $x3> $y1 $y2 $y3> <-pDelta Mratios> <-mass $m>
element twoNodeLink 1 1 3 -mat 1 -dir 2
element twoNodeLink 2 2 4 -mat 2 -dir 2

# spring
# element truss $eleTag $iNode $jNode $A $matTag
element truss 3 3 4 1.0 3

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
#rayleigh $alphaM $betaK $betaKinit $betaKcomm
# ------------------------------
# End of model generation
# ------------------------------


# ------------------------------
# Start of analysis generation
# ------------------------------
# create the system of equations
system ProfileSPD
# create the DOF numberer
numberer Plain
# create the constraint handler
constraints Plain
# create the convergence test
test NormDispIncr 1.0e-12 10
# create the integration scheme
#integrator Newmark 0.5 0.25
integrator NewmarkExplicit 0.5
#integrator AlphaOS 1.0
# create the solution algorithm
#algorithm Newton
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
# --------------------------------
# End of recorder generation
# --------------------------------


# ------------------------------
# Finally perform the analysis
# ------------------------------
record
# perform an eigenvalue analysis
set lambda [eigen -fullGenLapack 2]
puts "\nEigenvalues at start of transient:"
puts "|  lambda   |  omega   |  period | frequency |"
foreach lambda $lambda {
    set omega [expr pow($lambda,0.5)]
    set period [expr 2.0*$pi/$omega]
    set frequ [expr 1.0/$period]
    puts [format "| %5.3e | %8.4f | %7.4f | %9.4f |" $lambda $omega $period $frequ]
}

#modalDamping 0.05 0.05

# open output file for writing
set outFileID [open elapsedTime.txt w]
# perform the transient analysis
set dtAna [expr 20.0/1024.0]
set tTot [time {
    for {set i 1} {$i < 1790} {incr i} {
        set t [time {analyze  1  $dtAna}]
        puts $outFileID $t
        #puts "step $i"
    }
}]
puts "\nElapsed Time = $tTot \n"
# close the output file
close $outFileID

wipe
exit
# --------------------------------
# End of analysis
# --------------------------------
