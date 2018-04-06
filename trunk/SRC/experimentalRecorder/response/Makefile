include ../../../Makefile.def

OBJS = \
       ExpControlResponse.o \
       ExpSetupResponse.o \
       ExpSignalFilterResponse.o \
       ExpSiteResponse.o \
       ExpTangentStiffResponse.o

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
