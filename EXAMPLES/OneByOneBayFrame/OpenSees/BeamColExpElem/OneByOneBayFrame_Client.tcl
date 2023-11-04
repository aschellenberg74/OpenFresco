# File: OneByOneBayFrame_Client.tcl (use with OneByOneBayFrame_SimAppServer.tcl)
# Units: [kN,mm]

# Start timing of this analysis sequence 
# -------------------------------------- 
set tStart [clock clicks -milliseconds] 

# ------------------------------
# Start of model generation
# ------------------------------
logFile "OneByOneBayFrame_Client.log" 
defaultUnits -force kN -length mm -time sec -temp C
set systemTime [clock seconds] 

# create ModelBuilder (with three-dimensions and 6 DOF/node)
model BasicBuilder -ndm 3 -ndf 6

# Load OpenFresco package
# -----------------------
# (make sure all dlls are in the same folder as openSees.exe)
loadPackage OpenFresco

# Define geometry for model
# -------------------------
# node    tag    xCrd    yCrd    zCrd    ndf 
node       1  +0.000000E+00  +0.000000E+00  +0.000000E+00  -ndf 6 
node       2  +0.000000E+00  +0.000000E+00  +1.000000E+03  -ndf 6 
node       3  +1.000000E+03  +0.000000E+00  +0.000000E+00  -ndf 6 
node       4  +1.000000E+03  +0.000000E+00  +1.000000E+03  -ndf 6 
node       5  +0.000000E+00  +0.000000E+00  +1.000000E+03  -ndf 6 
node       6  +1.000000E+03  +0.000000E+00  +5.000000E+02  -ndf 6 
node       7  +5.000000E+02  +0.000000E+00  +1.000000E+03  -ndf 6 
node       8  +0.000000E+00  +1.000000E+03  +0.000000E+00  -ndf 6 
node       9  +0.000000E+00  +1.000000E+03  +1.000000E+03  -ndf 6 
node      10  +1.000000E+03  +1.000000E+03  +0.000000E+00  -ndf 6 
node      11  +1.000000E+03  +1.000000E+03  +1.000000E+03  -ndf 6 
node      12  +0.000000E+00  +1.000000E+03  +5.000000E+02  -ndf 6 
node      13  +1.000000E+03  +1.000000E+03  +5.000000E+02  -ndf 6 
node      14  +5.000000E+02  +1.000000E+03  +1.000000E+03  -ndf 6 
node      15  +0.000000E+00  +5.000000E+02  +1.000000E+03  -ndf 6 
node      16  +1.000000E+03  +5.000000E+02  +1.000000E+03  -ndf 6 

# Define single point constraints
# -------------------------------
# fix    tag    Dx    Dy    Dz    Rx    Ry    Rz 
fix       1     1     1     1     1     1     1 
fix       3     1     1     1     1     1     1 
fix       8     1     1     1     1     1     1 
fix      10     1     1     1     1     1     1 

# Define nodal masses
# -------------------
# mass    tag    mx    my    mz    mIx    mIy    mIz 
mass       2  +2.000000E-04  +2.000000E-04  +2.000000E-04  +0.000000E+00  +0.000000E+00  +0.000000E+00 
mass       4  +2.000000E-04  +2.000000E-04  +2.000000E-04  +0.000000E+00  +0.000000E+00  +0.000000E+00 
mass       6  +2.000000E-04  +2.000000E-04  +2.000000E-04  +0.000000E+00  +0.000000E+00  +0.000000E+00 
mass       7  +2.000000E-04  +2.000000E-04  +2.000000E-04  +0.000000E+00  +0.000000E+00  +0.000000E+00 
mass       9  +2.000000E-04  +2.000000E-04  +2.000000E-04  +0.000000E+00  +0.000000E+00  +0.000000E+00 
mass      11  +2.000000E-04  +2.000000E-04  +2.000000E-04  +0.000000E+00  +0.000000E+00  +0.000000E+00 
mass      12  +2.000000E-04  +2.000000E-04  +2.000000E-04  +0.000000E+00  +0.000000E+00  +0.000000E+00 
mass      13  +2.000000E-04  +2.000000E-04  +2.000000E-04  +0.000000E+00  +0.000000E+00  +0.000000E+00 
mass      14  +2.000000E-04  +2.000000E-04  +2.000000E-04  +0.000000E+00  +0.000000E+00  +0.000000E+00 
mass      15  +2.000000E-04  +2.000000E-04  +2.000000E-04  +0.000000E+00  +0.000000E+00  +0.000000E+00 
mass      16  +2.000000E-04  +2.000000E-04  +2.000000E-04  +0.000000E+00  +0.000000E+00  +0.000000E+00 

# Define multi point constraints
# ------------------------------
# equalDOF    mNodeTag    sNodeTag    dof 
equalDOF       2       5  1  2  3  6 

# Define materials
# ----------------
# uniaxialMaterial  Elastic  matTag    E    <eta>    <Eneg>  
uniaxialMaterial  Elastic       1  +4.800000E-02  +0.000000E+00;  # shear
uniaxialMaterial  Elastic       2  +4.800000E-02  +0.000000E+00;  # shear

# Define sections
# ---------------
# section Elastic $secTag $E $A $Iz $Iy $G $J <$alphaY> <$alphaZ> 
section  Elastic       1  +2.000000E+02  +5.000000E+02  +2.000000E+04  +2.000000E+04  +8.000000E+01  +4.000000E+04  +8.333333E-01  +8.333333E-01 

# Define geometric transformations
# --------------------------------
# geomTransf    type    tag    vec_xz 
geomTransf  Linear       1  +1.000000E+00  +0.000000E+00  +0.000000E+00 
geomTransf  Linear       2  +0.000000E+00  +1.000000E+00  +0.000000E+00 
geomTransf  Linear       3  +0.000000E+00  -1.000000E+00  +0.000000E+00 
geomTransf  Linear       4  +0.000000E+00  -1.000000E+00  +0.000000E+00 
geomTransf  Linear       5  +0.000000E+00  -1.000000E+00  +0.000000E+00 
geomTransf  Linear       6  +0.000000E+00  -1.000000E+00  +0.000000E+00 

# Define elements
# ---------------
puts "o Using 'BeamColumn' Experimental Element\n"
# expElement  beamColumn    eleTag    NodeI    NodeJ    GeoTranTag    -site siteTag    -initStif Kij    <-iMod>    <-noRayleigh>    <-rho rho> 
expElement  beamColumn       1       1       5     2  -server 8090  -initStif +1.000000E+02 +0.000000E+00 +0.000000E+00 +0.000000E+00 +0.000000E+00 +0.000000E+00 +0.000000E+00 +4.800000E-02 -2.400000E+01 +0.000000E+00 +0.000000E+00 +0.000000E+00 +0.000000E+00 -2.400000E+01 +1.600000E+04 +0.000000E+00 +0.000000E+00 +0.000000E+00 +0.000000E+00 +0.000000E+00 +0.000000E+00 +4.800000E-02 +2.400000E+01 +0.000000E+00 +0.000000E+00 +0.000000E+00 +0.000000E+00 +2.400000E+01 +1.600000E+04 +0.000000E+00 +0.000000E+00 +0.000000E+00 +0.000000E+00 +0.000000E+00 +0.000000E+00 +3.200000E+03

# element  dispBeamColumn    eleTag    NodeI    NodeJ    NIP    secTag    geoTranTag    <-mass massDens>    <-cMass> 
element  dispBeamColumn       3       3       6     5     1     2  
element  dispBeamColumn       4       6       4     5     1     2  
element  dispBeamColumn       5       2       7     5     1     3  
element  dispBeamColumn       6       7       4     5     1     4  
element  dispBeamColumn       7       8      12     5     1     2  
element  dispBeamColumn       8      12       9     5     1     2  
element  dispBeamColumn       9      10      13     5     1     2  
element  dispBeamColumn      10      13      11     5     1     2  
element  dispBeamColumn      11       9      14     5     1     5  
element  dispBeamColumn      12      14      11     5     1     6  
element  dispBeamColumn      13       2      15     5     1     1  
element  dispBeamColumn      14      15       9     5     1     1  
element  dispBeamColumn      15       4      16     5     1     1  
element  dispBeamColumn      16      16      11     5     1     1  

# Define time series
# ------------------
timeSeries  Linear     1  -factor +1.000000E-01 
timeSeries  Path       2  -time {0   5  10}  -values {0  1  1}  -factor +1.000000E+00 
timeSeries  Path       3  -dt +2.000000E-02  -filePath  x.thf  -factor  +9.806000E+03  -prependZero 
timeSeries  Path       4  -dt +2.000000E-02  -filePath  y.thf  -factor  +9.806000E+03  -prependZero 

# Start of analysis generation
# ============================

# Get Initial Stiffness
# ---------------------
initialize 

puts "o Running Analysis" 
# ~~~~~~~~~~~~~~~~~~~~~~

# Define load pattern
# -------------------
pattern  UniformExcitation       1     1  -accel      3  -fact +1.000000E-02  -vel0 +0.000000E+00 
pattern  UniformExcitation       2     2  -accel      4  -fact +1.000000E-02  -vel0 +0.000000E+00 

# Define recorders
# ----------------
recorder  Node  -file Node_Dsp.out  -time  -nodeRange 1  16  -dof 1  2  3  4  5  6  disp 
recorder  Node  -file Node_Vel.out  -time  -nodeRange 1  16  -dof 1  2  3  4  5  6  vel 
recorder  Node  -file Node_Acc.out  -time  -nodeRange 1  16  -dof 1  2  3  4  5  6  accel 
recorder  Node  -file Node_RFrc.out -time  -nodeRange 1  16  -dof 1  2  3  4  5  6  reaction 

recorder  Element  -file Elmt_GlbForce.out  -time  -ele 1   3   4   5   6   7   8   9  10  11  12  13  14  15  16  force 
recorder  Element  -file Elmt_LocForce.out  -time  -ele 1   3   4   5   6   7   8   9  10  11  12  13  14  15  16  localForce 

# Define analysis options
# -----------------------
# Constraint Handler 
constraints  Plain 
# DOF Numberer 
numberer  Plain 
# System of Equations 
system  UmfPack -lvalueFact   10 
# Convergence Test 
test  NormDispIncr  +1.000000E-06    25     0     2 
# Solution Algorithm 
algorithm  Linear 
# Integrator 
integrator  AlphaOSGeneralized  0.5 
# Analysis Type 
analysis  Transient 

# Define damping parameters
# -------------------------
# parameter set "DampingParam01":    alphaM    betaK    betaKinit    betaKcomm 
rayleigh +5.483900E-01 +0.000000E+00 +0.000000E+00 +4.384400E-03

# Record initial state of model
# -----------------------------
record 

# Analyze model
# -------------
for {set i 1} {$i <= 4000} {incr i} {
    analyze  1  0.01
}

# Stop timing of this analysis sequence
# -------------------------------------
set tStop [clock clicks -milliseconds]
puts ""
puts "o Time taken: [expr ($tStop-$tStart)/1000.0] sec" 

# Clean up
# --------
wipeExp
wipe 
exit 
