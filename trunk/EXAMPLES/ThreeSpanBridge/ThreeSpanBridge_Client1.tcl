# File: TreeSpanBridge_Client1.tcl
# (use with TreeSpanBridge_Server1a.tcl & TreeSpanBridge_Server1b.tcl)
#
# $Revision$
# $Date$
# $URL$
#
# Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
# Created: 09/06
# Revision: A
#
# Purpose: this file contains the tcl input to perform
# a distributed hybrid simulation of a three span bridge
# with two experimental beam column elements.
# The specimens are simulated using the SimUniaxialMaterials
# controller.
# The experimental setups are on the server sides.


# ------------------------------
# Start of model generation
# ------------------------------
# create ModelBuilder (with two-dimensions and 3 DOF/node)
model BasicBuilder -ndm 2 -ndf 3

# Load OpenFresco package
# -----------------------
# (make sure all dlls are in the same folder as openSees.exe)
loadPackage OpenFresco

# Define geometry for model
# -------------------------
set mass 0.12
# node $tag $xCrd $yCrd $mass
node  1     0.0   0.00
node  2   108.0 -54.00
node  3   216.0 -42.00
node  4   324.0   0.00
node  5     0.0   0.00  -mass $mass $mass 116.6
node  6   108.0   0.00  -mass $mass $mass 116.6
node  7   216.0   0.00  -mass $mass $mass 116.6
node  8   324.0   0.00  -mass $mass $mass 116.6
node  9   108.0   0.00  -mass $mass $mass 116.6
node 10   216.0   0.00  -mass $mass $mass 116.6

# set the boundary conditions
# fix $tag $DX $DY $RZ
fix 1   1  1  1
fix 2   1  1  1
fix 3   1  1  1
fix 4   1  1  1

# Define materials
# ----------------
# uniaxialMaterial Steel02 $matTag $Fy $E $b $R0 $cR1 $cR2 $a1 $a2 $a3 $a4 
uniaxialMaterial Elastic 1 1E8
#uniaxialMaterial Elastic 2 12.0
uniaxialMaterial Steel02 2 6.0 12.0 0.05 18.5 0.925 0.15 0.0 1.0 0.0 1.0

# Define experimental site
# ------------------------
# expSite ShadowSite $tag <-setup $setupTag> $ipAddr $ipPort <-ssl> <-udp> <-dataSize $size>
expSite ShadowSite 1 "127.0.0.1" 8090
expSite ShadowSite 2 "127.0.0.1" 8091

# Define numerical elements
# -------------------------
# geometric transformation
# geomTransf type $tag 
geomTransf Linear 1
geomTransf Corotational 2

# abutment isolators
# element zeroLength $eleTag $iNode $jNode -mat $matTag -dir $dirs -orient $x1 $x2 $x3 $yp1 $yp2 $yp3
element zeroLength 1 1 5 -mat 1 2 -dir 1 2 -orient 0 1 0 -1 0 0
element zeroLength 4 4 8 -mat 1 2 -dir 1 2 -orient 0 1 0 -1 0 0

# hinges at top of columns
# element zeroLength $eleTag $iNode $jNode -mat $matTag -dir $dirs -orient $x1 $x2 $x3 $yp1 $yp2 $yp3
element zeroLength 8 6  9 -mat 1 1 -dir 1 2 -orient 0 1 0 -1 0 0
element zeroLength 9 7 10 -mat 1 1 -dir 1 2 -orient 0 1 0 -1 0 0

# bridge deck
# element elasticBeamColumn $eleTag $iNode $jNode $A $E $Iz $transTag
element elasticBeamColumn 5 5 6 3.55 29000 22.1 1
element elasticBeamColumn 6 6 7 3.55 29000 22.1 1
element elasticBeamColumn 7 7 8 3.55 29000 22.1 1

# Define experimental elements
# ----------------------------
# expElement beamColumn $eleTag $iNode $jNode $transTag -site $siteTag -initStif $Kij <-iMod> <-rho $rho>
expElement beamColumn 2 2  9 1 -site 1 -initStif 1213 0 0 0 11.2 -302.4 0 -302.4 10886.4
expElement beamColumn 3 3 10 1 -site 2 -initStif 1560 0 0 0 24.0 -504.0 0 -504.0 14112.0

# Define dynamic loads
# --------------------
# set time series to be passed to uniform excitation
set dt 0.01
#set scale 1.0
#timeSeries Path 1 -filePath CosinePulse.txt -dt $dt -factor [expr 386.1*$scale]
#set scale 0.6
#timeSeries Path 1 -filePath SACNF01.txt -dt $dt -factor [expr 386.1*$scale]
set scale 1.0
timeSeries Path 1 -filePath ELC270.txt -dt $dt -factor [expr 386.1*$scale]

# create UniformExcitation load pattern
# pattern UniformExcitation $tag $dir -accel $tsTag <-vel0 $vel0>
pattern UniformExcitation 1 1 -accel 1

# calculate the rayleigh damping factors for nodes & elements
set alphaM     0.0;       # D = alphaM*M
set betaK      0.0;       # D = betaK*Kcurrent
set betaKinit  0.014836;  # D = beatKinit*Kinit
set betaKcomm  0.0;       # D = betaKcomm*KlastCommit

# set the rayleigh damping 
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
test NormDispIncr 1.0e-6 10
#test FixedNumIter 2

# create the integration scheme
#integrator NewmarkHybridSimulation 0.5 0.25
#integrator HHTHybridSimulation 0.667
integrator AlphaOS 0.6667

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
recorder Node -file Node_Dsp.out -time -node 6 7 -dof 1 disp
recorder Node -file Node_Vel.out -time -node 6 7 -dof 1 vel
recorder Node -file Node_Acc.out -time -node 6 7 -dof 1 accel

recorder Element -file Elmt_Frc.out -time -ele 2 3 basicForces
recorder Element -file Elmt_Def.out -time -ele 2 3 basicDeformations
# --------------------------------
# End of recorder generation
# --------------------------------


# ------------------------------
# Finally perform the analysis
# ------------------------------
# perform an eigenvalue analysis
set pi [expr acos(-1.0)]
set lambda [eigen -fullGenLapack 12]
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
set tTot [time {
    for {set i 1} {$i < 4000} {incr i} {
        set t [time {analyze  1  $dt}]
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
