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
// Description: This file contains the implementatation of ExpSetupRecorder.

#include <ExpSetupRecorder.h>

#include <ExperimentalSetup.h>

#include <StandardStream.h>
#include <DataFileStream.h>
#include <DataFileStreamAdd.h>
#include <XmlFileStream.h>
#include <BinaryFileStream.h>
//#include <DatabaseStream.h>
#include <TCP_Stream.h>
#include <elementAPI.h>


void* OPF_ExpSetupRecorder()
{
    if (OPS_GetNumRemainingInputArgs() < 5) {
        opserr << "WARNING insufficient number of experimental recorder arguments\n";
        opserr << "Want: expRecorder type <specific recorder args>\n";
        return 0;
    }
    
    const char** data = 0;
    int nargrem = 0;
    OPS_Stream* theOutputStream = 0;
    const char* filename = 0;
    FE_Datastore* theDatabase = 0;
    const char* tablename = 0;
    
    const int STANDARD_STREAM = 0;
    const int DATA_STREAM = 1;
    const int XML_STREAM = 2;
    const int DATABASE_STREAM = 3;
    const int BINARY_STREAM = 4;
    const int DATA_STREAM_CSV = 5;
    const int TCP_STREAM = 6;
    const int DATA_STREAM_ADD = 7;
    
    int eMode = STANDARD_STREAM;
    bool echoTimeFlag = false;
    double deltaT = 0.0;
    bool doScientific = false;
    int precision = 6;
    bool closeOnWrite = false;
    const char* inetAddr = 0;
    int inetPort;
    
    int numSetups = 0;
    ID setupTags(0, 32);
    ExperimentalSetup** theSetups = 0;
    
    while (OPS_GetNumRemainingInputArgs() > 0) {
        
        int numdata;
        const char* option = OPS_GetString();
        if (strcmp(option, "-time") == 0 ||
            strcmp(option, "-load") == 0) {
            echoTimeFlag = true;
        }
        else if (strcmp(option, "-dT") == 0 ||
            strcmp(option, "-dt") == 0) {
            if (OPS_GetNumRemainingInputArgs() > 0) {
                numdata = 1;
                if (OPS_GetDoubleInput(&numdata, &deltaT) < 0) {
                    opserr << "WARNING: failed to read dT\n";
                    return 0;
                }
            }
        }
        else if (strcmp(option, "-precision") == 0) {
            if (OPS_GetNumRemainingInputArgs() > 0) {
                numdata = 1;
                if (OPS_GetIntInput(&numdata, &precision) < 0) {
                    opserr << "WARNING: failed to read precision\n";
                    return 0;
                }
            }
        }
        else if (strcmp(option, "-scientific") == 0) {
            doScientific = true;
        }
        else if (strcmp(option, "-closeOnWrite") == 0) {
            closeOnWrite = true;
        }
        else if (strcmp(option, "-file") == 0) {
            if (OPS_GetNumRemainingInputArgs() > 0) {
                filename = OPS_GetString();
            }
            eMode = DATA_STREAM;
        }
        else if (strcmp(option, "-fileAdd") == 0) {
            if (OPS_GetNumRemainingInputArgs() > 0) {
                filename = OPS_GetString();
            }
            eMode = DATA_STREAM_ADD;
        }
        else if (strcmp(option, "-csv") == 0 ||
            strcmp(option, "-CSV") == 0) {
            if (OPS_GetNumRemainingInputArgs() > 0) {
                filename = OPS_GetString();
            }
            eMode = DATA_STREAM_CSV;
        }
        else if (strcmp(option, "-xml") == 0 ||
            strcmp(option, "-XML") == 0) {
            if (OPS_GetNumRemainingInputArgs() > 0) {
                filename = OPS_GetString();
            }
            eMode = XML_STREAM;
        }
        else if (strcmp(option, "-bin") == 0 ||
            strcmp(option, "-BIN") == 0 ||
            strcmp(option, "-binary") == 0) {
            if (OPS_GetNumRemainingInputArgs() > 0) {
                filename = OPS_GetString();
            }
            eMode = BINARY_STREAM;
        }
        else if (strcmp(option, "-tcp") == 0 ||
            strcmp(option, "-TCP") == 0) {
            if (OPS_GetNumRemainingInputArgs() > 0) {
                inetAddr = OPS_GetString();
            }
            if (OPS_GetNumRemainingInputArgs() > 0) {
                numdata = 1;
                if (OPS_GetIntInput(&numdata, &inetPort) < 0) {
                    opserr << "WARNING: failed to read inetPort\n";
                    return 0;
                }
            }
            eMode = TCP_STREAM;
        }
        /*else if (strcmp(option, "-database") == 0) {
            theDatabase = OPS_GetFEDatastore();
            if (theDatabase != 0) {
                if (OPS_GetNumRemainingInputArgs() > 0) {
                    tablename = OPS_GetString();
                }
                eMode = DATABASE_STREAM;
            }
            else {
                opserr << "WARNING: no current database\n";
            }
        }*/
        else if (strcmp(option, "-setup") == 0) {
            while (OPS_GetNumRemainingInputArgs() > 0) {
                int tag;
                numdata = 1;
                if (OPS_GetIntInput(&numdata, &tag) < 0) {
                    break;
                }
                setupTags[numSetups++] = tag;
            }
        }
        else if (strcmp(option, "-setupRange") == 0) {
            int start = 0, end = 0;
            if (OPS_GetNumRemainingInputArgs() > 0) {
                numdata = 1;
                if (OPS_GetIntInput(&numdata, &start) < 0) {
                    opserr << "WARNING: failed to read start setup tag\n";
                    return 0;
                }
            }
            if (OPS_GetNumRemainingInputArgs() > 0) {
                numdata = 1;
                if (OPS_GetIntInput(&numdata, &end) < 0) {
                    opserr << "WARNING: failed to read end setup tag\n";
                    return 0;
                }
            }
            if (start > end) {
                int swap = end;
                end = start;
                start = swap;
            }
            for (int i = start; i <= end; i++)
                setupTags[numSetups++] = i;
        }
        else {
            // first unknown string then is assumed to start response request
            nargrem = 1 + OPS_GetNumRemainingInputArgs();
            data = new const char* [nargrem];
            data[0] = option;
            for (int i = 1; i < nargrem; i++)
                data[i] = OPS_GetString();
        }
    }
    
    // data handler
    if (eMode == DATA_STREAM && filename != 0)
        theOutputStream = new DataFileStream(filename, OVERWRITE, 2, 0, closeOnWrite, precision, doScientific);
    else if (eMode == DATA_STREAM_ADD && filename != 0)
        theOutputStream = new DataFileStreamAdd(filename, OVERWRITE, 2, 0, closeOnWrite, precision, doScientific);
    else if (eMode == DATA_STREAM_CSV && filename != 0)
        theOutputStream = new DataFileStream(filename, OVERWRITE, 2, 1, closeOnWrite, precision, doScientific);
    else if (eMode == XML_STREAM && filename != 0)
        theOutputStream = new XmlFileStream(filename);
    //else if (eMode == DATABASE_STREAM && tablename != 0)
    //    theOutputStream = new DatabaseStream(theDatabase, tablename);
    else if (eMode == BINARY_STREAM && filename != 0)
        theOutputStream = new BinaryFileStream(filename);
    else if (eMode == TCP_STREAM && inetAddr != 0)
        theOutputStream = new TCP_Stream(inetPort, inetAddr);
    else
        theOutputStream = new StandardStream();
    
    // set precision for stream
    theOutputStream->setPrecision(precision);
    
    // construct array of experimental setups
    theSetups = new ExperimentalSetup * [numSetups];
    if (theSetups == 0) {
        opserr << "WARNING out of memory creating experimental setup recorder\n";
        return 0;
    }
    for (int i = 0; i < numSetups; i++) {
        theSetups[i] = OPF_getExperimentalSetup(setupTags(i));
        if (theSetups[i] == 0) {
            opserr << "WARNING experimental setup with tag "
                << setupTags(i) << " not found\n";
            return 0;
        }
    }
    
    ExpSetupRecorder* recorder = new ExpSetupRecorder(numSetups, theSetups,
        data, nargrem, echoTimeFlag, *theOutputStream, deltaT);
    
    // cleanup dynamic memory
    if (data != 0)
        delete[] data;
    
    return recorder;
}


ExpSetupRecorder::ExpSetupRecorder(int numsetups,
    ExperimentalSetup** thesetups, const char** argv, int argc,
    bool echotime, OPS_Stream &theoutputstream, double deltat)
    : Recorder(RECORDER_TAGS_ExpSetupRecorder),
    numSetups(numsetups), theSetups(thesetups), responseArgs(0),
    numArgs(0), echoTime(echotime), theOutputStream(&theoutputstream),
    deltaT(deltat), theResponses(0), data(0), nextTimeStampToRecord(0.0)
{
    // create a copy of the response request
    responseArgs = new char* [argc];
    if (responseArgs == 0)  {
        opserr << "ExpSetupRecorder::ExpSetupRecorder() - out of memory\n";
        numSetups = 0;
    }
    for (int i=0; i<argc; i++)  {
        responseArgs[i] = new char [strlen(argv[i])+1];
        if (responseArgs[i] == 0)  {
            delete [] responseArgs;
            opserr << "ExpSetupRecorder::ExpSetupRecorder() - out of memory\n";
            numSetups = 0;
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
    //   2. iterate over the setups invoking setResponse() to get the new objects & determine size of data
    theResponses = new Response *[numSetups];
    if (theResponses == 0)  {
        opserr << "ExpSetupRecorder::ExpSetupRecorder() - out of memory\n";
        exit(OF_ReturnType_failed);
    }
    for (int i=0; i<numSetups; i++)
        theResponses[i] = 0;
    
    // loop over setups & set Responses
    for (int i=0; i<numSetups; i++)  {
        theResponses[i] = theSetups[i]->setResponse((const char **)responseArgs, numArgs, *theOutputStream);
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
        opserr << "ExpSetupRecorder::ExpSetupRecorder() - out of memory\n";
        exit(OF_ReturnType_failed);
    }
    
    theOutputStream->tag("Data");
    
    // record once at zero
    this->record(0, 0.0);
}


ExpSetupRecorder::~ExpSetupRecorder()
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
        for (int i=0; i<numSetups; i++)
            if (theResponses[i] != 0)
                delete theResponses[i];
        delete [] theResponses;
    }
    if (data != 0)
        delete data;
}


int ExpSetupRecorder::record(int commitTag, double timeStamp)
{
    int result = 0;
    if (deltaT == 0.0 || timeStamp >= nextTimeStampToRecord)  {
        
        if (deltaT != 0.0) 
            nextTimeStampToRecord = timeStamp + deltaT;
        
        int loc = 0;
        if (echoTime == true) 
            (*data)(loc++) = timeStamp;
        
        // for each setup if responses exist, put them in response vector
        for (int i=0; i<numSetups; i++)  {
            if (theResponses[i] != 0)  {
                // ask the setup for the response
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


int ExpSetupRecorder::restart()
{
    if (data != 0)
        data->Zero();
    
    return 0;
}
