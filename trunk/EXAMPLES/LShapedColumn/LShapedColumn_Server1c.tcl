# File: LShapedColumn_Server1c.tcl (use with LShapedColumn_Client1.tcl)
# Units: [kip,in.]
#
# $Revision$
# $Date$
# $URL$
#
# Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
# Created: 07/07
# Revision: A
#
# Purpose: this file contains the tcl input to perform
# a distributed hybrid simulation of a L-shaped column
# with one experimental beamColumn element.
# The NoTransformation experimental setup is used.


# ------------------------------
# Start of model generation
# ------------------------------
logFile "LShapedColumn_Server1c.log"
defaultUnits -force kip -length in -time sec -temp F

# create ModelBuilder (with three-dimensions and 6 DOF/node)
model BasicBuilder -ndm 3 -ndf 6

# Define control points
# ---------------------
# expControlPoint tag nodeTag dir resp <-fact f> <-lim l u> ...
expControlPoint 1 2  ux disp uz disp ry disp
expControlPoint 2 2  ux disp ux force uz disp uz force ry disp ry force

# Define experimental control
# ---------------------------
# expControl LabVIEW tag ipAddr <ipPort> -trialCP cpTags -outCP cpTags
expControl LabVIEW 1 "127.0.0.1" 11997  -trialCP 1  -outCP 2;  # use with NEES-SAM

# Define experimental setup
# -------------------------
# expSetup NoTransformation $tag <�control $ctrlTag> �dir $dirs -sizeTrialOut $t $o <�trialDispFact $f> ...
expSetup NoTransformation 1 -control 1 -dir 2 1 3 -sizeTrialOut 3 3 -trialDispFact 1 -1 1 -outDispFact 1 -1 1 -outForceFact 1 -1 1

# Define experimental site
# ------------------------
# expSite ActorSite $tag -setup $setupTag $ipPort <-ssl> <-udp>
expSite ActorSite 1 -setup 1 8090
# ------------------------------
# End of model generation
# ------------------------------


# ------------------------------
# Start the server process
# ------------------------------
# startLabServer $siteTag
startLabServer  1

wipeExp
exit
# --------------------------------
# End of analysis
# --------------------------------
