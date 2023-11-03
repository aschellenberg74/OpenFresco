# File: Truss_Local_SimAppServer.tcl (use with Truss_Local_Client.tcl)
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
logFile "Truss_Local_SimAppServer.log"
defaultUnits -force kip -length in -time sec -temp F

# create ModelBuilder (with two-dimensions and 3 DOF/node)
model BasicBuilder -ndm 2 -ndf 2

# Define geometry for model
# -------------------------
# node $tag $xCrd $yCrd $mass
node 3 168.0  0.0
node 4  72.0 96.0

# Define materials
# ----------------
set E 3000.0
set A3 5.0
set L3 [expr sqrt(pow(168.0-72.0,2.0) + pow(96.0,2.0))]
set kInit [expr $E*$A3/$L3]
# uniaxialMaterial Elastic $matTag $E
uniaxialMaterial Elastic 1 $kInit

# Define experimental control
# ---------------------------
# expControl SimUniaxialMaterials $tag $matTags
expControl SimUniaxialMaterials 1 1

# Define experimental setup
# -------------------------
# expSetup OneActuator $tag <-control $ctrlTag> $dir -sizeTrialOut $t $o <-trialDispFact $f> ...
expSetup OneActuator 1 -control 1 1 -sizeTrialOut 1 1

# Define experimental site
# ------------------------
# expSite LocalSite $tag $setupTag
expSite LocalSite 1 1

# Define experimental element
# ---------------------------
# expElement truss $eleTag $iNode $jNode -site $siteTag -initStif $Kij <-iMod> <-rho $rho> 
#expElement truss 3 3 4 -site 1 -initStif $kInit
# ------------------------------
# End of model generation
# ------------------------------


# ------------------------------
# Start the server process
# ------------------------------
# startSimAppElemServer $eleTag $port <-ssl> <-udp>
#startSimAppElemServer 3 8090 -udp;  # use with generic client element in FEA

# startSimAppSiteServer $siteTag $port <-ssl> <-udp>
startSimAppSiteServer 1 8090 -udp;  # use with experimental element in FEA
wipeExp
exit
# --------------------------------
# End of analysis
# --------------------------------
