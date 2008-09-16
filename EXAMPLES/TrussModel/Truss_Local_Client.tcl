# File: Truss_Local_Client.tcl (use with Truss_Local_SimAppServer.tcl)
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
# a hybrid simulation of a truss model with one experimental
# truss element.
# The specimen is simulated using the SimUniaxialMaterials
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
# uniaxialMaterial Elastic $matTag $E
uniaxialMaterial Elastic 1 3000

# Define numerical elements
# -------------------------
# element truss $eleTag $iNode $jNode $A $matTag
element truss 1 1 4 10.0 1
element truss 2 2 4 5.0 1

# Define experimental elements
# ----------------------------
# element genericClient $eleTag -node $Ndi -dof $dofNdi -dof $dofNdj ... -server $ipPort <$ipAddr> <-ssl> <-dataSize $size>
element genericClient 3 -node 3 4 -dof 1 2 -dof 1 2 -server 8090;  # use with SimAppElemServer
#expElement truss 3 3 4 -server 8090 -initStif [expr 3000.0*5.0/135.76];  # use with SimAppSiteServer

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
# perform the static analysis
analyze 2

# print the current state at node 4 and at all elements
print node 4
print ele
wipe
# --------------------------------
# End of analysis
# --------------------------------
