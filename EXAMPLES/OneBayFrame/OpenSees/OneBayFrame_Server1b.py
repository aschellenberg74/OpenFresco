# File: OneBayFrame_Server1b.py (use with OneBayFrame_Client1.py)
# Units: [kip,in.]
#
# Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
# Created: 11/06
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
opf.logFile("OneBayFrame_Server1b.log")
opf.defaultUnits("-force", "kip", "-length", "in", "-time", "sec", "-temp", "F")

# create ModelBuilder (with two-dimensions and 2 DOF/node)
opf.model("BasicBuilder", "-ndm", 2, "-ndf", 2)

# Define materials
# ----------------
# uniaxialMaterial("Steel02", matTag, Fy, E, b, R0, cR1, cR2, a1, a2, a3, a4) 
opf.uniaxialMaterial("Elastic", 1, 5.6);  # UC Berkeley Cantilever Column [kip/in]
#opf.uniaxialMaterial("Elastic", 1, 16.0);  # UBC Cantilever Column [kN/cm]
#opf.uniaxialMaterial("Elastic", 1, ????);  # Kyoto University Cantilever Column [??/??]

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
opf.expSetup("OneActuator", 1, "-control", 1, 1, "-sizeTrialOut", 1, 1);  # UC Berkeley setup
#opf.expSetup("OneActuator", 1, "-control", 1, 1, "-sizeTrialOut", 1, 1, "-trialDispFact", 2.54, "-outDispFact", 1.0/2.54, "-outForceFact", 0.2248);  # UBC setup with units conversion
#opf.expSetup("OneActuator", 1, "-control", 1, 1, "-sizeTrialOut", 1, 1, "-trialDispFact", ???, "-outDispFact", ???, "-outForceFact", ???);  # Kyoto University with units conversion

# Define experimental site
# ------------------------
# expSite("ActorSite", tag, "-setup", setupTag, ipPort, <"-ssl",> <"-udp">)
opf.expSite("ActorSite", 2, "-setup", 1, 8091)
# ------------------------------
# End of model generation
# ------------------------------


# ------------------------------
# Start the server process
# ------------------------------
# startLabServer(siteTag)
opf.startLabServer(2)

opf.wipeExp()
exit()
# --------------------------------
# End of analysis
# --------------------------------
