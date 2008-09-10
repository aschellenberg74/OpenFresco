#
# Makefile for building ls971.exe
# on 'Windows 
# SMPS
FC=ifort
FFLAGS=-c -W0 -WB -unroll -fp:precise -4Yportlib -assume:byterecl,buffered_io \
-Qfpp2 -DPCWIN -DINTEL -QxK

NSMPS= 
SMPS = -DOPENMP -Qopenmp  
NSMPD= -DAUTODOUBLE -4R8 -4I8 
SMPD = -DOPENMP -Qopenmp -DAUTODOUBLE -4R8 -4I8 

FFLAGS = $(FFLAGS) $(SMPS)
LFLAGS = -link -force -nodefaultlib:msvcrt.lib
OBJS= dyn21.obj dyn21b.obj 
LIBS= libdyna.lib libansys.lib shell32.lib

ls971.exe: $(OBJS)
	$(FC) -w -q -o ls971.exe $(OBJS) $(LIBS) $(LFLAGS)

dyn21.obj: dyn21.F
	$(FC) $(FFLAGS) dyn21.F 
dyn21b.obj: dyn21b.F
	$(FC) $(FFLAGS) dyn21b.F

clean:
	-if exist *.obj erase *.obj
#	-if exist del $(OBJS)
	-if exist ls971.exe erase ls971.exe


