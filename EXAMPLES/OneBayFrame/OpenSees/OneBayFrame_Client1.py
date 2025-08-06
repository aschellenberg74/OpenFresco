# File: OneBayFrame_Client1.py
# (use with OneBayFrame_Server1a.py & OneBayFrame_Server1b.py)
# Units: [kip,in.]
#
# Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
# Created: 06/21
# Revision: A
#
# Purpose: this file contains the python input to perform
# a distributed hybrid simulation of a one bay frame with
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
ops.logFile("OneBayFrame_Client1.log")
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
ops.uniaxialMaterial("Elastic", 3, 2.0*100.0/1.0)

# Define experimental site
# ------------------------
# expSite("ShadowSite", tag, <"-setup", setupTag,> ipAddr, ipPort, <"-ssl",> <"-udp",> <"-dataSize", size>)
ops.expSite("ShadowSite", 1, "127.0.0.1", 8090)
ops.expSite("ShadowSite", 2, "127.0.0.1", 8091)

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
ops.recorder("Element", "-file", "Elmt_ctrlDsp.out", "-time", "-ele", 1, "ctrlDisp")
ops.recorder("Element", "-file", "Elmt_daqDsp.out", "-time", "-ele", 1, "daqDisp")
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
