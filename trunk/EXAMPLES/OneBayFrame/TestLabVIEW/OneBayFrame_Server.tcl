# File: OneBayFrame_Server.tcl (use with OneBayFrame_Client.tcl)
#
# $Revision$
# $Date$
# $URL$
#
# Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
# Created: 03/07
# Revision: A
#
# Purpose: this file contains the tcl input to perform
# a distributed hybrid simulation of a one bay frame
# with two experimental twoNodeLink elements.
# The specimen is simulated using the LabVIEW
# controller.
# The experimental setup is on the server side.


# ------------------------------
# Start of model generation
# ------------------------------
# create ModelBuilder (with two-dimensions and 2 DOF/node)
model BasicBuilder -ndm 2 -ndf 2

# Define materials
# ----------------
# uniaxialMaterial Steel02 $matTag $Fy $E $b $R0 $cR1 $cR2 $a1 $a2 $a3 $a4 
uniaxialMaterial Elastic 1 2.8655
#uniaxialMaterial Steel02 1 1.5 2.8 0.01 18.5 0.925 0.15 0.0 1.0 0.0 1.0

# Define control points
# ---------------------
# expControlPoint tag nodeTag dir resp <-fact f> <-lim l u> ...
expControlPoint 1 1  ux disp -fact 0.003 -lim -0.01 0.01
expControlPoint 2 1  ux disp -fact [expr 1.0/0.003] ux force -fact [expr 7.0/18.0]

# Define experimental control
# ---------------------------
# expControl LabVIEW tag ipAddr <ipPort> -trialCP cpTags -outCP cpTags
expControl LabVIEW 1 "127.0.0.1" 11997  -trialCP 1  -outCP 2;  # use with NEES-SAM
#expControl LabVIEW 1 "130.126.242.175" 44000  -trialCP 1  -outCP 2;  # use with Mini-Most at UIUC
#expControl SimUniaxialMaterials 1 1;  # use for simulation

# Define experimental setup
# -------------------------
# expSetup OneActuator $tag <-control $ctrlTag> $dir -sizeTrialOut $t $o <-trialDispFact $f> ...
expSetup OneActuator 1 -control 1 1 -sizeTrialOut 1 1

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
