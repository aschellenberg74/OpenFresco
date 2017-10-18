# File: OneBayFrame_Distr_SimAppServer.tcl
# (use with OneBayFrame_Distr_Client.tcl & OneBayFrame_Distr_LabServer.tcl)
# Units: [kip,in.]
#
# $Revision$
# $Date$
# $URL$
#
# Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
# Created: 11/06
# Revision: A
#
# Purpose: this file contains the tcl input to perform
# a distributed hybrid simulation of a one bay frame
# with two experimental twoNodeLink elements.
# The specimen is simulated using the SimUniaxialMaterials
# controller.


# ------------------------------
# Start of model generation
# ------------------------------
logFile "OneBayFrame_Distr_SimAppServer.log"
defaultUnits -force kip -length in -time sec -temp F

# create ModelBuilder (with two-dimensions and 2 DOF/node)
model BasicBuilder -ndm 2 -ndf 2

# Define geometry for model
# -------------------------
# node $tag $xCrd $yCrd $mass
node  1     0.0   0.00
node  3     0.0  54.00

# Define experimental site
# ------------------------
# expSite ShadowSite $tag <-setup $setupTag> $ipAddr $ipPort <-ssl> <-udp> <-dataSize $size>
expSite ShadowSite 1 "127.0.0.1" 8091

# Define experimental element
# ---------------------------
# left column
# expElement twoNodeLink $eleTag $iNode $jNode -dir $dirs -site $siteTag -initStif $Kij <-orient <$x1 $x2 $x3> $y1 $y2 $y3> <-pDelta Mratios> <-iMod> <-mass $m>
expElement twoNodeLink 1 1 3 -dir 2 -site 1 -initStif 2.8
# ------------------------------
# End of model generation
# ------------------------------


# ------------------------------
# Start the server process
# ------------------------------
# startSimAppElemServer $eleTag $port <-ssl> <-udp>
#startSimAppElemServer 1 8090 -udp;  # use with generic client element in FEA

# startSimAppSiteServer $siteTag $port <-ssl> <-udp>
startSimAppSiteServer 1 8090 -udp;  # use with experimental element in FEA

wipe
exit
# --------------------------------
# End of analysis
# --------------------------------
