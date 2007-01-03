
include ./Makefile.def


EXPERIMENTALTEST_OBJS = $(OPENFRESCO)/ExperimentalCP.o \
	   $(OPENFRESCO)/ExpCPIter.o \
	   $(OPENFRESCO)/experimentalElement/ExperimentalElement.o \
	   $(OPENFRESCO)/experimentalElement/EETruss.o \
	   $(OPENFRESCO)/experimentalElement/EEBeamColumn2d.o \
	   $(OPENFRESCO)/experimentalElement/EEBeamColumn3d.o \
	   $(OPENFRESCO)/experimentalElement/EEChevronBrace2d.o \
	   $(OPENFRESCO)/experimentalElement/EEZeroLength.o \
	   $(OPENFRESCO)/experimentalSite/ExperimentalSite.o \
	   $(OPENFRESCO)/experimentalSetup/ExperimentalSetup.o \
	   $(OPENFRESCO)/experimentalSetup/ESTwoActuators2d.o \
	   $(OPENFRESCO)/experimentalSetup/ESThreeActuators2d.o \
	   $(OPENFRESCO)/experimentalSetup/ESChevronBrace2d.o \
	   $(OPENFRESCO)/experimentalSetup/ESChevronBraceJntOff2d.o \
	   $(OPENFRESCO)/experimentalControl/ExperimentalControl.o \
	   $(OPENFRESCO)/experimentalSite/LocalExpSite.o \
	   $(OPENFRESCO)/experimentalSite/RemoteExpSite.o \
	   $(OPENFRESCO)/experimentalSite/ActorExpSite.o \
	   $(OPENFRESCO)/experimentalSetup/ESNoTransformation.o \
	   $(OPENFRESCO)/experimentalSetup/ESOneActuator.o \
	   $(OPENFRESCO)/experimentalControl/ECSimulation.o \
	   $(OPENFRESCO)/experimentalControl/ECSimUniaxialMaterials.o \
	   $(OPENFRESCO)/experimentalElement/EEZeroLength.o \
	   $(OPENFRESCO)/experimentalSignalFilter/SignalFilter.o \
	   $(OPENFRESCO)/experimentalSignalFilter/ErrorFilter.o \
	   $(OPENFRESCO)/experimentalSignalFilter/UndershootErrorFilter.o \
	   $(OPENFRESCO)/experimentalSignalFilter/RandomErrorFilter.o

# Compilation control

all: ExperimentalCP.o ExpCPIter.o FrescoGlobals.h
	@$(CD) $(OPENFRESCO)/experimentalSetup; $(MAKE);
	@$(CD) $(OPENFRESCO)/experimentalSite; $(MAKE);
	@$(CD) $(OPENFRESCO)/experimentalControl; $(MAKE);
	@$(CD) $(OPENFRESCO)/experimentalElement; $(MAKE);
	@$(CD) $(OPENFRESCO)/experimentalSignalFilter; $(MAKE);
	$(AR) $(ARFLAGS) $(OPENFRESCO_LIBRARY) $(EXPERIMENTALTEST_OBJS)
	@$(RANLIB) $(RANLIBFLAGS) $(OPENFRESCO_LIBRARY)
	@$(CD) $(OPENFRESCO)/tcl; $(MAKE);

# Miscellaneous
tidy:	
	@$(RM) $(RMFLAGS) Makefile.bak *~ #*# core

clean: tidy
	@$(RM) $(RMFLAGS) Makefile.bak $(OBJS) *.o *~ #*# core

spotless: clean
	@$(CD) $(OPENFRESCO)/experimentalSite; $(MAKE) wipe;
	@$(CD) $(OPENFRESCO)/experimentalSetup; $(MAKE) wipe;
	@$(CD) $(OPENFRESCO)/experimentalControl; $(MAKE) wipe;
	@$(CD) $(OPENFRESCO)/experimentalElement; $(MAKE) wipe;
	@$(CD) $(OPENFRESCO)/experimentalSignalFilter; $(MAKE) wipe;
	@$(CD) $(OPENFRESCO)/experimentalSignalFilter; $(MAKE) wipe;

wipe: spotless

# DO NOT DELETE THIS LINE -- make depend depends on it.
