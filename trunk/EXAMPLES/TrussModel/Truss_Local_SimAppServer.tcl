# File: Truss_Local_SimAppServer.tcl (use with Truss_Local_Client.tcl)
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
# create ModelBuilder (with two-dimensions and 3 DOF/node)
model BasicBuilder -ndm 2 -ndf 2

# Define geometry for model
# -------------------------
# node $tag $xCrd $yCrd $mass
node 3 168.0  0.0
node 4  72.0 96.0

# Define materials
# ----------------
# uniaxialMaterial Steel02 $matTag $Fy $E $b $R0 $cR1 $cR2 $a1 $a2 $a3 $a4 
uniaxialMaterial Elastic 1 [expr 3000.0*5.0/135.76]

# Define experimental control
# ---------------------------
# expControl SimUniaxialMaterials $tag $matTags
expControl SimUniaxialMaterials 1 1

# Define experimental setup
# -------------------------
# expSetup OneActuator $tag <-control $ctrlTag> $dir <-ctrlDispFact $f> ...
expSetup OneActuator 1 -control 1 1

# Define experimental site
# ------------------------
# expSite LocalSite $tag $setupTag
expSite LocalSite 1 1

# Define experimental element
# ---------------------------
# expElement truss $eleTag $iNode $jNode -site $siteTag -initStif $Kij <-iMod> <-rho $rho> 
expElement truss 3 3 4 -site 1 -initStif [expr 3000.0*5.0/135.76]
# ------------------------------
# End of model generation
# ------------------------------


# ------------------------------
# Start the server process
# ------------------------------
# startSimAppSiteServer $siteTag $port <-ssl>
#startSimAppSiteServer 1 8090

# startSimAppElemServer $eleTag $port <-ssl>
startSimAppElemServer 3 8090
# --------------------------------
# End of analysis
# --------------------------------
