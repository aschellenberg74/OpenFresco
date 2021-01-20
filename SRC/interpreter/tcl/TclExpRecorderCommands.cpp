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
// Created: 08/08
// Revision: A
//
// Description: This file contains the function invoked when the user
// invokes the expRecorder command in the interpreter. 

#include <tcl.h>
#include <ID.h>
#include <Domain.h>
#include <SimulationInformation.h>
#include <Recorder.h>
#include <elementAPI.h>
#include <ExperimentalSite.h>

// recorders
extern void* OPF_ExpSiteRecorder();
extern void* OPF_ExpSetupRecorder();
extern void* OPF_ExpControlRecorder();
extern void* OPF_ExpSignalFilterRecorder();
extern void* OPF_ExpTangentStiffRecorder();

// output streams
#include <StandardStream.h>
#include <DataFileStream.h>
#include <XmlFileStream.h>
#include <BinaryFileStream.h>
#include <TCP_Stream.h>
#include <DatabaseStream.h>

extern SimulationInformation *theSimulationInfo;

int TclCreateExpRecorder(ClientData clientData, Tcl_Interp *interp, int argc,
    TCL_Char **argv, Domain *theDomain, Recorder **theRecorder)
{
    // make sure there is a minimum number of arguments
    if (argc < 2)  {
        opserr << "WARNING insufficient number of experimental recorder arguments\n";
        opserr << "Want: expRecorder type <specific recorder args>\n";
        return TCL_ERROR;
    }
    
    // reset the input args
    OPS_ResetInputNoBuilder(clientData, interp, 2, argc, argv, theDomain);

    FE_Datastore *theRecorderDatabase = 0;
    OPS_Stream *theOutputStream = 0;
    
    TCL_Char *fileName = 0;
    TCL_Char *tableName = 0;
    
    // ----------------------------------------------------------------------------	
    if (strcmp(argv[1],"Site") == 0)  {
        void* theRec = OPF_ExpSiteRecorder();
        if (theRec != 0)
            (*theRecorder) = (Recorder*)theRec;
        else
            return TCL_ERROR;
    }
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"Setup") == 0)  {
        void* theRec = OPF_ExpSetupRecorder();
        if (theRec != 0)
            (*theRecorder) = (Recorder*)theRec;
        else
            return TCL_ERROR;
    }
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"Control") == 0)  {
        void* theRec = OPF_ExpControlRecorder();
        if (theRec != 0)
            (*theRecorder) = (Recorder*)theRec;
        else
            return TCL_ERROR;
    }
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"SignalFilter") == 0)  {
        void* theRec = OPF_ExpSignalFilterRecorder();
        if (theRec != 0)
            (*theRecorder) = (Recorder*)theRec;
        else
            return TCL_ERROR;
    }
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"TangentStiff") == 0)  {
        void* theRec = OPF_ExpTangentStiffRecorder();
        if (theRec != 0)
            (*theRecorder) = (Recorder*)theRec;
        else
            return TCL_ERROR;
    }
    // ----------------------------------------------------------------------------
    else  {
        opserr << "WARNING no recorder exists ";
        opserr << "for recorder of type:" << argv[1];
        return TCL_ERROR;
    }
    
    // check we instantiated a recorder .. if not ran out of memory
    if ((*theRecorder) == 0)  {
        opserr << "WARNING ran out of memory - expRecorder " << argv[1] << endln;
        return TCL_ERROR;
    }
    
    // operation successfull
    return TCL_OK;
}


int TclAddExpRecorder(ClientData clientData, Tcl_Interp *interp,
    int argc, TCL_Char **argv, Domain *theDomain)
{
    Recorder *theRecorder = 0;
    
    TclCreateExpRecorder(clientData, interp, argc, argv, theDomain, &theRecorder);
    
    if (theRecorder == 0)  {
        char buffer[] = "-1";
        Tcl_SetResult(interp, buffer, TCL_VOLATILE);
        return TCL_ERROR;
    }
    
    ExperimentalSite *theSite = OPF_GetExperimentalSiteFirst();
    if (theSite == 0)  {
        opserr << "WARNING failed to get first experimental site\n";
        delete theRecorder;
        char buffer[] = "-1";
        Tcl_SetResult(interp, buffer, TCL_VOLATILE);
        return TCL_ERROR;
    }
    if ((theSite->addRecorder(*theRecorder)) < 0)  {
        opserr << "WARNING could not add to experimental site - expRecorder " << argv[1] << endln;
        delete theRecorder;
        char buffer[] = "-1";
        Tcl_SetResult(interp, buffer, TCL_VOLATILE);
        return TCL_ERROR;
    }
    
    int recorderTag = theRecorder->getTag();
    char buffer[30];
    sprintf(buffer, "%d", recorderTag);
    Tcl_SetResult(interp, buffer, TCL_VOLATILE);
    
    return TCL_OK;
}


int TclRemoveExpRecorder(ClientData clientData, Tcl_Interp *interp,
    int argc, TCL_Char **argv)
{
    ExperimentalSite *theSite = OPF_GetExperimentalSiteFirst();
    if (theSite == 0)  {
        opserr << "WARNING failed to get first experimental site\n";
        return TCL_ERROR;
    }
    
    if (strcmp(argv[1],"recorder") == 0)  {
        if (argc != 3)  {
            opserr << "WARNING invalid number of arguments\n";
            opserr << "Want: removeExp recorder tag\n";
            return TCL_ERROR;
        }
        int tag;
        if (Tcl_GetInt(interp, argv[2], &tag) != TCL_OK)  {
            opserr << "WARNING invalid removeExp recorder tag\n";
            return TCL_ERROR;
        }
        if ((theSite->removeRecorder(tag)) < 0)  {
            opserr << "WARNING could not remove expRecorder with tag " << argv[2] << endln;
            return TCL_ERROR;
        }
    }
    else if (strcmp(argv[1],"recorders") == 0)  {
        if ((theSite->removeRecorders()) < 0)  {
            opserr << "WARNING could not remove expRecorders\n";
            return TCL_ERROR;
        }
    }
    
    return TCL_OK;
}


int TclExpRecord(ClientData clientData, Tcl_Interp *interp,
    int argc, TCL_Char **argv)
{
    ExperimentalSite *theSite = OPF_GetExperimentalSiteFirst();
    if (theSite == 0)  {
        opserr << "WARNING failed to get first experimental site\n";
        return TCL_ERROR;
    }
    if ((theSite->record()) < 0)  {
        opserr << "WARNING could not record\n";
        return TCL_ERROR;
    }
    
    return TCL_OK;
}
