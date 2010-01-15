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

// $Revision: 133 $
// $Date: 2008-02-06 09:25:00 -0800 (Wed, 06 Feb 2008) $
// $Source: $

// Written: MTS Systems Corporation
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
    ctrlDisp(0), daqResp(0), daqDisp(0), daqForce(0),
    respSize(0)
{
    try  {
        CsiController->loadConfiguration(cfgFile);
    }
    catch (const Mts::ICsiException& xcp)  {
        opserr << "ECMtsCsi::ECMtsCsi() - "
            << "loadConfiguration: error = " << xcp.what() << endln;
        exit(OF_ReturnType_failed);
    }

    opserr << "****************************************************************\n";
    opserr << "* The following CSI configuration file has been loaded:\n";
    opserr << "* " << cfgFile << endln;
    opserr << "****************************************************************\n";
    opserr << endln;    
}


ECMtsCsi::ECMtsCsi(const ECMtsCsi& ec)
    : ExperimentalControl(ec),
    CsiController(Mts::CsiFactory::newController()),
    rampId(-1),
    ctrlDisp(0), daqResp(0), daqDisp(0), daqForce(0),
    respSize(0)
{
    cfgFile = ec.cfgFile;
    rampTime = ec.rampTime;

    try  {
        CsiController->loadConfiguration(ec.CsiController->getConfiguration().getFileName());
    }
    catch (const Mts::ICsiException& xcp)  {
        opserr << "ECMtsCsi::ECMtsCsi() - "
            << "loadConfiguration: error = " << xcp.what() << endln;
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
    
    // delete memory of ctrl vectors
    if (ctrlDisp != 0)
        delete ctrlDisp;
    if (ctrlForce != 0)
        delete ctrlForce;
    
    // delete memory of daq vectors
    if (daqDisp != 0)
        delete daqDisp;
    if (daqForce != 0)
        delete daqForce;
    if (daqResp != 0)
        delete [] daqResp;
    
    // delete memory of string
    if (cfgFile != 0)
        delete [] cfgFile;

    opserr << endln;
    opserr << "*************************************\n";
    opserr << "* The CSI controller has been reset *\n";
    opserr << "*************************************\n";
    opserr << endln;
}


int ECMtsCsi::setup()
{
    int rValue = 0;
    
    if (ctrlDisp != 0)
        delete ctrlDisp;
    if (ctrlForce != 0)
        delete ctrlForce;
    
    if ((*sizeCtrl)(OF_Resp_Disp) != 0)  {
        ctrlDisp = new Vector((*sizeCtrl)(OF_Resp_Disp));
        ctrlDisp->Zero();
    }
    if ((*sizeCtrl)(OF_Resp_Force) != 0)  {
        ctrlForce = new Vector((*sizeCtrl)(OF_Resp_Force));
        ctrlForce->Zero();
    }
    
    if (daqDisp != 0)
        delete daqDisp;
    if (daqForce != 0)
        delete daqForce;
    if (daqResp != 0)
        delete [] daqResp;
    
    int id = 0;
    respSize = (*sizeDaq)(OF_Resp_Disp) + (*sizeDaq)(OF_Resp_Force);
    daqResp = new double [respSize];
    daqDisp = new Vector(&daqResp[id],(*sizeDaq)(OF_Resp_Disp));
    id += (*sizeDaq)(OF_Resp_Disp);
    daqForce = new Vector(&daqResp[id],(*sizeDaq)(OF_Resp_Force));
    
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
    try  {
        CsiController->startHardware();
        CsiController->startTest();
    }
    catch (const Mts::ICsiException& xcp)  {
        opserr << "ECMtsCsi::setup() - "
            << "startHardware: error = " << xcp.what() << endln;
        exit(OF_ReturnType_failed);
    }
    
	do  {
        rValue += this->control();
        rValue += this->acquire();
        
        int i;
        opserr << "****************************************************************\n";
        opserr << "* Initial values of DAQ are:\n";
        opserr << "*\n";
        opserr << "* dspDaq = [";
        for (i=0; i<(*sizeDaq)(OF_Resp_Disp); i++)
            opserr << " " << daqDisp[i];
        opserr << " ]\n";
        opserr << "* frcDaq = [";
        for (i=0; i<(*sizeDaq)(OF_Resp_Force); i++)
            opserr << " " << daqForce[i];
        opserr << " ]\n";
        opserr << "*\n";
        opserr << "* Press 'Enter' to start the test or\n";
        opserr << "* 'r' to repeat the measurement or\n";
        opserr << "* 'c' to cancel the initialization\n";
        opserr << "****************************************************************\n";
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
    
    opserr << "*****************\n";
    opserr << "* Running...... *\n";
    opserr << "*****************\n";
    opserr << endln;
    
    return rValue;
}


int ECMtsCsi::setSize(ID sizeT, ID sizeO)
{
    if (sizeT(OF_Resp_Disp) == 0 ||
        sizeO(OF_Resp_Disp) == 0 ||
        sizeO(OF_Resp_Force) == 0) {
            opserr << "ECMtsCsi::setSize() - wrong sizeTrial/Out"; 
            opserr << "see User Manual.\n";
            exit(OF_ReturnType_failed);
    }

    *sizeCtrl = sizeT;
    *sizeDaq  = sizeO;

    // the loaded configuration must contain:
    //		at least 1 control point
    //		at least 1 degree of freedom (defining a displacement control channel)
    //         in each control point
    //		and 2 feedback signals (defining a displacement and force signal, respectively)
    //		   per degree of freedom (all displacement signals must be first in the array
    //         followed by all force signals)

    Mts::ICsiConfiguration& cfg = CsiController->getConfiguration();

    opserr << "using MtsCsi configuration file '" << cfg.getFileName() << "'.\n";

    if (cfg.getControlPoints().count() < 1)  {
        opserr << "ECMtsCsi::setSize() - configuration must "
            << "define at least one control point.\n";
        exit(OF_ReturnType_failed);
    }

    int numDoFs = 0;
    int numFdbkSigs = 0;

    for (int i=0; i<cfg.getControlPoints().count(); i++)  {

        Mts::ICsiControlPoint& ctrlPt = cfg.getControlPoints()[i];

        if (ctrlPt.getDegreesOfFreedom().count() < 1)  {
            opserr << "ECMtsCsi::setSize() - configuration define "
                << "at least one degree of freedom per control point.\n";
            exit(OF_ReturnType_failed);
        }

        numDoFs += ctrlPt.getDegreesOfFreedom().count();

        if (ctrlPt.getFeedbackSignals().count() != (ctrlPt.getDegreesOfFreedom().count() * 2))  {
            opserr << "ECMtsCsi::setSize() - configuration must define "
                << "at least two feedback signals (displacement and force) " 
                << "per degree of freedom.\n";
            exit(OF_ReturnType_failed);
        }

        numFdbkSigs += ctrlPt.getFeedbackSignals().count();
    }

    opserr << "MtsCsi configuration: " << numDoFs << " degrees of freedom; " 
        << numFdbkSigs << " feedback signals.\n";

    int numTrial  = sizeT(OF_Resp_Disp);
    int numOutput = sizeO(OF_Resp_Disp) + sizeO(OF_Resp_Force);

    if (numTrial != numDoFs)  {
        opserr << "warning: ECMtsCsi::setSize() - specified trial size (" << numTrial << ") does not "
            << "match total number of degrees of freedom (" << numDoFs << ") defined in the "
            << "configuration.\n";
    }

    if (numOutput != numFdbkSigs)  {
        opserr << "warning: ECMtsCsi::setSize() - specified output size (" << numOutput << ") does not "
            << "match total number of feedback signals (" << numFdbkSigs << ") defined in the "
            << "configuration.\n";
    }

    return OF_ReturnType_completed;
}


// User needs to handle the DOF mapping in this function. 
// In this example, only displacement control mode is used for
// DOF #1, we will map the (*disp)(0) to (*ctrlDisp)(0).
int ECMtsCsi::setTrialResponse(const Vector* disp,
    const Vector* vel,
    const Vector* accel,
    const Vector* force,
    const Vector* time)
{
    int i, rValue = 0;
    if (disp != 0)  {
        *ctrlDisp = *disp;
        if (theCtrlFilters[OF_Resp_Disp] != 0)  {
            for (i=0; i<(*sizeCtrl)(OF_Resp_Disp); i++)
                (*ctrlDisp)(i) = theCtrlFilters[OF_Resp_Disp]->filtering((*ctrlDisp)(i));
        }
    }
    if (force != 0)  {
        *ctrlForce = *force;
        if (theCtrlFilters[OF_Resp_Force] != 0)  {
            for (i=0; i<(*sizeCtrl)(OF_Resp_Force); i++)
                (*ctrlForce)(i) = theCtrlFilters[OF_Resp_Force]->filtering((*ctrlForce)(i));
        }
    }

    rValue = this->control();

    return rValue;
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

    int i;
    if (disp != 0)  {
        if (theDaqFilters[OF_Resp_Disp] != 0)  {
            for (i=0; i<(*sizeDaq)(OF_Resp_Disp); i++)
                (*daqDisp)(i) = theDaqFilters[OF_Resp_Disp]->filtering((*daqDisp)(i));
        }
        *disp = *daqDisp;
    }
    if (force != 0)  {
        if (theDaqFilters[OF_Resp_Force] != 0)  {
            for (i=0; i<(*sizeDaq)(OF_Resp_Force); i++)
                (*daqForce)(i) = theDaqFilters[OF_Resp_Force]->filtering((*daqForce)(i));
        }
        *force = *daqForce;
    }

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


Response* ECMtsCsi::setResponse(const char **argv, int argc,
    OPS_Stream &output)
{
    int i;
    char outputData[15];
    Response *theResponse = 0;
    
    output.tag("ExpControlOutput");
    output.attr("ctrlType",this->getClassType());
    output.attr("ctrlTag",this->getTag());
    
    // ctrl displacements
    if (ctrlDisp != 0 && (
        strcmp(argv[0],"ctrlDisp") == 0 ||
        strcmp(argv[0],"ctrlDisplacement") == 0 ||
        strcmp(argv[0],"ctrlDisplacements") == 0))
    {
        for (i=0; i<(*sizeCtrl)(OF_Resp_Disp); i++)  {
            sprintf(outputData,"ctrlDisp%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpControlResponse(this, 1, *ctrlDisp);
    }
    
    // ctrl forces
    if (ctrlForce != 0 && (
        strcmp(argv[0],"ctrlForce") == 0 ||
        strcmp(argv[0],"ctrlForces") == 0))
    {
        for (i=0; i<(*sizeCtrl)(OF_Resp_Force); i++)  {
            sprintf(outputData,"ctrlForce%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpControlResponse(this, 2, *ctrlForce);
    }
    
    // daq displacements
    if (daqDisp != 0 && (
        strcmp(argv[0],"daqDisp") == 0 ||
        strcmp(argv[0],"daqDisplacement") == 0 ||
        strcmp(argv[0],"daqDisplacements") == 0))
    {
        for (i=0; i<(*sizeDaq)(OF_Resp_Disp); i++)  {
            sprintf(outputData,"daqDisp%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpControlResponse(this, 3, *daqDisp);
    }
    
    // daq forces
    if (daqForce != 0 && (
        strcmp(argv[0],"daqForce") == 0 ||
        strcmp(argv[0],"daqForces") == 0))
    {
        for (i=0; i<(*sizeDaq)(OF_Resp_Force); i++)  {
            sprintf(outputData,"daqForce%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpControlResponse(this, 4, *daqForce);
    }
    
    output.endTag();
    
    return theResponse;
}


int ECMtsCsi::getResponse(int responseID, Information &info)
{
    switch (responseID)  {
    case 1:  // ctrl displacements
        return info.setVector(*ctrlDisp);
        
    case 2:  // ctrl forces
        return info.setVector(*ctrlForce);
        
    case 3:  // daq displacements
        return info.setVector(*daqDisp);
        
    case 4:  // daq forces
        return info.setVector(*daqForce);
        
    default:
        return -1;
    }
}


void ECMtsCsi::Print(OPS_Stream &s, int flag)
{
    s << "****************************************************************\n";
    s << "* ExperimentalControl: " << this->getTag() << endln; 
    s << "*   type: ECMtsCsi\n";
    s << "*   cfgFile: " << cfgFile << endln;
    s << "*   rampTime: " << rampTime << endln;
    s << "*   ctrlFilters:";
    for (int i=0; i<OF_Resp_All; i++)  {
        if (theCtrlFilters[i] != 0)
            s << " " << theCtrlFilters[i]->getTag();
        else
            s << " 0";
    }
    s << "\n*   daqFilters:";
    for (int i=0; i<OF_Resp_All; i++)  {
        if (theDaqFilters[i] != 0)
            s << " " << theDaqFilters[i]->getTag();
        else
            s << " 0";
    }
    s << endln;
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
    ramp->setChannelCount((*sizeCtrl)(OF_Resp_Disp));
    ramp->setRampTime(rampTime);

    for (int i=0; i<(*sizeCtrl)(OF_Resp_Disp); i++)
		(*ramp)[i] = (*ctrlDisp)(i);

    try  {
        rampId = CsiController->runRamp(ramp);
    }
    catch (const Mts::ICsiException& xcp)  {
        opserr << "ECMtsCsi::control() - "
            << "runRamp: error = " << xcp.what() << endln;
        exit(OF_ReturnType_failed);
    }

    return OF_ReturnType_completed;
}


// acquire function acquires a list of the feedback signals, which are 
// pre-defined in the MTS FlexTest Computer Simulation Interface configuration.
int ECMtsCsi::acquire()
{
    try  {
        CsiController->acquireFeedback(rampId, daqResp);
    }
    catch (const Mts::ICsiException& xcp)  {
        opserr << "ECMtsCsi::acquire() - "
            << "acquireFeedback: error = " << xcp.what() << endln;
        exit(OF_ReturnType_failed);
    }

    return OF_ReturnType_completed;
}
