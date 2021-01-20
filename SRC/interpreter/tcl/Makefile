include ../../Makefile.def

OBJS  = tclMain.o \
        tclAppInit.o \
        TclLabServerCommands.o \
        TclSimAppElemServerCommands.o \
        TclSimAppSiteServerCommands.o \
DYLIBOBJS  = OpenFrescoPackage.o

EXPERIMENTALTEST_OBJS = $(OPENFRESCO)/utility/ExperimentalCP.o \
	   $(OPENFRESCO)/utility/ExpControlSP.o \
	   $(OPENFRESCO)/experimentalElement/ExperimentalElement.o \
	   $(OPENFRESCO)/experimentalElement/EETruss.o \
	   $(OPENFRESCO)/experimentalElement/EETrussCorot.o \
	   $(OPENFRESCO)/experimentalElement/EEBeamColumn2d.o \
	   $(OPENFRESCO)/experimentalElement/EEBeamColumn3d.o \
	   $(OPENFRESCO)/experimentalElement/EEInvertedVBrace2d.o \
	   $(OPENFRESCO)/experimentalElement/EETwoNodeLink.o \
	   $(OPENFRESCO)/experimentalElement/EEGeneric.o \
       $(OPENFRESCO)/experimentalElement/EEBearing.o \
	   $(OPENFRESCO)/experimentalSite/ExperimentalSite.o \
	   $(OPENFRESCO)/experimentalSite/LocalExpSite.o \
	   $(OPENFRESCO)/experimentalSite/ShadowExpSite.o \
	   $(OPENFRESCO)/experimentalSite/ActorExpSite.o \
	   $(OPENFRESCO)/experimentalSetup/ExperimentalSetup.o \
	   $(OPENFRESCO)/experimentalSetup/ESNoTransformation.o \
	   $(OPENFRESCO)/experimentalSetup/ESAggregator.o \
	   $(OPENFRESCO)/experimentalSetup/ESOneActuator.o \
	   $(OPENFRESCO)/experimentalSetup/ESTwoActuators2d.o \
	   $(OPENFRESCO)/experimentalSetup/ESThreeActuators2d.o \
	   $(OPENFRESCO)/experimentalSetup/ESThreeActuatorsJntOff2d.o \
	   $(OPENFRESCO)/experimentalSetup/ESInvertedVBrace2d.o \
	   $(OPENFRESCO)/experimentalSetup/ESInvertedVBraceJntOff2d.o \
	   $(OPENFRESCO)/experimentalSetup/ESFourActuators3d.o \
	   $(OPENFRESCO)/experimentalControl/ExperimentalControl.o \
	   $(OPENFRESCO)/experimentalControl/ECSimulation.o \
	   $(OPENFRESCO)/experimentalControl/ECSimUniaxialMaterials.o \
	   $(OPENFRESCO)/experimentalControl/ECSimDomain.o \
	   $(OPENFRESCO)/experimentalControl/ECSimFEAdapter.o \
	   $(OPENFRESCO)/experimentalControl/ECSimSimulink.o \
       $(OPENFRESCO)/experimentalControl/ECGenericTCP.o \
	   $(OPENFRESCO)/experimentalSignalFilter/ESFErrorSimRandomGauss.o \
	   $(OPENFRESCO)/experimentalSignalFilter/ESFErrorSimulation.o \
	   $(OPENFRESCO)/experimentalSignalFilter/ESFErrorSimUndershoot.o \
	   $(OPENFRESCO)/experimentalSignalFilter/ExperimentalSignalFilter.o \
	   $(OPENFRESCO)/experimentalRecorder/ExpControlRecorder.o \
	   $(OPENFRESCO)/experimentalRecorder/ExpSetupRecorder.o \
	   $(OPENFRESCO)/experimentalRecorder/ExpSignalFilterRecorder.o \
	   $(OPENFRESCO)/experimentalRecorder/ExpSiteRecorder.o \
	   $(OPENFRESCO)/experimentalRecorder/response/ExpControlResponse.o \
	   $(OPENFRESCO)/experimentalRecorder/response/ExpSetupResponse.o \
	   $(OPENFRESCO)/experimentalRecorder/response/ExpSignalFilterResponse.o \
	   $(OPENFRESCO)/experimentalRecorder/response/ExpSiteResponse.o
       $(OPENFRESCO)/openseesCore/Analysis.o \
       $(OPENFRESCO)/openseesCore/AnalysisModel.o \
       $(OPENFRESCO)/openseesCore/ArrayOfTaggedObjects.o \
       $(OPENFRESCO)/openseesCore/ArrayOfTaggedObjectsIter.o \
       $(OPENFRESCO)/openseesCore/BeamFiberMaterial.o \
       $(OPENFRESCO)/openseesCore/BeamFiberMaterial2d.o \
       $(OPENFRESCO)/openseesCore/BinaryFileStream.o \
       $(OPENFRESCO)/openseesCore/Channel.o \
       $(OPENFRESCO)/openseesCore/ConstraintHandler.o \
       $(OPENFRESCO)/openseesCore/DataFileStream.o \
       $(OPENFRESCO)/openseesCore/DOF_Group.o \
       $(OPENFRESCO)/openseesCore/DOF_GrpIter..o \
       $(OPENFRESCO)/openseesCore/Domain.o \
       $(OPENFRESCO)/openseesCore/DomainComponent.o \
       $(OPENFRESCO)/openseesCore/Element.o \
       $(OPENFRESCO)/openseesCore/ElementalLoad.o \
       $(OPENFRESCO)/openseesCore/ElementalLoadIter.o \
       $(OPENFRESCO)/openseesCore/ElementResponse.o \
       $(OPENFRESCO)/openseesCore/FEM_ObjectBroker.o \
       $(OPENFRESCO)/openseesCore/FE_Datastore.o \
       $(OPENFRESCO)/openseesCore/FE_EleIter.o \
       $(OPENFRESCO)/openseesCore/FE_Element.o \
       $(OPENFRESCO)/openseesCore/FiberResponse.o \
       $(OPENFRESCO)/openseesCore/File.o \
       $(OPENFRESCO)/openseesCore/FileIter.o \
       $(OPENFRESCO)/openseesCore/Graph.o \
       $(OPENFRESCO)/openseesCore/ID.o \
       $(OPENFRESCO)/openseesCore/IncrementalIntegrator.o \
       $(OPENFRESCO)/openseesCore/Information..o \
       $(OPENFRESCO)/openseesCore/Integrator.o \
       $(OPENFRESCO)/openseesCore/LinearSOE.o \
       $(OPENFRESCO)/openseesCore/LinearSOESolver.o \
       $(OPENFRESCO)/openseesCore/Load.o \
       $(OPENFRESCO)/openseesCore/LoadPatternIter.o \
       $(OPENFRESCO)/openseesCore/MapOfTaggedObjects.o \
       $(OPENFRESCO)/openseesCore/MapOfTaggedObjectsIter.o \
       $(OPENFRESCO)/openseesCore/Material.o \
       $(OPENFRESCO)/openseesCore/MaterialResponse.o \
       $(OPENFRESCO)/openseesCore/Matrix.o \
       $(OPENFRESCO)/openseesCore/Message.o \
       $(OPENFRESCO)/openseesCore/MovableObject.o \
       $(OPENFRESCO)/openseesCore/MP_Constraint.o \
       $(OPENFRESCO)/openseesCore/NDMaterial.o \
       $(OPENFRESCO)/openseesCore/NodalLoad.o \
       $(OPENFRESCO)/openseesCore/NodalLoadIter.o \
       $(OPENFRESCO)/openseesCore/Node.o \
       $(OPENFRESCO)/openseesCore/ObjectBroker.o \
       $(OPENFRESCO)/openseesCore/OPS_Stream.o \
       $(OPENFRESCO)/openseesCore/Parameter.o \
       $(OPENFRESCO)/openseesCore/PlaneStrainMaterial.o \
       $(OPENFRESCO)/openseesCore/PlaneStressMaterial.o \
       $(OPENFRESCO)/openseesCore/PlateFiberMaterial.o \
       $(OPENFRESCO)/openseesCore/Pressure_Constraint.o \
       $(OPENFRESCO)/openseesCore/Recorder.o \
       $(OPENFRESCO)/openseesCore/Renderer.o \
       $(OPENFRESCO)/openseesCore/Response.o \
       $(OPENFRESCO)/openseesCore/SimulationInformation.o \
       $(OPENFRESCO)/openseesCore/SingleDomAllSP_Iter.o \
       $(OPENFRESCO)/openseesCore/SingleDomEleIter.o \
       $(OPENFRESCO)/openseesCore/SingleDomMP_Iter.o \
       $(OPENFRESCO)/openseesCore/SingleDomNodIter.o \
       $(OPENFRESCO)/openseesCore/SingleDomParamIter.o \
       $(OPENFRESCO)/openseesCore/SingleDomPC_Iter.o \
       $(OPENFRESCO)/openseesCore/SingleDomSP_Iter.o \
       $(OPENFRESCO)/openseesCore/SolutionAlgorithm.o \
       $(OPENFRESCO)/openseesCore/SP_Constraint.o \
       $(OPENFRESCO)/openseesCore/StandardStream.o \
       $(OPENFRESCO)/openseesCore/StaticIntegrator.o \
       $(OPENFRESCO)/openseesCore/StringContainer.o \
       $(OPENFRESCO)/openseesCore/Subdomain.o \
       $(OPENFRESCO)/openseesCore/SubdomainNodIter.o \
       $(OPENFRESCO)/openseesCore/TaggedObject.o \
       $(OPENFRESCO)/openseesCore/TransientIntegrator.o \
       $(OPENFRESCO)/openseesCore/UniaxialMaterial.o \
       $(OPENFRESCO)/openseesCore/Vector.o \
       $(OPENFRESCO)/openseesCore/Vertex.o \
       $(OPENFRESCO)/openseesCore/VertexIter.o \
       $(OPENFRESCO)/openseesCore/win32Functions.o \
       $(OPENFRESCO)/openseesExtra/Actor.o \
       $(OPENFRESCO)/openseesExtra/BandGenLinLapackSolver.o \
       $(OPENFRESCO)/openseesExtra/BandGenLinSOE.o \
       $(OPENFRESCO)/openseesExtra/BandGenLinSolver.o \
       $(OPENFRESCO)/openseesExtra/ConvergenceTest.o \
       $(OPENFRESCO)/openseesExtra/CTestNormDispIncr.o \
       $(OPENFRESCO)/openseesExtra/DatabaseStream.o \
       $(OPENFRESCO)/openseesExtra/DOF_Numberer.o \
       $(OPENFRESCO)/openseesExtra/DummyStream.o \
       $(OPENFRESCO)/openseesExtra/EquiSolnAlgo.o \
       $(OPENFRESCO)/openseesExtra/LinearSeries.o \
       $(OPENFRESCO)/openseesExtra/LineSearch.o \
       $(OPENFRESCO)/openseesExtra/LoadControl.o \
       $(OPENFRESCO)/openseesExtra/LoadPattern.o \
       $(OPENFRESCO)/openseesExtra/ModelBuilder.o \
       $(OPENFRESCO)/openseesExtra/NewtonLineSearch.o \
       $(OPENFRESCO)/openseesExtra/NewtonRaphson.o \
       $(OPENFRESCO)/openseesExtra/PenaltyConstraintHandler.o \
       $(OPENFRESCO)/openseesExtra/PenaltyMP_FE.o \
       $(OPENFRESCO)/openseesExtra/PenaltySP_FE.o \
       $(OPENFRESCO)/openseesExtra/PlainNumberer.o \
       $(OPENFRESCO)/openseesExtra/RegulaFalsiLineSearch.o \
       $(OPENFRESCO)/openseesExtra/Shadow.o \
       $(OPENFRESCO)/openseesExtra/Socket.o \
       $(OPENFRESCO)/openseesExtra/StaticAnalysis.o \
       $(OPENFRESCO)/openseesExtra/TCP_Socket.o \
       $(OPENFRESCO)/openseesExtra/TCP_SocketSSL.o \
       $(OPENFRESCO)/openseesExtra/TCP_Stream.o \
       $(OPENFRESCO)/openseesExtra/TimeSeries.o \
       $(OPENFRESCO)/openseesExtra/UDP_Socket.o \
       $(OPENFRESCO)/openseesExtra/XmlFileStream.o

OPENFRESCO_TCLFILES = \
	$(OPENFRESCO)/utility/TclExpCPCommand.o \
	$(OPENFRESCO)/experimentalElement/TclEEBeamColumnCommand.o \
    $(OPENFRESCO)/experimentalElement/TclEEBearingCommand.o \
	$(OPENFRESCO)/experimentalElement/TclEEGenericCommand.o \
	$(OPENFRESCO)/experimentalElement/TclEEInvertedVBraceCommand.o \
	$(OPENFRESCO)/experimentalElement/TclEETrussCommand.o \
	$(OPENFRESCO)/experimentalElement/TclEETwoNodeLinkCommand.o \
	$(OPENFRESCO)/experimentalElement/TclExpElementCommands.o \
	$(OPENFRESCO)/experimentalSite/TclExpSiteCommand.o \
	$(OPENFRESCO)/experimentalSetup/TclExpSetupCommand.o \
	$(OPENFRESCO)/experimentalControl/TclExpControlCommand.o \
	$(OPENFRESCO)/experimentalSignalFilter/TclExpSignalFilterCommand.o \
	$(OPENFRESCO)/experimentalRecorder/TclExpRecorderCommands.o \
	$(FE)/tcl/TclFeViewer.o

all: $(OBJS) $(DYLIBOBJS)
	@$(CD) $(OPENFRESCO)/utility; $(MAKE) tcl;
	@$(CD) $(OPENFRESCO)/experimentalElement; $(MAKE) tcl;
	@$(CD) $(OPENFRESCO)/experimentalSetup; $(MAKE) tcl;
	@$(CD) $(OPENFRESCO)/experimentalSite; $(MAKE) tcl;
	@$(CD) $(OPENFRESCO)/experimentalControl; $(MAKE) tcl;
	@$(CD) $(OPENFRESCO)/experimentalSignalFilter; $(MAKE) tcl;
	@$(CD) $(OPENFRESCO)/experimentalRecorder; $(MAKE) tcl;
	echo $(TCL_LIBRARY)
	$(LINKER) $(LINKFLAGS) $(OBJS) $(OPENFRESCO_TCLFILES) \
	$(OPENFRESCO_LIBRARY) $(FE_LIBRARY) $(MACHINE_LINKLIBS) \
	$(MACHINE_NUMERICAL_LIBS)  \
	$(MACHINE_SPECIFIC_LIBS) $(TCL_LIBRARY) \
	-o $(OPENFRESCO_SERVERPROGRAM)
	echo $(TCL_LIBRARY)
	$(LINKER) $(LINKFLAGS2) $(DYLIBOBJS) $(OPENFRESCO_TCLFILES) \
	$(EXPERIMENTALTEST_OBJS) \
	$(FE_LIBRARY) $(MACHINE_LINKLIBS) \
	$(MACHINE_NUMERICAL_LIBS)  \
	$(MACHINE_SPECIFIC_LIBS) $(TCL_LIBRARY) \
	-o $(OPENFRESCO_DYLIBRARY)
	@$(ECHO) DYNAMIC LIBRARIES BUILT ...

# Miscellaneous
tidy:
	@$(RM) $(RMFLAGS) Makefile.bak *~ #*# core

clean:  tidy
	@$(RM) $(RMFLAGS) $(OBJS) *.o

spotless: clean
	@$(RM) $(RMFLAGS) $(PROGRAM) fake

wipe: spotless

# DO NOT DELETE THIS LINE -- make depend depends on it.





