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

#ifndef ExpControlRecorder_h
#define ExpControlRecorder_h

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 08/08
// Revision: A
//
// Description: This file contains the class definition for
// ExpControlRecorder.

#include <Recorder.h>

class ExperimentalControl;
class Response;

#define RECORDER_TAGS_ExpControlRecorder 102

class ExpControlRecorder: public Recorder
{
public:
    // constructors
    ExpControlRecorder(int numSites, ExperimentalControl** theControls,
        const char** argv, int argc, bool echoTime,
        OPS_Stream &theOutputStream, double deltaT = 0.0);
    
    // destructor
    ~ExpControlRecorder();
    
    int record(int commitTag, double timeStamp);
    int restart();
    
protected:

private:
    int numControls;
    ExperimentalControl **theControls;

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
