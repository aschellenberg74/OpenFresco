include ../../Makefile.def

OBJS  = ExperimentalSignalFilter.o \
        ESFErrorSimulation.o \
        ESFErrorSimUndershoot.o \
        ESFErrorSimRandomGauss.o

all:         $(OBJS)

tcl: 	TclExpSignalFilterCommand.o

# Miscellaneous
tidy:	
	@$(RM) $(RMFLAGS) Makefile.bak *~ #*# core example main.o

clean: tidy
	@$(RM) $(RMFLAGS) $(OBJS) *.o

spotless: clean

wipe: spotless

# DO NOT DELETE THIS LINE -- make depend depends on it.
