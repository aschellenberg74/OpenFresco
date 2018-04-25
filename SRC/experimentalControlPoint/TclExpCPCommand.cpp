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

// Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
// Created: 02/07
// Revision: A
//
// Description: This file contains the function invoked when the user
// invokes the expControlPoint command in the interpreter. 

#include <string.h>
#include <tcl.h>
#include <ArrayOfTaggedObjects.h>
#include <Domain.h>
#include <Node.h>
#include <elementAPI.h>

#include <ExperimentalCP.h>

static ArrayOfTaggedObjects *theExperimentalCPs(0);


int addExperimentalCP(ExperimentalCP &theCP)
{
    bool result = theExperimentalCPs->addComponent(&theCP);
    if (result == true)
        return 0;
    else  {
        opserr << "addExperimentalCP() - "
            << "failed to add experimental control point: " << theCP;
        return -1;
    }
}


extern ExperimentalCP *getExperimentalCP(int tag)
{
    if (theExperimentalCPs == 0)  {
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


extern int removeExperimentalCP(int tag)
{
    if (theExperimentalCPs != 0)
        theExperimentalCPs->removeComponent(tag);
    
    return 0;
}


extern int clearExperimentalCPs(Tcl_Interp *interp)
{
    if (theExperimentalCPs != 0)
        theExperimentalCPs->clearAll(false);
    
    return 0;
}


static void printCommand(int argc, TCL_Char **argv)
{
    opserr << "Input command: ";
    for (int i=0; i<argc; i++)
        opserr << argv[i] << " ";
    opserr << endln;
}


int TclExpCPCommand(ClientData clientData, Tcl_Interp *interp,
    int argc, TCL_Char **argv, Domain *theDomain)
{
    if (theExperimentalCPs == 0)
        theExperimentalCPs = new ArrayOfTaggedObjects(32);
    
    // make sure there is a minimum number of arguments
    if (argc < 4)  {
        opserr << "WARNING invalid number of arguments\n";
        printCommand(argc,argv);
        opserr << "Want: expControlPoint tag <-node nodeTag> dof rspType <-fact f> <-lim l u> <-isRel> ...\n";
        return TCL_ERROR;
    }
    
    int tag, i, argi = 1;
    int nodeTag = 0, ndf = 0, ndm = 0;
    Node *theNode = 0;
    int numSignals = 0, numLim = 0, numRefType = 0;
    double f, lim;
    ExperimentalCP *theCP = 0;
    
    if (Tcl_GetInt(interp, argv[argi], &tag) != TCL_OK)  {
        opserr << "WARNING invalid expControlPoint tag\n";
        return TCL_ERROR;
    }
    argi++;
    if (strcmp(argv[argi],"-node") == 0)  {
        argi++;
        if (Tcl_GetInt(interp, argv[argi], &nodeTag) != TCL_OK)  {
            opserr << "WARNING invalid nodeTag for control point: " << tag << endln;
            printCommand(argc,argv);
            opserr << "Want: expControlPoint tag <-node nodeTag> dof rspType <-fact f> <-lim l u> <-isRel> ...\n";
            return TCL_ERROR;
        }
        theNode = theDomain->getNode(nodeTag);
        ndf = theNode->getNumberDOF();
        ndm = OPS_GetNDM();
        argi++;
    }
    // count number of signals
    i = argi;
    while (i < argc)  {
        if (strcmp(argv[i],"-fact") == 0 || strcmp(argv[i],"-factor") == 0)
            i += 2;
        else if (strcmp(argv[i],"-isRel") == 0 || strcmp(argv[i],"-isRelative") == 0)
            i++;
        else if (strcmp(argv[i],"-lim") == 0 || strcmp(argv[i],"-limit") == 0)  {
            i += 3;
            numLim++;
        }
        else  {
            i += 2;
            numSignals++;
        }
    }
    if (numSignals == 0)  {
        opserr << "WARNING invalid number of arguments for control point: " << tag << endln;
        printCommand(argc,argv);
        opserr << "Want: expControlPoint tag <-node nodeTag> dof rspType <-fact f> <-lim l u> <-isRel> ...\n";
        return TCL_ERROR;
    }
    if (numLim > 0 && numLim != numSignals)  {
        opserr << "WARNING invalid number of limits for control point: " << tag << endln;
        printCommand(argc,argv);
        opserr << "Want: expControlPoint tag <-node nodeTag> dof rspType <-fact f> <-lim l u> <-isRel> ...\n";
        return TCL_ERROR;
    }
    ID dof(numSignals);
    ID rspType(numSignals);
    Vector factor(numSignals);
    Vector lowerLim(numSignals);
    Vector upperLim(numSignals);
    ID isRelative(numSignals);
    for (i=0; i<numSignals; i++)  {
        if (ndf == 0)  {
            int dofID = 0;
            if (sscanf(argv[argi],"%d",&dofID) != 1)  {
                if (sscanf(argv[argi],"%*[dfouDFOU]%d",&dofID) != 1)  {
                    opserr << "WARNING invalid dof for control point: " << tag << endln;
                    printCommand(argc,argv);
                    opserr << "Want: expControlPoint tag <-node nodeTag> dof rspType <-fact f> <-lim l u> <-isRel> ...\n";
                    return TCL_ERROR;
                }
            }
            dof(i) = dofID - 1;
        }
        else if (ndm == 1 && ndf == 1)  {
            if (strcmp(argv[argi],"1") == 0 || 
                strcmp(argv[argi],"dof1") == 0 || strcmp(argv[argi],"DOF1") == 0 ||
                strcmp(argv[argi],"u1") == 0 || strcmp(argv[argi],"U1") == 0 ||
                strcmp(argv[argi],"ux") == 0 || strcmp(argv[argi],"UX") == 0)
                dof(i) = 0;
            else  {
                opserr << "WARNING invalid dof for control point: " << tag << endln;
                printCommand(argc,argv);
                opserr << "Want: expControlPoint tag <-node nodeTag> dof rspType <-fact f> <-lim l u> <-isRel> ...\n";
                return TCL_ERROR;
            }
        }
        else if (ndm == 2 && ndf == 2)  {
            if (strcmp(argv[argi],"1") == 0 ||
                strcmp(argv[argi],"dof1") == 0 || strcmp(argv[argi],"DOF1") == 0 ||
                strcmp(argv[argi],"u1") == 0 || strcmp(argv[argi],"U1") == 0 ||
                strcmp(argv[argi],"ux") == 0 || strcmp(argv[argi],"UX") == 0)
                dof(i) = 0;
            else if (strcmp(argv[argi],"2") == 0 ||
                strcmp(argv[argi],"dof2") == 0 || strcmp(argv[argi],"DOF2") == 0 ||
                strcmp(argv[argi],"u2") == 0 || strcmp(argv[argi],"U2") == 0 ||
                strcmp(argv[argi],"uy") == 0 || strcmp(argv[argi],"UY") == 0)
                dof(i) = 1;
            else  {
                opserr << "WARNING invalid dof for control point: " << tag << endln;
                printCommand(argc,argv);
                opserr << "Want: expControlPoint tag <-node nodeTag> dof rspType <-fact f> <-lim l u> <-isRel> ...\n";
                return TCL_ERROR;
            }
        }
        else if (ndm == 2 && ndf == 3)  {
            if (strcmp(argv[argi],"1") == 0 ||
                strcmp(argv[argi],"dof1") == 0 || strcmp(argv[argi],"DOF1") == 0 ||
                strcmp(argv[argi],"u1") == 0 || strcmp(argv[argi],"U1") == 0 ||
                strcmp(argv[argi],"ux") == 0 || strcmp(argv[argi],"UX") == 0)
                dof(i) = 0;
            else if (strcmp(argv[argi],"2") == 0 ||
                strcmp(argv[argi],"dof2") == 0 || strcmp(argv[argi],"DOF2") == 0 ||
                strcmp(argv[argi],"u2") == 0 || strcmp(argv[argi],"U2") == 0 ||
                strcmp(argv[argi],"uy") == 0 || strcmp(argv[argi],"UY") == 0)
                dof(i) = 1;
            else if (strcmp(argv[argi],"3") == 0 ||
                strcmp(argv[argi],"dof3") == 0 || strcmp(argv[argi],"DOF3") == 0 ||
                strcmp(argv[argi],"r3") == 0 || strcmp(argv[argi],"R3") == 0 ||
                strcmp(argv[argi],"rz") == 0 || strcmp(argv[argi],"RZ") == 0)
                dof(i) = 2;
            else  {
                opserr << "WARNING invalid dof for control point: " << tag << endln;
                printCommand(argc,argv);
                opserr << "Want: expControlPoint tag <-node nodeTag> dof rspType <-fact f> <-lim l u> <-isRel> ...\n";
                return TCL_ERROR;
            }
        }
        else if (ndm == 3 && ndf == 3)  {
            if (strcmp(argv[argi],"1") == 0 ||
                strcmp(argv[argi],"dof1") == 0 || strcmp(argv[argi],"DOF1") == 0 ||
                strcmp(argv[argi],"u1") == 0 || strcmp(argv[argi],"U1") == 0 ||
                strcmp(argv[argi],"ux") == 0 || strcmp(argv[argi],"UX") == 0)
                dof(i) = 0;
            else if (strcmp(argv[argi],"2") == 0 ||
                strcmp(argv[argi],"dof2") == 0 || strcmp(argv[argi],"DOF2") == 0 ||
                strcmp(argv[argi],"u2") == 0 || strcmp(argv[argi],"U2") == 0 ||
                strcmp(argv[argi],"uy") == 0 || strcmp(argv[argi],"UY") == 0)
                dof(i) = 1;
            else if (strcmp(argv[argi],"3") == 0 ||
                strcmp(argv[argi],"dof3") == 0 || strcmp(argv[argi],"DOF3") == 0 ||
                strcmp(argv[argi],"u3") == 0 || strcmp(argv[argi],"U3") == 0 ||
                strcmp(argv[argi],"uz") == 0 || strcmp(argv[argi],"UZ") == 0)
                dof(i) = 2;
            else  {
                opserr << "WARNING invalid dof for control point: " << tag << endln;
                printCommand(argc,argv);
                opserr << "Want: expControlPoint tag <-node nodeTag> dof rspType <-fact f> <-lim l u> <-isRel> ...\n";
                return TCL_ERROR;
            }
        }
        else if (ndm == 3 && ndf == 6)  {
            if (strcmp(argv[argi],"1") == 0 ||
                strcmp(argv[argi],"dof1") == 0 || strcmp(argv[argi],"DOF1") == 0 ||
                strcmp(argv[argi],"u1") == 0 || strcmp(argv[argi],"U1") == 0 ||
                strcmp(argv[argi],"ux") == 0 || strcmp(argv[argi],"UX") == 0)
                dof(i) = 0;
            else if (strcmp(argv[argi],"2") == 0 ||
                strcmp(argv[argi],"dof2") == 0 || strcmp(argv[argi],"DOF2") == 0 ||
                strcmp(argv[argi],"u2") == 0 || strcmp(argv[argi],"U2") == 0 ||
                strcmp(argv[argi],"uy") == 0 || strcmp(argv[argi],"UY") == 0)
                dof(i) = 1;
            else if (strcmp(argv[argi],"3") == 0 ||
                strcmp(argv[argi],"dof3") == 0 || strcmp(argv[argi],"DOF3") == 0 ||
                strcmp(argv[argi],"u3") == 0 || strcmp(argv[argi],"U3") == 0 ||
                strcmp(argv[argi],"uz") == 0 || strcmp(argv[argi],"UZ") == 0)
                dof(i) = 2;
            else if (strcmp(argv[argi],"4") == 0 ||
                strcmp(argv[argi],"dof4") == 0 || strcmp(argv[argi],"DOF4") == 0 ||
                strcmp(argv[argi],"r1") == 0 || strcmp(argv[argi],"R1") == 0 ||
                strcmp(argv[argi],"rx") == 0 || strcmp(argv[argi],"RX") == 0)
                dof(i) = 3;
            else if (strcmp(argv[argi],"5") == 0 ||
                strcmp(argv[argi],"dof5") == 0 || strcmp(argv[argi],"DOF5") == 0 ||
                strcmp(argv[argi],"r2") == 0 || strcmp(argv[argi],"R2") == 0 ||
                strcmp(argv[argi],"ry") == 0 || strcmp(argv[argi],"RY") == 0)
                dof(i) = 4;
            else if (strcmp(argv[argi],"6") == 0 ||
                strcmp(argv[argi],"dof6") == 0 || strcmp(argv[argi],"DOF6") == 0 ||
                strcmp(argv[argi],"r3") == 0 || strcmp(argv[argi],"R3") == 0 ||
                strcmp(argv[argi],"rz") == 0 || strcmp(argv[argi],"RZ") == 0)
                dof(i) = 5;
            else  {
                opserr << "WARNING invalid dof for control point: " << tag << endln;
                printCommand(argc,argv);
                opserr << "Want: expControlPoint tag <-node nodeTag> dof rspType <-fact f> <-lim l u> <-isRel> ...\n";
               return TCL_ERROR;
            }
        }
        argi++;
        if (strcmp(argv[argi],"1") == 0 || strcmp(argv[argi],"dsp") == 0 ||
            strcmp(argv[argi],"disp") == 0 || strcmp(argv[argi],"displacement") == 0)
            rspType(i) = OF_Resp_Disp;
        else if (strcmp(argv[argi],"2") == 0 || strcmp(argv[argi],"vel") == 0 ||
            strcmp(argv[argi],"velocity") == 0)
            rspType(i) = OF_Resp_Vel;
        else if (strcmp(argv[argi],"3") == 0 || strcmp(argv[argi],"acc") == 0 ||
            strcmp(argv[argi],"accel") == 0 || strcmp(argv[argi],"acceleration") == 0)
            rspType(i) = OF_Resp_Accel;
        else if (strcmp(argv[argi],"4") == 0 || strcmp(argv[argi],"frc") == 0 ||
            strcmp(argv[argi],"force") == 0)
            rspType(i) = OF_Resp_Force;
        else if (strcmp(argv[argi],"5") == 0 ||  strcmp(argv[argi],"t") == 0 ||
            strcmp(argv[argi],"tme") == 0 || strcmp(argv[argi],"time") == 0)
            rspType(i) = OF_Resp_Time;
        else  {
            opserr << "WARNING invalid rspType for control point: " << tag << endln;
            printCommand(argc,argv);
            opserr << "Want: expControlPoint tag <-node nodeTag> dof rspType <-fact f> <-lim l u> <-isRel> ...\n";
            return TCL_ERROR;
        }
        argi++;
        if (argi<argc && (strcmp(argv[argi],"-fact") == 0 || strcmp(argv[argi],"-factor") == 0))  {
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &f) != TCL_OK)  {
                opserr << "WARNING invalid factor for control point: " << tag << endln;
                printCommand(argc,argv);
                opserr << "Want: expControlPoint tag <-node nodeTag> dof rspType <-fact f> <-lim l u> <-isRel> ...\n";
                return TCL_ERROR;
            }
            factor(i) = f;
            argi++;
        } else  {
            factor(i) = 1.0;
        }
        if (argi<argc && (strcmp(argv[argi],"-lim") == 0 || strcmp(argv[argi],"-limit") == 0))  {
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &lim) != TCL_OK)  {
                opserr << "WARNING invalid lower limit for control point: " << tag << endln;
                printCommand(argc,argv);
                opserr << "Want: expControlPoint tag <-node nodeTag> dof rspType <-fact f> <-lim l u> <-isRel> ...\n";
                return TCL_ERROR;
            }
            lowerLim(i) = lim;
            argi++;
            if (Tcl_GetDouble(interp, argv[argi], &lim) != TCL_OK)  {
                opserr << "WARNING invalid upper limit for control point: " << tag << endln;
                printCommand(argc,argv);
                opserr << "Want: expControlPoint tag <-node nodeTag> dof rspType <-fact f> <-lim l u> <-isRel> ...\n";
                return TCL_ERROR;
            }
            upperLim(i) = lim;
            argi++;
        }
        if (argi<argc && (strcmp(argv[argi],"-isRel") == 0 || strcmp(argv[argi],"-isRelative") == 0))  {
            isRelative(i) = 1;
            numRefType++;
            argi++;
        }
    }
    
    // parsing was successful, allocate the control point
    theCP = new ExperimentalCP(tag, dof, rspType, factor);
    
    if (theCP == 0)  {
        opserr << "WARNING could not create experimental control point " << argv[1] << endln;
        return TCL_ERROR;
    }
    
    // add limits if available
    if (numLim > 0)
        theCP->setLimits(lowerLim, upperLim);
    
    // add signal reference types if available
    if (numRefType > 0)
        theCP->setSigRefType(isRelative);
    
    // add node if available
    if (theNode != 0)
        theCP->setNode(theNode);
    
    // now add the control point to the modelBuilder
    if (addExperimentalCP(*theCP) < 0)  {
        delete theCP; // invoke the destructor, otherwise mem leak
        return TCL_ERROR;
    }
    
    return TCL_OK;
}


int TclRemoveExpCP(ClientData clientData, Tcl_Interp *interp,
    int argc, TCL_Char **argv)
{
    if (strcmp(argv[1], "controlPoint") == 0)  {
        if (argc != 3)  {
            opserr << "WARNING invalid number of arguments\n";
            printCommand(argc, argv);
            opserr << "Want: removeExp controlPoint tag\n";
            return TCL_ERROR;
        }
        int tag;
        if (Tcl_GetInt(interp, argv[2], &tag) != TCL_OK)  {
            opserr << "WARNING invalid removeExp controlPoint tag\n";
            return TCL_ERROR;
        }
        if (removeExperimentalCP(tag) < 0)  {
            opserr << "WARNING could not remove expControlPoint with tag " << argv[2] << endln;
            return TCL_ERROR;
        }
    }
    else if (strcmp(argv[1], "controlPoints") == 0)  {
        if (clearExperimentalCPs(interp) < 0)  {
            opserr << "WARNING could not remove expControlPoints\n";
            return TCL_ERROR;
        }
    }
    
    return TCL_OK;
}
