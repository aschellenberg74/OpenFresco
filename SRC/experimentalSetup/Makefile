include ../../Makefile.def

OBJS  = \
        ESAggregator.o \
        ESFourActuators3d.o \
        ESInvertedVBrace2d.o \
        ESInvertedVBraceJntOff2d.o \
        ESNoTransformation.o \
        ESOneActuator.o \
        ESThreeActuators2d.o \
        ESThreeActuatorsJntOff2d.o \
        ESTwoActuators2d.o \
        ExperimentalSetup.o

all:         $(OBJS)

tcl: TclExpSetupCommands.o

test:  test.o
	$(LINKER) $(LINKFLAGS) test.o \
	../experimentalControl/ExperimentalControl.o \
	../experimentalControl/ECSimulation.o \
	../experimentalControl/ECSimUniaxialMaterials.o \
	ExperimentalSetup.o \
	ESNoTransformation.o \
	ESOneActuator.o \
	$(FE_LIBRARY) $(MACHINE_LINKLIBS) \
		-lm \
		$(ARPACK_LIBRARY) \
		$(SUPERLU_LIBRARY) \
		$(UMFPACK_LIBRARY) \
	        $(LAPACK_LIBRARY) $(BLAS_LIBRARY) $(CBLAS_LIBRARY) \
		$(GRAPHIC_LIBRARY) $(RELIABILITY_LIBRARY) \
                $(COROT_LIBRARY) $(FE_LIBRARY) \
		-lg2c -ldl -lpng \
	$(MACHINE_NUMERICAL_LIBS) $(MACHINE_SPECIFIC_LIBS) \
	-o test

# Miscellaneous
tidy:	
	@$(RM) $(RMFLAGS) Makefile.bak *~ #*# core example main.o

clean: tidy
	@$(RM) $(RMFLAGS) $(OBJS) *.o test outD.dat outF.dat *.out

spotless: clean

wipe: spotless

# DO NOT DELETE THIS LINE -- make depend depends on it.
