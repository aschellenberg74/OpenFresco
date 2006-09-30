include ../../../Makefile.def

OBJS       = ExperimentalElement.o \
	     EEZeroLength.o \
	     EETruss.o \
	     EEBeamColumn2d.o \
	     EEBeamColumn3d.o

all:         $(OBJS)

# Miscellaneous
tidy:	
	@$(RM) $(RMFLAGS) Makefile.bak *~ #*# core example main.o

clean: tidy
	@$(RM) $(RMFLAGS) $(OBJS) *.o

spotless: clean

wipe: spotless

# DO NOT DELETE THIS LINE -- make depend depends on it.
