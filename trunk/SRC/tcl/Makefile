include ../../Makefile.def

OBJS       = tclMain.o tclAppInit.o

OPENFRESCO_TCLFILES = \
	$(OPENFRESCO)/utility/TclExpCPCommand.o \
	$(OPENFRESCO)/experimentalElement/TclEEBeamColumnCommand.o \
	$(OPENFRESCO)/experimentalElement/TclEEGenericCommand.o \
	$(OPENFRESCO)/experimentalElement/TclEETrussCommand.o \
	$(OPENFRESCO)/experimentalElement/TclExpElementCommands.o \
	$(OPENFRESCO)/experimentalElement/TclEEInvertedVBraceCommand.o \
	$(OPENFRESCO)/experimentalElement/TclEETwoNodeLinkCommand.o \
	$(OPENFRESCO)/experimentalControl/TclExpControlCommand.o \
	$(OPENFRESCO)/experimentalSite/TclExpSiteCommand.o \
	$(OPENFRESCO)/experimentalSignalFilter/TclExpSignalFilterCommand.o \
	$(OPENFRESCO)/experimentalSetup/TclExpSetupCommand.o

all: $(OBJS)
	@$(CD) $(OPENFRESCO)/utility; $(MAKE) tcl;
	@$(CD) $(OPENFRESCO)/experimentalSetup; $(MAKE) tcl;
	@$(CD) $(OPENFRESCO)/experimentalSite; $(MAKE) tcl;
	@$(CD) $(OPENFRESCO)/experimentalControl; $(MAKE) tcl;
	@$(CD) $(OPENFRESCO)/experimentalElement; $(MAKE) tcl;
	@$(CD) $(OPENFRESCO)/experimentalSignalFilter; $(MAKE) tcl;
	echo $(TCL_LIBRARY)
	@$(LINKER) $(LINKFLAGS) $(OBJS) $(OPENFRESCO_TCLFILES) \
	$(OPENFRESCO_LIBRARY) $(FE_LIBRARY) $(MACHINE_LINKLIBS) \
	$(MACHINE_NUMERICAL_LIBS) $(TCL_LIBRARY)  \
	$(MACHINE_SPECIFIC_LIBS) \
	-o $(OPENFRESCO_SERVERPROGRAM)


# Miscellaneous
tidy:
	@$(RM) $(RMFLAGS) Makefile.bak *~ #*# core

clean:  tidy
	@$(RM) $(RMFLAGS) $(OBJS) *.o

spotless: clean
	@$(RM) $(RMFLAGS) $(PROGRAM) fake

wipe: spotless


# DO NOT DELETE THIS LINE -- make depend depends on it.





