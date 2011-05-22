# File: TwoDOF_Server1.tcl (use with TwoDOF_Client1.tcl)
#
# $Revision$
# $Date$
# $URL$
#
# Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
# Created: 09/06
# Revision: A
#
# Purpose: this file contains the tcl input to perform
# a distributed hybrid simulation of a two degree of freedom
# system with one experimental zero length element.
# The specimen is simulated using the SimUniaxialMaterials
# controller.
# The experimental setup is on the server side.


# ------------------------------
# Start of model generation
# ------------------------------
# create ModelBuilder (with two-dimensions and 3 DOF/node)
model BasicBuilder -ndm 2 -ndf 3

# Define materials
# ----------------
set E1 35.E6;    # [N/m], Stiffness of the column
set E2 50.E6;    # [N/m], Stiffness of two isolators
set Fy2 250.E3;  # [N], Yield strength of two isolators

# Define similitude
set S 0.5;                        # ratio of length from prototype to specimen
set factNtoTonf [expr 1./9.8E3];  # from [N] to [tonf]
set factMtoMM 1000.;              # from [m] to [mm]

set nIso 2;  # number of isolators

set Ee [expr $E2*$factNtoTonf/$factMtoMM*$S/$nIso];  # [tonf/mm], Stiffness of one isolator in test setup unit
set Fye [expr $Fy2*$factNtoTonf*$S*$S/$nIso];        # [tonf], Yield strength of one isolator in test setup unit

# uniaxialMaterial Steel01 $matTag $Fy $E $b
uniaxialMaterial Elastic 1 $E1
uniaxialMaterial Steel01 2 $Fye $Ee 0.1

# Define experimental control
# ---------------------------
# expControl SimUniaxialMaterials $tag $matTags
expControl SimUniaxialMaterials 1 2

# Define experimental setup
# -------------------------
# expSetup OneActuator $tag <-control $ctrlTag> $dir -sizeTrialOut $t $o <-trialDispFact $f> ...
expSetup OneActuator 1 -control 1 1 -sizeTrialOut 1 1 -trialDispFact [expr $S*$factMtoMM] -outDispFact [expr 1.0/($S*$factMtoMM)] -outForceFact [expr 1.0/($S*$S*$factNtoTonf/$nIso)]

# Define experimental site
# ------------------------
# expSite ActorSite $tag -setup $setupTag $ipPort <-ssl>
expSite ActorSite 1 -setup 1 8090
# ------------------------------
# End of model generation
# ------------------------------


# ------------------------------
# Start the server process
# ------------------------------
# startLabServer $siteTag
startLabServer  1
# --------------------------------
# End of analysis
# --------------------------------
