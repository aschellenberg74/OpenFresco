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
// Created: 04/10
// Revision: A
//
// Description: This file contains the implementatation of 
// ExpTangentStiffRecorder.

#include <ExpTangentStiffRecorder.h>
#include <ExperimentalTangentStiff.h>


ExpTangentStiffRecorder::ExpTangentStiffRecorder(int numtangstiffs,
    ExperimentalTangentStiff** thetangstiffs, const char** argv, int argc,
    bool echotime, OPS_Stream &theoutputstream, double deltat)
    : Recorder(RECORDER_TAGS_ExpTangentStiffRecorder),
    numTangStiffs(numtangstiffs), theTangStiffs(thetangstiffs), responseArgs(0),
    numArgs(0), echoTime(echotime), theOutputStream(&theoutputstream),
    deltaT(deltat), theResponses(0), data(0), nextTimeStampToRecord(0.0)
{
    // create a copy of the response request
    responseArgs = new char* [argc];
    if (responseArgs == 0)  {
        opserr << "ExpTangentStiffRecorder::ExpTangentStiffRecorder() - out of memory\n";
        numTangStiffs = 0;
    }
    for (int i=0; i<argc; i++)  {
        responseArgs[i] = new char [strlen(argv[i])+1];
        if (responseArgs[i] == 0)  {
            delete [] responseArgs;
            opserr << "ExpTangentStiffRecorder::ExpTangentStiffRecorder() - out of memory\n";
            numTangStiffs = 0;
        }
        strcpy(responseArgs[i], argv[i]);
    }
    numArgs = argc;
    
    theOutputStream->tag("OpenFrescoOutput");
    
    int numDbColumns = 0;
    if (echoTime == true) {
        theOutputStream->tag("TimeOutput");
        theOutputStream->tag("ResponseType","time");
        theOutputStream->endTag();
        numDbColumns += 1;
    }
    
    // Set the response objects:
    //   1. create an array of pointers and zero them
    //   2. iterate over the tangent stiffnesses invoking setResponse() to get the new objects & determine size of data
    theResponses = new Response *[numTangStiffs];
    if (theResponses == 0)  {
        opserr << "ExpTangentStiffRecorder::ExpTangentStiffRecorder() - out of memory\n";
        exit(OF_ReturnType_failed);
    }
    for (int i=0; i<numTangStiffs; i++)
        theResponses[i] = 0;
    
    // loop over tangent stiffnesses & set Responses
    for (int i=0; i<numTangStiffs; i++)  {
        theResponses[i] = theTangStiffs[i]->setResponse((const char **)responseArgs, numArgs, *theOutputStream);
        if (theResponses[i] != 0) {
            // from the response type determine numCols for each
            Information &siteInfo = theResponses[i]->getInformation();
            const Vector &siteData = siteInfo.getData();
            numDbColumns += siteData.Size();
        }
    }
    
    // create the vector to hold the data
    data = new Vector(numDbColumns);
    if (data == 0)  {
        opserr << "ExpTangentStiffRecorder::ExpTangentStiffRecorder() - out of memory\n";
        exit(OF_ReturnType_failed);
    }
    
    theOutputStream->tag("Data");
    
    // record once at zero
    this->record(0, 0.0);
}


ExpTangentStiffRecorder::~ExpTangentStiffRecorder()
{
    theOutputStream->endTag(); // Data
    theOutputStream->endTag(); // OpenFrescoOutput
    
    if (theOutputStream != 0)
        delete theOutputStream;
    if (responseArgs != 0)  {
        for (int i=0; i<numArgs; i++)
            if (responseArgs[i] != 0)
                delete responseArgs[i];
        delete [] responseArgs;
    }
    if (theResponses != 0)  {
        for (int i=0; i<numTangStiffs; i++)
            if (theResponses[i] != 0)
                delete theResponses[i];
        delete [] theResponses;
    }
    if (data != 0)
        delete data;
}


int ExpTangentStiffRecorder::record(int commitTag, double timeStamp)
{
    int result = 0;
    if (deltaT == 0.0 || timeStamp >= nextTimeStampToRecord)  {
        
        if (deltaT != 0.0) 
            nextTimeStampToRecord = timeStamp + deltaT;
        
        int loc = 0;
        if (echoTime == true) 
            (*data)(loc++) = timeStamp;
        
        // for each tangent stiffness if responses exist, put them in response vector
        for (int i=0; i<numTangStiffs; i++)  {
            if (theResponses[i] != 0)  {
                // ask the tangent stiffness for the response
                int res;
                if ((res = theResponses[i]->getResponse()) < 0)  {
                    result += res;
                } else  {
                    Information &siteInfo = theResponses[i]->getInformation();
                    const Vector &siteData = siteInfo.getData();
                    for (int j=0; j<siteData.Size(); j++)
                        (*data)(loc++) = siteData(j);
                }
            } 
        }
        
        // send the response vector to the output handler for o/p
        theOutputStream->write(*data);
    }
    
    // succesfull completion - return 0
    return result;
}


int ExpTangentStiffRecorder::restart()
{
    if (data != 0)
        data->Zero();
    
    return 0;
}
