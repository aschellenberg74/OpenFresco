
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

wipe: spotless

# DO NOT DELETE THIS LINE -- make depend depends on it.
