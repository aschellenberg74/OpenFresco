# File: OneBayFrame_Distr_SimAppServer.tcl
# (use with OneBayFrame_Distr_Client.tcl & OneBayFrame_Distr_LabServer.tcl)
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
# a distributed hybrid simulation of a one bay frame
# with two experimental zero length elements.
# The specimen is simulated using the SimUniaxialMaterials
# controller.


# ------------------------------
# Start of model generation
# ------------------------------
# create ModelBuilder (with two-dimensions and 2 DOF/node)
model BasicBuilder -ndm 2 -ndf 2

# Define geometry for model
# -------------------------
# node $tag $xCrd $yCrd $mass
node  1     0.0   0.00
node  3     0.0  54.00

# Define experimental site
# ------------------------
# expSite RemoteSite $tag <-setup $setupTag> $ipAddr $ipPort <-ssl> <-dataSize $size>
expSite RemoteSite 1 "127.0.0.1" 8091

# Define experimental element
# ---------------------------
# left column
# expElement twoNodeLink $eleTag $iNode $jNode -dir $dirs -site $siteTag -initStif $Kij <-orient $x1 $x2 $x3 $y1 $y2 $y3> <-iMod> <-mass $m>
expElement twoNodeLink 1 1 3 -dir 2 -site 1 -initStif 2.8 -orient 0 1 0 -1 0 0
# ------------------------------
# End of model generation
# ------------------------------


# ------------------------------
# Start the server process
# ------------------------------
# startSimAppSiteServer $siteTag $port <-ssl>
#startSimAppSiteServer 1 8090

# startSimAppElemServer $eleTag $port <-ssl>
startSimAppElemServer 1 8090
# --------------------------------
# End of analysis
# --------------------------------
