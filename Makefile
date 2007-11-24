############################################################################
#
#  Program:  OpenFresco
#
#  Purpose:  A Top-level Makefile to create the libraries needed
#	     to use the OpenFresco framework.
#
#  Written: Yoshi
#  Created: 11/2007
#
#  Send bug reports, comments or suggestions to yos@catfish.dpri.kyoto-u.ac.jp
#
############################################################################

include Makefile.def

############################################################################
#
#  First, modify the definitions in Makefile.def to match 
#  your library archiver, compiler, and the options to be used.
#
#  Sample Makefile.def's can be found in the directory MAKES
#  
#  Then to create or add to the libraries needed, enter make after having
#  making the modifications to this file.
#
#  The name of the libraries created and their placement are defined 
#  in the file called Makefile.def.
#
#  To remove the object files after the libraries and testing executables
#  are created, enter
#       make clean
#  To remove the object files and the libraries specified in WIPE_LIBS, enter
#       make wipe
#  To just make the libs, enter 
#	make lib
#  To just build the interpreter type
#	make OpenFresco
############################################################################

all: 
	@( \
	for f in $(OPENFRESCO); \
	do \
		$(CD) $$f; \
		$(MAKE); \
		$(CD) ..; \
	done );
	@$(ECHO) LIBRARIES BUILT ... NOW LINKING OpenFresco PROGRAM;
	@$(CD) $(OPENFRESCO)/tcl;  $(MAKE);

OpenFresco: tcl

tcl:
	@$(ECHO) Building OpenFresco Program ..;
	@$(CD) $(OPENFRESCO)/tcl;  $(MAKE);

libs:
	@( \
	for f in $(OPENFRESCO); \
	do \
		$(CD) $$f; \
		$(MAKE); \
		$(CD) ..; \
	done );

clean:
	@( \
	for f in $(OPENFRESCO); \
	do \
		$(CD) $$f; \
		$(ECHO) Making lib in $$f; \
		$(MAKE) wipe; \
		$(CD) ..; \
	done );
	@$(RM) $(RMFLAGS) *.o *~ core
	@$(CD) $(FE)/../EXAMPLES;  $(MAKE) wipe;

wipe: 
	@( \
	for f in $(OPENFRESCO); \
	do \
		$(CD) $$f; \
		$(ECHO) Making lib in $$f; \
		$(MAKE) wipe; \
		$(CD) ..; \
	done );
	@$(RM) $(RMFLAGS) $(OPENFRESCO_LIBRARY) *.o *~ core 
	@$(CD) $(FE)/../EXAMPLES;  $(MAKE) wipe;

wipeall: 
	@( \
	for f in $(OPENFRESCO); \
	do \
		$(CD) $$f; \
		$(ECHO) Making lib in $$f; \
		$(MAKE) wipe; \
		$(CD) ..; \
	done );
	@$(RM) $(RMFLAGS) $(OPENFRESCO_LIBRARY) *.o *~ core
	@$(CD) $(FE)/../EXAMPLES;  $(MAKE) wipe
	@$(RM) $(RMFLAGS) $(OpenFresco_PROGRAM);

help:
    @$(ECHO) "usage: make ?"






