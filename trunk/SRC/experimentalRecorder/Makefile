include ../../Makefile.def

OBJS = \
       ExpControlRecorder.o \
       ExpSetupRecorder.o \
       ExpSignalFilterRecorder.o \
       ExpSiteRecorder.o \
       ExpTangentStiffRecorder.o

all:         $(OBJS)
	@$(CD) response; $(MAKE);

tcl: 	TclExpRecorderCommands.o

# Miscellaneous
tidy:	
	@$(RM) $(RMFLAGS) Makefile.bak *~ #*# core example main.o

clean: tidy
	@$(RM) $(RMFLAGS) $(OBJS) *.o

spotless: clean

wipe: spotless

# DO NOT DELETE THIS LINE -- make depend depends on it.
