include ../../../Makefile.def

OBJS  = ExpSiteResponse.o \
        ExpSetupResponse.o \
        ExpControlResponse.o \
        ExpSignalFilterResponse.o

all:         $(OBJS)

tcl: 	

# Miscellaneous
tidy:	
	@$(RM) $(RMFLAGS) Makefile.bak *~ #*# core example main.o

clean: tidy
	@$(RM) $(RMFLAGS) $(OBJS) *.o

spotless: clean

wipe: spotless

# DO NOT DELETE THIS LINE -- make depend depends on it.
