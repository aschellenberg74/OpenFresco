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

# import the OpenSees and OpenFresco Python module
import sys
sys.path.append("C:/Users/Andreas/Documents/OpenFresco/SourceCode/Win64/bin")
import OpenFrescoPy as opf
import math

# ------------------------------
# Start of model generation
# ------------------------------
opf.logFile("OneBayFrame_Local_SimAppServer.log")
opf.defaultUnits("-force", "kip", "-length", "in", "-time", "sec", "-temp", "F")

# create ModelBuilder (with two-dimensions and 2 DOF/node)
opf.model("BasicBuilder", "-ndm", 2, "-ndf", 2)

# Define geometry for model
# -------------------------
# node(tag, xCrd, yCrd, "-mass", mass)
opf.node(1,   0.0,  0.00)
opf.node(3,   0.0, 54.00)

# Define materials
# ----------------
# uniaxialMaterial("Steel02", matTag, Fy, E, b, R0, cR1, cR2, a1, a2, a3, a4) 
#opf.uniaxialMaterial("Elastic", 1, 2.8)
opf.uniaxialMaterial("Steel02", 1, 1.5, 2.8, 0.01, 18.5, 0.925, 0.15, 0.0, 1.0, 0.0, 1.0)

# Define control points
# ---------------------
# expControlPoint(tag, <"-node", nodeTag,> dof, rspType, <"-fact", f,> <"-lim", l, u,> <"-relTrial",> <"-relCtrl",> <"-relDaq",> ...)
opf.expControlPoint(1, "1", "disp")
opf.expControlPoint(2, "1", "disp", "1", "force")

# Define experimental control
# ---------------------------
# expControl("SimUniaxialMaterials", tag, matTags)
opf.expControl("SimUniaxialMaterials", 1, 1)
#ops.expControl("xPCtarget", 1, "192.168.2.20", 22222, "D:/PredictorCorrector/RTActualTestModels/cmAPI-xPCTarget-SCRAMNet-STS/HybridControllerD2D2", "-trialCP", 1, "-outCP", 2)
#ops.expControl("Speedgoat", 1, "10.10.10.5", 25000, "-udp", "-trialCP", 1, "-outCP", 2)
#ops.expControl("SCRAMNet", 1, "-nodeID", 3, 381020, "-trialCP", 1, "-outCP", 2)
#ops.expControl("SCRAMNetGT", 1, "-nodeID", 3, 4096, "-trialCP", 1, "-outCP", 2)

# Define experimental setup
# -------------------------
# expSetup("OneActuator", tag, <"-control", ctrlTag,> dir, "-sizeTrialOut", t, o, <"-trialDispFact", f,> ...)
opf.expSetup("OneActuator", 1, "-control", 1, 1, "-sizeTrialOut", 1, 1)

# Define experimental site
# ------------------------
# expSite("LocalSite", tag, setupTag)
opf.expSite("LocalSite", 1, 1)

# Define experimental elements
# ----------------------------
# left column
# expElement("twoNodeLink", eleTag, iNode, jNode, "-dir", dirs, "-site", siteTag, "-initStif", Kij, <"-orient", <x1, x2, x3,> y1, y2, y3,> <"-pDelta", Mratios,> <"-iMod",> <"-mass", m>)
opf.expElement("twoNodeLink", 1, 1, 3, "-dir", 2, "-site", 1, "-initStif", 2.8)
# ------------------------------
# End of model generation
# ------------------------------


# ------------------------------
# Start of recorder generation
# ------------------------------
# create the recorder objects
#ops.recorder("Node", "-file", "ServerNode_Dsp.out", "-time", "-node", 1, 3, "-dof", 1, "disp")
#ops.recorder("Node", "-file", "ServerNode_Vel.out", "-time", "-node", 1, 3, "-dof", 1, "vel")
#ops.recorder("Node", "-file", "ServerNode_Acc.out", "-time", "-node", 1, 3, "-dof", 1, "accel")

#ops.recorder("Element", "-file", "ServerElmt_Frc.out", "-time", "-ele", 1, "forces")
#ops.recorder("Element", "-file", "ServerElmt_ctrlDsp.out", "-time", "-ele", 1, "ctrlDisp")
#ops.recorder("Element", "-file", "ServerElmt_daqDsp.out", "-time", "-ele", 1, "daqDisp")

opf.expRecorder("Site", "-file", "ServerSite_trialDsp.out", "-time", "-site", 1, "trialDisp")
opf.expRecorder("Site", "-file", "ServerSite_trialVel.out", "-time", "-site", 1, "trialVel")
opf.expRecorder("Site", "-file", "ServerSite_trialAcc.out", "-time", "-site", 1, "trialAccel")
opf.expRecorder("Site", "-file", "ServerSite_trialTme.out", "-time", "-site", 1, "trialTime")
opf.expRecorder("Site", "-file", "ServerSite_outDsp.out", "-time", "-site", 1, "outDisp")
opf.expRecorder("Site", "-file", "ServerSite_outVel.out", "-time", "-site", 1, "outVel")
opf.expRecorder("Site", "-file", "ServerSite_outAcc.out", "-time", "-site", 1, "outAccel")
opf.expRecorder("Site", "-file", "ServerSite_outFrc.out", "-time", "-site", 1, "outForce")
opf.expRecorder("Site", "-file", "ServerSite_outTme.out", "-time", "-site", 1, "outTime")

opf.expRecorder("Setup", "-file", "ServerSetup_trialDsp.out", "-time", "-setup", 1, "trialDisp")
opf.expRecorder("Setup", "-file", "ServerSetup_trialVel.out", "-time", "-setup", 1, "trialVel")
opf.expRecorder("Setup", "-file", "ServerSetup_trialAcc.out", "-time", "-setup", 1, "trialAccel")
opf.expRecorder("Setup", "-file", "ServerSetup_trialTme.out", "-time", "-setup", 1, "trialTime")
opf.expRecorder("Setup", "-file", "ServerSetup_outDsp.out", "-time", "-setup", 1, "outDisp")
opf.expRecorder("Setup", "-file", "ServerSetup_outVel.out", "-time", "-setup", 1, "outVel")
opf.expRecorder("Setup", "-file", "ServerSetup_outAcc.out", "-time", "-setup", 1, "outAccel")
opf.expRecorder("Setup", "-file", "ServerSetup_outFrc.out", "-time", "-setup", 1, "outForce")
opf.expRecorder("Setup", "-file", "ServerSetup_outTme.out", "-time", "-setup", 1, "outTime")
opf.expRecorder("Setup", "-file", "ServerSetup_ctrlDsp.out", "-time", "-setup", 1, "ctrlDisp")
opf.expRecorder("Setup", "-file", "ServerSetup_ctrlVel.out", "-time", "-setup", 1, "ctrlVel")
opf.expRecorder("Setup", "-file", "ServerSetup_ctrlAcc.out", "-time", "-setup", 1, "ctrlAccel")
opf.expRecorder("Setup", "-file", "ServerSetup_ctrlTme.out", "-time", "-setup", 1, "ctrlTime")
opf.expRecorder("Setup", "-file", "ServerSetup_daqDsp.out", "-time", "-setup", 1, "daqDisp")
opf.expRecorder("Setup", "-file", "ServerSetup_daqVel.out", "-time", "-setup", 1, "daqVel")
opf.expRecorder("Setup", "-file", "ServerSetup_daqAcc.out", "-time", "-setup", 1, "daqAccel")
opf.expRecorder("Setup", "-file", "ServerSetup_daqFrc.out", "-time", "-setup", 1, "daqForce")
opf.expRecorder("Setup", "-file", "ServerSetup_daqTme.out", "-time", "-setup", 1, "daqTime")

opf.expRecorder("Control", "-file", "ServerControl_ctrlDsp.out", "-time", "-control", 1, "ctrlDisp")
opf.expRecorder("Control", "-file", "ServerControl_ctrlVel.out", "-time", "-control", 1, "ctrlVel")
opf.expRecorder("Control", "-file", "ServerControl_daqDsp.out", "-time", "-control", 1, "daqDisp")
opf.expRecorder("Control", "-file", "ServerControl_daqVel.out", "-time", "-control", 1, "daqVel")
opf.expRecorder("Control", "-file", "ServerControl_daqFrc.out", "-time", "-control", 1, "daqForce")
# --------------------------------
# End of recorder generation
# --------------------------------


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
