# File: LShapedColumn_Client1.tcl
# (use with LShapedColumn_Server1a.tcl or LShapedColumn_Server1b.tcl)
# Units: [kip,in.]
#
# Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
# Created: 07/07
# Revision: A
#
# Purpose: this file contains the tcl input to perform
# a distributed hybrid simulation of a L-shaped column
# with one experimental beamColumn element.
# Since the shear and moment DOF are coupled, the
# specimen is simulated using the SimDomain controller.
# The experimental setup is an the server side.


# ------------------------------
# Start of model generation
# ------------------------------
logFile "LShapedColumn_Client1.log"
defaultUnits -force kip -length in -time sec -temp F

# create ModelBuilder (with two-dimensions and 2 DOF/node)
model BasicBuilder -ndm 2 -ndf 3

# Load OpenFresco package
# -----------------------
# (make sure all dlls are in the same folder as openSees.exe)
loadPackage OpenFresco

# Define geometry for model
# -------------------------
set mass2 0.04
set mass3 0.02
# node $tag $xCrd $yCrd $mass
node  1     0.0    0.0
node  2     0.0   54.0  -mass $mass2 $mass2 0.4
node  3    36.0   54.0  -mass $mass3 $mass3 0.1

# set the boundary conditions
# fix $tag $DX $DY $RZ
fix 1   1  1  1

# Define experimental site
# ------------------------
# expSite ShadowSite $tag <-setup $setupTag> $ipAddr $ipPort <-ssl> <-udp> <-dataSize $size>
expSite ShadowSite 1 "127.0.0.1" 8090

# Define coordinate transformation
# --------------------------------
# geomTransf Linear $transfTag
geomTransf Linear 1

# Define experimental element
# ---------------------------
# expElement beamColumn $eleTag $iNode $jNode $transTag –site $siteTag –initStif $Kij … <–iMod> <–rho $rho>
expElement beamColumn 1 1 2 1 -site 1 -initStif 1213 0 0 0 11.2 -302.4 0 -302.4 10886.4

# Define numerical element
# ------------------------
# element elasticBeamColumn $eleTag $iNode $jNode $A $E $Iz $transfTag
element elasticBeamColumn 2 2 3 2.26 29000 5.067806896551724 1

# Define dynamic loads
# --------------------
# set time series to be passed to uniform excitation
set dt 0.02
set scale 1.0
timeSeries Path 1 -filePath elcentro.txt -dt $dt -factor [expr 386.1*$scale]

# create UniformExcitation load pattern
# pattern UniformExcitation $tag $dir -accel $tsTag <-vel0 $vel0>
pattern UniformExcitation 1 1 -accel 1

# calculate the rayleigh damping factors for nodes & elements
set alphaM     1.010017396536;  # D = alphaM*M
set betaK      0.0;             # D = betaK*Kcurrent
set betaKinit  0.0;             # D = beatKinit*Kinit
set betaKcomm  0.0;             # D = betaKcomm*KlastCommit

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
test EnergyIncr 1.0e-8 25
# create the integration scheme
#integrator Newmark 0.5 0.25
#integrator NewmarkExplicit 0.5
integrator AlphaOS 1.0
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
recorder Node -file Node_Dsp.out -time -node 2 3 -dof 1 2 3 disp
recorder Node -file Node_Vel.out -time -node 2 3 -dof 1 2 3 vel
recorder Node -file Node_Acc.out -time -node 2 3 -dof 1 2 3 accel

recorder Element -file Elmt_Frc.out     -time -ele 1 2 forces
recorder Element -file Elmt_ctrlDsp.out -time -ele 1   ctrlDisp
recorder Element -file Elmt_daqDsp.out  -time -ele 1   daqDisp
# --------------------------------
# End of recorder generation
# --------------------------------


# ------------------------------
# Finally perform the analysis
# ------------------------------
record
# perform an eigenvalue analysis
set lambda [eigen -fullGenLapack 6]
puts "\nEigenvalues at start of transient:"
puts "|   lambda  |   omega  |  period | frequency |"
puts "----------------------------------------------"
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
    for {set i 1} {$i < 16000} {incr i} {
        set t [time {analyze  1  [expr $dt/10.0]}]
        puts $outFileID $t
        #puts "step $i"
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
