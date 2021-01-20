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
**   Andreas Schellenberg (andreas.schellenberg@gmail.com)            **
**   Yoshikazu Takahashi (yos@catfish.dpri.kyoto-u.ac.jp)             **
**   Gregory L. Fenves (fenves@berkeley.edu)                          **
**   Stephen A. Mahin (mahin@berkeley.edu)                            **
**                                                                    **
** ****************************************************************** */

// Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
// Created: 09/06
// Revision: A
//
// Description: This file contains the function invoked when the user
// invokes the expElement command in the interpreter. 

#include <string.h>
#include <tcl.h>
#include <ArrayOfTaggedObjects.h>
#include <Domain.h>
#include <elementAPI.h>

#include <ExperimentalElement.h>

extern void* OPF_EETruss();
extern void* OPF_EETrussCorot();
extern void* OPF_EEBeamColumn2d();
extern void* OPF_EEBeamColumn3d();
extern void* OPF_EETwoNodeLink();
extern void* OPF_EEGeneric();
extern void* OPF_EEInvertedVBrace2d();
extern void* OPF_EEBearing2d();
extern void* OPF_EEBearing3d();


static void printCommand(int argc, TCL_Char **argv)
{
    opserr << "Input command: ";
    for (int i=0; i<argc; i++)
		opserr << argv[i] << " ";
    opserr << endln;
} 


int TclExpElementCommand(ClientData clientData, Tcl_Interp *interp,
    int argc, TCL_Char **argv, Domain *theDomain)
{
    // check that there is at least three arguments
    if (argc < 3)  {
        opserr << "WARNING need to specify an element type\n";
        printCommand(argc,argv);
        opserr << "Want: expElement eleType tag <specific element args> .. see manual for valid eleTypes & arguments\n";
        return TCL_ERROR;
    }
    
    // reset the input args
    OPS_ResetInputNoBuilder(clientData, interp, 2, argc, argv, theDomain);
    
    // pointer to element that will be added
    ExperimentalElement* theElement = 0;
    
    // ----------------------------------------------------------------------------	
    if (strcmp(argv[1],"truss") == 0)  {
        void* theEE = OPF_EETruss();
        if (theEE != 0)
            theElement = (ExperimentalElement*)theEE;
        else
            return TCL_ERROR;
    }
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1], "corotTruss") == 0) {
        void* theEE = OPF_EETrussCorot();
        if (theEE != 0)
            theElement = (ExperimentalElement*)theEE;
        else
            return TCL_ERROR;
    }
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"beamColumn") == 0) {
        ExperimentalElement* theEE = 0;
        if (OPS_GetNDM() == 2)
            theEE = (ExperimentalElement*)OPF_EEBeamColumn2d();
        else
            theEE = (ExperimentalElement*)OPF_EEBeamColumn3d();
        if (theEE != 0)
            theElement = theEE;
        else
            return TCL_ERROR;
    }
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"twoNodeLink") == 0) {
        void* theEE = OPF_EETwoNodeLink();
        if (theEE != 0)
            theElement = (ExperimentalElement*)theEE;
        else
            return TCL_ERROR;
    }
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"generic") == 0) {
        void* theEE = OPF_EEGeneric();
        if (theEE != 0)
            theElement = (ExperimentalElement*)theEE;
        else
            return TCL_ERROR;
    }
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"invertedVBrace") == 0) {
        ExperimentalElement* theEE = 0;
        if (OPS_GetNDM() == 2)
            theEE = (ExperimentalElement*)OPF_EEInvertedVBrace2d();
        else
            opserr << "WARNING expElement invertedVBrace command not implemented yet for ndm = 3\n";
        if (theEE != 0)
            theElement = theEE;
        else
            return TCL_ERROR;
    }
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"bearing") == 0) {
        ExperimentalElement* theEE = 0;
        if (OPS_GetNDM() == 2)
            theEE = (ExperimentalElement*)OPF_EEBearing2d();
        else
            theEE = (ExperimentalElement*)OPF_EEBearing3d();
        if (theEE != 0)
            theElement = theEE;
        else
            return TCL_ERROR;
    }
    // ----------------------------------------------------------------------------	
    else {
        // element type not recognized
        opserr << "WARNING unknown element type: expElement "
            <<  argv[1] << ": check the manual\n";
        return TCL_ERROR;
    }
    
    if (theElement == 0) {
        opserr << "WARNING could not create experimental element " << argv[1] << endln;
        return TCL_ERROR;
    }
    
    // now add the element to the domain
    if (theDomain->addElement(theElement) == false) {
        opserr << "WARNING could not add element of with tag: "
            << theElement->getTag() << " and of type: "
            << theElement->getClassType() << " to the domain\n";
        delete theElement;
        return TCL_ERROR;
    }
    
    return TCL_OK;
}
