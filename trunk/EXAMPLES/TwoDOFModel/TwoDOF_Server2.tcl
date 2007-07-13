# File: TwoDOF_Server2.tcl (use with TwoDOF_Client2.tcl)
#
# $Revision: $
# $Date: $
# $URL: $
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
# The experimental setup is on the client side.


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

# Define experimental site
# ------------------------
# expSite ActorSite $tag -control $ctrlTag $ipPort <-ssl>
expSite ActorSite 1 -control 1 8090
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
