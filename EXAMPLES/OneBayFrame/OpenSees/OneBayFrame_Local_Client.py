# File: OneBayFrame_Local_Client.py (use with OneBayFrame_Local_SimAppServer.tcl)
# Units: [kip,in.]
#
# Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
# Created: 11/06
# Revision: A
#
# Purpose: this file contains the python input to perform
# a local hybrid simulation of a one bay frame with
# two experimental twoNodeLink elements.
# The specimens are simulated using the SimUniaxialMaterials
# controller.

# import the OpenSees Python module
import sys
sys.path.append("C:\\Users\\Andreas\\Documents\\OpenSees\\SourceCode\\Win64\\bin")
from opensees import *
sys.path.append("C:\\Users\\Andreas\\Documents\\OpenFresco\\SourceCode\\WIN64\\bin")
#from openfresco import *
import math

# ------------------------------
# Start of model generation
# ------------------------------
logFile("OneBayFrame_Local_Client.log")
defaultUnits("-force", "kip", "-length", "in", "-time", "sec", "-temp", "F")

# create ModelBuilder (with two-dimensions and 2 DOF/node)
model("BasicBuilder", "-ndm", 2, "-ndf", 2)

# Define geometry for model
# -------------------------
mass3 = 0.04
mass4 = 0.02
# node(tag, xCrd, yCrd, "-mass", mass)
node(1,   0.0,  0.00)
node(2, 100.0,  0.00)
node(3,   0.0, 54.00, "-mass", mass3, mass3)
node(4, 100.0, 54.00, "-mass", mass4, mass4)

# set the boundary conditions
# fix(tag, DX, DY)
fix(1, 1, 1)
fix(2, 1, 1)
fix(3, 0, 1)
fix(4, 0, 1)

# Define materials
# ----------------
# uniaxialMaterial("Steel02", matTag, Fy, E, b, R0, cR1, cR2, a1, a2, a3, a4) 
#uniaxialMaterial("Elastic", 1, 2.8)
uniaxialMaterial("Steel02", 1, 1.5, 2.8, 0.01, 18.5, 0.925, 0.15, 0.0, 1.0, 0.0, 1.0)
uniaxialMaterial("Elastic", 2, 5.6)
#uniaxialMaterial("Steel02", 2, 3.0, 5.6, 0.01, 18.5, 0.925, 0.15, 0.0, 1.0, 0.0, 1.0) 
uniaxialMaterial("Elastic", 3, 2.0*100.0/1.0)

# Define experimental control
# ---------------------------
# expControl SimUniaxialMaterials $tag $matTags
#expControl SimUniaxialMaterials 2 2

# Define experimental setup
# -------------------------
# expSetup OneActuator $tag <-control $ctrlTag> $dir -sizeTrialOut $t $o <-trialDispFact $f> ...
#expSetup OneActuator 2 -control 2 1 -sizeTrialOut 1 1

# Define experimental site
# ------------------------
# expSite LocalSite $tag $setupTag
#expSite LocalSite 2 2

# Define experimental elements
# ----------------------------
# left column
# element("genericClient", eleTag, "-node", Ndi, Ndj, ..., "-dof", dofNdi, "-dof", dofNdj, ..., "-server", ipPort, ipAddr, "-ssl", "-udp", "-dataSize", size)
element("genericClient", 1, "-node", 1, 3, "-dof", 1, 2, "-dof", 1, 2, "-server", 8090, "-udp");  # use with SimAppElemServer

# expElement twoNodeLink $eleTag $iNode $jNode -dir $dirs -server $ipPort <ipAddr> <-ssl> <-udp> <-dataSize $size> -initStif $Kij <-orient <$x1 $x2 $x3> $y1 $y2 $y3> <-pDelta Mratios> <-iMod> <-mass $m>
#expElement("twoNodeLink", 1, 1, 3, "-dir", 2, "-server", 8090, "-udp", "-initStif", 2.8);  # use with SimAppSiteServer

# Define numerical elements
# -------------------------
# left and right columns
# element("twoNodeLink", eleTag, iNode, jNode, "-mat", matTags, "-dir", dirs, "-orient", x1, x2, x3, y1, y2, y3, "-pDelta", Mratios, "-mass", m)
element("twoNodeLink", 2, 2, 4, "-mat", 2, "-dir", 2)

# spring
# element("truss", eleTag, iNode, jNode, A, matTag)
element("truss", 3, 3, 4, 1.0, 3)

# Define dynamic loads
# --------------------
# set time series to be passed to uniform excitation
dt = 0.02
scale = 1.0
timeSeries("Path", 1, "-filePath", "elcentro.txt", "-dt", dt, "-factor", 386.1*scale)

# create UniformExcitation load pattern
# pattern("UniformExcitation", tag, dir, "-accel", tsTag, "-vel0", v0)
pattern("UniformExcitation", 1, 1, "-accel", 1)

# calculate the Rayleigh damping factors for nodes & elements
alphaM    = 1.010017396536;  # D = alphaM*M
betaK     = 0.0;             # D = betaK*Kcurrent
betaKinit = 0.0;             # D = beatKinit*Kinit
betaKcomm = 0.0;             # D = betaKcomm*KlastCommit

# set the Rayleigh damping 
rayleigh(alphaM, betaK, betaKinit, betaKcomm)
# ------------------------------
# End of model generation
# ------------------------------


# ------------------------------
# Start of analysis generation
# ------------------------------
# create the system of equations
system("BandGeneral")
# create the DOF numberer
numberer("Plain")
# create the constraint handler
constraints("Plain")
# create the convergence test
test("EnergyIncr", 1.0e-6, 10)
# create the integration scheme
integrator("NewmarkExplicit", 0.5)
#integrator("AlphaOS", 1.0)
# create the solution algorithm
algorithm("Linear")
# create the analysis object 
analysis("Transient")
# ------------------------------
# End of analysis generation
# ------------------------------


# ------------------------------
# Start of recorder generation
# ------------------------------
# create the recorder objects
recorder("Node", "-file", "Node_Dsp.out", "-time", "-node", 3, 4, "-dof", 1, "disp")
recorder("Node", "-file", "Node_Vel.out", "-time", "-node", 3, 4, "-dof", 1, "vel")
recorder("Node", "-file", "Node_Acc.out", "-time", "-node", 3, 4, "-dof", 1, "accel")

recorder("Element", "-file", "Elmt_Frc.out", "-time", "-ele", 1, 2, 3, "forces")
recorder("Element", "-file", "Elmt_ctrlDsp.out", "-time", "-ele", 1, "ctrlDisp")
recorder("Element", "-file", "Elmt_daqDsp.out", "-time", "-ele", 1, "daqDisp")
# --------------------------------
# End of recorder generation
# --------------------------------


# ------------------------------
# Finally perform the analysis
# ------------------------------
start()
record()

# perform an eigenvalue analysis
lam = eigen("-fullGenLapack", 2)
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
    analyze(1, dtAna)

print('')
stop()
#print("\nElapsed Time = $tTot \n")

#wipeExp()
wipe()
exit()
# --------------------------------
# End of analysis
# --------------------------------
