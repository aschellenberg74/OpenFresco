# File: PortalFrame_Local.tcl
# Units: [kip,in.]
#
# $Revision$
# $Date$
# $URL$
#
# Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
# Created: 10/07
# Revision: A
#
# Purpose: this file contains the tcl input to perform
# a local hybrid simulation of a portal frame with
# two experimental beamColumn elements.
# The frame can be analyzed with or without gravity loads.
# The specimens are simulated using the SimUniaxialMaterials
# controller.


# ------------------------------
# Start of model generation
# ------------------------------
logFile "PortalFrame_Local.log"
defaultUnits -force kip -length in -time sec -temp F

# create ModelBuilder (with two-dimensions and 3 DOF/node)
model BasicBuilder -ndm 2 -ndf 3

# Load OpenFresco package
# -----------------------
# (make sure all dlls are in the same folder as openSees.exe)
loadPackage OpenFresco

# Define geometry for model
# -------------------------
set withGravity 1;
set Pc 10.638;
set P [expr 0.5*$Pc];
set mass3 [expr $P/386.1];
set mass4 [expr $P/386.1];
# node $tag $xCrd $yCrd $mass
node  1     0.0   0.0
node  2   100.0   0.0
node  3     0.0  50.0  -mass $mass3 $mass3 0.0
node  4   100.0  50.0  -mass $mass4 $mass4 0.0

# set the boundary conditions
# fix $tag $DX $DY $RZ
fix 1   1  1  0
fix 2   1  1  0

# Define materials
# ----------------
# uniaxialMaterial Steel02 $matTag $Fy $E $b $R0 $cR1 $cR2 $a1 $a2 $a3 $a4 
uniaxialMaterial Steel02 1 1.5 2.8 0.01 18.5 0.925 0.15 0.0 1.0 0.0 1.0
#uniaxialMaterial Elastic 1 2.8

# Define control points
# ---------------------
# expControlPoint $tag <-node $nodeTag> $dof $rspType <-fact $f> <-lim $l $u> ...
expControlPoint 1  1 disp
expControlPoint 2  1 disp 1 force

# Define experimental control
# ---------------------------
# expControl SimUniaxialMaterials $tag $matTags
expControl SimUniaxialMaterials 1 1
#expControl xPCtarget 1 "192.168.2.20" 22222 "D:/PredictorCorrector/RTActualTestModels/cmAPI-xPCTarget-SCRAMNet-STS/HybridControllerD2D2" -trialCP 1 -outCP 2
expControl SimUniaxialMaterials 2 1

# Define experimental setup
# -------------------------
# expSetup OneActuator $tag <-control $ctrlTag> $dir -sizeTrialOut $t $o <-trialDispFact $f> ...
expSetup OneActuator 1 -control 1 2 -sizeTrialOut 3 3
expSetup OneActuator 2 -control 2 2 -sizeTrialOut 3 3

# Define experimental site
# ------------------------
# expSite LocalSite $tag $setupTag
expSite LocalSite 1 1
expSite LocalSite 2 2

# Define geometric transformation
# -------------------------------
#geomTransf PDelta 1
geomTransf Corotational 1

# Define experimental elements
# ----------------------------
# left and right columns
# expElement beamColumn $eleTag $iNode $jNode $transTag -site $siteTag -initStif $Kij <-iMod> <-rho $rho>
expElement beamColumn 1 3 1 1 -site 1 -initStif 1310.8 0 0 0 11.2 -280.0 0 -280.0 9333.3333
expElement beamColumn 2 4 2 1 -site 2 -initStif 1310.8 0 0 0 11.2 -280.0 0 -280.0 9333.3333

# Define numerical elements
# -------------------------
# element elasticBeamColumn $eleTag $iNode $jNode $A $E $Iz $transfTag
element elasticBeamColumn 3 3 4 3.55 29000 22.1 1

if {$withGravity} {
    # Define gravity loads
    # --------------------
    # Create a Plain load pattern with a Linear TimeSeries
    pattern Plain 1 "Linear" {
        # Create nodal loads at nodes 2
        #    nd    FX          FY  MZ 
        load  3   0.0  [expr -$P] 0.0
        load  4   0.0  [expr -$P] 0.0
    }
    # ------------------------------
    # End of model generation
    # ------------------------------


    # ------------------------------
    # Start of analysis generation
    # ------------------------------
    # Create the system of equation
    system BandGeneral
    # Create the DOF numberer
    numberer Plain
    # Create the constraint handler
    constraints Plain
    # Create the convergence test
    test EnergyIncr 1.0e-6 10
    # Create the integration scheme
    integrator LoadControl 0.1
    # Create the solution algorithm
    algorithm Newton
    # Create the analysis object
    analysis Static
    # ------------------------------
    # End of analysis generation
    # ------------------------------


    # ------------------------------
    # Start of recorder generation
    # ------------------------------
    # create a Recorder object for the nodal displacements at node 2
    recorder Node -file Gravity_Node_Dsp.out -time -node 3 4 -dof 1 2 3 disp
    recorder Element -file Gravity_Elmt_glbFrc.out  -time -ele 1 2 3 force
    recorder Element -file Gravity_Elmt_ctrlDsp.out -time -ele 1 2   ctrlDisp
    recorder Element -file Gravity_Elmt_daqDsp.out  -time -ele 1 2   daqDisp
    expRecorder Control -file Gravity_Ctrl_ctrlDsp.out -time -control 1 2 ctrlDisp
    expRecorder Control -file Gravity_Ctrl_daqDsp.out  -time -control 1 2 daqDisp
    expRecorder Control -file Gravity_Ctrl_daqFrc.out  -time -control 1 2 daqForce
    # --------------------------------
    # End of recorder generation
    # --------------------------------


    # ------------------------------
    # Perform the gravity analysis
    # ------------------------------
    # perform the gravity load analysis, requires 10 steps to reach the load level
    record
    if {[analyze 10] == 0} {
        puts "\nGravity load analysis completed"
    } else {
        puts "\nGravity load analysis failed"
        exit -1
    }


    # ------------------------------
    # Start of model generation
    # ------------------------------
    # Set the gravity loads to be constant & reset the time in the domain
    loadConst -time 0.0
    remove recorders
    removeExp recorders
}

# Define dynamic loads
# --------------------
# set time series to be passed to uniform excitation
set dt 0.01
set scale 1.2
timeSeries Path 1 -filePath SACNF01.txt -dt $dt -factor [expr 386.1*$scale]

# create UniformExcitation load pattern
# pattern UniformExcitation $tag $dir -accel $tsTag <-vel0 $vel0>
pattern UniformExcitation 2 1 -accel 1

# calculate the Rayleigh damping factors for nodes & elements
set alphaM     1.2797;    # D = alphaM*M
set betaK      0.0;       # D = betaK*Kcurrent
set betaKinit  0.0;       # D = beatKinit*Kinit
set betaKcomm  0.0;       # D = betaKcomm*KlastCommit

# set the Rayleigh damping 
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
constraints Plain
# create the convergence test
test FixedNumIter 5
#test NormDispIncr 1E-8 25
# create the integration scheme
integrator NewmarkHSFixedNumIter 0.5 0.25
#integrator HHTHSFixedNumIter 0.5
#integrator CollocationHSFixedNumIter 1.5
#integrator AlphaOS 0.9
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
recorder Node -file Node_Dsp.out -time -node     3 4 -dof 1 2 3 disp
recorder Node -file Node_Vel.out -time -node     3 4 -dof 1 2 3 vel
recorder Node -file Node_Acc.out -time -node     3 4 -dof 1 2 3 accel
recorder Node -file Node_Rxn.out -time -node 1 2 3 4 -dof 1 2 3 reactionIncludingInertia

recorder Element -file Elmt_glbFrc.out  -time -ele 1 2 3 forces
recorder Element -file Elmt_ctrlDsp.out -time -ele 1 2   ctrlDisp
recorder Element -file Elmt_daqDsp.out  -time -ele 1 2   daqDisp
expRecorder Control -file Ctrl_ctrlDsp.out -time -control 1 2 ctrlDisp
expRecorder Control -file Ctrl_daqDsp.out  -time -control 1 2 daqDisp
expRecorder Control -file Ctrl_daqFrc.out  -time -control 1 2 daqForce
# --------------------------------
# End of recorder generation
# --------------------------------


# ------------------------------
# Finally perform the analysis
# ------------------------------
record
# perform an eigenvalue analysis
set pi [expr acos(-1.0)]
set lambda [eigen -fullGenLapack 4]
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
    for {set i 1} {$i < 2500} {incr i} {
        set t [time {analyze  1  [expr $dt/1.0]}]
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
