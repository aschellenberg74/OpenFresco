include ../../Makefile.def

OBJS  = Analysis.o \
        AnalysisModel.o \
        ArrayOfTaggedObjects.o \
        ArrayOfTaggedObjectsIter.o \
        BeamFiberMaterial.o \
        BeamFiberMaterial2d.o \
        BinaryFileStream.o \
        Channel.o \
        ConstraintHandler.o \
        DataFileStream.o \
        DOF_Group.o \
        DOF_GrpIter..o \
        Domain.o \
        DomainComponent.o \
        Element.o \
        ElementalLoad.o \
        ElementalLoadIter.o \
        ElementResponse.o \
        FEM_ObjectBroker.o \
        FE_Datastore.o \
        FE_EleIter.o \
        FE_Element.o \
        FiberResponse.o \
        File.o \
        FileIter.o \
        Graph.o \
        ID.o \
        IncrementalIntegrator.o \
        Information..o \
        Integrator.o \
        LinearSOE.o \
        LinearSOESolver.o \
        Load.o \
        LoadPatternIter.o \
        MapOfTaggedObjects.o \
        MapOfTaggedObjectsIter.o \
        Material.o \
        MaterialResponse.o \
        Matrix.o \
        Message.o \
        MovableObject.o \
        MP_Constraint.o \
        NDMaterial.o \
        NodalLoad.o \
        NodalLoadIter.o \
        Node.o \
        ObjectBroker.o \
        OPS_Stream.o \
        Parameter.o \
        PlaneStrainMaterial.o \
        PlaneStressMaterial.o \
        PlateFiberMaterial.o \
        Pressure_Constraint.o \
        Recorder.o \
        Renderer.o \
        Response.o \
        SimulationInformation.o \
        SingleDomAllSP_Iter.o \
        SingleDomEleIter.o \
        SingleDomMP_Iter.o \
        SingleDomNodIter.o \
        SingleDomParamIter.o \
        SingleDomPC_Iter.o \
        SingleDomSP_Iter.o \
        SolutionAlgorithm.o \
        SP_Constraint.o \
        StandardStream.o \
        StaticIntegrator.o \
        StringContainer.o \
        Subdomain.o \
        SubdomainNodIter.o \
        TaggedObject.o \
        TransientIntegrator.o \
        UniaxialMaterial.o \
        Vector.o \
        Vertex.o \
        VertexIter.o \
        win32Functions.o

all:         $(OBJS)

tcl: 	

# Miscellaneous
tidy:	
	@$(RM) $(RMFLAGS) Makefile.bak *~ #*# core example main.o

clean: tidy
	@$(RM) $(RMFLAGS) $(OBJS) *.o

spotless: clean

wipe: spotless

# DO NOT DELETE THIS LINE -- make depend depends on it.
