# File: OneBayFrame_Client1.tcl
# (use with OneBayFrame_Server1a.tcl & OneBayFrame_Server1b.tcl)
#
# $Revision$
# $Date$
# $URL$
#
# Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
# Created: 07/07
# Revision: A
#
# Purpose: this file contains the tcl input to perform
# a distributed hybrid simulation of a one bay frame with
# two experimental twoNodeLink elements.
# The client is executed on the NEESit DataStar super-computer.

# ------------------------------
# Start of model generation
# ------------------------------
# create ModelBuilder (with two-dimensions and 2 DOF/node)
model BasicBuilder -ndm 2 -ndf 2

# Load OpenFresco package
# -----------------------
# (make sure all dlls are in the same folder as OpenSeesMP.exe)
loadPackage OpenFresco

set np  [getNP]
set pid [getPID]

if {$pid == 0}  {
    
    set mass3 0.04
    set mass4 0.02
    # node $tag $xCrd $yCrd $mass
    node  1     0.0   0.00
    node  3     0.0  54.00  -mass $mass3 $mass3
    node  4   100.0  54.00  -mass $mass4 $mass4
    
    # set the boundary conditions
    # fix $tag $DX $DY
    fix 1   1  1
    fix 3   0  1
    fix 4   0  1
    
    # Define materials
    # ----------------
    uniaxialMaterial Elastic 3 [expr 2.0*100.0/1.0]
    
    # Define experimental site
    # ------------------------
    # expSite ShadowSite $tag <-setup $setupTag> $ipAddr $ipPort <-ssl> <-dataSize $size>
    expSite ShadowSite 1 "127.0.0.1" 8090
    
    # Define experimental elements
    # ----------------------------
    # left column
    # expElement twoNodeLink $eleTag $iNode $jNode -dir $dirs -site $siteTag -initStif $Kij <-orient <$x1 $x2 $x3> $y1 $y2 $y3> <-pDelta Mratios> <-iMod> <-mass $m>
    expElement twoNodeLink 1 1 3 -dir 2 -site 1 -initStif 2.8
    
    # Define numerical elements
    # -------------------------
    # spring
    # element truss $eleTag $iNode $jNode $A $matTag
    element truss 3 3 4 1.0 3
    
} else {
    
    set mass3 0.04
    set mass4 0.02
    # node $tag $xCrd $yCrd $mass
    node  2   100.0   0.00
    node  4   100.0  54.00  -mass $mass4 $mass4
    
    # set the boundary conditions
    # fix $tag $DX $DY
    fix 2   1  1
    fix 4   0  1
    
    # Define experimental site
    # ------------------------
    # expSite ShadowSite $tag <-setup $setupTag> $ipAddr $ipPort <-ssl> <-udp> <-dataSize $size>
    expSite ShadowSite 2 "127.0.0.1" 8091
    
    # Define experimental elements
    # ----------------------------
    # right column
    # expElement twoNodeLink $eleTag $iNode $jNode -dir $dirs -site $siteTag -initStif $Kij <-orient <$x1 $x2 $x3> $y1 $y2 $y3> <-pDelta Mratios> <-iMod> <-mass $m>
    expElement twoNodeLink 2 2 4 -dir 2 -site 2 -initStif 5.6 -orient -1 0 0
}

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
rayleigh $alphaM $betaK $betaKinit $betaKcomm
# ------------------------------
# End of model generation
# ------------------------------


# ------------------------------
# Start of analysis generation
# ------------------------------
# create the system of equations
system ParallelProfileSPD

# create the DOF numberer
numberer ParallelPlain

# create the constraint handler
constraints Plain

# create the convergence test
test EnergyIncr 1.0e-6 10

# create the integration scheme
integrator NewmarkExplicit 0.5

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
recorder Node -file Node_Dsp$pid.out -time -node 3 4 -dof 1 disp
recorder Node -file Node_Vel$pid.out -time -node 3 4 -dof 1 vel
recorder Node -file Node_Acc$pid.out -time -node 3 4 -dof 1 accel

recorder Element -file Elmt_Frc$pid.out     -time -ele 1 2 3 forces
recorder Element -file Elmt_ctrlDsp$pid.out -time -ele 1 2   ctrlDisp
recorder Element -file Elmt_daqDsp$pid.out  -time -ele 1 2   daqDisp
# --------------------------------
# End of recorder generation
# --------------------------------


# ------------------------------
# Finally perform the analysis
# ------------------------------
# perform the transient analysis
set tTot [time {
    for {set i 1} {$i < 1600} {incr i} {
        set t [time {analyze  1  $dt}]
        #if {$pid == 0} {puts "step $i"}
    }
}]
puts "Elapsed Time = $tTot \n"

wipe
exit
# --------------------------------
# End of analysis
# --------------------------------
