# File: OneBayFrame_Server1a.py (use with OneBayFrame_Client1.tcl)
# Units: [kip,in.]
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
# The experimental setups are on the server sides.

# import the OpenFrescoPy Python module
import sys
sys.path.append("C:/Users/Andreas/Documents/OpenFresco/SourceCode/WIN64/bin")
from OpenFrescoPy import *
import math

# ------------------------------
# Start of model generation
# ------------------------------
logFile("OneBayFrame_Server1a.log")
defaultUnits("-force", "kip", "-length", "in", "-time", "sec", "-temp", "F")

# create ModelBuilder (with two-dimensions and 2 DOF/node)
model("BasicBuilder", "-ndm", 2, "-ndf", 2)

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
# expSite("ActorSite", tag, "-setup", setupTag, ipPort, <"-ssl",> <"-udp">)
expSite("ActorSite", 1, "-setup", 1, 8090)
# ------------------------------
# End of model generation
# ------------------------------


# ------------------------------
# Start the server process
# ------------------------------
# startLabServer(siteTag)
startLabServer(1)
#stepLabServer(1, 1790)
#startLabServerInteractive(1, 1790)

wipeExp()
exit()
# --------------------------------
# End of analysis
# --------------------------------
