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
    targDisp(0), measResp(0), measDisp(0), measForce(0),
    respSize(0)
{
    try  {
        CsiController->loadConfiguration(cfgFile);
    }
    catch (const Mts::ICsiException& xcp)  {
        opserr << xcp.what() << endln;
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
    targDisp(0), measResp(0), measDisp(0), measForce(0),
    respSize(0)
{
    cfgFile = ec.cfgFile;
    rampTime = ec.rampTime;

    try  {
        CsiController->loadConfiguration(ec.CsiController->getConfiguration().getFileName());
    }
    catch (const Mts::ICsiException& xcp)  {
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
    
    // delete memory of target vectors
    if (targDisp != 0)
        delete targDisp;
    if (targForce != 0)
        delete targForce;
    
    // delete memory of measured vectors
    if (measDisp != 0)
        delete measDisp;
    if (measForce != 0)
        delete measForce;
    if (measResp != 0)
        delete [] measResp;
    
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
    
    if (targDisp != 0)
        delete targDisp;
    if (targForce != 0)
        delete targForce;
    
    if ((*sizeCtrl)(OF_Resp_Disp) != 0)  {
        targDisp = new Vector((*sizeCtrl)(OF_Resp_Disp));
        targDisp->Zero();
    }
    if ((*sizeCtrl)(OF_Resp_Force) != 0)  {
        targForce = new Vector((*sizeCtrl)(OF_Resp_Force));
        targForce->Zero();
    }
    
    if (measDisp != 0)
        delete measDisp;
    if (measForce != 0)
        delete measForce;
    if (measResp != 0)
        delete [] measResp;
    
    int id = 0;
    respSize = (*sizeDaq)(OF_Resp_Disp) + (*sizeDaq)(OF_Resp_Force);
    measResp = new double [respSize];
    measDisp = new Vector(&measResp[id],(*sizeDaq)(OF_Resp_Disp));
    id += (*sizeDaq)(OF_Resp_Disp);
    measForce = new Vector(&measResp[id],(*sizeDaq)(OF_Resp_Force));
    
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
        opserr << xcp.what() << endln;
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
            opserr << " " << measDisp[i];
        opserr << " ]\n";
        opserr << "* frcDaq = [";
        for (i=0; i<(*sizeDaq)(OF_Resp_Force); i++)
            opserr << " " << measForce[i];
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
// DOF #1, we will map the (*disp)(0) to (*targDisp)(0).
int ECMtsCsi::setTrialResponse(const Vector* disp,
    const Vector* vel,
    const Vector* accel,
    const Vector* force,
    const Vector* time)
{
    int i, rValue = 0;
    if (disp != 0)  {
        *targDisp = *disp;
        if (theCtrlFilters[OF_Resp_Disp] != 0)  {
            for (i=0; i<(*sizeCtrl)(OF_Resp_Disp); i++)
                (*targDisp)(i) = theCtrlFilters[OF_Resp_Disp]->filtering((*targDisp)(i));
        }
    }
    if (force != 0)  {
        *targForce = *force;
        if (theCtrlFilters[OF_Resp_Force] != 0)  {
            for (i=0; i<(*sizeCtrl)(OF_Resp_Force); i++)
                (*targForce)(i) = theCtrlFilters[OF_Resp_Force]->filtering((*targForce)(i));
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
                (*measDisp)(i) = theDaqFilters[OF_Resp_Disp]->filtering((*measDisp)(i));
        }
        *disp = *measDisp;
    }
    if (force != 0)  {
        if (theDaqFilters[OF_Resp_Force] != 0)  {
            for (i=0; i<(*sizeDaq)(OF_Resp_Force); i++)
                (*measForce)(i) = theDaqFilters[OF_Resp_Force]->filtering((*measForce)(i));
        }
        *force = *measForce;
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
        
    // target displacements
    if (strcmp(argv[0],"targDisp") == 0 ||
        strcmp(argv[0],"targetDisp") == 0 ||
        strcmp(argv[0],"targetDisplacement") == 0 ||
        strcmp(argv[0],"targetDisplacements") == 0)
    {
        for (i=0; i<(*sizeCtrl)(OF_Resp_Disp); i++)  {
            sprintf(outputData,"targDisp%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpControlResponse(this, 1,
            Vector((*sizeCtrl)(OF_Resp_Disp)));
    }
    
    // target forces
    if (strcmp(argv[0],"targForce") == 0 ||
        strcmp(argv[0],"targetForce") == 0 ||
        strcmp(argv[0],"targetForces") == 0)
    {
        for (i=0; i<(*sizeCtrl)(OF_Resp_Force); i++)  {
            sprintf(outputData,"targForce%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpControlResponse(this, 2,
            Vector((*sizeCtrl)(OF_Resp_Force)));
    }
    
    // measured displacements
    if (strcmp(argv[0],"measDisp") == 0 ||
        strcmp(argv[0],"measuredDisp") == 0 ||
        strcmp(argv[0],"measuredDisplacement") == 0 ||
        strcmp(argv[0],"measuredDisplacements") == 0)
    {
        for (i=0; i<(*sizeDaq)(OF_Resp_Disp); i++)  {
            sprintf(outputData,"measDisp%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpControlResponse(this, 3,
            Vector((*sizeDaq)(OF_Resp_Disp)));
    }
    
    // measured forces
    if (strcmp(argv[0],"measForce") == 0 ||
        strcmp(argv[0],"measuredForce") == 0 ||
        strcmp(argv[0],"measuredForces") == 0)
    {
        for (i=0; i<(*sizeDaq)(OF_Resp_Force); i++)  {
            sprintf(outputData,"measForce%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpControlResponse(this, 4,
            Vector((*sizeDaq)(OF_Resp_Force)));
    }
    
    output.endTag();
    
    return theResponse;
}


int ECMtsCsi::getResponse(int responseID, Information &info)
{
    switch (responseID)  {
    case 1:  // target displacements
        return info.setVector(*targDisp);
        
    case 2:  // target forces
        return info.setVector(*targForce);
        
    case 3:  // measured displacements
        return info.setVector(*measDisp);
        
    case 4:  // measured forces
        return info.setVector(*measForce);
        
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
        if (theCtrlFilters[i] != 0)
            s << " " << theCtrlFilters[i]->getTag();
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
		(*ramp)[i] = (*targDisp)(i);

    try  {
        rampId = CsiController->runRamp(ramp);
    }
    catch (const Mts::ICsiException& xcp)  {
        opserr << xcp.what() << endln;
        exit(OF_ReturnType_failed);
    }

    return OF_ReturnType_completed;
}


// acquire function acquires a list of the feedback signals, which are 
// pre-defined in the MTS FlexTest Computer Simulation Interface configuration.
int ECMtsCsi::acquire()
{
    try  {
        CsiController->acquireFeedback(rampId, measResp);
    }
    catch (const Mts::ICsiException& xcp)  {
        opserr << xcp.what() << endln;
        exit(OF_ReturnType_failed);
    }

    return OF_ReturnType_completed;
}
