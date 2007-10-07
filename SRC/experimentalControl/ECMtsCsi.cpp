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

// Written: 
// Created: 01/07
// Revision: A
//
// Description: This file contains the implementation of the ECMtsCsi class.

#include "ECMtsCsi.h"


// Three parameters will be passed to the constructor,
// 1. tag
// 2. MTS FlexTest Computer Simulation Interface configuration file name
// 3. ramp time
ECMtsCsi::ECMtsCsi(int tag, char *cfgfile, double ramptime)
    : ExperimentalControl(tag),
    CsiController(Mts::CsiFactory::newController()),
    cfgFile(cfgfile), rampTime(ramptime), rampId(-1),
    targDisp(0), measResp(0), measDisp(0), measForce(0),
    respSize(0)
{
    try
    {
        CsiController->loadConfiguration(cfgFile);
    }
    catch (const Mts::ICsiException& xcp)
    {
        opserr << xcp.what() << endln;
        exit(OF_ReturnType_failed);
    }
}


ECMtsCsi::ECMtsCsi(const ECMtsCsi& ec)
    : ExperimentalControl(ec),
    CsiController(Mts::CsiFactory::newController()),
    rampId(-1)
{
    rampTime = ec.rampTime;

    try
    {
        CsiController->loadConfiguration(ec.CsiController->getConfiguration().getFileName());
    }
    catch (const Mts::ICsiException& xcp)
    {
        opserr << xcp.what() << endln;
        exit(OF_ReturnType_failed);
    }
}


ECMtsCsi::~ECMtsCsi()
{
    // reset the csi-controller
    CsiController->reset();

    // delete the csi-controller
    if (CsiController != 0)
        delete CsiController;
    CsiController = 0;

    // delete memory of target vector
    if (targDisp != 0)
        delete targDisp;
    
    // delete memory of measured vectors
    if (measDisp != 0)
        delete measDisp;
    if (measForce != 0)
        delete measForce;
    if (measResp != 0)
        delete [] measResp;
}


int ECMtsCsi::setSize(ID sizeT, ID sizeO)
{
    if (sizeT[OF_Resp_Disp] == 0 ||
        sizeO[OF_Resp_Disp] == 0 ||
        sizeO[OF_Resp_Force] == 0) {
        opserr << "ECMtsCsi::setSize() - wrong sizeTrial/Out"; 
        opserr << "see User Manual.\n";
        exit(OF_ReturnType_failed);
    }

    *sizeCtrl = sizeT;
    *sizeDaq  = sizeO;

    // the loaded configuration must contain:
    //		1 control point
    //		1 degree of freedom (defining a displacement control channel)
    //		2 feedback signals (defining a displacement and force signal, respectively)
    
    Mts::ICsiConfiguration& cfg = CsiController->getConfiguration();

    if (cfg.getControlPoints().count() != 1)
    {
        opserr << "ECMtsCsi::setSize() - configuration must only "
            << "define one control point" << endln;
        exit(OF_ReturnType_failed);
    }

    Mts::ICsiControlPoint& ctrlPt = cfg.getControlPoints()[0];

    if (ctrlPt.getDegreesOfFreedom().count() != 1)
    {
        opserr << "ECMtsCsi::setSize() - configuration must only "
            << "define one degree of freedom" << endln;
        exit(OF_ReturnType_failed);
    }

    if (ctrlPt.getFeedbackSignals().count() != 2)
    {
        opserr << "ECMtsCsi::setSize() - configuration must only "
            << "define two feedback signals (displacement and force)" << endln;
        exit(OF_ReturnType_failed);
    }

    return OF_ReturnType_completed;
}


int ECMtsCsi::setup()
{
    if (targDisp != 0)
        delete targDisp;
    if (targForce != 0)
        delete targForce;

    if ((*sizeCtrl)[OF_Resp_Disp] != 0)  {
        targDisp = new Vector((*sizeCtrl)[OF_Resp_Disp]);
        targDisp->Zero();
    }
    if ((*sizeCtrl)[OF_Resp_Force] != 0)  {
        targForce = new Vector((*sizeCtrl)[OF_Resp_Force]);
        targForce->Zero();
    }

    if (measResp != 0)
        delete [] measResp;
    if (measDisp != 0)
        delete measDisp;
    if (measForce != 0)
        delete measForce;

    int id = 0;
    respSize = (*sizeDaq)[OF_Resp_Disp] + (*sizeDaq)[OF_Resp_Force];
    measResp = new double [respSize];
    measDisp = new Vector(&measResp[id],(*sizeDaq)[OF_Resp_Disp]);
    id += (*sizeDaq)[OF_Resp_Disp];
    measForce = new Vector(&measResp[id],(*sizeDaq)[OF_Resp_Force]);
    
    // print experimental control information
    this->Print(opserr);
    
    opserr << "****************************************************************\n";
    opserr << "* Make sure that offset values of controller are set to ZERO   *\n";
    opserr << "*                                                              *\n";
    opserr << "* Press 'Enter' to proceed or 'c' to cancel the initialization *\n";
    opserr << "****************************************************************\n";
    opserr << endln;
    int c = getchar();
    if (c == 'c')  {
        getchar();
        CsiController->reset();
        delete CsiController;
        exit(OF_ReturnType_failed);
    }

    // start the csi-controller
    try
    {
        CsiController->startHardware();
        CsiController->startTest();
    }
    catch (const Mts::ICsiException& xcp)
    {
        opserr << xcp.what() << endln;
        exit(OF_ReturnType_failed);
    }

	do  {
        this->control();
        this->acquire();
        
        int i;
        opserr << "**************************************\n";
        opserr << "* Initial values of DAQ are:         *\n";
        opserr << "*                                    *\n";
        opserr << "* dspDaq = [";
        for (i=0; i<(*sizeDaq)[OF_Resp_Disp]; i++)
            opserr << " " << measDisp[i];
        opserr << " ]\n";
        opserr << "* frcDaq = [";
        for (i=0; i<(*sizeDaq)[OF_Resp_Force]; i++)
            opserr << " " << measForce[i];
        opserr << " ]\n";
        opserr << "*                                    *\n";
        opserr << "* Press 'Enter' to start the test or *\n";
        opserr << "* 'r' to repeat the measurement or   *\n";
        opserr << "* 'c' to cancel the initialization   *\n";
        opserr << "**************************************\n";
        opserr << endln;
        c = getchar();
        if (c == 'c')  {
            getchar();
            CsiController->reset();
            delete CsiController;
            exit(OF_ReturnType_failed);
        } else if (c == 'r')  {
            getchar();
        }
    } while (c == 'r');
    
    opserr << "******************\n";
    opserr << "* Running......  *\n";
    opserr << "******************\n";
    opserr << endln;

    return OF_ReturnType_completed;
}


// User needs to handle the DOF mapping in this function. 
// In this example, only displacement control mode is used for
// DOF #1, we will map the (*disp)(0) to (*targDisp)(0).
int ECMtsCsi::setTrialResponse(const Vector* disp,
    const Vector* vel,
    const Vector* accel,
    const Vector* force,
    const Vector* time)
{
    *targDisp = *disp;
    if (theFilter != 0)  {
        for (int i=0; i<(*sizeCtrl)[OF_Resp_Disp]; i++)
            (*targDisp)(i) = theFilter->filtering((*targDisp)(i));
    }
    *targForce = *force;

    this->control();

    return OF_ReturnType_completed;
}


// User needs to handle the signal mapping in this function.
// In this example, we need to carefully map the signal list from the
// MTS FlexTest Computer Simulation Interface configuration to the actual
// signal vectors.
// In this demo configuration, we have two controller signals - displacement and force.
// We need to map the first signal to the displacement vector, and the second signal
// to the force vector. Mis-map the controller signals to the vectors could result 
// un-expected experimental control.
int ECMtsCsi::getDaqResponse(Vector* disp,
    Vector* vel,
    Vector* accel,
    Vector* force,
    Vector* time)
{
    this->acquire();

    *disp = *measDisp;
    *force = *measForce;

    return OF_ReturnType_completed;
}


int ECMtsCsi::commitState()
{
    return OF_ReturnType_completed;
}


ExperimentalControl *ECMtsCsi::getCopy()
{
    return new ECMtsCsi(*this);
}


void ECMtsCsi::Print(OPS_Stream &s, int flag)
{
    s << "****************************************************************\n";
    s << "* ExperimentalControl: " << this->getTag() << endln; 
    s << "* type: ECMtsCsi\n";
    s << "*   cfgFile: " << cfgFile << endln;
    s << "*   rampTime: " << rampTime << endln;
    if (theFilter != 0) {
        s << "*\tFilter: " << *theFilter << endln;
    }
    s << "****************************************************************\n";
    s << endln;
}


// To create a ramp object, user needs to
// 1. setup the ramp time
// 2. define actual ramp command for all DOFs
int ECMtsCsi::control()
{
    Mts::ICsiRamp* ramp = Mts::CsiFactory::newRamp();

    ramp->setWaitUntilCompletion(true);
    ramp->setChannelCount((*sizeCtrl)[OF_Resp_Disp]);
    ramp->setRampTime(rampTime);

    (*ramp)[0] = (*targDisp)(0);

    try 
    {
        rampId = CsiController->runRamp(ramp);
    }
    catch (const Mts::ICsiException& xcp)
    {
        opserr << xcp.what() << endln;
        exit(OF_ReturnType_failed);
    }

    return OF_ReturnType_completed;
}


// acquire function acquires a list of the feedback signals, which are 
// pre-defined in the MTS FlexTest Computer Simulation Interface configuration.
int ECMtsCsi::acquire()
{
    try
    {
        CsiController->acquireFeedback(rampId, measResp);
    }
    catch (const Mts::ICsiException& xcp)
    {
        opserr << xcp.what() << endln;
        exit(OF_ReturnType_failed);
    }

    return OF_ReturnType_completed;
}
