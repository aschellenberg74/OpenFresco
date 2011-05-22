/* ****************************************************************** **
**    OpenFRESCO - Open Framework                                     **
**                 for Experimental Setup and Control                 **
**                                                                    **
**                                                                    **
** Copyright (c) 2006, Yoshikazu Takahashi, Kyoto University          **
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
** Developed by:                                                      **
**   Yoshikazu Takahashi (yos@catfish.dpri.kyoto-u.ac.jp)             **
**   Andreas Schellenberg (andreas.schellenberg@gmx.net)              **
**   Gregory L. Fenves (fenves@berkeley.edu)                          **
**                                                                    **
** ****************************************************************** */

// $Revision$
// $Date$
// $URL$

#ifndef ECNIEseries_h
#define ECNIEseries_h

// Written: Yoshi (yos@catfish.dpri.kyoto-u.ac.jp)
// Created: 11/06
// Revision: A
//
// Description: This file contains the class definition for 
// ECNIEseries.

#include "ExperimentalControl.h"

#include <nidaqex.h>

class ECNIEseries : public ExperimentalControl
{
public:
    //constructors
    ECNIEseries(int tag, int device);
    ECNIEseries(const ECNIEseries& ast);

    //destructor
    virtual ~ECNIEseries();

    // method to get class type
    const char *getClassType() const {return "ECNIEseries";};
    
    //public method to set and to get response
    virtual int setup();
    virtual int setSize(ID sizeT, ID size0);

    virtual int setTrialResponse(const Vector* disp, 
        const Vector* vel,
        const Vector* accel,
        const Vector* force,
        const Vector* time);
    virtual int getDaqResponse(Vector* disp,
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
    //protected methods to set and to get response
    virtual int control();
    virtual int acquire();

private:
    int NIDAQMakeRampBuffer(void* pvBuffer, u32 ulNumPts,
        f64 mm_s, f64 mm_t, f64 u2V);
    
    void sleep(const clock_t wait);

    Vector *ctrlDisp;
    Vector *daqDisp, *daqForce;

    double actForce;
    double actDisp;

    // NI PCI-6036E setting
    i16 iDevice;     // id number of device

    f64 V_da;        // (V) : Single Amplitude of full range of DA
    i16 r_da;        // Range number of DA
    i16 nbit_da;     // bit of DA
    i16 *iChan_da;   // id number of da channel
    i16 *piChanVect; // vector of da channel
    i16 iGroup;      // id number of da group

    f64 *unit2Vol;   // transformation coef (unit -> Voltage)
    i16 factor;      // factor of control width
    f64 rate;        // signal generation rate (mm/s)
    f64 ctrl_w;      // control width (unit/pt)
    f64 dUpdateRate; // (pts/s)

    f64 V_ad;        // (V) : Single Amplitude of full range of AD
    i16 nbit_ad;     // bit of AD
    i16 *iChan_ad;   // id number array of ad channel
    f64 *dVoltage;   // voltage array of ad channel
    f64 *Vol2unit;   // transformation coef (Voltage -> unit)

    u32 ulIterations;
    i16 iFIFOMode;
    i16 iDelayMode;
    i16 iUpdateTB;
    u32 ulUpdateInt;
    i16 iWhichClock;
    i16 iUnits;
    i16 iWFMstopped;
    u32 ulItersDone;
    u32 ulPtsDone;
    i16 iOpSTART;
    i16 iOpCLEAR;
    i16 iIgnoreWarning;
    i16 iYieldON;
};

#endif
