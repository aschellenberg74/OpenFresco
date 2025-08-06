# File: OneBayFrame_Distr_LabServer.py
# (use with OneBayFrame_Distr_Client.py & OneBayFrame_Distr_SimAppServer.py)
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
# The experimental setups are on the server sides.

# import the OpenFresco Python module
import sys
sys.path.append("C:/Users/Andreas/Documents/OpenFresco/SourceCode/WIN64/bin")
import OpenFrescoPy as opf
import math

# ------------------------------
# Start of model generation
# ------------------------------
opf.logFile("OneBayFrame_Distr_LabServer.log")
opf.defaultUnits("-force", "kip", "-length", "in", "-time", "sec", "-temp", "F")

# create ModelBuilder (with two-dimensions and 2 DOF/node)
opf.model("BasicBuilder", "-ndm", 2, "-ndf", 2)

# Define materials
# ----------------
# uniaxialMaterial("Steel02", matTag, Fy, E, b, R0, cR1, cR2, a1, a2, a3, a4) 
#opf.uniaxialMaterial("Elastic", 1, 2.8)
opf.uniaxialMaterial("Steel02", 1, 1.5, 2.8, 0.01, 18.5, 0.925, 0.15, 0.0, 1.0, 0.0, 1.0)

# Define control points
# ---------------------
# expControlPoint(tag, <"-node", nodeTag,> dof, rspType, <"-fact", f,> <"-lim", l, u,> <"-relTrial",> <"-relCtrl",> <"-relDaq",> ...)
opf.expControlPoint(1,  "1", "disp")
opf.expControlPoint(2,  "1", "disp", "1", "force")

# Define experimental control
# ---------------------------
# expControl("SimUniaxialMaterials", tag, matTags)
opf.expControl("SimUniaxialMaterials", 1, 1)
#opf.expControl("xPCtarget", 1, "192.168.2.20", 22222, "D:/PredictorCorrector/RTActualTestModels/cmAPI-xPCTarget-SCRAMNet-STS/HybridControllerD2D2", "-trialCP", 1, "-outCP", 2)
#opf.expControl("SCRAMNet", 1, 381020, "-trialCP", 1, "-outCP", 2)
#opf.expControl("SCRAMNetGT", 1, 4096, "-trialCP", 1, "-outCP", 2)

# Define experimental setup
# -------------------------
# expSetup("OneActuator", tag, <"-control", ctrlTag,> dir, "-sizeTrialOut", t, o, <"-trialDispFact", f,> ...)
opf.expSetup("OneActuator", 1, "-control", 1, 1, "-sizeTrialOut", 1, 1)

# Define experimental site
# ------------------------
# expSite("ActorSite", tag, "-setup", setupTag, ipPort, <"-ssl",> <"-udp">)
opf.expSite("ActorSite", 1, "-setup", 1, 8091)
# ------------------------------
# End of model generation
# ------------------------------


# ------------------------------
# Start the server process
# ------------------------------
# startLabServer(siteTag)
opf.startLabServer(1)
#opf.stepLabServer(1, 1790)
#opf.startLabServerInteractive(1, 1790)

opf.wipeExp()
exit()
# --------------------------------
# End of analysis
# --------------------------------
