/* ****************************************************************** **
**    OpenFRESCO - Open Framework                                     **
**                 for Experimental Setup and Control                 **
**                                                                    **
**                                                                    **
** Copyright (c) 2006, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited. See    **
** file 'COPYRIGHT_UCB' in main directory for information on usage    **
** and redistribution, and for a DISCLAIMER OF ALL WARRANTIES.        **
**                                                                    **
** Developed by:                                                      **
**   Andreas Schellenberg (andreas.schellenberg@gmx.net)              **
**   Yoshikazu Takahashi (yos@catfish.dpri.kyoto-u.ac.jp)             **
**   Gregory L. Fenves (fenves@berkeley.edu)                          **
**   Stephen A. Mahin (mahin@berkeley.edu)                            **
**                                                                    **
** ****************************************************************** */

// Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
// Created: 11/06
// Revision: A
//
// Description: This file contains the function invoked when the user
// invokes the expSignalFilter command in the interpreter.

#include <string.h>
#include <tcl.h>
#include <ArrayOfTaggedObjects.h>
#include <elementAPI.h>

#include <ExperimentalSignalFilter.h>

extern void* OPF_ESFErrorSimRandomGauss();
extern void* OPF_ESFErrorSimRandomGauss();
extern void* OPF_ESFKrylovForceConverter();
extern void* OPF_ESFTangForceConverter();


static void printCommand(int argc, TCL_Char **argv)
{
    opserr << "Input command: ";
    for (int i=0; i<argc; i++)
        opserr << argv[i] << " ";
    opserr << endln;
}


int TclExpSignalFilterCommand(ClientData clientData, Tcl_Interp *interp,
    int argc, TCL_Char **argv, Domain *theDomain)
{   
    // make sure there is a minimum number of arguments
    if (argc < 3)  {
        opserr << "WARNING insufficient number of experimental signal filter arguments\n";
        opserr << "Want: expSignalFilter type tag <specific experimental signal filter args>\n";
        return TCL_ERROR;
    }
    
    // reset the input args
    OPS_ResetInputNoBuilder(clientData, interp, 2, argc, argv, theDomain);
    
    // pointer to signal filter that will be added
    ExperimentalSignalFilter *theFilter = 0;
    
    // ----------------------------------------------------------------------------	
    if (strcmp(argv[1],"ErrorSimRandomGauss") == 0)  {
        void* theESF = OPF_ESFErrorSimRandomGauss();
        if (theESF != 0)
            theFilter = (ExperimentalSignalFilter*)theESF;
        else
            return TCL_ERROR;
    }
    
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"ErrorSimUndershoot") == 0)  {
        void* theESF = OPF_ESFErrorSimRandomGauss();
        if (theESF != 0)
            theFilter = (ExperimentalSignalFilter*)theESF;
        else
            return TCL_ERROR;
    }
    
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"KrylovForceConverter") == 0)  {
        void* theESF = OPF_ESFKrylovForceConverter();
        if (theESF != 0)
            theFilter = (ExperimentalSignalFilter*)theESF;
        else
            return TCL_ERROR;
    }
    
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"TangentForceConverter") == 0)  {
        void* theESF = OPF_ESFTangForceConverter();
        if (theESF != 0)
            theFilter = (ExperimentalSignalFilter*)theESF;
        else
            return TCL_ERROR;
    }
    
    // ----------------------------------------------------------------------------	
    else  {
        // experimental signal filter type not recognized
        opserr << "WARNING unknown experimental signal filter type: "
            <<  argv[1] << ": check the manual\n";
        return TCL_ERROR;
    }
    
    if (theFilter == 0)  {
        opserr << "WARNING could not create experimental signal filter " << argv[1] << endln;
        return TCL_ERROR;
    }
    
    // now add the signal filter to the modelBuilder
    if (OPF_AddExperimentalSignalFilter(theFilter) == false)  {
        delete theFilter; // invoke the destructor, otherwise mem leak
        return TCL_ERROR;
    }
    
    return TCL_OK;
}


int TclRemoveExpSignalFilter(ClientData clientData, Tcl_Interp *interp,
    int argc, TCL_Char **argv)
{
    if (strcmp(argv[1], "signalFilter") == 0)  {
        if (argc != 3)  {
            opserr << "WARNING invalid number of arguments\n";
            printCommand(argc, argv);
            opserr << "Want: removeExp signalFilter tag\n";
            return TCL_ERROR;
        }
        int tag;
        if (Tcl_GetInt(interp, argv[2], &tag) != TCL_OK)  {
            opserr << "WARNING invalid removeExp signalFilter tag\n";
            return TCL_ERROR;
        }
        if (OPF_RemoveExperimentalSignalFilter(tag) == false) {
            opserr << "WARNING could not remove expSignalFilter with tag " << tag << endln;
            return TCL_ERROR;
        }
        //ExperimentalSignalFilter *theFilter = removeExperimentalSignalFilter(tag);
        //if (theFilter != 0)  {
        //    delete theFilter;
        //    theFilter = 0;
        //} else  {
        //    opserr << "WARNING could not remove expSignalFilter with tag " << argv[2] << endln;
        //    return TCL_ERROR;
        //}
    }
    else if (strcmp(argv[1], "signalFilters") == 0)  {
        OPF_ClearExperimentalSignalFilters();
    }
    
    return TCL_OK;
}
