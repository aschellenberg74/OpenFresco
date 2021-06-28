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

// Written: Yoshi (yos@catfish.dpri.kyoto-u.ac.jp)
// Created: 09/06
// Revision: A
//
// Description: This file contains the implementation of
// ExperimentalControl.

#include "ExperimentalControl.h"

#include <TaggedObject.h>
#include <MapOfTaggedObjects.h>

static MapOfTaggedObjects theExperimentalControls;


bool OPF_addExperimentalControl(ExperimentalControl* newComponent)
{
    return theExperimentalControls.addComponent(newComponent);
}


bool OPF_removeExperimentalControl(int tag)
{
    TaggedObject* obj = theExperimentalControls.removeComponent(tag);
    if (obj != 0) {
        delete obj;
        return true;
    }
    return false;
}


ExperimentalControl* OPF_getExperimentalControl(int tag)
{
    TaggedObject* theResult = theExperimentalControls.getComponentPtr(tag);
    if (theResult == 0) {
        opserr << "OPF_GetExperimentalControl() - "
            << "none found with tag: " << tag << endln;
        return 0;
    }
    ExperimentalControl* theControl = (ExperimentalControl*)theResult;
    
    return theControl;
}


void OPF_clearExperimentalControls()
{
    theExperimentalControls.clearAll();
}


ExperimentalControl::ExperimentalControl(int tag)
    : TaggedObject(tag),
    sizeCtrl(0), sizeDaq(0),
    theCtrlFilters(0), theDaqFilters(0)
{
    sizeCtrl = new ID(OF_Resp_All);
    sizeDaq = new ID(OF_Resp_All);
    if (sizeCtrl == 0 || sizeDaq == 0)  {
        opserr << "ExperimentalControl::ExperimentalControl() - "
            << "fail to create ID.\n";
        exit(OF_ReturnType_failed);
    }
    
    // allocate memory for the signal filters
    theCtrlFilters = new ExperimentalSignalFilter* [OF_Resp_All];
    if (theCtrlFilters == 0)  {
        opserr << "ExperimentalControl::ExperimentalControl() - "
            << "failed to allocate pointers for ctrl filters.\n";
        exit(OF_ReturnType_failed);
    }
    theDaqFilters = new ExperimentalSignalFilter* [OF_Resp_All];
    if (theDaqFilters == 0)  {
        opserr << "ExperimentalControl::ExperimentalControl() - "
            << "failed to allocate pointers for daq filters.\n";
        exit(OF_ReturnType_failed);
    }
    
    // initialize signal filter pointers to zero
    for (int i=0; i<OF_Resp_All; i++)  {
        theCtrlFilters[i] = 0;
        theDaqFilters[i] = 0;
    }
}

ExperimentalControl::ExperimentalControl(const ExperimentalControl& ec) 
    : TaggedObject(ec), 
    sizeCtrl(0), sizeDaq(0),
    theCtrlFilters(0), theDaqFilters(0)
{
    sizeCtrl = new ID(OF_Resp_All);
    sizeDaq = new ID(OF_Resp_All);
    if (sizeCtrl == 0 || sizeDaq == 0)  {
        opserr << "ExperimentalControl::ExperimentalControl() - "
            << "fail to create ID.\n";
        exit(OF_ReturnType_failed);
    }
    *sizeCtrl = *(ec.sizeCtrl);
    *sizeDaq = *(ec.sizeDaq);
    
    // allocate memory for the signal filters
    theCtrlFilters = new ExperimentalSignalFilter* [OF_Resp_All];
    if (theCtrlFilters == 0)  {
        opserr << "ExperimentalControl::ExperimentalControl() - "
            << "failed to allocate pointers for ctrl filters.\n";
        exit(OF_ReturnType_failed);
    }
    theDaqFilters = new ExperimentalSignalFilter* [OF_Resp_All];
    if (theDaqFilters == 0)  {
        opserr << "ExperimentalControl::ExperimentalControl() - "
            << "failed to allocate pointers for daq filters.\n";
        exit(OF_ReturnType_failed);
    }
    
    for (int i=1; i<OF_Resp_All; i++)  {
        if (ec.theCtrlFilters[i] != 0) 
            theCtrlFilters[i] = ec.theCtrlFilters[i]->getCopy();
        else
            theCtrlFilters[i] = 0;
        
        if (ec.theDaqFilters[i] != 0) 
            theDaqFilters[i] = ec.theDaqFilters[i]->getCopy();
        else
            theDaqFilters[i] = 0;
    }
}


ExperimentalControl::~ExperimentalControl()
{
    if (sizeCtrl != 0)
        delete sizeCtrl;
    if (sizeDaq != 0)
        delete sizeDaq;
    if (theCtrlFilters != 0)
        delete [] theCtrlFilters;
    if (theDaqFilters != 0)
        delete [] theDaqFilters;
}


const char* ExperimentalControl::getClassType() const
{
    return "UnknownExpControlObject";
}


int ExperimentalControl::commitState()
{
    return 0;
}


Response* ExperimentalControl::setResponse(const char **argv, int argc,
    OPS_Stream &output)
{
    Response *theResponse = 0;
    
    output.tag("ExpControlOutput");
    output.attr("ctrlType",this->getClassType());
    output.attr("ctrlTag",this->getTag());
        
    output.endTag();
    
    return theResponse;
}


int ExperimentalControl::getResponse(int responseID, Information &info)
{
    // each subclass must implement its own response
    return -1;
}


void ExperimentalControl::setCtrlFilter(ExperimentalSignalFilter* theFilter,
    int respType)
{
    if (theFilter == 0)  {
        opserr << "ExperimentalControl::setCtrlFilter() - "
            << "null signal filter pointer passed.\n";
        exit(OF_ReturnType_failed);
    }
    if (respType < OF_Resp_Disp || OF_Resp_Time < respType)  {
        opserr << "ExperimentalControl::setCtrlFilter() - "
            << "invalid response type.\n";
        exit(OF_ReturnType_failed);
    }
    theCtrlFilters[respType] = theFilter->getCopy();
    if (theCtrlFilters[respType] == 0) {
        opserr << "ExperimentalControl::setCtrlFilter() - "
            << "failed to copy signal filter.\n";
        exit(OF_ReturnType_failed);
    }
}


void ExperimentalControl::setDaqFilter(ExperimentalSignalFilter* theFilter,
    int respType)
{
    if (theFilter == 0)  {
        opserr << "ExperimentalControl::setDaqFilter() - "
            << "null signal filter pointer passed.\n";
        exit(OF_ReturnType_failed);
    }
    if (respType < OF_Resp_Disp || OF_Resp_Time < respType)  {
        opserr << "ExperimentalControl::setDaqFilter() - "
            << "invalid response type.\n";
        exit(OF_ReturnType_failed);
    }
    theDaqFilters[respType] = theFilter->getCopy();
    if (theDaqFilters[respType] == 0) {
        opserr << "ExperimentalControl::setDaqFilter() - "
            << "failed to copy signal filter.\n";
        exit(OF_ReturnType_failed);
    }
}


const ID& ExperimentalControl::getSizeCtrl()
{
    return *sizeCtrl;
}


const ID& ExperimentalControl::getSizeDaq()
{
    return *sizeDaq;
}


void ExperimentalControl::sleep(const clock_t wait)
{
    clock_t goal;
    goal = wait + clock();
    while (goal>clock());
}
