# File: OneBayFrame_Local_SimAppServer.py (use with OneBayFrame_Local_Client.py)
# Units: [kip,in.]
#
# Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
# Created: 11/06
# Revision: A
#
# Purpose: this file contains the python input to perform
# a local hybrid simulation of a one bay frame with
# two experimental twoNodeLink elements.
# The specimen is simulated using the SimUniaxialMaterials
# controller.

# import the OpenFrescoPy Python module
import sys
sys.path.append("C:/Users/Andreas/Documents/OpenFresco/SourceCode/WIN64/bin")
from OpenFrescoPy import *
import math

# ------------------------------
# Start of model generation
# ------------------------------
logFile("OneBayFrame_Local_SimAppServer.log")
defaultUnits("-force", "kip", "-length", "in", "-time", "sec", "-temp", "F")

# create ModelBuilder (with two-dimensions and 2 DOF/node)
model("BasicBuilder", "-ndm", 2, "-ndf", 2)

# Load OpenFresco package
# -----------------------
# (make sure all dlls are in the same folder as OpenFrescoPy)
#loadPackage("OpenFrescoPy")

# Define geometry for model
# -------------------------
# node(tag, xCrd, yCrd, "-mass", mass)
node(1,   0.0,  0.00)
node(3,   0.0, 54.00)

# Define materials
# ----------------
# uniaxialMaterial("Steel02", matTag, Fy, E, b, R0, cR1, cR2, a1, a2, a3, a4) 
#uniaxialMaterial("Elastic", 1, 2.8)
uniaxialMaterial("Steel02", 1, 1.5, 2.8, 0.01, 18.5, 0.925, 0.15, 0.0, 1.0, 0.0, 1.0)

# Define control points
# ---------------------
# expControlPoint(tag, <"-node", nodeTag,> dof, rspType, <"-fact", f,> <"-lim", l, u,> <"-relTrial",> <"-relCtrl",> <"-relDaq",> ...)
expControlPoint(1,  "1", "disp")
expControlPoint(2,  "1", "disp", "1", "force")

# Define experimental control
# ---------------------------
# expControl("SimUniaxialMaterials", tag, matTags)
expControl("SimUniaxialMaterials", 1, 1)
#expControl("xPCtarget", 1, "192.168.2.20", 22222, "D:/PredictorCorrector/RTActualTestModels/cmAPI-xPCTarget-SCRAMNet-STS/HybridControllerD2D2", "-trialCP", 1, "-outCP", 2)
#expControl("SCRAMNet", 1, 381020, "-trialCP", 1, "-outCP", 2)
#expControl("SCRAMNetGT", 1, 4096, "-trialCP", 1, "-outCP", 2)

# Define experimental setup
# -------------------------
# expSetup("OneActuator", tag, <"-control", ctrlTag,> dir, "-sizeTrialOut", t, o, <"-trialDispFact", f,> ...)
expSetup("OneActuator", 1, "-control", 1, 1, "-sizeTrialOut", 1, 1)

# Define experimental site
# ------------------------
# expSite("LocalSite", tag, setupTag)
expSite("LocalSite", 1, 1)

# Define experimental elements
# ----------------------------
# left column
# expElement("twoNodeLink", eleTag, iNode, jNode, "-dir", dirs, "-site", siteTag, "-initStif", Kij, <"-orient", <x1, x2, x3,> y1, y2, y3,> <"-pDelta", Mratios,> <"-iMod",> <"-mass", m>)
expElement("twoNodeLink", 1, 1, 3, "-dir", 2, "-site", 1, "-initStif", 2.8)
# ------------------------------
# End of model generation
# ------------------------------


# ------------------------------
# Start the server process
# ------------------------------
# startSimAppElemServer(eleTag, port, <"-ssl",> <"-udp">)
startSimAppElemServer(1, 8090, "-udp");  # use with generic client element in FEA

# startSimAppSiteServer(siteTag, port, <"-ssl",> <"-udp">)
#startSimAppSiteServer(1, 8090, "-udp");  # use with experimental element in FEA

wipeExp()
exit()
# --------------------------------
# End of analysis
# --------------------------------
