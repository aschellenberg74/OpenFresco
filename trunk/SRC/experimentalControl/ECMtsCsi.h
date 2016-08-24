/* ****************************************************************** **
**    OpenFRESCO - Open Framework                                     **
**                 for Experimental Setup and Control                 **
**                                                                    **
**                                                                    **
** Copyright (c) 2006, MTS Systems Corporation                        **
** All rights reserved.                                               **
**                                                                    **
** Licensed under the modified BSD License (the "License");           **
** you may not use this file except in compliance with the License.   **
** You may obtain a copy of the License in main directory.            **
** Unless required by applicable law or agreed to in writing,         **
** software distributed under the License is distributed on an        **
** "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,       **
** either express or implied. See the License for the specific        **
** language governing permissions and limitations under the License.  **
**                                                                    **
** ****************************************************************** */

// $Revision$
// $Date$
// $URL$

#ifndef ECMtsCsi_h
#define ECMtsCsi_h

// Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
// Created: 01/07
// Revision: A
//
// Description: This file contains the class definition for ECMtsCsi.
// ECMtsCsi is a controller class for communicating with MTS hardware
// through the MTS Computer Simulation Interface (MTSCsi).

#include "ExperimentalControl.h"

#include <MtsCsi.h>

class ExperimentalCP;

class ECMtsCsi : public ExperimentalControl
{
public:
    // constructors
    ECMtsCsi(int tag,
        int nTrialCPs, ExperimentalCP **trialCPs,
        int nOutCPs, ExperimentalCP **outCPs,
        char *cfgFile, double rampTime = 0.1,
        int useRelativeTrial = 0);
    ECMtsCsi(const ECMtsCsi& ec);
    
    // destructor
    virtual ~ECMtsCsi();
    
    // method to get class type
    const char *getClassType() const {return "ECMtsCsi";};
    
    // public methods to set and to get response
    virtual int setup();
    virtual int setSize(ID sizeT, ID sizeO);
    
    virtual int setTrialResponse(
        const Vector* disp,
        const Vector* vel,
        const Vector* accel,
        const Vector* force,
        const Vector* time);
    virtual int getDaqResponse(
        Vector* disp,
        Vector* vel,
        Vector* accel,
        Vector* force,
        Vector* time);
    
    virtual int commitState();
    
    virtual ExperimentalControl *getCopy();
    
    // public methods for experimental control recorder
    virtual Response *setResponse(const char **argv, int argc,
        OPS_Stream &output);
    virtual int getResponse(int responseID, Information &info);
    
    // public methods for output
    void Print(OPS_Stream &s, int flag = 0);

protected:
    // protected methods to set and to get response
    virtual int control();
    virtual int acquire();

private:
    Mts::ICsiController* CsiController;
    
    int numTrialCPs;            // number of trial control points
    ExperimentalCP **trialCPs;  // trial control points
    int numOutCPs;              // number of output control points
    ExperimentalCP **outCPs;    // output control points
    char *cfgFile;              // CSI controller configuration file
    double rampTime;            // time to ramp signals to new targets
    
    int numCtrlSignals, numDaqSignals;       // number of signals
    double *ctrlSignal, *daqSignal;          // signal arrays
    Vector ctrlSigOffset, daqSigOffset;      // signal offsets (i.e. setpoints)
    Vector trialSigOffset;                   // trial signal offsets
    int useRelativeTrial, gotRelativeTrial;  // relative trial signal flags
    
    int rampId;  // set this to -1 to get current feedback
};

#endif
