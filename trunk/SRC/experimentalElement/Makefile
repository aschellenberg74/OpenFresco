include ../Makefile.def

OBJS       = ExperimentalElement.o \
	     EEZeroLength.o \
	     EETruss.o \
	     EEBeamColumn2d.o \
	     EEBeamColumn3d.o \
             EEChevronBrace2d.o

all:         $(OBJS)

tcl: 	TclEEBeamColumnCommand.o \
	TclEEChevronBraceCommand.o \
	TclEETrussCommand.o \
	TclEEZeroLengthCommand.o \
	TclExpElementCommands.o 



# Miscellaneous
tidy:	
	@$(RM) $(RMFLAGS) Makefile.bak *~ #*# core example main.o

clean: tidy
	@$(RM) $(RMFLAGS) $(OBJS) *.o

spotless: clean

wipe: spotless

# DO NOT DELETE THIS LINE -- make depend depends on it.
