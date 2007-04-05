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
// Created: 02/07
// Revision: A
//
// Description: This file contains the function invoked when the user
// invokes the expControlPoint command in the interpreter. 

#include <TclModelBuilder.h>
#include <ArrayOfTaggedObjects.h>

#include <ExperimentalCP.h>

#include <string.h>

static ArrayOfTaggedObjects *theExperimentalCPs(0);


int addExperimentalCP(ExperimentalCP &theCP)
{
    bool result = theExperimentalCPs->addComponent(&theCP);
    if (result == true)
        return 0;
    else {
        opserr << "addExperimentalCP() - "
            << "failed to add experimental control point: " << theCP;
        return -1;
    }
}


extern ExperimentalCP *getExperimentalCP(int tag)
{
    if (theExperimentalCPs == 0) {
        opserr << "getExperimentalCP() - "
            << "failed to get experimental control point: " << tag << endln
            << "no experimental control point objects have been defined\n";
        return 0;
    }

    TaggedObject *mc = theExperimentalCPs->getComponentPtr(tag);
    if (mc == 0) 
        return 0;

    // otherwise we do a cast and return
    ExperimentalCP *result = (ExperimentalCP *)mc;
    return result;
}


static void printCommand(int argc, TCL_Char **argv)
{
    opserr << "Input command: ";
    for (int i=0; i<argc; i++)
        opserr << argv[i] << " ";
    opserr << endln;
} 


int TclExpCPCommand(ClientData clientData, Tcl_Interp *interp, int argc,
    TCL_Char **argv, Domain *theDomain, TclModelBuilder *theTclBuilder)
{
    if (theExperimentalCPs == 0)
        theExperimentalCPs = new ArrayOfTaggedObjects(32);

    // make sure there is a minimum number of arguments
    if (argc < 5)  {
        opserr << "WARNING invalid number of arguments\n";
        printCommand(argc,argv);
        opserr << "Want: expControlPoint tag nodeTag dir resp <-fact f> <-lim l u> ...\n";
        return TCL_ERROR;
    }    


    int tag, nodeTag, numDir = 0, numLim = 0, argi = 1;
    double f, lim;
    ExperimentalCP *theCP = 0;

    if (Tcl_GetInt(interp, argv[argi], &tag) != TCL_OK)  {
        opserr << "WARNING invalid expControlPoint tag\n";
        return TCL_ERROR;		
    }
    argi++;
    if (Tcl_GetInt(interp, argv[argi], &nodeTag) != TCL_OK)  {
        opserr << "WARNING invalid nodeTag\n";
        opserr << "control point: " << tag << endln;
        return TCL_ERROR;		
    }
    argi++;
    // count number of directions
    int i = argi;
    while (i<argc)  {
        if (strcmp(argv[i],"-fact") == 0)  {
            i += 2;
        }
        else if (strcmp(argv[i],"-lim") == 0)  {
            i += 3;
            numLim++;
        }
        else  {
            i += 2;
            numDir++;
        }
    }
    if (numDir == 0)  {
        opserr << "WARNING invalid number of arguments\n";
        printCommand(argc,argv);
        opserr << "Want: expControlPoint tag nodeTag dir resp <-fact f> <-lim l u> ...\n";
        return TCL_ERROR;
    }
    ID dir(numDir);
    ID resp(numDir);
    Vector fact(numDir);
    if (numLim > 0 && numLim != numDir)  {
        opserr << "WARNING invalid number of limits\n";
        printCommand(argc,argv);
        opserr << "Want: expControlPoint tag nodeTag dir resp <-fact f> <-lim l u> ...\n";
        return TCL_ERROR;
    }
    Vector lowerLim(numDir);
    Vector upperLim(numDir);
    for (int i=0; i<numDir; i++)  {
        if (strcmp(argv[argi],"1") == 0 || strcmp(argv[argi],"ux") == 0 
            || strcmp(argv[argi],"UX") == 0)
            dir(i) = OF_Dir_X;
        else if (strcmp(argv[argi],"2") == 0 || strcmp(argv[argi],"uy") == 0 
            || strcmp(argv[argi],"UY") == 0)
            dir(i) = OF_Dir_Y;
        else if (strcmp(argv[argi],"3") == 0 || strcmp(argv[argi],"uz") == 0 
            || strcmp(argv[argi],"UZ") == 0)
            dir(i) = OF_Dir_Z;
        else if (strcmp(argv[argi],"4") == 0 || strcmp(argv[argi],"rx") == 0 
            || strcmp(argv[argi],"RX") == 0)
            dir(i) = OF_Dir_RX;
        else if (strcmp(argv[argi],"5") == 0 || strcmp(argv[argi],"ry") == 0 
            || strcmp(argv[argi],"RY") == 0)
            dir(i) = OF_Dir_RY;
        else if (strcmp(argv[argi],"6") == 0 || strcmp(argv[argi],"rz") == 0 
            || strcmp(argv[argi],"RZ") == 0)
            dir(i) = OF_Dir_RZ;
        else  {
            opserr << "WARNING invalid dir\n";
            opserr << "control point: " << tag << endln;
            return TCL_ERROR;		
        }
        argi++;
        if (strcmp(argv[argi],"1") == 0 || strcmp(argv[argi],"disp") == 0 
            || strcmp(argv[argi],"displacement") == 0)
            resp(i) = OF_Resp_Disp;
        else if (strcmp(argv[argi],"2") == 0 || strcmp(argv[argi],"vel") == 0 
            || strcmp(argv[argi],"velocity") == 0)
            resp(i) = OF_Resp_Vel;
        else if (strcmp(argv[argi],"3") == 0 || strcmp(argv[argi],"accel") == 0 
            || strcmp(argv[argi],"acceleration") == 0)
            resp(i) = OF_Resp_Accel;
        else if (strcmp(argv[argi],"4") == 0 || strcmp(argv[argi],"force") == 0)
            resp(i) = OF_Resp_Force;
        else if (strcmp(argv[argi],"5") == 0 || strcmp(argv[argi],"time") == 0)
            resp(i) = OF_Resp_Time;
        else  {
            opserr << "WARNING invalid resp\n";
            opserr << "control point: " << tag << endln;
            return TCL_ERROR;		
        }
        argi++;
        if (argi<argc && strcmp(argv[argi],"-fact") == 0)  {
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &f) != TCL_OK)  {
                opserr << "WARNING invalid factor\n";
                opserr << "control point: " << tag << endln;
                return TCL_ERROR;	
            }
            fact(i) = f;
            argi++;
        }
        else  {
            fact(i) = 1.0;
        }
        if (argi<argc && strcmp(argv[argi],"-lim") == 0)  {
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &lim) != TCL_OK)  {
                opserr << "WARNING invalid lower limit\n";
                opserr << "control point: " << tag << endln;
                return TCL_ERROR;	
            }
            lowerLim(i) = lim;
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &lim) != TCL_OK)  {
                opserr << "WARNING invalid upper limit\n";
                opserr << "control point: " << tag << endln;
                return TCL_ERROR;	
            }
            upperLim(i) = lim;
            argi++;
        }
    }

    // parsing was successful, allocate the control point
    theCP = new ExperimentalCP(tag, nodeTag, dir, resp, fact);

    if (theCP == 0)  {
        opserr << "WARNING could not create experimental control point " << argv[1] << endln;
        return TCL_ERROR;
    }

    // add limits if available
    if (numLim > 0)  {
        theCP->setLimits(lowerLim, upperLim);
    }

    // now add the control point to the modelBuilder
    if (addExperimentalCP(*theCP) < 0)  {
        delete theCP; // invoke the destructor, otherwise mem leak
        return TCL_ERROR;
    }
	
	return TCL_OK;
}
