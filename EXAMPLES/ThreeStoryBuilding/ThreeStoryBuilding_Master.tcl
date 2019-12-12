# File: ThreeStoryBuilding_Master.tcl (use with ThreeStoryBuilding_Slave.tcl)
# Units: [kip,in.]
#
# Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
# Created: 09/07
# Revision: A
#
# Purpose: this file contains the tcl input to perform
# a coupled simulation of a three-story building.
# Since the simulation of the concrete shear wall is
# performed in an other FE-software, the SimFEAdapter
# experimental control is used to communicate with such
# software.


# ------------------------------
# Start of model generation
# ------------------------------
logFile "ThreeStoryBuilding_Master.log"
defaultUnits -force kip -length in -time sec -temp F

# create ModelBuilder (with two-dimensions and 2 DOF/node)
model BasicBuilder -ndm 2 -ndf 3

# Load OpenFresco package
# -----------------------
# (make sure all dlls are in the same folder as openSees.exe)
loadPackage OpenFresco

# Define geometry for model
# -------------------------
set numBay 4
set numStory 3
set bayWidth [expr 30.0*12.0]
set storyHeight [expr 13.5*12.0]
set w1 3160
set w2 3160
set w3 2600
set m1 [expr $w1/($numBay*2.0)/386.1]
set m2 [expr $w2/($numBay*2.0)/386.1]
set m3 [expr $w3/($numBay*2.0)/386.1]

# node $tag $xCrd $yCrd
node   1  [expr 0.0*$bayWidth]  [expr 0.0*$storyHeight]
node   2  [expr 0.0*$bayWidth]  [expr 1.0*$storyHeight]
node   3  [expr 0.0*$bayWidth]  [expr 2.0*$storyHeight]
node   4  [expr 0.0*$bayWidth]  [expr 3.0*$storyHeight]
node   5  [expr 1.0*$bayWidth]  [expr 0.0*$storyHeight]
node   6  [expr 1.0*$bayWidth]  [expr 1.0*$storyHeight]
node   7  [expr 1.0*$bayWidth]  [expr 2.0*$storyHeight]
node   8  [expr 1.0*$bayWidth]  [expr 3.0*$storyHeight]
node   9  [expr 2.0*$bayWidth]  [expr 0.0*$storyHeight]
node  10  [expr 2.0*$bayWidth]  [expr 1.0*$storyHeight]
node  11  [expr 2.0*$bayWidth]  [expr 2.0*$storyHeight]
node  12  [expr 2.0*$bayWidth]  [expr 3.0*$storyHeight]
node  13  [expr 3.0*$bayWidth]  [expr 0.0*$storyHeight]
node  14  [expr 3.0*$bayWidth]  [expr 1.0*$storyHeight]
node  15  [expr 3.0*$bayWidth]  [expr 2.0*$storyHeight]
node  16  [expr 3.0*$bayWidth]  [expr 3.0*$storyHeight]
node  17  [expr 4.0*$bayWidth]  [expr 0.0*$storyHeight]
node  18  [expr 4.0*$bayWidth]  [expr 1.0*$storyHeight]
node  19  [expr 4.0*$bayWidth]  [expr 2.0*$storyHeight]
node  20  [expr 4.0*$bayWidth]  [expr 3.0*$storyHeight]

# mass $tag $MX $MY $RZ
mass   2  [expr 1.0*$m1]  [expr 1.0*$m1]  0.0
mass   3  [expr 1.0*$m2]  [expr 1.0*$m2]  0.0
mass   4  [expr 1.0*$m3]  [expr 1.0*$m3]  0.0
mass   6  [expr 2.0*$m1]  [expr 2.0*$m1]  0.0
mass   7  [expr 2.0*$m2]  [expr 2.0*$m2]  0.0
mass   8  [expr 2.0*$m3]  [expr 2.0*$m3]  0.0
mass  10  [expr 2.0*$m1]  [expr 2.0*$m1]  0.0
mass  11  [expr 2.0*$m2]  [expr 2.0*$m2]  0.0
mass  12  [expr 2.0*$m3]  [expr 2.0*$m3]  0.0
mass  14  [expr 2.0*$m1]  [expr 2.0*$m1]  0.0
mass  15  [expr 2.0*$m2]  [expr 2.0*$m2]  0.0
mass  16  [expr 2.0*$m3]  [expr 2.0*$m3]  0.0
mass  18  [expr 3.0*$m1]  [expr 3.0*$m1]  0.0
mass  19  [expr 3.0*$m2]  [expr 3.0*$m2]  0.0
mass  20  [expr 3.0*$m3]  [expr 3.0*$m3]  0.0

# set the boundary conditions
# fix $tag $DX $DY $RZ
fix  1   1  1  1
fix  5   1  1  1
fix  9   1  1  1
fix 13   1  1  1
fix 17   1  1  1
fix 18   0  1  1
fix 19   0  1  1
fix 20   0  1  1

# set the mp-constraints
# Equal DOF: 1stFloor: mNodeTag sNodeTag dof 
equalDOF 2   6  1 
equalDOF 2  10  1 
equalDOF 2  14  1 
equalDOF 2  18  1 
# Equal DOF: 2ndFloor: mNodeTag sNodeTag dof 
equalDOF 3   7  1 
equalDOF 3  11  1 
equalDOF 3  15  1 
equalDOF 3  19  1 
# Equal DOF: 3rdFloor: mNodeTag sNodeTag dof 
equalDOF 4   8  1 
equalDOF 4  12  1 
equalDOF 4  16  1 
equalDOF 4  20  1 

# Define material
# ---------------
# uniaxialMaterial Elastic $tag $E
uniaxialMaterial Elastic  1  29000

# Define control points
# ---------------------
# expControlPoint $cpTag <-node $nodeTag> $dof $rspType <-fact $f> <-lim $l $u> <-isRel> ...
expControlPoint 1  1 disp 2 disp 3 disp
expControlPoint 2  1 disp 2 disp 3 disp  1 force 2 force 3 force 

# Define experimental control
# ---------------------------
# expControl SimFEAdapter $tag ipAddr $ipPort -trialCP $cpTags -outCP $cpTags
expControl SimFEAdapter 1 "127.0.0.1" 44000 -trialCP 1 -outCP 2

# Define experimental setup
# -------------------------
# expSetup NoTransformation $tag <–control $ctrlTag> –dof $DOFs … -sizeTrialOut t o <–ctrlDispFact $f> ...
expSetup NoTransformation 1 -control 1 -dof 1 2 3 -sizeTrialOut 3 3

# Define experimental site
# ------------------------
# expSite LocalSite $tag $setupTag
expSite LocalSite 1 1

# Define coordinate transformation
# --------------------------------
# geomTransf Linear $transfTag
geomTransf Linear 1

# Define numerical elements
# -------------------------
# exterior column W14x257
# element elasticBeamColumn $eleTag $iNode $jNode $A $E $Iz $transfTag
element elasticBeamColumn  1  1  2 75.6 29000 3400 1
element elasticBeamColumn  2  2  3 75.6 29000 3400 1
element elasticBeamColumn  3  3  4 75.6 29000 3400 1
# interior columns W14x311
# element elasticBeamColumn $eleTag $iNode $jNode $A $E $Iz $transfTag
element elasticBeamColumn  4  5  6 91.4 29000 4330 1
element elasticBeamColumn  5  6  7 91.4 29000 4330 1
element elasticBeamColumn  6  7  8 91.4 29000 4330 1
element elasticBeamColumn  7  9 10 91.4 29000 4330 1
element elasticBeamColumn  8 10 11 91.4 29000 4330 1
element elasticBeamColumn  9 11 12 91.4 29000 4330 1
# exterior column W14x257
# element elasticBeamColumn $eleTag $iNode $jNode $A $E $Iz $transfTag
element elasticBeamColumn 10 13 14 75.6 29000 3400 1
element elasticBeamColumn 11 14 15 75.6 29000 3400 1
element elasticBeamColumn 12 15 16 75.6 29000 3400 1
# 1st floor beams W33x118
# element elasticBeamColumn $eleTag $iNode $jNode $A $E $Iz $transfTag
element elasticBeamColumn 101  2  6 34.7 29000 5900 1
element elasticBeamColumn 102  6 10 34.7 29000 5900 1
element elasticBeamColumn 103 10 14 34.7 29000 5900 1
element truss             104 14 18 34.7 1
# 2nd floor beams W30x116
# element elasticBeamColumn $eleTag $iNode $jNode $A $E $Iz $transfTag
element elasticBeamColumn 105  3  7 34.2 29000 4930 1
element elasticBeamColumn 106  7 11 34.2 29000 4930 1
element elasticBeamColumn 107 11 15 34.2 29000 4930 1
element truss             108 15 19 34.2 1
# 3rd floor beams W24x68
# element elasticBeamColumn $eleTag $iNode $jNode $A $E $Iz $transfTag
element elasticBeamColumn 109  4  8 20.1 29000 1830 1
element elasticBeamColumn 110  8 12 20.1 29000 1830 1
element elasticBeamColumn 111 12 16 20.1 29000 1830 1
element truss             112 16 20 20.1 1

# Define experimental element
# ---------------------------
# expElement generic $eleTag -node $Ndi -dof $dofNdi -dof $dofNdj ... -server $ipPort <$ipAddr>  <-ssl> <-dataSize $size>
expElement generic 200 -node 18  19  20 -dof 1 -dof 1 -dof 1 -site 1 -initStif +9.320470E+004 -5.106310E+004 +1.019010E+004 -5.106310E+004 +8.238120E+004 -3.623530E+004 +1.019010E+004 -3.623530E+004 +2.330480E+004

# Define dynamic loads
# --------------------
# set time series to be passed to uniform excitation
set dt 0.01
set scale 1.0
timeSeries Path 1 -filePath SACNF01.txt -dt $dt -factor [expr 386.1*$scale]

# create UniformExcitation load pattern
# pattern UniformExcitation $tag $dir -accel $tsTag <-vel0 $vel0>
pattern UniformExcitation 1 1 -accel 1

# calculate the rayleigh damping factors for nodes & elements
set alphaM     2.0138;          # D = alphaM*M
set betaK      0.0;             # D = betaK*Kcurrent
set betaKinit  0.0;             # D = beatKinit*Kinit
set betaKcomm  0.0;             # D = betaKcomm*KlastCommit

# set the rayleigh damping 
rayleigh $alphaM $betaK $betaKinit $betaKcomm
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
constraints Transformation
# create the convergence test
test EnergyIncr 1.0e-12 25
# create the integration scheme
integrator Newmark 0.5 0.25
#integrator NewmarkExplicit 0.5
#integrator AlphaOS 1.0
# create the solution algorithm
algorithm Newton
#algorithm Linear
# create the analysis object 
analysis Transient
# ------------------------------
# End of analysis generation
# ------------------------------


# ------------------------------
# Start of recorder generation
# ------------------------------
# create the recorder objects
recorder Node -file Master_Node_Dsp.out -time -node 2 3 4 18 19 20 -dof 1 2 3 disp
recorder Node -file Master_Node_Vel.out -time -node 2 3 4 18 19 20 -dof 1 2 3 vel
recorder Node -file Master_Node_Acc.out -time -node 2 3 4 18 19 20 -dof 1 2 3 accel
# --------------------------------
# End of recorder generation
# --------------------------------


# ------------------------------
# Finally perform the analysis
# ------------------------------
# perform an eigenvalue analysis
puts "\nStarting Eigenvalue analysis ..."
set lambda [eigen -fullGenLapack 15]
puts "\nEigenvalues at start of transient:"
puts "|   lambda   |  omega   |  period | frequency |"
foreach lambda $lambda {
    set omega [expr pow($lambda,0.5)]
    set period [expr 2.0*$pi/$omega]
    set frequ [expr 1.0/$period]
    puts [format "| %5.3e | %8.4f | %7.4f | %9.4f |" $lambda $omega $period $frequ]
}

# open output file for writing
set outFileID [open elapsedTime.txt w]
# perform the transient analysis
puts "\nStarting transient analysis ..."
puts "step 0"
record
set tTot [time {
    for {set i 1} {$i < 2500} {incr i} {
        set t [time {analyze  1  $dt}]
        puts $outFileID $t
        if {[expr fmod($i,100)] == 0.0} {
            puts "step $i"
        }
    }
}]
puts "\nElapsed Time = $tTot \n"
# close the output file
close $outFileID
wipeExp
wipe
exit
# --------------------------------
# End of analysis
# --------------------------------
