include ../../Makefile.def

OBJS  = Actor.o \
        BandGenLinLapackSolver.o \
        BandGenLinSOE.o \
        BandGenLinSolver.o \
        ConvergenceTest.o \
        CTestNormDispIncr.o \
        DatabaseStream.o \
        DOF_Numberer.o \
        DummyStream.o \
        EquiSolnAlgo.o \
        LinearSeries.o \
        LineSearch.o \
        LoadControl.o \
        LoadPattern.o \
        ModelBuilder.o \
        NewtonLineSearch.o \
        NewtonRaphson.o \
        PenaltyConstraintHandler.o \
        PenaltyMP_FE.o \
        PenaltySP_FE.o \
        PlainNumberer.o \
        RegulaFalsiLineSearch.o \
        Shadow.o \
        Socket.o \
        StaticAnalysis.o \
        TCP_Socket.o \
        TCP_SocketSSL.o \
        TCP_Stream.o \
        TimeSeries.o \
        UDP_Socket.o \
        XmlFileStream.o

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
