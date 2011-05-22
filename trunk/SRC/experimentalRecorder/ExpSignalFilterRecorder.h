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

#ifndef ExpSignalFilterRecorder_h
#define ExpSignalFilterRecorder_h

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 08/08
// Revision: A
//
// Description: This file contains the class definition for
// ExpSignalFilterRecorder.

#include <Recorder.h>

class ExperimentalSignalFilter;
class Response;

#define RECORDER_TAGS_ExpSignalFilterRecorder 103

class ExpSignalFilterRecorder: public Recorder
{
public:
    // constructors
    ExpSignalFilterRecorder(int numSites, ExperimentalSignalFilter** theFilters,
        const char** argv, int argc, bool echoTime,
        OPS_Stream &theOutputStream, double deltaT = 0.0);
    
    // destructor
    ~ExpSignalFilterRecorder();
    
    int record(int commitTag, double timeStamp);
    int restart();
    
protected:

private:
    int numFilters;
    ExperimentalSignalFilter **theFilters;

    char **responseArgs;
    int numArgs;
    bool echoTime;
    OPS_Stream *theOutputStream;
    double deltaT;
    
    Response **theResponses;
    Vector *data;
    double nextTimeStampToRecord;
};

#endif
