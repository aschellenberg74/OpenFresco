include ../../Makefile.def

OBJS  = \
        ECGenericTCP.o \
        ECSimDomain.o \
        ECSimFEAdapter.o \
        ECSimSimulink.o \
        ECSimulation.o \
        ECSimUniaxialMaterials.o \
        ExperimentalControl.o

all:         $(OBJS)

tcl: TclExpControlCommand.o

test:  test.o
	$(LINKER) $(LINKFLAGS) test.o \
	ExperimentalControl.o \
	ECSimulation.o \
	ECSimUniaxialMaterials.o \
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
