
include ../../Makefile.def

EXPERIMENTALTEST_OBJS = $(FE)/experimentalTest/ExperimentalCP.o \
	   $(FE)/experimentalTest/ExpCPIter.o \
	   $(FE)/experimentalTest/experimentalElement/ExperimentalElement.o \
	   $(FE)/experimentalTest/experimentalSite/ExperimentalSite.o \
	   $(FE)/experimentalTest/experimentalSetup/ExperimentalSetup.o \
	   $(FE)/experimentalTest/experimentalControl/ExperimentalControl.o \
	   $(FE)/experimentalTest/experimentalSite/LocalExpSite.o \
	   $(FE)/experimentalTest/experimentalSite/RemoteExpSite.o \
	   $(FE)/experimentalTest/experimentalSite/ActorExpSite.o \
	   $(FE)/experimentalTest/experimentalSetup/ESNoTransformation.o \
	   $(FE)/experimentalTest/experimentalSetup/ESOneActuator.o \
	   $(FE)/experimentalTest/experimentalControl/ECSimulation.o \
	   $(FE)/experimentalTest/experimentalControl/ECSimUniaxialMaterials.o \
	   $(FE)/experimentalTest/experimentalElement/EEZeroLength.o \
	   $(FE)/experimentalTest/experimentalSignalFilter/SignalFilter.o \
	   $(FE)/experimentalTest/experimentalSignalFilter/ErrorFilter.o \
	   $(FE)/experimentalTest/experimentalSignalFilter/UndershootErrorFilter.o \
	   $(FE)/experimentalTest/experimentalSignalFilter/RandomErrorFilter.o

#	   $(FE)/experimentalTest/experimentalSetup/ESTwoActuators2d.o \
#	   $(FE)/experimentalTest/experimentalControl/ECSimDomain.o \
#	   $(FE)/experimentalTest/experimentalElement/EEBeamColumn2d.o \

# Compilation control

all: ExperimentalCP.o ExpCPIter.o FrescoGlobals.h
	@$(CD) $(FE)/experimentalTest/experimentalSetup; $(MAKE);
	@$(CD) $(FE)/experimentalTest/experimentalSite; $(MAKE);
	@$(CD) $(FE)/experimentalTest/experimentalControl; $(MAKE);
	@$(CD) $(FE)/experimentalTest/experimentalElement; $(MAKE);
	@$(CD) $(FE)/experimentalTest/experimentalSignalFilter; $(MAKE);
	$(AR) $(ARFLAGS) $(FRESCO_LIBRARY) $(EXPERIMENTALTEST_OBJS)
	@$(RANLIB) $(RANLIBFLAGS) $(FRESCO_LIBRARY)

# Miscellaneous
tidy:	
	@$(RM) $(RMFLAGS) Makefile.bak *~ #*# core

clean: tidy
	@$(RM) $(RMFLAGS) Makefile.bak $(OBJS) *.o *~ #*# core

spotless: clean
	@$(CD) $(FE)/experimentalTest/experimentalSite; $(MAKE) wipe;
	@$(CD) $(FE)/experimentalTest/experimentalSetup; $(MAKE) wipe;
	@$(CD) $(FE)/experimentalTest/experimentalControl; $(MAKE) wipe;
	@$(CD) $(FE)/experimentalTest/experimentalElement; $(MAKE) wipe;
	@$(CD) $(FE)/experimentalTest/experimentalSignalFilter; $(MAKE) wipe;
#	@$(CD) $(FE)/experimentalTest/remoteExperimentalBeamColumn; $(MAKE) wipe;
#	@$(CD) $(FE)/experimentalTest/remoteExperimentalSetup; $(MAKE) wipe;

wipe: spotless

# DO NOT DELETE THIS LINE -- make depend depends on it.
