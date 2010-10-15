include ../../Makefile.def

OBJS       = tclMain.o tclAppInit.o 
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

OPENFRESCO_TCLFILES = \
	$(OPENFRESCO)/utility/TclExpCPCommand.o \
	$(OPENFRESCO)/experimentalElement/TclEEBeamColumnCommand.o \
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





