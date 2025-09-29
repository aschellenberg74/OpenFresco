# File: OneBayFrame_Local.py
# Units: [kip,in.]
#
# Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
# Created: 06/21
# Revision: A
#
# Purpose: this file contains the python input to perform
# a local hybrid simulation of a one bay frame with
# two experimental twoNodeLink elements.
# The specimens are simulated using the SimUniaxialMaterials
# controller.

# import the OpenSees and OpenFresco Python module
import sys
sys.path.append("C:/Users/Andreas/Documents/OpenSees/SourceCode/Win64/bin")
sys.path.append("C:/Users/Andreas/Documents/OpenFresco/SourceCode/Win64/bin")
import opensees as ops
import math

# ------------------------------
# Start of model generation
# ------------------------------
ops.logFile("OneBayFrame_Local.log")
ops.defaultUnits("-force", "kip", "-length", "in", "-time", "sec", "-temp", "F")

# create ModelBuilder (with two-dimensions and 2 DOF/node)
ops.model("BasicBuilder", "-ndm", 2, "-ndf", 2)

# Load OpenFresco package
# -----------------------
# (make sure all dlls are in the same folder as OpenFrescoPy)
ops.loadPackage("OpenFrescoPy")

# Define geometry for model
# -------------------------
mass3 = 0.04
mass4 = 0.02
# node(tag, xCrd, yCrd, <"-mass", mass>)
ops.node(1,   0.0,  0.00)
ops.node(2, 100.0,  0.00)
ops.node(3,   0.0, 54.00, "-mass", mass3, mass3)
ops.node(4, 100.0, 54.00, "-mass", mass4, mass4)

# set the boundary conditions
# fix(tag, DX, DY)
ops.fix(1, 1, 1)
ops.fix(2, 1, 1)
ops.fix(3, 0, 1)
ops.fix(4, 0, 1)

# Define materials
# ----------------
# uniaxialMaterial("Steel02", matTag, Fy, E, b, R0, cR1, cR2, a1, a2, a3, a4) 
#ops.uniaxialMaterial("Elastic", 1, 2.8)
ops.uniaxialMaterial("Steel02", 1, 1.5, 2.8, 0.01, 18.5, 0.925, 0.15, 0.0, 1.0, 0.0, 1.0)
ops.uniaxialMaterial("Elastic", 2, 5.6)
#ops.uniaxialMaterial("Steel02", 2, 3.0, 5.6, 0.01, 18.5, 0.925, 0.15, 0.0, 1.0, 0.0, 1.0) 
ops.uniaxialMaterial("Elastic", 3, 2.0*100.0/1.0)

# Define control points
# ---------------------
# expControlPoint(tag, <"-node", nodeTag,> dof, rspType, <"-fact", f,> <"-lim", l, u,> <"-relTrial",> <"-relCtrl",> <"-relDaq",> ...)
ops.expControlPoint(1,  "1", "disp")
ops.expControlPoint(2,  "1", "disp", "1", "force")

# Define experimental control
# ---------------------------
# expControl("SimUniaxialMaterials", tag, matTags)
ops.expControl("SimUniaxialMaterials", 1, 1)
#ops.expControl("xPCtarget", 1, "192.168.2.20", 22222, "D:/PredictorCorrector/RTActualTestModels/cmAPI-xPCTarget-SCRAMNet-STS/HybridControllerD2D2", "-trialCP", 1, "-outCP", 2)
#ops.expControl("SCRAMNet", 1, 381020, "-trialCP", 1, "-outCP", 2)
#ops.expControl("SCRAMNetGT", 1, 4096, "-trialCP", 1, "-outCP", 2)
ops.expControl("SimUniaxialMaterials", 2, 2)

# Define experimental setup
# -------------------------
# expSetup("OneActuator", tag, <"-control", ctrlTag,> dir, "-sizeTrialOut", t, o, <"-trialDispFact", f,> ...)
ops.expSetup("OneActuator", 1, "-control", 1, 1, "-sizeTrialOut", 1, 1)
ops.expSetup("OneActuator", 2, "-control", 2, 1, "-sizeTrialOut", 1, 1)

# Define experimental site
# ------------------------
# expSite("LocalSite", tag, setupTag)
ops.expSite("LocalSite", 1, 1)
ops.expSite("LocalSite", 2, 2)

# Define experimental elements
# ----------------------------
# left and right columns
# expElement("twoNodeLink", eleTag, iNode, jNode, "-dir", dirs, "-site", siteTag, "-initStif", Kij, <"-orient", <x1, x2, x3,> y1, y2, y3,> <"-pDelta", Mratios,> <"-iMod",> <"-mass", m>)
ops.expElement("twoNodeLink", 1, 1, 3, "-dir", 2, "-site", 1, "-initStif", 2.8)
ops.expElement("twoNodeLink", 2, 2, 4, "-dir", 2, "-site", 2, "-initStif", 5.6)

# Define numerical elements
# -------------------------
# spring
# element("truss", eleTag, iNode, jNode, A, matTag)
ops.element("truss", 3, 3, 4, 1.0, 3)

# Define dynamic loads
# --------------------
# set time series to be passed to uniform excitation
dt = 0.02
scale = 1.0
ops.timeSeries("Path", 1, "-filePath", "elcentro.txt", "-dt", dt, "-factor", 386.1*scale)

# create UniformExcitation load pattern
# pattern("UniformExcitation", tag, dir, "-accel", tsTag, <"-vel0", v0>)
ops.pattern("UniformExcitation", 1, 1, "-accel", 1)

# calculate the Rayleigh damping factors for nodes & elements
alphaM    = 1.010017396536;  # D = alphaM*M
betaK     = 0.0;             # D = betaK*Kcurrent
betaKinit = 0.0;             # D = beatKinit*Kinit
betaKcomm = 0.0;             # D = betaKcomm*KlastCommit

# set the Rayleigh damping 
ops.rayleigh(alphaM, betaK, betaKinit, betaKcomm)
# ------------------------------
# End of model generation
# ------------------------------


# ------------------------------
# Start of analysis generation
# ------------------------------
# create the system of equations
ops.system("BandGeneral")
# create the DOF numberer
ops.numberer("Plain")
# create the constraint handler
ops.constraints("Plain")
# create the convergence test
ops.test("EnergyIncr", 1.0e-6, 10)
# create the integration scheme
#ops.integrator("Newmark", 0.5, 0.25)
ops.integrator("NewmarkExplicit", 0.5)
#integrator("AlphaOS", 1.0)
# create the solution algorithm
#ops.algorithm("Newton)
ops.algorithm("Linear")
# create the analysis object 
ops.analysis("Transient")
# ------------------------------
# End of analysis generation
# ------------------------------


# ------------------------------
# Start of recorder generation
# ------------------------------
# create the recorder objects
ops.recorder("Node", "-file", "Node_Dsp.out", "-time", "-node", 3, 4, "-dof", 1, "disp")
ops.recorder("Node", "-file", "Node_Vel.out", "-time", "-node", 3, 4, "-dof", 1, "vel")
ops.recorder("Node", "-file", "Node_Acc.out", "-time", "-node", 3, 4, "-dof", 1, "accel")

ops.recorder("Element", "-file", "Elmt_Frc.out", "-time", "-ele", 1, 2, 3, "forces")
ops.recorder("Element", "-file", "Elmt_ctrlDsp.out", "-time", "-ele", 1, 2, "ctrlDisp")
ops.recorder("Element", "-file", "Elmt_daqDsp.out", "-time", "-ele", 1, 2, "daqDisp")

ops.expRecorder("Site", "-file", "Site_trialDsp.out", "-time", "-site", 1, 2, "trialDisp")
ops.expRecorder("Site", "-file", "Site_trialVel.out", "-time", "-site", 1, 2, "trialVel")
ops.expRecorder("Site", "-file", "Site_trialAcc.out", "-time", "-site", 1, 2, "trialAccel")
ops.expRecorder("Site", "-file", "Site_trialTme.out", "-time", "-site", 1, 2, "trialTime")
ops.expRecorder("Site", "-file", "Site_outDsp.out", "-time", "-site", 1, 2, "outDisp")
ops.expRecorder("Site", "-file", "Site_outVel.out", "-time", "-site", 1, 2, "outVel")
ops.expRecorder("Site", "-file", "Site_outAcc.out", "-time", "-site", 1, 2, "outAccel")
ops.expRecorder("Site", "-file", "Site_outFrc.out", "-time", "-site", 1, 2, "outForce")
ops.expRecorder("Site", "-file", "Site_outTme.out", "-time", "-site", 1, 2, "outTime")

ops.expRecorder("Setup", "-file", "Setup_trialDsp.out", "-time", "-setup", 1, 2, "trialDisp")
ops.expRecorder("Setup", "-file", "Setup_trialVel.out", "-time", "-setup", 1, 2, "trialVel")
ops.expRecorder("Setup", "-file", "Setup_trialAcc.out", "-time", "-setup", 1, 2, "trialAccel")
ops.expRecorder("Setup", "-file", "Setup_trialTme.out", "-time", "-setup", 1, 2, "trialTime")
ops.expRecorder("Setup", "-file", "Setup_outDsp.out", "-time", "-setup", 1, 2, "outDisp")
ops.expRecorder("Setup", "-file", "Setup_outVel.out", "-time", "-setup", 1, 2, "outVel")
ops.expRecorder("Setup", "-file", "Setup_outAcc.out", "-time", "-setup", 1, 2, "outAccel")
ops.expRecorder("Setup", "-file", "Setup_outFrc.out", "-time", "-setup", 1, 2, "outForce")
ops.expRecorder("Setup", "-file", "Setup_outTme.out", "-time", "-setup", 1, 2, "outTime")
ops.expRecorder("Setup", "-file", "Setup_ctrlDsp.out", "-time", "-setup", 1, 2, "ctrlDisp")
ops.expRecorder("Setup", "-file", "Setup_ctrlVel.out", "-time", "-setup", 1, 2, "ctrlVel")
ops.expRecorder("Setup", "-file", "Setup_ctrlAcc.out", "-time", "-setup", 1, 2, "ctrlAccel")
ops.expRecorder("Setup", "-file", "Setup_ctrlTme.out", "-time", "-setup", 1, 2, "ctrlTime")
ops.expRecorder("Setup", "-file", "Setup_daqDsp.out", "-time", "-setup", 1, 2, "daqDisp")
ops.expRecorder("Setup", "-file", "Setup_daqVel.out", "-time", "-setup", 1, 2, "daqVel")
ops.expRecorder("Setup", "-file", "Setup_daqAcc.out", "-time", "-setup", 1, 2, "daqAccel")
ops.expRecorder("Setup", "-file", "Setup_daqFrc.out", "-time", "-setup", 1, 2, "daqForce")
ops.expRecorder("Setup", "-file", "Setup_daqTme.out", "-time", "-setup", 1, 2, "daqTime")

ops.expRecorder("Control", "-file", "Control_ctrlDsp.out", "-time", "-control", 1, 2, "ctrlDisp")
ops.expRecorder("Control", "-file", "Control_ctrlVel.out", "-time", "-control", 1, 2, "ctrlVel")
ops.expRecorder("Control", "-file", "Control_daqDsp.out", "-time", "-control", 1, 2, "daqDisp")
ops.expRecorder("Control", "-file", "Control_daqVel.out", "-time", "-control", 1, 2, "daqVel")
ops.expRecorder("Control", "-file", "Control_daqFrc.out", "-time", "-control", 1, 2, "daqForce")
# --------------------------------
# End of recorder generation
# --------------------------------


# ------------------------------
# Finally perform the analysis
# ------------------------------
ops.start()
ops.record()

# perform an eigenvalue analysis
lam = ops.eigen("-fullGenLapack", 2)
print("\nEigenvalues at start of transient:")
print('{:>6}{:>12}{:>10}{:>10}{:>10}'.format('mode', 'lambda', 'omega', 'period', 'frequ'))

for i in range(2):
    omega = math.sqrt(lam[i])
    period = 2.0*math.pi/omega
    frequ = 1.0/period
    print('{:>6}{:>12.3e}{:>10.4f}{:>10.4f}{:>10.4f}'.format(i+1, lam[i], omega, period, frequ))

# perform the transient analysis
dtAna = 20.0/1024.0
for i in range(1790):
    ops.analyze(1, dtAna)

print('')
ops.stop()
#print("\nElapsed Time = $tTot \n")

ops.wipeExp()
ops.wipe()
exit()
# --------------------------------
# End of analysis
# --------------------------------
