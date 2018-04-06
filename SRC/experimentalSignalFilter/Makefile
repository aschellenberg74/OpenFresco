include ../../Makefile.def

OBJS = \
	   ESFErrorSimRandomGauss.o \
	   ESFErrorSimulation.o \
       ESFErrorSimUndershoot.o \
	   ESFKrylovForceConverter.o \
	   ESFTangForceConverter.o \
       ExperimentalSignalFilter.o

all:         $(OBJS)

tcl: 	TclExpSignalFilterCommand.o

test:  test.o
	$(LINKER) $(LINKFLAGS) test.o \
	ExperimentalSignalFilter.o \
	ESFTangForceConverter.o \
	ESFKrylovForceConverter.o \
	$(OPENFRESCO)/experimentalTangentStiff/experimentalTangentStiff.o \
	$(OPENFRESCO)/experimentalTangentStiff/ETBroyden.o \
	$(OPENFRESCO)/experimentalTangentStiff/ETBfgs.o \
	$(FE_LIBRARY) $(MACHINE_LINKLIBS) \
		-lm \
		$(ARPACK_LIBRARY) \
		$(SUPERLU_LIBRARY) \
		$(UMFPACK_LIBRARY) \
	        $(LAPACK_LIBRARY) $(BLAS_LIBRARY) $(CBLAS_LIBRARY) \
		$(GRAPHIC_LIBRARY) $(RELIABILITY_LIBRARY) \
                $(COROT_LIBRARY) $(FE_LIBRARY) \
		-lc -ldl -lstdc++ \
	$(MACHINE_NUMERICAL_LIBS) $(MACHINE_SPECIFIC_LIBS) \
	-o test
	
# Miscellaneous
tidy:	
	@$(RM) $(RMFLAGS) Makefile.bak *~ #*# core example main.o

clean: tidy
	@$(RM) $(RMFLAGS) $(OBJS) *.o

spotless: clean

wipe: spotless

# DO NOT DELETE THIS LINE -- make depend depends on it.
