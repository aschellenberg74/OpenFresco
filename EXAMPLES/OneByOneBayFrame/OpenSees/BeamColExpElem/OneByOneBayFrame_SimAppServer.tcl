# File: OneByOneBayFrame_SimAppServer.tcl (use with OneByOneBayFrame_Client.tcl)
# Units: [kN,mm]

# ------------------------------
# Start of model generation
# ------------------------------
logFile "OneByOneBayFrame_SimAppServer.log" 
defaultUnits -force kN -length mm -time sec -temp C
set expCtrlType "SimDomain";  # SimDomain, MTSCsi

# create ModelBuilder (with three-dimensions and 6 DOF/node)
model BasicBuilder -ndm 3 -ndf 6

# Load OpenFresco package
# -----------------------
# (make sure all dlls are in the same folder as openSees.exe)
loadPackage OpenFresco

if {$expCtrlType == "SimDomain"} {
	# Define geometry for model
	# -------------------------
	# node    tag    xCrd    yCrd    zCrd    ndf 
	node  1  0.0  0.0     0.0
	node  2  0.0  0.0  1000.0
	
	# set the boundary conditions
	# fix $tag $DX $DY $DZ $RX $RY $RZ
	fix   1    1   1   1   1   1   1
	#fix   2    0   0   1   0   0   1
	
	# Define sections
	# ---------------
	# section Elastic $secTag $E $A $Iz $Iy $G $J <$alphaY> <$alphaZ> 
	section  Elastic  1  200  500  20000  20000  80  40000  0.8333333  0.8333333 
	
	# Define coordinate transformations
	# ---------------------------------
	# geomTransf Linear $transfTag $vec_xz
	geomTransf  Linear  1  0  1  0 
	
	# Define test specimen element
	# ----------------------------
	# element  dispBeamColumn $eleTag $NodeI $NodeJ $NIP $secTag $geoTranTag <-mass $massDens> <-cMass> 
	element  dispBeamColumn  1  1  2  5  1  1 
	
	# Define control points
	# ---------------------
	# expControlPoint $tag <-node $nodeTag> $dof $rspType <-fact $f> <-lim $l $u> <-relTrial> <-relOut> <-relCtrl> <-relDaq> ...
	expControlPoint 1 -node 2  1 disp  2 disp
	expControlPoint 2 -node 2  1 disp  2 disp  1 force  2 force
	
	# Define experimental control
	# ---------------------------
	# expControl SimDomain $tag -trialCP $cpTags -outCP $cpTags
	expControl SimDomain  1  -trialCP 1  -outCP 2
	
} elseif {$expCtrlType == "MTSCsi"} {
	# Define control points
	# ---------------------
	# expControlPoint $tag <-node $nodeTag> $dof $rspType <-fact $f> <-lim $l $u> <-relTrial> <-relOut> <-relCtrl> <-relDaq> ...
	expControlPoint 1  1 disp -relCtrl  2 disp -relCtrl
	expControlPoint 2  1 disp -relDaq  2 disp -relDaq  1 force -relDaq  2 force -relDaq
	
	# Define experimental control
	# ---------------------------
	# expControl SimDomain $tag -trialCP $cpTags -outCP $cpTags
	expControl  MTSCsi   1  "node2.mtscs"  0.1  -trialCP  1  -outCP  2
}

# Define experimental setup
# -------------------------
# expSetup NoTransformation $tag <–control $ctrlTag> –dof $DOFs -sizeTrialOut $t $o <–trialDispFact $f> ...
expSetup  NoTransformation   1  -control  1  -dof 2 4  -sizeTrialOut 6 6 

# Define experimental site
# ------------------------
# expSite LocalSite $tag $setupTag
expSite LocalSite 1 1
# ------------------------------
# End of model generation
# ------------------------------


# ------------------------------
# Start of recorder generation
# ------------------------------
if {$expCtrlType == "SimDomain"} {
	recorder Node -file SimAppServer_Node_Disp.out -time -node 2 -dof 1 2 disp
	recorder Element -file SimAppServer_Elmt_Force.out -time -ele 1 forces
}

# ExpControl Recorder "ExpCtrl":    fileName    <ctrlTag>    arguments 
expRecorder  Control  -file SimAppServer_ExpCtrl_CtrlDisp.out  -time  -control 1  ctrlDisp 
expRecorder  Control  -file SimAppServer_ExpCtrl_DaqDisp.out  -time  -control 1  daqDisp 
expRecorder  Control  -file SimAppServer_ExpCtrl_DaqForce.out  -time  -control 1  daqForce 

# ExpSetup Recorder "ExpSetup":    fileName    <setupTag>    arguments 
expRecorder  Setup  -file SimAppServer_ExpSetup_TrialDisp.out  -time  -setup 1  trialDisp 
expRecorder  Setup  -file SimAppServer_ExpSetup_OutDisp.out  -time  -setup 1  outDisp 
expRecorder  Setup  -file SimAppServer_ExpSetup_OutForce.out  -time  -setup 1  outForce 
# --------------------------------
# End of recorder generation
# --------------------------------


# ------------------------------
# Start the server process
# ------------------------------
record
# startSimAppSiteServer $siteTag $port <-ssl> <-udp>
startSimAppSiteServer 1 8090;  # use with experimental element in FEA

wipeExp
exit
# --------------------------------
# End of analysis
# --------------------------------
