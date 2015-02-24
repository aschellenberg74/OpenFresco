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
// $Source: $

// Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
// Created: 01/07
// Revision: A
//
// Description: This file contains the implementation of the ECMtsCsi class.

#include "ECMtsCsi.h"
#include <ExperimentalCP.h>


ECMtsCsi::ECMtsCsi(int tag, char *cfgfile, double ramptime,
    int nTrialCPs, ExperimentalCP **trialcps,
    int nOutCPs, ExperimentalCP **outcps)
    : ExperimentalControl(tag),
    CsiController(Mts::CsiFactory::newController()),
    cfgFile(cfgfile), rampTime(ramptime),
    numTrialCPs(nTrialCPs), numOutCPs(nOutCPs),
    numCtrlSignals(0), numDaqSignals(0),
    ctrlSignal(0), daqSignal(0),
    ctrlSigOffset(0), daqSigOffset(0),
    rampId(-1)
{
    // get trial and output control points
    if (trialcps == 0 || outcps == 0)  {
      opserr << "ECSimDomain::ECSimDomain() - "
          << "null trialCPs or outCPs array passed.\n";
      exit(OF_ReturnType_failed);
    }
    trialCPs = trialcps;
    outCPs = outcps;
    
    // get total number of control and daq signals and
    // check if any signals use relative reference
    int ctrlIsRelative = 0;
    for (int i=0; i<numTrialCPs; i++)  {
        int numSignals = trialCPs[i]->getNumSignal();
        numCtrlSignals += numSignals;
        
        int j = 0;
        ID isRel = trialCPs[i]->getSigRefType();
        while (!ctrlIsRelative && j < numSignals)
            ctrlIsRelative = isRel(j++);
    }
    int daqIsRelative = 0;
    for (int i=0; i<numOutCPs; i++)  {
        int numSignals = outCPs[i]->getNumSignal();
        numDaqSignals += numSignals;
        
        int j = 0;
        ID isRel = outCPs[i]->getSigRefType();
        while (!daqIsRelative && j < numSignals)
            daqIsRelative = isRel(j++);
    }
    
    // resize offsets if any signals use relative reference
    if (ctrlIsRelative)
        ctrlSigOffset.resize(numCtrlSignals);
    if (daqIsRelative)
        daqSigOffset.resize(numDaqSignals);
    
    // load configuration file for CSI controller
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
    numCtrlSignals(0), numDaqSignals(0),
    ctrlSignal(0), daqSignal(0),
    ctrlSigOffset(0), daqSigOffset(0),
    rampId(-1)
{
    cfgFile     = ec.cfgFile;
    rampTime    = ec.rampTime;
    numTrialCPs = ec.numTrialCPs;
    trialCPs    = ec.trialCPs;
    numOutCPs   = ec.numOutCPs;
    outCPs      = ec.outCPs;
    
    numCtrlSignals = ec.numCtrlSignals;
    numDaqSignals  = ec.numDaqSignals;
    ctrlSigOffset  = ec.ctrlSigOffset;
    daqSigOffset   = ec.daqSigOffset;
    
    // load configuration file for CSI controller
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
    
    // delete memory of signal vectors
    if (ctrlSignal != 0)
        delete [] ctrlSignal;
    if (daqSignal != 0)
        delete [] daqSignal;
    
    // delete memory of control points
    int i;
    if (trialCPs != 0)  {
        for (i=0; i<numTrialCPs; i++)  {
            if (trialCPs[i] != 0)
                delete trialCPs[i];
        }
        delete [] trialCPs;
    }
    if (outCPs != 0)  {
        for (i=0; i<numOutCPs; i++)  {
            if (outCPs[i] != 0)
                delete outCPs[i];
        }
        delete [] outCPs;
    }
    
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
    
    if (ctrlSignal != 0)
        delete [] ctrlSignal;
    if (daqSignal != 0)
        delete [] daqSignal;
    
    // create control signal array
    ctrlSignal = new double [numCtrlSignals];
    if (ctrlSignal == 0)  {
        opserr << "ECMtsCsi::setup() - failed to create ctrlSignal array.\n";
        CsiController->reset();
        delete CsiController;
        exit(OF_ReturnType_failed);
    }
    for (int i=0; i<numCtrlSignals; i++)
        ctrlSignal[i] = 0.0;
    
    // create daq signal array
    daqSignal = new double [numDaqSignals];
    if (daqSignal == 0)  {
        opserr << "ECMtsCsi::setup() - failed to create daqSignal array.\n";
        CsiController->reset();
        delete CsiController;
        exit(OF_ReturnType_failed);
    }
    for (int i=0; i<numDaqSignals; i++)
        daqSignal[i] = 0.0;
    
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
        CsiController->reset();
        delete CsiController;
        exit(OF_ReturnType_failed);
    }
    
    do  {
        //rValue += this->control();  // CHECK IF WE NEED THIS, CHECK rampId
        rValue += this->acquire();
        
        if (ctrlSigOffset.Size() > 0)  {
            int kT = 0;
            for (int iT=0; iT<numTrialCPs; iT++)  {
                // get trial control point parameters
                int numSigT = trialCPs[iT]->getNumSignal();
                ID dofT = trialCPs[iT]->getDOF();
                ID rspT = trialCPs[iT]->getRspType();
                ID isRelT = trialCPs[iT]->getSigRefType();
                // loop through all the trial control point signals
                for (int jT=0; jT<numSigT; jT++)  {
                    if (isRelT(jT))  {
                        // now search through ouput control points to
                        // find the signal with the same DOF and rspType
                        int kO = 0;
                        for (int iO=0; iO<numOutCPs; iO++)  {
                            int numSigO = outCPs[iO]->getNumSignal();
                            ID dofO = outCPs[iO]->getDOF();
                            ID rspO = outCPs[iO]->getRspType();
                            // loop through all the output control point signals
                            for (int jO=0; jO<numSigO; jO++)  {
                                if (dofT(jT)==dofO(jO) && rspT(jT)==rspO(jO))
                                    ctrlSigOffset(kT) = daqSignal[kO];
                                kO++;
                            }
                        }
                    }
                    kT++;
                }
            }
        }
        if (daqSigOffset.Size() > 0)  {
            int k = 0;
            for (int i=0; i<numOutCPs; i++)  {
                // get output control point parameters
                int numSignals = outCPs[i]->getNumSignal();
                ID isRel = outCPs[i]->getSigRefType();
                // loop through all the output control point signals
                for (int j=0; j<numSignals; j++)  {
                    if (isRel(j))  {
                        daqSigOffset(k) = -daqSignal[k];
                        daqSignal[k] = 0.0;
                    }
                    k++;
                }
            }
        }
        
        int i;
        opserr << "****************************************************************\n";
        opserr << "* Initial signal values of DAQ are:\n";
        opserr << "*\n";
        for (i=0; i<numDaqSignals; i++)
            opserr << "*   s" << i << " = " << daqSignal[i] << endln;
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
    // check sizeTrial and sizeOut against sizes
    // specified in the control points
    // ECMtsCsi objects can use:
    //     disp, vel, accel, force and time for trial and
    //     disp, vel, accel, force and time for output
    
    // get maximum dof IDs for each trial response quantity
    int mdfTDisp = 0, mdfTForce = 0, mdfTTime = 0, mdfTVel = 0, mdfTAccel = 0;
    for (int i=0; i<numTrialCPs; i++)  {
        // get trial control point parameters
        int numSignals = trialCPs[i]->getNumSignal();
        ID dof = trialCPs[i]->getDOF();
        ID rsp = trialCPs[i]->getRspType();
        
        // loop through all the trial control point signals
        for (int j=0; j<numSignals; j++)  {
            if (rsp(j) == OF_Resp_Disp)
                mdfTDisp = dof(j) > mdfTDisp ? dof(j) : mdfTDisp;
            else if (rsp(j) == OF_Resp_Force)
                mdfTForce = dof(j) > mdfTForce ? dof(j) : mdfTForce;
            else if (rsp(j) == OF_Resp_Time)
                mdfTTime = dof(j) > mdfTTime ? dof(j) : mdfTTime;
            else if (rsp(j) == OF_Resp_Vel)
                mdfTVel = dof(j) > mdfTVel ? dof(j) : mdfTVel;
            else if (rsp(j) == OF_Resp_Accel)
                mdfTAccel = dof(j) > mdfTAccel ? dof(j) : mdfTAccel;
        }
    }
    // get maximum dof IDs for each output response quantity
    int mdfODisp = 0, mdfOForce = 0, mdfOTime = 0, mdfOVel = 0, mdfOAccel = 0;
    for (int i=0; i<numOutCPs; i++)  {
        // get output control point parameters
        int numSignals = outCPs[i]->getNumSignal();
        ID dof = outCPs[i]->getDOF();
        ID rsp = outCPs[i]->getRspType();
        
        // loop through all the output control point signals
        for (int j=0; j<numSignals; j++)  {
            if (rsp(j) == OF_Resp_Disp)
                mdfODisp = dof(j) > mdfODisp ? dof(j) : mdfODisp;
            else if (rsp(j) == OF_Resp_Force)
                mdfOForce = dof(j) > mdfOForce ? dof(j) : mdfOForce;
            else if (rsp(j) == OF_Resp_Time)
                mdfOTime = dof(j) > mdfOTime ? dof(j) : mdfOTime;
            else if (rsp(j) == OF_Resp_Vel)
                mdfOVel = dof(j) > mdfOVel ? dof(j) : mdfOVel;
            else if (rsp(j) == OF_Resp_Accel)
                mdfOAccel = dof(j) > mdfOAccel ? dof(j) : mdfOAccel;
        }
    }
    // now check if dof IDs are within limits
    if ((mdfTDisp  != 0  &&  mdfTDisp  > sizeT(OF_Resp_Disp))  ||
        (mdfTVel   != 0  &&  mdfTVel   > sizeT(OF_Resp_Vel))   ||
        (mdfTAccel != 0  &&  mdfTAccel > sizeT(OF_Resp_Accel)) ||
        (mdfTForce != 0  &&  mdfTForce > sizeT(OF_Resp_Force)) ||
        (mdfTTime  != 0  &&  mdfTTime  > sizeT(OF_Resp_Time))  ||
        (mdfODisp  != 0  &&  mdfODisp  > sizeO(OF_Resp_Disp))  ||
        (mdfOVel   != 0  &&  mdfOVel   > sizeO(OF_Resp_Vel))   ||
        (mdfOAccel != 0  &&  mdfOAccel > sizeO(OF_Resp_Accel)) ||
        (mdfOForce != 0  &&  mdfOForce > sizeO(OF_Resp_Force)) ||
        (mdfOTime  != 0  &&  mdfOTime  > sizeO(OF_Resp_Time)))  {
        opserr << "ECMtsCsi::setSize() - wrong sizeTrial/Out\n";
        opserr << "see User Manual.\n";
        CsiController->reset();
        delete CsiController;
        exit(OF_ReturnType_failed);
    }
    
    (*sizeCtrl) = sizeT;
    (*sizeDaq)  = sizeO;
    
    // check sizes of signals defined in the CSI configuration
    // the loaded configuration must contain:
    //     at least 1 control point
    //     at least 1 degree of freedom in each control point
    //     and at least 1 feedback signals per degree of freedom
    
    Mts::ICsiConfiguration& cfg = CsiController->getConfiguration();
    
    opserr << "using MtsCsi configuration file '" << cfg.getFileName() << "'.\n";
    
    if (cfg.getControlPoints().count() < 1)  {
        opserr << "ECMtsCsi::setSize() - MtsCsi configuration "
            << "must define at least one control point.\n";
        CsiController->reset();
        delete CsiController;
        exit(OF_ReturnType_failed);
    }
    
    int numDOFs = 0;
    int numFdbkSigs = 0;
    
    for (int i=0; i<cfg.getControlPoints().count(); i++)  {
        
        Mts::ICsiControlPoint& ctrlPt = cfg.getControlPoints()[i];
        
        if (ctrlPt.getDegreesOfFreedom().count() < 1)  {
            opserr << "ECMtsCsi::setSize() - MtsCsi configuration must define "
                << "at least one degree of freedom per control point.\n";
            CsiController->reset();
            delete CsiController;
            exit(OF_ReturnType_failed);
        }
        
        numDOFs += ctrlPt.getDegreesOfFreedom().count();
        
        if (ctrlPt.getFeedbackSignals().count() < 1)  {
            opserr << "ECMtsCsi::setSize() - MtsCsi configuration must define "
                << "at least one feedback signal per control point.\n";
            CsiController->reset();
            delete CsiController;
            exit(OF_ReturnType_failed);
        }
        
        numFdbkSigs += ctrlPt.getFeedbackSignals().count();
    }
    
    opserr << "MtsCsi configuration: " << numDOFs << " degrees of freedom; " 
        << numFdbkSigs << " feedback signals.\n";
    
    if (numCtrlSignals != numDOFs)  {
        opserr << "ECMtsCsi::setSize() - specified number of control signals ("
            << numCtrlSignals << ") does not match total number of degrees of "
            << "freedom (" << numDOFs << ") defined in the MtsCsi configuration.\n";
        CsiController->reset();
        delete CsiController;
        exit(OF_ReturnType_failed);
    }
    
    if (numDaqSignals != numFdbkSigs)  {
        opserr << "ECMtsCsi::setSize() - specified number of daq signals ("
            << numDaqSignals << ") does not match total number of feedback signals "
            << "(" << numFdbkSigs << ") defined in the MtsCsi configuration.\n";
        CsiController->reset();
        delete CsiController;
        exit(OF_ReturnType_failed);
    }
    
    return OF_ReturnType_completed;
}


int ECMtsCsi::setTrialResponse(const Vector* disp,
    const Vector* vel,
    const Vector* accel,
    const Vector* force,
    const Vector* time)
{
    // loop through all the trial control points
    int k = 0;
    for (int i=0; i<numTrialCPs; i++)  {
        // get trial control point parameters
        int numSignals = trialCPs[i]->getNumSignal();
        ID dof = trialCPs[i]->getDOF();
        ID rsp = trialCPs[i]->getRspType();
        
        if (trialCPs[i]->hasLimits() == 0)  {
            // loop through all the trial control point dofs
            for (int j=0; j<numSignals; j++)  {
                // assemble the control signal array
                if (rsp(j) == OF_Resp_Disp  &&  disp != 0)  {
                    ctrlSignal[k] = (*disp)(dof(j));
                }
                else if (rsp(j) == OF_Resp_Force  &&  force != 0)  {
                    ctrlSignal[k] = (*force)(dof(j));
                }
                else if (rsp(j) == OF_Resp_Time  &&  time != 0)  {
                    ctrlSignal[k] = (*time)(dof(j));
                }
                else if (rsp(j) == OF_Resp_Vel  &&  vel != 0)  {
                    ctrlSignal[k] = (*vel)(dof(j));
                }
                else if (rsp(j) == OF_Resp_Accel  &&  accel != 0)  {
                    ctrlSignal[k] = (*accel)(dof(j));
                }
                // filter control signal if the filter exists
                if (theCtrlFilters[rsp(j)] != 0)
                    ctrlSignal[k] = theCtrlFilters[rsp(j)]->filtering(ctrlSignal[k]);
                k++;
            }
        
        } else  {
            Vector lowerLim = trialCPs[i]->getLowerLimit();
            Vector upperLim = trialCPs[i]->getUpperLimit();
            
            // loop through all the trial control point dofs
            for (int j=0; j<numSignals; j++)  {
                // assemble the control signal array
                if (rsp(j) == OF_Resp_Disp  &&  disp != 0)  {
                    if (lowerLim(j) < (*disp)(dof(j)) && (*disp)(dof(j)) < upperLim(j))
                        ctrlSignal[k] = (*disp)(dof(j));
                    else if ((*disp)(dof(j)) <= lowerLim(j))
                        ctrlSignal[k] = lowerLim(j);
                    else if ((*disp)(dof(j)) >= upperLim(j))
                        ctrlSignal[k] = upperLim(j);
                }
                else if (rsp(j) == OF_Resp_Force  &&  force != 0)  {
                    if (lowerLim(j) < (*force)(dof(j)) && (*force)(dof(j)) < upperLim(j))
                        ctrlSignal[k] = (*force)(dof(j));
                    else if ((*force)(dof(j)) <= lowerLim(j))
                        ctrlSignal[k] = lowerLim(j);
                    else if ((*force)(dof(j)) >= upperLim(j))
                        ctrlSignal[k] = upperLim(j);
                }
                else if (rsp(j) == OF_Resp_Time  &&  time != 0)  {
                    if (lowerLim(j) < (*time)(dof(j)) && (*time)(dof(j)) < upperLim(j))
                        ctrlSignal[k] = (*time)(dof(j));
                    else if ((*time)(dof(j)) <= lowerLim(j))
                        ctrlSignal[k] = lowerLim(j);
                    else if ((*time)(dof(j)) >= upperLim(j))
                        ctrlSignal[k] = upperLim(j);
                }
                else if (rsp(j) == OF_Resp_Vel  &&  vel != 0)  {
                    if (lowerLim(j) < (*vel)(dof(j)) && (*vel)(dof(j)) < upperLim(j))
                        ctrlSignal[k] = (*vel)(dof(j));
                    else if ((*vel)(dof(j)) <= lowerLim(j))
                        ctrlSignal[k] = lowerLim(j);
                    else if ((*vel)(dof(j)) >= upperLim(j))
                        ctrlSignal[k] = upperLim(j);
                }
                else if (rsp(j) == OF_Resp_Accel  &&  accel != 0)  {
                    if (lowerLim(j) < (*accel)(dof(j)) && (*accel)(dof(j)) < upperLim(j))
                        ctrlSignal[k] = (*accel)(dof(j));
                    else if ((*accel)(dof(j)) <= lowerLim(j))
                        ctrlSignal[k] = lowerLim(j);
                    else if ((*accel)(dof(j)) >= upperLim(j))
                        ctrlSignal[k] = upperLim(j);
                }
                // filter control signal if the filter exists
                if (theCtrlFilters[rsp(j)] != 0)
                    ctrlSignal[k] = theCtrlFilters[rsp(j)]->filtering(ctrlSignal[k]);
                k++;
            }
        }
    }
    
    // send control signal array to controller
    k += this->control();
    
    return (k - numCtrlSignals);
}


int ECMtsCsi::getDaqResponse(Vector* disp,
    Vector* vel,
    Vector* accel,
    Vector* force,
    Vector* time)
{
    // get daq signal array from controller/daq
    int rValue = this->acquire();
    
    // loop through all the output control points
    int k = 0;
    for (int i=0; i<numOutCPs; i++)  {
        // get output control point parameters
        int numSignals = outCPs[i]->getNumSignal();
        ID dof = outCPs[i]->getDOF();
        ID rsp = outCPs[i]->getRspType();
        
        // loop through all the output control point dofs
        for (int j=0; j<numSignals; j++)  {
            // filter daq signal if the filter exists
            if (theDaqFilters[rsp(j)] != 0)
                daqSignal[k] = theDaqFilters[rsp(j)]->filtering(daqSignal[k]);
            // populate the daq response vectors
            if (rsp(j) == OF_Resp_Disp  &&  disp != 0)  {
                (*disp)(dof(j)) = daqSignal[k];
            }
            else if (rsp(j) == OF_Resp_Force  &&  force != 0)  {
                (*force)(dof(j)) = daqSignal[k];
            }
            else if (rsp(j) == OF_Resp_Time  &&  time != 0)  {
                (*time)(dof(j)) = daqSignal[k];
            }
            else if (rsp(j) == OF_Resp_Vel  &&  vel != 0)  {
                (*vel)(dof(j)) = daqSignal[k];
            }
            else if (rsp(j) == OF_Resp_Accel  &&  accel != 0)  {
                (*accel)(dof(j)) = daqSignal[k];
            }
            k++;
        }
    }
    
    return (k + rValue - numDaqSignals);
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
    
    // ctrl signals
    if (ctrlSignal != 0 && (
        strcmp(argv[0],"ctrlSig") == 0 ||
        strcmp(argv[0],"ctrlSignal") == 0 ||
        strcmp(argv[0],"ctrlSignals") == 0))
    {
        for (i=0; i<numCtrlSignals; i++)  {
            sprintf(outputData,"ctrlSignal%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpControlResponse(this, 1,
            Vector(numCtrlSignals));
    }
    
    // daq signals
    if (daqSignal != 0 && (
        strcmp(argv[0],"daqSig") == 0 ||
        strcmp(argv[0],"daqSignal") == 0 ||
        strcmp(argv[0],"daqSignals") == 0))
    {
        for (i=0; i<numDaqSignals; i++)  {
            sprintf(outputData,"daqSignal%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpControlResponse(this, 2,
            Vector(numDaqSignals));
    }
    output.endTag();
    
    return theResponse;
}


int ECMtsCsi::getResponse(int responseID, Information &info)
{
    Vector resp(0);
    
    switch (responseID)  {
    case 1:  // ctrl signals
        resp.setData(ctrlSignal,numCtrlSignals);
        return info.setVector(resp);
        
    case 2:  // daq signals
        resp.setData(daqSignal,numDaqSignals);
        return info.setVector(resp);
        
    default:
        return OF_ReturnType_failed;
    }
}


void ECMtsCsi::Print(OPS_Stream &s, int flag)
{
    s << "****************************************************************\n";
    s << "* ExperimentalControl: " << this->getTag() << endln; 
    s << "*   type: ECMtsCsi\n";
    s << "*   cfgFile: " << cfgFile << endln;
    s << "*   rampTime: " << rampTime << endln;
    s << "*   trialCPs:";
    for (int i=0; i<numTrialCPs; i++)
        s << " " << trialCPs[i]->getTag();
    s << "\n*   outCPs:";
    for (int i=0; i<numOutCPs; i++)
        s << " " << outCPs[i]->getTag();
    s << "\n*   ctrlFilters:";
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
    s << "\n****************************************************************\n\n";
}


int ECMtsCsi::control()
{
    // create a ramp object and set ramp time
    Mts::ICsiRamp* ramp = Mts::CsiFactory::newRamp();
    ramp->setWaitUntilCompletion(true);
    ramp->setChannelCount(numCtrlSignals);
    ramp->setRampTime(rampTime);
    
    // set ramp commands for all the control signals
    for (int i=0; i<numCtrlSignals; i++)  {
        if (ctrlSigOffset.Size() > 0)
            ctrlSignal[i] += ctrlSigOffset(i);
        
        (*ramp)[i] = ctrlSignal[i];
    }
    
    // now run the ramp
    try  {
        rampId = CsiController->runRamp(ramp);
    }
    catch (const Mts::ICsiException& xcp)  {
        opserr << "ECMtsCsi::control() - "
            << "runRamp: error = " << xcp.what() << endln;
        CsiController->reset();
        delete CsiController;
        return OF_ReturnType_failed;
    }
    
    return OF_ReturnType_completed;
}


int ECMtsCsi::acquire()
{
    // get feedback signals for the previously run ramp
    // order of feedback signals is defined in MTS CSI configuration
    // if rampID = -1 the current feedback is returned
    try  {
        CsiController->acquireFeedback(rampId, daqSignal);
        if (daqSigOffset.Size() > 0)  {
            for (int i=0; i<numDaqSignals; i++)
                daqSignal[i] += daqSigOffset(i);
        }
    }
    catch (const Mts::ICsiException& xcp)  {
        opserr << "ECMtsCsi::acquire() - "
            << "acquireFeedback: error = " << xcp.what() << endln;
        CsiController->reset();
        delete CsiController;
        return OF_ReturnType_failed;
    }
    
    return OF_ReturnType_completed;
}
