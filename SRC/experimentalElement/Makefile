include ../Makefile.def

OBJS       = ExperimentalElement.o \
	     EETwoNodeLink.o \
	     EETruss.o \
	     EEGeneric.o \
	     EEBeamColumn2d.o \
	     EEBeamColumn3d.o \
             EEInvertedVBrace2d.o

all:         $(OBJS)

tcl: 	TclEEBeamColumnCommand.o \
	TclEEGenericCommand.o \
	TclEEInvertedVBraceCommand.o \
	TclEETrussCommand.o \
	TclEETwoNodeLinkCommand.o \
	TclExpElementCommands.o 



# Miscellaneous
tidy:	
	@$(RM) $(RMFLAGS) Makefile.bak *~ #*# core example main.o

clean: tidy
	@$(RM) $(RMFLAGS) $(OBJS) *.o

spotless: clean

wipe: spotless

# DO NOT DELETE THIS LINE -- make depend depends on it.
