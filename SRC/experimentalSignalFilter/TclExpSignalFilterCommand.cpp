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

// $Revision: $
// $Date: $
// $URL: $

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 11/06
// Revision: A
//
// Description: This file contains the function invoked when the user
// invokes the expSignalFilter command in the interpreter. 

#include <TclModelBuilder.h>
#include <ArrayOfTaggedObjects.h>

#include <RandomErrorFilter.h>
#include <UndershootErrorFilter.h>

#include <string.h>

static ArrayOfTaggedObjects *theExperimentalSignalFilters(0);


int addExperimentalSignalFilter(SignalFilter &theSignalFilter)
{
    bool result = theExperimentalSignalFilters->addComponent(&theSignalFilter);
    if (result == true)
        return 0;
    else {
        opserr << "addExperimentalSignalFilter() - "
            << "failed to add experimental signal filter: " << theSignalFilter;
        return -1;
    }
}


extern SignalFilter *getExperimentalSignalFilter(int tag)
{
    if (theExperimentalSignalFilters == 0) {
        opserr << "getExperimentalSignalFilter() - "
            << "failed to get experimental signal filter: " << tag << endln
            << "no experimental signal filter objects have been defined\n";
        return 0;
    }

    TaggedObject *mc = theExperimentalSignalFilters->getComponentPtr(tag);
    if (mc == 0) 
        return 0;

    // otherwise we do a cast and return
    SignalFilter *result = (SignalFilter *)mc;
    return result;
}


static void printCommand(int argc, TCL_Char **argv)
{
    opserr << "Input command: ";
    for (int i=0; i<argc; i++)
        opserr << argv[i] << " ";
    opserr << endln;
} 


int TclExpSignalFilterCommand(ClientData clientData, Tcl_Interp *interp, int argc,
    TCL_Char **argv, Domain *theDomain, TclModelBuilder *theTclBuilder)
{
    if (theExperimentalSignalFilters == 0)
        theExperimentalSignalFilters = new ArrayOfTaggedObjects(32);

    // make sure there is a minimum number of arguments
    if (argc < 3)  {
		opserr << "WARNING insufficient number of experimental signal filter arguments\n";
		opserr << "Want: expSignalFilter type tag <specific experimental signal filter args>\n";
		return TCL_ERROR;
    }
    
    // ----------------------------------------------------------------------------	
    if (strcmp(argv[1],"RandomError") == 0)  {
		if (argc != 5)  {
			opserr << "WARNING invalid number of arguments\n";
			printCommand(argc,argv);
			opserr << "Want: expSignalFilter RandomError tag avg std\n";
			return TCL_ERROR;
		}    
		
		int tag;
        double avg, std;
        SignalFilter *theSignalFilter = 0;
		
		if (Tcl_GetInt(interp, argv[2], &tag) != TCL_OK)  {
			opserr << "WARNING invalid RandomError tag\n";
			return TCL_ERROR;		
		}
		if (Tcl_GetDouble(interp, argv[3], &avg) != TCL_OK)  {
			opserr << "WARNING invalid avg\n";
			opserr << "RandomError signal filter: " << tag << endln;
			return TCL_ERROR;	
		}
		if (Tcl_GetDouble(interp, argv[4], &std) != TCL_OK)  {
			opserr << "WARNING invalid std\n";
			opserr << "RandomError signal filter: " << tag << endln;
			return TCL_ERROR;	
		}
		
		// parsing was successful, allocate the signal filter
		theSignalFilter = new RandomErrorFilter(tag, avg, std);

        if (theSignalFilter == 0)  {
            opserr << "WARNING could not create experimental signal filter " << argv[1] << endln;
            return TCL_ERROR;
        }
        
        // now add the signal filter to the modelBuilder
        if (addExperimentalSignalFilter(*theSignalFilter) < 0)  {
            delete theSignalFilter; // invoke the destructor, otherwise mem leak
            return TCL_ERROR;
        }
    }
	
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"UndershootError") == 0)  {
		if (argc != 4)  {
			opserr << "WARNING invalid number of arguments\n";
			printCommand(argc,argv);
			opserr << "Want: expSignalFilter UndershootError tag error\n";
			return TCL_ERROR;
		}    
		
		int tag;
		double error;
        SignalFilter *theSignalFilter = 0;
		
		if (Tcl_GetInt(interp, argv[2], &tag) != TCL_OK)  {
			opserr << "WARNING invalid UndershootError tag\n";
			return TCL_ERROR;		
		}
		if (Tcl_GetDouble(interp, argv[3], &error) != TCL_OK)  {
			opserr << "WARNING invalid error\n";
			opserr << "UndershootError signal filter: " << tag << endln;
			return TCL_ERROR;	
		}
		
		// parsing was successful, allocate the controller
		theSignalFilter = new UndershootErrorFilter(tag, error);

        if (theSignalFilter == 0)  {
            opserr << "WARNING could not create experimental signal filter " << argv[1] << endln;
            return TCL_ERROR;
        }
        
        // now add the signal filter to the modelBuilder
        if (addExperimentalSignalFilter(*theSignalFilter) < 0)  {
            delete theSignalFilter; // invoke the destructor, otherwise mem leak
            return TCL_ERROR;
        }
    }
	
    // ----------------------------------------------------------------------------	
    else  {
        // experimental signal filter type not recognized
        opserr << "WARNING unknown experimental signal filter type: "
            <<  argv[1] << ": check the manual\n";
        return TCL_ERROR;
    }

	return TCL_OK;
}
