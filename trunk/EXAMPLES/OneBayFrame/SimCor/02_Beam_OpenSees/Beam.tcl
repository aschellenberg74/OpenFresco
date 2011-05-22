# File: Beam.tcl
#
# $Revision$
# $Date$
# $URL$
#
# Written: Hong Kim (hongkim@berkeley.edu)
# Created: 1/08
# Revision: A
#
# Purpose: this file contains the tcl input to perform
# a distributed hybrid simulation with SimCor of a one bay frame
# example with two experimental two-node link elements.
# The beam is modeled in OpenSees

# _____________________________________________________________________________
#
# Start of model generation
# _____________________________________________________________________________

# Create ModelBuilder (with three-dimensions and 6 DOF/node)
model BasicBuilder -ndm 2

# --------------------------------------------------------------------
#    Create nodes
# --------------------------------------------------------------------
# Node 3 and 4 are connected with spring with large stiffness.

#    tag        X         Y      
node  1       0.0       0.0      
node  2     100.0       0.0      

# Controlling node

# --------------------------------------------------------------------
#     Boundary condition
# --------------------------------------------------------------------
# Fix supports at ends of truss
#    tag   DX   DY   RZ 
fix   1    0    1    0 
fix   2    0    1    0 

# --------------------------------------------------------------------
#     Material property
# --------------------------------------------------------------------
# Define materials
# ----------------
uniaxialMaterial Elastic 1 [expr 2.0*100.0/1.0]

# Define truss elements
# -------------------------
# spring
# element truss $eleTag $iNode $jNode $A $matTag
element truss 1 1 2 1.0 1

# ------------------------------
# End of model generation
# ------------------------------
