# File: OneStoryFrame2d_Local.tcl
# Units: [kip,in.]
#
# $Revision: $
# $Date: $
# $URL: $
#
# Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
# Created: 03/14
# Revision: A
#
# Purpose: this file tests the 2D experimental bearing element.
# It models an isolated one story one bay building and the bearing
# element has finite length.
# The specimen is simulated using the SimUniaxialMaterials
# controller.

# ------------------------------
# Start of model generation
# ------------------------------
# Create ModelBuilder (with two-dimensions and 3 DOF/node)
model BasicBuilder -ndm 2 -ndf 3

# Load OpenFresco package
# -----------------------
# (make sure all dlls are in the same folder as openSees.exe)
loadPackage OpenFresco

# Define geometry for model
# -------------------------
set g [expr 32.174*12]
set P 9.0
set mass [expr $P/$g]
#    tag   xCrd   yCrd        mass
node  1     0.0    0.0
node  2     0.0   10.0  -mass $mass $mass 0.0
node  3     0.0  154.0  -mass $mass $mass 0.0
node  4   144.0    0.0
node  5   144.0   10.0  -mass $mass $mass 0.0
node  6   144.0  154.0  -mass $mass $mass 0.0

# set the boundary conditions
#   tag DX DY RZ
fix  1   1  1  1
fix  4   1  1  1

# Define material models
# ----------------------
set mv [expr 2.0*$mass] 
set kv 1000.0
set zetaVertical 0.10
set cv [expr 2.0*$zetaVertical*sqrt($kv*$mv)]
uniaxialMaterial Elastic 1 [expr $kv/100.0] $cv $kv
uniaxialMaterial Elastic 2 169.0
uniaxialMaterial Steel01 3 6.48 6.51 0.08
#uniaxialMaterial Steel02 3 6.48 6.51 0.08 18.5 0.925 0.15 0.0 1.0 0.0 1.0

# Define experimental control
# ---------------------------
# expControl SimUniaxialMaterials $tag $matTags
expControl SimUniaxialMaterials 1 3;      # shear deformation only
#expControl SimUniaxialMaterials 1 3 1;    # shear and axial deformation
#expControl SimUniaxialMaterials 1 3 1 2;  # shear, axial and moment deformation

# Define experimental setup
# -------------------------
# expSetup NoTransformation $tag <–control $ctrlTag> –dir $dirs -sizeTrialOut $t $o <–trialDispFact $f> ...
expSetup NoTransformation 1 -control 1 -dir 2 -sizeTrialOut 3 3;      # shear deformation only
#expSetup NoTransformation 1 -control 1 -dir 2 1 -sizeTrialOut 3 3;    # shear and axial deformation
#expSetup NoTransformation 1 -control 1 -dir 2 1 3 -sizeTrialOut 3 3;  # shear, axial and moment deformation

# Define experimental site
# ------------------------
# expSite LocalSite $tag $setupTag
expSite LocalSite 1 1

# Define geometric transformations
# --------------------------------
# geomTransf type tag 
geomTransf Linear 1 

# Define experimental elements
# ----------------------------
# left isolator
# expElement bearing $eleTag $iNode $jNode $pFrcCtrl -P $matTag -Mz $matTag -site $siteTag -initStif $Kij <-orient $x1 $x2 $x3 $y1 $y2 $y3> <-pDelta $Mratios> <-shearDist $sDratio> <-iMod> <-doRayleigh> <-mass $m>
expElement bearing 1 1 2 0 -P 1 -Mz 2 -site 1 -initStif 6.51 -pDelta 0.5 0.5

# Define numerical elements
# -------------------------
# right isolator
# element elastomericBearingPlasticity eleTag NodeI NodeJ kInit fy alpha1 alpha2 mu -P matTag -Mz matTag <-orient x1 x2 x3 y1 y2 y3> <-shearDist sDratio> <-doRayleigh> <-mass m>
element elastomericBearingPlasticity 2 4 5 6.51 6.48 0.08 0.0 3.0 -P 1 -Mz 2

# element elasticBeamColumn eleTag NodeI NodeJ A E Iz geoTranTag <alpha d> <-mass massDens> 
element elasticBeamColumn 3 2 3 20.0 29000.0 400.0 1 
element elasticBeamColumn 4 5 6 20.0 29000.0 400.0 1 
element elasticBeamColumn 5 2 5 20.0 29000.0 400.0 1 
element elasticBeamColumn 6 3 6 20.0 29000.0 400.0 1 

# Define gravity loads
# --------------------
# create a Plain load pattern with a Linear TimeSeries
pattern Plain 1 "Linear" {
    # create nodal loads
    #    nd    FX          FY   MZ 
    load  2   0.0  [expr -$P]  0.0
    load  3   0.0  [expr -$P]  0.0
    load  5   0.0  [expr -$P]  0.0
    load  6   0.0  [expr -$P]  0.0
}
# ------------------------------
# End of model generation
# ------------------------------



# ------------------------------
# Start of analysis generation
# ------------------------------
# create the system of equation
system BandGeneral
# create the DOF numberer
numberer Plain
# create the constraint handler
constraints Plain
# create the convergence test
test NormDispIncr 1.0e-12 10
# create the integration scheme
integrator LoadControl 0.005
# create the solution algorithm
algorithm Linear
# create the analysis object
analysis Static
# ------------------------------
# End of analysis generation
# ------------------------------



# ------------------------------
# Start of recorder generation
# ------------------------------
# create a Recorder object for the nodal displacements at node 2
recorder Node -file Gravity_Dsp.out -time -node 2 3 5 6 -dof 1 2 3 disp
recorder Element -file Gravity_Frc.out -time -ele 1 2 force
# --------------------------------
# End of recorder generation
# --------------------------------



# ------------------------------
# Perform the gravity analysis
# ------------------------------
logFile "OneStoryFrame2d_Local.log"
# perform the gravity load analysis, requires 200 steps to reach the load level
analyze 200
puts "\nGravity load analysis completed";

# set the gravity loads to be constant & reset the time in the domain
loadConst -time 0.0
remove recorders



# --------------------------------
# Perform an eigenvalue analysis
# --------------------------------
set pi [expr acos(-1.0)]
set lambda [eigen -fullGenLapack 8]
set omega1 [expr pow([lindex $lambda 0],0.5)]
puts "\nEigenvalues at start of transient:"
puts "|   lambda   |  omega   |  period | frequency |"
foreach lambda $lambda {
    set omega [expr pow($lambda,0.5)]
    set period [expr 2.0*$pi/$omega]
    set frequ [expr 1.0/$period]
    puts [format "| %5.3e | %8.4f | %7.4f | %9.4f |" $lambda $omega $period $frequ]
}



# ------------------------------
# Start of model generation
# ------------------------------

# Define dynamic loads
# --------------------
# set time series to be passed to uniform excitation
set dt 0.005
set scale 1.0
set npts 9000
timeSeries Path 2 -filePath SCS052.AT2 -dt $dt -factor [expr $g*$scale]
timeSeries Path 3 -filePath SCSUP.AT2  -dt $dt -factor [expr $g*$scale]

# create UniformExcitation load pattern
#                         tag dir -accel tsTag 
pattern UniformExcitation  2   1  -accel   2
pattern UniformExcitation  3   2  -accel   3

# calculate the Rayleigh damping factors for nodes & elements
set zeta 0.01
set beta [expr 2.0*$zeta/$omega1];
set alphaM     0.0;     # mass proportional damping;       D = alphaM*M
set betaK      0.0;     # stiffness proportional damping;  D = betaK*Kcurrent
set betaKinit  0.0;     # stiffness proportional damping;  D = beatKinit*Kinit
set betaKcomm  $beta;   # stiffness proportional damping;  D = betaKcomm*KlastCommit

# set the Rayleigh damping 
rayleigh $alphaM $betaK $betaKinit $betaKcomm
# ------------------------------
# End of model generation
# ------------------------------



# ------------------------------
# Start of recorder generation
# ------------------------------
# create a Recorder object for the nodal displacements at node 2
recorder Node -file Node_Dsp.out -time -node 2 3 5 6 -dof 1 2 3 disp
recorder Node -file Node_Vel.out -time -node 2 3 5 6 -dof 1 2 3 vel
recorder Node -file Node_Acc.out -time -node 2 3 5 6 -dof 1 2 3 accel

recorder Element -file Elmt_Frc.out -time -ele 1 2 force
recorder Element -file Elmt_Def.out -time -ele 1 2 basicDeformation

# recorder display "Display" xLoc yLoc xPixels yPixels -wipe 
recorder  display  "Display"  5  5  630  630 -wipe
# "normal" vector to the view window
vpn +0.000000E+000  +0.000000E+000  +1.000000E+000
# "up" vector of the view window
vup +0.000000E+000  +1.000000E+000  +0.000000E+000
# Projection Reference Point (direction vector to the eye)
prp +0.000000E+000  +0.000000E+000  +1.000000E+000
# dimention of the view window
viewWindow -1.160000E+002  +1.160000E+002  -1.160000E+002  +1.160000E+002
# center of the view window
vrp +7.200000E+001  +7.700000E+001  +0.000000E+000
# display    elemDispOpt    nodeDispOpt    magFactor
display 1  3  +2.000000E+000
# --------------------------------
# End of recorder generation
# --------------------------------



# ------------------------------
# Start of analysis generation
# ------------------------------
# create the system of equation
system BandGeneral
# create the DOF numberer
numberer Plain
# create the constraint handler
constraints Plain
# set the test parameters
set testType NormDispIncr
set testTol 1.0e-12;
set testIter 25;
test $testType $testTol $testIter 
# set the integrator parameters
#integrator Newmark 0.5 0.25
integrator AlphaOS 1.0
# set the algorithm parameters
#algorithm Newton
algorithm Linear
# create the analysis object 
analysis Transient
# ------------------------------
# End of analysis generation
# ------------------------------



# ------------------------------
# Finally perform the analysis
# ------------------------------
set dtAna [expr $dt/1.0]
set dtMin 1.0e-8
set dtMax $dtAna

set ok 0;
set tFinal [expr $npts * $dt]
set tCurrent [getTime "%1.12E"]

record
while {$ok == 0 && $tCurrent < $tFinal} {
    
    set ok [analyze 1 $dtAna]
    
    if {$ok != 0} {
        if {[expr $dtAna/2.0] >= $dtMin} {
            set dtAna [expr $dtAna/2.0]
            puts [format "\nREDUCING time step size (dtNew = %1.6e)" $dtAna]
            set ok 0
        }
    } else {
        set tCurrent [getTime "%1.12E"]
        #puts [format "t = %1.4f sec" $tCurrent]
        if {[expr $dtAna*2.0] <= $dtMax} {
            set dtAna [expr $dtAna*2.0]
            puts [format "\nINCREASING time step size (dtNew = %1.6e)" $dtAna]
        }
    }
}

if {$ok != 0} {
    puts [format "\nModel failed (time = %1.3e)" $tCurrent]
} else {
    puts [format "\nResponse-history analysis completed"]
}

wipe
exit
# --------------------------------
# End of analysis
# --------------------------------
