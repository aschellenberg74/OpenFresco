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

// $Revision: $
// $Date: $
// $Source: $

#ifndef ECMtsCsi_h
#define ECMtsCsi_h

// Written: 
// Created: 01/07
// Revision: A
//
// Description: This file contains the class definition for ECMtsCsi.
// ECMtsCsi is a controller class for communicating with MTS hardware
// through the MTS Computer Simulation Interface (MTSCsi).

#include "ExperimentalControl.h"

#include <MtsCsi.h>

class ECMtsCsi : public ExperimentalControl
{
public:
    // constructors
    ECMtsCsi(int tag, char *cfgFile, double rampTime);
    ECMtsCsi(const ECMtsCsi& ec);
    
    // destructor
    virtual ~ECMtsCsi();
    
    // public methods to set and to get response
    virtual int setSize(ID sizeT, ID sizeO);
    virtual int setup();
    
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

    // public methods for output
	void Print(OPS_Stream &s, int flag = 0);
    
protected:
    // protected methods to set and to get response
    virtual int control();
    virtual int acquire();

private:
	Mts::ICsiController* CsiController;
	
    char *cfgFile;
	double rampTime;
    int	rampId;
	
    Vector *targDisp;
    double *measResp;
    Vector *measDisp, *measForce;

    int respSize;
};

#endif
