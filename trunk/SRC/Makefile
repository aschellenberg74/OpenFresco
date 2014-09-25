
include ../Makefile.def

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
	   $(OPENFRESCO)/experimentalSignalFilter/ExperimentalSignalFilter.o \
	   $(OPENFRESCO)/experimentalSignalFilter/ESFErrorSimRandomGauss.o \
	   $(OPENFRESCO)/experimentalSignalFilter/ESFErrorSimulation.o \
	   $(OPENFRESCO)/experimentalSignalFilter/ESFErrorSimUndershoot.o \
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

# Compilation control
all: FrescoGlobals.h
	@$(CD) $(OPENFRESCO)/utility; $(MAKE);
	@$(CD) $(OPENFRESCO)/experimentalSetup; $(MAKE);
	@$(CD) $(OPENFRESCO)/experimentalSite; $(MAKE);
	@$(CD) $(OPENFRESCO)/experimentalControl; $(MAKE);
	@$(CD) $(OPENFRESCO)/experimentalElement; $(MAKE);
	@$(CD) $(OPENFRESCO)/experimentalSignalFilter; $(MAKE);
	@$(CD) $(OPENFRESCO)/experimentalRecorder; $(MAKE);
	@$(CD) $(OPENFRESCO)/experimentalRecorder/response; $(MAKE);
    @$(CD) $(OPENFRESCO)/openseesCore; $(MAKE);
    @$(CD) $(OPENFRESCO)/openseesExtra; $(MAKE);
	$(AR) $(ARFLAGS) $(OPENFRESCO_LIBRARY) $(EXPERIMENTALTEST_OBJS)

# Miscellaneous
tidy:	
	@$(RM) $(RMFLAGS) Makefile.bak *~ #*# core

clean: tidy
	@$(RM) $(RMFLAGS) Makefile.bak $(OBJS) *.o *~ #*# core

spotless: clean
	@$(CD) $(OPENFRESCO)/utility; $(MAKE) wipe;
	@$(CD) $(OPENFRESCO)/experimentalSite; $(MAKE) wipe;
	@$(CD) $(OPENFRESCO)/experimentalSetup; $(MAKE) wipe;
	@$(CD) $(OPENFRESCO)/experimentalControl; $(MAKE) wipe;
	@$(CD) $(OPENFRESCO)/experimentalElement; $(MAKE) wipe;
	@$(CD) $(OPENFRESCO)/experimentalSignalFilter; $(MAKE) wipe;
	@$(CD) $(OPENFRESCO)/experimentalRecorder; $(MAKE) wipe;
	@$(CD) $(OPENFRESCO)/experimentalRecorder/response; $(MAKE) wipe;
	@$(CD) $(OPENFRESCO)/openseesCore; $(MAKE) wipe;
	@$(CD) $(OPENFRESCO)/openseesExtra; $(MAKE) wipe;

wipe: spotless

# DO NOT DELETE THIS LINE -- make depend depends on it.
