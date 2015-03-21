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

// $Revision$
// $Date$
// $URL$

// Written: Hong Kim (hongkim@berkeley.edu)
// Created: 05/10
// Revision: A
//
// Description: This file contains the function invoked when the user
// invokes the expTangentStiff command in the interpreter. 

#include <string.h>
#include <tcl.h>
#include <ArrayOfTaggedObjects.h>
#include <elementAPI.h>

#include <ETBroyden.h>
#include <ETBfgs.h>
#include <ETTranspose.h>

static ArrayOfTaggedObjects *theExperimentalTangentStiffs(0);


int addExperimentalTangentStiff(ExperimentalTangentStiff &theTangentStiff)
{
    bool result = theExperimentalTangentStiffs->addComponent(&theTangentStiff);
    if (result == true)
        return 0;
    else {
        opserr << "addExperimentalTangentStiff() - "
            << "failed to add experimental tangent stiff: " << theTangentStiff;
        return -1;
    }
}


extern ExperimentalTangentStiff *getExperimentalTangentStiff(int tag)
{
    if (theExperimentalTangentStiffs == 0) {
        opserr << "getExperimentalTangentStiff() - "
            << "failed to get experimental tangent stiff: " << tag << endln
            << "no experimental tangent stiff objects have been defined\n";
        return 0;
    }
    
    TaggedObject *mc = theExperimentalTangentStiffs->getComponentPtr(tag);
    if (mc == 0) 
        return 0;
    
    // otherwise we do a cast and return
    ExperimentalTangentStiff *result = (ExperimentalTangentStiff *)mc;
    
    return result;
}


static void printCommand(int argc, TCL_Char **argv)
{
    opserr << "Input command: ";
    for (int i=0; i<argc; i++)
        opserr << argv[i] << " ";
    opserr << endln;
} 


int TclExpTangentStiffCommand(ClientData clientData, Tcl_Interp *interp,
    int argc, TCL_Char **argv, Domain *theDomain)
{
    if (theExperimentalTangentStiffs == 0)
        theExperimentalTangentStiffs = new ArrayOfTaggedObjects(32);
    
    // make sure there is a minimum number of arguments
    if (argc < 3)  {
        opserr << "WARNING insufficient number of experimental tangent stiff arguments\n";
        opserr << "Want: expTangentStiff type tag <specific experimental tangent stiff args>\n";
        return TCL_ERROR;
    }
    
    // pointer to tangent stiff that will be added
    ExperimentalTangentStiff *theTangentStiff = 0;
    
    // ----------------------------------------------------------------------------	
    if (strcmp(argv[1],"Broyden") == 0)  {
        if (argc != 3)  {
            opserr << "WARNING invalid number of arguments\n";
            printCommand(argc,argv);
            opserr << "Want: expTangentStiff Broyden tag\n";
            return TCL_ERROR;
        }    
        
        int tag;
        
        if (Tcl_GetInt(interp, argv[2], &tag) != TCL_OK)  {
            opserr << "WARNING invalid ETBroyden tag\n";
            return TCL_ERROR;		
        }
        
        // parsing was successful, allocate the tangent stiff
        theTangentStiff = new ETBroyden(tag);
    }
    
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"BFGS") == 0)  {
        if (argc < 3)  {
            opserr << "WARNING invalid number of arguments\n";
            printCommand(argc,argv);
            opserr << "Want: expTangentStiff BFGS tag <-eps value>\n";
            return TCL_ERROR;
        }    
        
        int tag;
        double eps;
        
        if (Tcl_GetInt(interp, argv[2], &tag) != TCL_OK)  {
            opserr << "WARNING invalid expTangentStiff BFGS tag\n";
            return TCL_ERROR;		
        }
        
        if (argc > 3) {
            if (strcmp(argv[3],"-eps") == 0) {
                if (Tcl_GetDouble(interp, argv[4], &eps) != TCL_OK)  {
                    opserr << "WARNING invalid epsilon value\n";
                    opserr << "expTangentStiff BFGS " << tag << endln;
                    return TCL_ERROR;	
                }
                // parsing was successful, allocate the tangent stiff
                theTangentStiff = new ETBfgs(tag, eps);
            }
        } else {
            // parsing was successful, allocate the tangent stiff
            theTangentStiff = new ETBfgs(tag);
        }
    }
    
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"Transpose") == 0)  {
        if (argc != 4)  {
            opserr << "WARNING invalid number of arguments\n";
            printCommand(argc,argv);
            opserr << "Want: expTangentStiff Transpose tag numCols\n";
            return TCL_ERROR;
        }
        
        int tag, numCols;
        
        if (Tcl_GetInt(interp, argv[2], &tag) != TCL_OK)  {
            opserr << "WARNING invalid expTangentStiff tag\n";
            return TCL_ERROR;		
        }
        if (Tcl_GetInt(interp, argv[3], &numCols) != TCL_OK)  {
            opserr << "WARNING invalid number of columns value\n";
            opserr << "expTangentStiff Transpose " << tag << endln;
            return TCL_ERROR;	
        }
        
        // parsing was successful, allocate the tangent stiff
        theTangentStiff = new ETTranspose(tag, numCols);
    }
    
    // ----------------------------------------------------------------------------	
    else  {
        // experimental tangent stiff type not recognized
        opserr << "WARNING unknown experimental tangent stiff type: "
            <<  argv[1] << ": check the manual\n";
        return TCL_ERROR;
    }
    
    if (theTangentStiff == 0)  {
        opserr << "WARNING could not create experimental tangent stiff " << argv[1] << endln;
        return TCL_ERROR;
    }
    
    // now add the tangent stiff to the modelBuilder
    if (addExperimentalTangentStiff(*theTangentStiff) < 0)  {
        delete theTangentStiff; // invoke the destructor, otherwise mem leak
        return TCL_ERROR;
    }
    
    return TCL_OK;
}
