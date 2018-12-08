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

#include <ESFErrorSimRandomGauss.h>
#include <ESFErrorSimUndershoot.h>
#include <ESFKrylovForceConverter.h>
#include <ESFTangForceConverter.h>

extern ExperimentalTangentStiff *getExperimentalTangentStiff(int tag);
static ArrayOfTaggedObjects *theExperimentalSignalFilters(0);


int addExperimentalSignalFilter(ExperimentalSignalFilter &theFilter)
{
    bool result = theExperimentalSignalFilters->addComponent(&theFilter);
    if (result == true)
        return 0;
    else  {
        opserr << "addExperimentalSignalFilter() - "
            << "failed to add experimental signal filter: " << theFilter;
        return -1;
    }
}


extern ExperimentalSignalFilter *getExperimentalSignalFilter(int tag)
{
    if (theExperimentalSignalFilters == 0)  {
        opserr << "getExperimentalSignalFilter() - "
            << "failed to get experimental signal filter: " << tag << endln
            << "no experimental signal filter objects have been defined\n";
        return 0;
    }
    
    TaggedObject *mc = theExperimentalSignalFilters->getComponentPtr(tag);
    if (mc == 0)
        return 0;
    
    // otherwise we do a cast and return
    ExperimentalSignalFilter *result = (ExperimentalSignalFilter *)mc;
    return result;
}


extern ExperimentalSignalFilter *removeExperimentalSignalFilter(int tag)
{
    if (theExperimentalSignalFilters == 0)  {
        opserr << "removeExperimentalSignalFilter() - "
            << "failed to remove experimental signal filter: " << tag << endln
            << "no experimental signal filter objects have been defined\n";
        return 0;
    }
    
    TaggedObject *mc = theExperimentalSignalFilters->removeComponent(tag);
    if (mc == 0)
        return 0;
    
    // otherwise we do a cast and return
    ExperimentalSignalFilter *result = (ExperimentalSignalFilter *)mc;
    return result;
}


extern int clearExperimentalSignalFilters(Tcl_Interp *interp)
{
    if (theExperimentalSignalFilters != 0)  {
        theExperimentalSignalFilters->clearAll();
        delete theExperimentalSignalFilters;
        theExperimentalSignalFilters = 0;
    }
    
    return 0;
}


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
    if (theExperimentalSignalFilters == 0)
        theExperimentalSignalFilters = new ArrayOfTaggedObjects(32);
    
    // make sure there is a minimum number of arguments
    if (argc < 3)  {
        opserr << "WARNING insufficient number of experimental signal filter arguments\n";
        opserr << "Want: expSignalFilter type tag <specific experimental signal filter args>\n";
        return TCL_ERROR;
    }
    
    // pointer to signal filter that will be added
    ExperimentalSignalFilter *theFilter = 0;
    
    // ----------------------------------------------------------------------------	
    if (strcmp(argv[1],"ErrorSimRandomGauss") == 0)  {
        if (argc != 5)  {
            opserr << "WARNING invalid number of arguments\n";
            printCommand(argc,argv);
            opserr << "Want: expSignalFilter ErrorSimRandomGauss tag avg std\n";
            return TCL_ERROR;
        }
        
        int tag;
        double avg, std;
        
        if (Tcl_GetInt(interp, argv[2], &tag) != TCL_OK)  {
            opserr << "WARNING invalid ErrorSimRandomGauss tag\n";
            return TCL_ERROR;
        }
        if (Tcl_GetDouble(interp, argv[3], &avg) != TCL_OK)  {
            opserr << "WARNING invalid avg\n";
            opserr << "expSignalFilter ErrorSimRandomGauss " << tag << endln;
            return TCL_ERROR;
        }
        if (Tcl_GetDouble(interp, argv[4], &std) != TCL_OK)  {
            opserr << "WARNING invalid std\n";
            opserr << "expSignalFilter ErrorSimRandomGauss " << tag << endln;
            return TCL_ERROR;
        }
        
        // parsing was successful, allocate the signal filter
        theFilter = new ESFErrorSimRandomGauss(tag, avg, std);
    }
    
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"ErrorSimUndershoot") == 0)  {
        if (argc != 4)  {
            opserr << "WARNING invalid number of arguments\n";
            printCommand(argc,argv);
            opserr << "Want: expSignalFilter ErrorSimUndershoot tag error\n";
            return TCL_ERROR;
        }
        
        int tag;
        double error;
        
        if (Tcl_GetInt(interp, argv[2], &tag) != TCL_OK)  {
            opserr << "WARNING invalid ErrorSimUndershoot tag\n";
            return TCL_ERROR;
        }
        if (Tcl_GetDouble(interp, argv[3], &error) != TCL_OK)  {
            opserr << "WARNING invalid error\n";
            opserr << "expSignalFilter ErrorSimUndershoot " << tag << endln;
            return TCL_ERROR;
        }
        
        // parsing was successful, allocate the signal filter
        theFilter = new ESFErrorSimUndershoot(tag, error);
    }
    
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"KrylovForceConverter") == 0)  {
        if (argc < 6)  {
            opserr << "WARNING invalid number of arguments\n";
            printCommand(argc,argv);
            opserr << "Want: expSignalFilter KrylovForceConverter"
                << "tag numSubspace -initStif Kij\n";
            return TCL_ERROR;
        }
        
        int tag, ss, argi = 2;
        
        if (Tcl_GetInt(interp, argv[argi], &tag) != TCL_OK)  {
            opserr << "WARNING invalid KrylovForceConverter tag\n";
            return TCL_ERROR;
        }
        argi++;
        if (Tcl_GetInt(interp, argv[argi], &ss) != TCL_OK)  {
            opserr << "WARNING invalid numSubspace\n";
            opserr << "expSignalFilter KrylovForceConverter " << tag << endln;
            return TCL_ERROR;
        }
        argi++;
        if (strcmp(argv[argi],"-initStif") != 0)  {
            opserr << "WARNING expecting -initStif Kij\n";
            opserr << "expSignalFilter KrylovForceConverter " << tag << endln;
            return TCL_ERROR;
        }
        argi++;
        
        // check size of stiffness matrix
        double numArg, dDim;
        int dim, iDim;
        numArg = argc - argi - 1;
        dDim = sqrt(numArg);
        iDim = (int)dDim;
        if (iDim == dDim) {
            dim = iDim;
        } else {
            opserr << "WARNING Kij is not a square matrix\n";
            opserr << "expSignalFilter KrylovForceConverter " << tag << endln;
            return TCL_ERROR;
        }
        
        // set the initial stiffness
        Matrix theInitStif(dim,dim);
        double stif;
        int i, j;
        for (i=0; i<dim; i++)  {
            for (j=0; j<dim; j++)  {
                if (Tcl_GetDouble(interp, argv[argi], &stif) != TCL_OK)  {
                    opserr << "WARNING invalid initial stiffness term\n";
                    opserr << "expSignalFilter KrylovForceConverter " << tag << endln;
                    return TCL_ERROR;
                }
                theInitStif(i,j) = stif;
                argi++;
            }
        }
        
        // parsing was successful, allocate the signal filter
        theFilter = new ESFKrylovForceConverter(tag, ss, theInitStif);
    }
    
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"TangentForceConverter") == 0)  {
        if (argc < 7)  {
            opserr << "WARNING invalid number of arguments\n";
            printCommand(argc,argv);
            opserr << "Want: expSignalFilter TangentForceConverter tag "
                << "-initStif Kij -tangStif tangStifTag\n";
            return TCL_ERROR;
        }
        
        int tag, tangStifTag, argi = 2;
        ExperimentalTangentStiff *theTangStif = 0;
        
        if (Tcl_GetInt(interp, argv[argi], &tag) != TCL_OK)  {
            opserr << "WARNING invalid TangentForceConverter tag\n";
            return TCL_ERROR;
        }
        argi++;
        if (strcmp(argv[argi],"-initStif") != 0)  {
            opserr << "WARNING expecting -initStif Kij\n";
            opserr << "expSignalFilter TangentForceConverter " << tag << endln;
            return TCL_ERROR;
        }
        argi++;
        
        // check size of stiffness matrix
        double numArg, dDim;
        int dim, iDim;
        numArg = argc - argi - 2;
        dDim = sqrt(numArg);
        iDim = (int)dDim;
        if (iDim == dDim) {
            dim = iDim;
        } else {
            opserr << "WARNING Kij is not a square matrix\n";
            opserr << "expSignalFilter TangentForceConverter " << tag << endln;
            return TCL_ERROR;
        }
        
        // set the initial stiffness
        Matrix theInitStif(dim,dim);
        double stif;
        int i, j;
        for (i=0; i<dim; i++)  {
            for (j=0; j<dim; j++)  {
                if (Tcl_GetDouble(interp, argv[argi], &stif) != TCL_OK)  {
                    opserr << "WARNING invalid initial stiffness term\n";
                    opserr << "expSignalFilter TangentForceConverter " << tag << endln;
                    return TCL_ERROR;
                }
                theInitStif(i,j) = stif;
                argi++;
            }
        }
        if (strcmp(argv[argi],"-tangStif") != 0)  {
            opserr << "WARNING expecting -tangStif tangStifTag\n";
            opserr << "expSignalFilter TangentForceConverter " << tag << endln;
            return TCL_ERROR;
        }
        argi++;
        if (Tcl_GetInt(interp, argv[argi], &tangStifTag) != TCL_OK)  {
            opserr << "WARNING invalid tangentStiff Tag\n";
            opserr << "expSignalFilter TangentForceConverter " << tag << endln;
            return TCL_ERROR;
        }
        theTangStif = getExperimentalTangentStiff(tangStifTag);
        
        // parsing was successful, allocate the signal filter
        theFilter = new ESFTangForceConverter(tag, theInitStif, theTangStif);
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
    if (addExperimentalSignalFilter(*theFilter) < 0)  {
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
        ExperimentalSignalFilter *theFilter = removeExperimentalSignalFilter(tag);
        if (theFilter != 0)  {
            delete theFilter;
            theFilter = 0;
        } else  {
            opserr << "WARNING could not remove expSignalFilter with tag " << argv[2] << endln;
            return TCL_ERROR;
        }
    }
    else if (strcmp(argv[1], "signalFilters") == 0)  {
        if (clearExperimentalSignalFilters(interp) < 0)  {
            opserr << "WARNING could not remove expSignalFilters\n";
            return TCL_ERROR;
        }
    }
    
    return TCL_OK;
}
