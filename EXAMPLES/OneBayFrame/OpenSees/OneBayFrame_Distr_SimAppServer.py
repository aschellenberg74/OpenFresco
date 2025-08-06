# File: OneBayFrame_Distr_SimAppServer.py
# (use with OneBayFrame_Distr_Client.py & OneBayFrame_Distr_LabServer.py)
# Units: [kip,in.]
#
# Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
# Created: 06/25
# Revision: A
#
# Purpose: this file contains the python input to perform
# a distributed hybrid simulation of a one bay frame
# with two experimental twoNodeLink elements.
# The specimen is simulated using the SimUniaxialMaterials
# controller.

# import the OpenFresco Python module
import sys
sys.path.append("C:/Users/Andreas/Documents/OpenFresco/SourceCode/Win64/bin")
import OpenFrescoPy as opf
import math

# ------------------------------
# Start of model generation
# ------------------------------
opf.logFile("OneBayFrame_Distr_SimAppServer.log")
opf.defaultUnits("-force", "kip", "-length", "in", "-time", "sec", "-temp", "F")

# create ModelBuilder (with two-dimensions and 2 DOF/node)
opf.model("BasicBuilder", "-ndm", 2, "-ndf", 2)

# Define geometry for model
# -------------------------
# node(tag, xCrd, yCrd, "-mass", mass)
opf.node(1,   0.0,  0.00)
opf.node(3,   0.0, 54.00)

# Define experimental site
# ------------------------
# expSite("ShadowSite", tag, <"-setup", setupTag,> ipAddr, ipPort, <"-ssl",> <"-udp",> <"-dataSize", size>)
opf.expSite("ShadowSite", 1, "127.0.0.1", 8091)

# Define experimental elements
# ----------------------------
# left column
# expElement("twoNodeLink", eleTag, iNode, jNode, "-dir", dirs, "-site", siteTag, "-initStif", Kij, <"-orient", <x1, x2, x3,> y1, y2, y3,> <"-pDelta", Mratios,> <"-iMod",> <"-mass", m>)
opf.expElement("twoNodeLink", 1, 1, 3, "-dir", 2, "-site", 1, "-initStif", 2.8)
# ------------------------------
# End of model generation
# ------------------------------


# ------------------------------
# Start the server process
# ------------------------------
# startSimAppElemServer(eleTag, port, <"-ssl",> <"-udp">)
#opf.startSimAppElemServer(1, 8090, "-udp");  # use with generic client element in FEA

# startSimAppSiteServer(siteTag, port, <"-ssl",> <"-udp">)
opf.startSimAppSiteServer(1, 8090, "-udp");  # use with experimental element in FEA

opf.wipeExp()
exit()
# --------------------------------
# End of analysis
# --------------------------------
