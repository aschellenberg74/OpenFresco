# File: PortalFrame_Local.py
# Units: [kip,in.]
#
# Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
# Created: 10/07
# Revision: A
#
# Purpose: this file contains the tcl input to perform
# a local hybrid simulation of a portal frame with
# two experimental beamColumn elements.
# The frame can be analyzed with or without gravity loads.
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
ops.logFile("PortalFrame_Local.log")
ops.defaultUnits("-force", "kip", "-length", "in", "-time", "sec", "-temp", "F")

# create ModelBuilder (with two-dimensions and 3 DOF/node)
ops.model("BasicBuilder", "-ndm", 2, "-ndf", 3)

# Load OpenFresco package
# -----------------------
# (make sure all dlls are in the same folder as OpenFrescoPy)
ops.loadPackage("OpenFrescoPy")

# Define geometry for model
# -------------------------
withGravity = 1;
Pc = 10.638;
P = 0.5*Pc;
mass3 = P/386.1;
mass4 = P/386.1;
# node(tag, xCrd, yCrd, <"-mass", mass>)
ops.node(1,   0.0,  0.00)
ops.node(2, 100.0,  0.00)
ops.node(3,   0.0, 50.00, "-mass", mass3, mass3, 0.0)
ops.node(4, 100.0, 50.00, "-mass", mass4, mass4, 0.0)

# set the boundary conditions
# fix(tag, DX, DY, RZ)
ops.fix(1, 1, 1, 0)
ops.fix(2, 1, 1, 0)

# Define materials
# ----------------
# uniaxialMaterial("Steel02", matTag, Fy, E, b, R0, cR1, cR2, a1, a2, a3, a4) 
ops.uniaxialMaterial("Steel02", 1, 1.5, 2.8, 0.01, 18.5, 0.925, 0.15, 0.0, 1.0, 0.0, 1.0)
#ops.uniaxialMaterial("Elastic", 1, 2.8)

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
ops.expControl("SimUniaxialMaterials", 2, 1)

# Define experimental setup
# -------------------------
# expSetup("OneActuator", tag, <"-control", ctrlTag,> dir, "-sizeTrialOut", t, o, <"-trialDispFact", f,> ...)
ops.expSetup("OneActuator", 1, "-control", 1, 2, "-sizeTrialOut", 3, 3)
ops.expSetup("OneActuator", 2, "-control", 2, 2, "-sizeTrialOut", 3, 3)

# Define experimental site
# ------------------------
# expSite("LocalSite", tag, setupTag)
ops.expSite("LocalSite", 1, 1)
ops.expSite("LocalSite", 2, 2)

# Define geometric transformation
# -------------------------------
#ops.geomTransf("PDelta", 1)
ops.geomTransf("Corotational", 1)

# Define experimental elements
# ----------------------------
# left and right columns
# expElement("beamColumn", eleTag, iNode, jNode, transTag, "-site", siteTag, "-initStif", Kij, <"-iMod",> <"-rho", rho>)
ops.expElement("beamColumn", 1, 3, 1, 1, "-site", 1, "-initStif", 1310.8, 0, 0, 0, 11.2, -280.0, 0, -280.0, 9333.3333)
ops.expElement("beamColumn", 2, 4, 2, 1, "-site", 2, "-initStif", 1310.8, 0, 0, 0, 11.2, -280.0, 0, -280.0, 9333.3333)

# Define numerical elements
# -------------------------
# element("elasticBeamColumn", eleTag, iNode, jNode, A, E, Iz, transfTag)
ops.element("elasticBeamColumn", 3, 3, 4, 3.55, 29000, 22.1, 1)

if (withGravity == 1):
    # Define gravity loads
    # --------------------
    # Create a Plain load pattern with a Linear TimeSeries
    ops.timeSeries('Linear', 1)
    ops.pattern("Plain", 1, 1)
    # Create nodal loads at nodes 2
    #    nd    FX          FY  MZ 
    ops.load(3, 0.0, -P, 0.0)
    ops.load(4, 0.0, -P, 0.0)
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
    ops.integrator("LoadControl", 0.1)
    # create the solution algorithm
    ops.algorithm("NewtonLineSearch")
    # create the analysis object 
    ops.analysis("Static")
    # ------------------------------
    # End of analysis generation
    # ------------------------------


    # ------------------------------
    # Start of recorder generation
    # ------------------------------
    # create a Recorder object for the nodal displacements at node 2
    ops.recorder("Node", "-file", "Gravity_Node_Dsp.out", "-time", "-node", 3, 4, "-dof", 1, 2, 3, "disp")
    ops.recorder("Element", "-file", "Gravity_Elmt_glbFrc.out", "-time", "-ele", 1, 2, 3, "force")
    ops.recorder("Element", "-file", "Gravity_Elmt_ctrlDsp.out", "-time", "-ele", 1, 2, "ctrlDisp")
    ops.recorder("Element", "-file", "Gravity_Elmt_daqDsp.out", "-time", "-ele", 1, 2, "daqDisp")
    ops.expRecorder("Control", "-file", "Gravity_Ctrl_ctrlDsp.out", "-time", "-control", 1, 2, "ctrlDisp")
    ops.expRecorder("Control", "-file", "Gravity_Ctrl_daqDsp.out", "-time", "-control", 1, 2, "daqDisp")
    ops.expRecorder("Control", "-file", "Gravity_Ctrl_daqFrc.out", "-time", "-control", 1, 2, "daqForce")
    # --------------------------------
    # End of recorder generation
    # --------------------------------


    # ------------------------------
    # Perform the gravity analysis
    # ------------------------------
    # perform the gravity load analysis, requires 10 steps to reach the load level
    ops.record()
    if (ops.analyze(10) == 0):
        print("\nGravity load analysis completed")
    else:
        print("\nGravity load analysis failed")
        exit(-1)


    # ------------------------------
    # Start of model generation
    # ------------------------------
    # Set the gravity loads to be constant & reset the time in the domain
    ops.loadConst("-time", 0.0)
    ops.remove("recorders")
    ops.removeExp("recorders")

# Define dynamic loads
# --------------------
# set time series to be passed to uniform excitation
dt = 0.01
scale = 1.2
ops.timeSeries("Path", 2, "-filePath", "SACNF01.txt", "-dt", dt, "-factor", 32.174*12*scale)

# create UniformExcitation load pattern
# ops.pattern("UniformExcitation", tag, dir, "-accel", tsTag, <"-vel0", vel0>)
ops.pattern("UniformExcitation", 2, 1, "-accel", 2)

# calculate the Rayleigh damping factors for nodes & elements
alphaM    = 1.2797;    # D = alphaM*M
betaK     = 0.0;       # D = betaK*Kcurrent
betaKinit = 0.0;       # D = beatKinit*Kinit
betaKcomm = 0.0;       # D = betaKcomm*KlastCommit

# set the Rayleigh damping 
ops.rayleigh(alphaM, betaK, betaKinit, betaKcomm)
# ------------------------------
# End of model generation
# ------------------------------


# ------------------------------
# Start of analysis generation
# ------------------------------
ops.wipeAnalysis()
# create the system of equations
ops.system("BandGeneral")
# create the DOF numberer
ops.numberer("Plain")
# create the constraint handler
ops.constraints("Plain")
# create the convergence test
ops.test("FixedNumIter", 5)
#ops.test("NormDispIncr", 1E-8, 25)
# create the integration scheme
ops.integrator("NewmarkHSFixedNumIter", 0.5, 0.25)
#ops.integrator("HHTHSFixedNumIter", 0.5)
#ops.integrator("CollocationHSFixedNumIter", 1.5)
#ops.integrator("AlphaOS", 0.9)
# create the solution algorithm
ops.algorithm("NewtonLineSearch")
#ops.algorithm("Linear")
# create the analysis object 
ops.analysis("Transient")
# ------------------------------
# End of analysis generation
# ------------------------------


# ------------------------------
# Start of recorder generation
# ------------------------------
# create the recorder objects
ops.recorder("Node", "-file", "Node_Dsp.out", "-time", "-node",       3, 4, "-dof", 1, 2, 3, "disp")
ops.recorder("Node", "-file", "Node_Vel.out", "-time", "-node",       3, 4, "-dof", 1, 2, 3, "vel")
ops.recorder("Node", "-file", "Node_Acc.out", "-time", "-node",       3, 4, "-dof", 1, 2, 3, "accel")
ops.recorder("Node", "-file", "Node_Rxn.out", "-time", "-node", 1, 2, 3, 4, "-dof", 1, 2, 3, "reactionIncludingInertia")

ops.recorder("Element", "-file", "Elmt_glbFrc.out", "-time", "-ele", 1, 2, 3, "forces")
ops.recorder("Element", "-file", "Elmt_ctrlDsp.out", "-time", "-ele", 1, 2, "ctrlDisp")
ops.recorder("Element", "-file", "Elmt_daqDsp.out", "-time", "-ele", 1, 2, "daqDisp")
ops.expRecorder("Control", "-file", "Ctrl_ctrlDsp.out", "-time", "-control", 1, 2, "ctrlDisp")
ops.expRecorder("Control", "-file", "Ctrl_daqDsp.out", "-time", "-control", 1, 2, "daqDisp")
ops.expRecorder("Control", "-file", "Ctrl_daqFrc.out", "-time", "-control", 1, 2, "daqForce")
# --------------------------------
# End of recorder generation
# --------------------------------


# ------------------------------
# Finally perform the analysis
# ------------------------------
ops.start()
ops.record()

# perform an eigenvalue analysis
lam = ops.eigen("-fullGenLapack", 4)
print("\nEigenvalues at start of transient:")
print('{:>6}{:>12}{:>10}{:>10}{:>10}'.format('mode', 'lambda', 'omega', 'period', 'frequ'))

for i in range(4):
    omega = math.sqrt(lam[i])
    period = 2.0*math.pi/omega
    frequ = 1.0/period
    print('{:>6}{:>12.3e}{:>10.4f}{:>10.4f}{:>10.4f}'.format(i+1, lam[i], omega, period, frequ))

# perform the transient analysis
dtAna = dt
for i in range(2500):
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
