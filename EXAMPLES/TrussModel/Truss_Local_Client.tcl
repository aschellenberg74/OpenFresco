# File: Truss_Local_Client.tcl (use with Truss_Local_SimAppServer.tcl)
# Units: [kip,in.]
#
# Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
# Created: 11/06
# Revision: A
#
# Purpose: this file contains the tcl input to perform
# a hybrid simulation of a truss model with one experimental
# truss element.
# The specimen is simulated using the SimUniaxialMaterials
# controller.


# ------------------------------
# Start of model generation
# ------------------------------
logFile "Truss_Local_Client.log"
defaultUnits -force kip -length in -time sec -temp F

# create ModelBuilder (with two-dimensions and 2 DOF/node)
model BasicBuilder -ndm 2 -ndf 2

# Load OpenFresco package
# -----------------------
# (make sure all dlls are in the same folder as openSees.exe)
loadPackage OpenFresco

# Define geometry for model
# -------------------------
# node $tag $xCrd $yCrd $mass
node 1   0.0  0.0
node 2 144.0  0.0
node 3 168.0  0.0
node 4  72.0 96.0

# set the boundary conditions
# fix $tag $DX $DY
fix 1 1 1 
fix 2 1 1
fix 3 1 1

# Define materials
# ----------------
set E 3000.0
set A3 5.0
set L3 [expr sqrt(pow(168.0-72.0,2.0) + pow(96.0,2.0))]
set kInit [expr $E*$A3/$L3]
# uniaxialMaterial Elastic $matTag $E
uniaxialMaterial Elastic 1 $E

# Define numerical elements
# -------------------------
# element truss $eleTag $iNode $jNode $A $matTag
element truss 1 1 4 10.0 1
element truss 2 2 4 5.0 1

# Define experimental elements
# ----------------------------
# element genericClient $eleTag -node $Ndi -dof $dofNdi -dof $dofNdj ... -server $ipPort <$ipAddr> <-ssl> <-udp> <-dataSize $size>
#element genericClient 3 -node 3 4 -dof 1 2 -dof 1 2 -server 8090 -udp;  # use with SimAppElemServer
expElement truss 3 3 4 -server 8090 -udp -initStif $kInit;  # use with SimAppSiteServer

# Define static loads
# -------------------
# create a Plain load pattern with a linear TimeSeries
pattern Plain 1 "Linear" {
    # load $nodeTag $xForce $yForce
    load 4 100 -50
}
# ------------------------------
# End of model generation
# ------------------------------


# ------------------------------
# Start of analysis generation
# ------------------------------
# create the system of equation
system BandSPD
# create the DOF numberer
numberer RCM
# create the constraint handler
constraints Plain
# create the integration scheme
integrator LoadControl 1.0
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
# create the recorder objects
recorder Node -file Node_Dsp.out -time -node 4 -dof 1 2 disp
# --------------------------------
# End of recorder generation
# ---------------------------------


# ------------------------------
# Finally perform the analysis
# ------------------------------
record
# perform the static analysis
analyze 2

# print the current state at node 4 and at all elements
print node 4
print ele
wipeExp
wipe
exit
# --------------------------------
# End of analysis
# --------------------------------
