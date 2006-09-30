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
// $Source$

// Written: Yoshi (yos@catfish.dpri.kyoto-u.ac.jp)
// Created: 09/06
// Revision: A
//
// Description: This file contains the implementation of
// ExperimentalControl.

#include <ExperimentalControl.h>


ExperimentalControl::ExperimentalControl(int tag)
    : TaggedObject(tag),
    sizeCtrl(0), sizeDaq(0), theFilter(0)
{
    sizeCtrl = new ID(OF_Resp_All);
    sizeDaq = new ID(OF_Resp_All);
    if(sizeCtrl == 0 || sizeDaq == 0) {
        opserr << "FATAL ExperimentalControl::ExperimentalControl - "
            << "fail to create ID."
            << endln;
        exit(OF_ReturnType_failed);
    }
}

ExperimentalControl::ExperimentalControl(const ExperimentalControl& ec) 
    : TaggedObject(ec), 
    sizeCtrl(0), sizeDaq(0), theFilter(0)
{
    sizeCtrl = new ID(OF_Resp_All);
    sizeDaq = new ID(OF_Resp_All);
    if(sizeCtrl == 0 || sizeDaq == 0) {
        opserr << "FATAL copy constructor of ExperimentalControl - "
            << "fail to create ID."
            << endln;
        exit(OF_ReturnType_failed);
    }
    *sizeCtrl = *(ec.sizeCtrl);
    *sizeDaq = *(ec.sizeDaq);
    
    if(ec.theFilter != 0) 
        theFilter = ec.theFilter->getCopy();
    else
        theFilter = 0;
}


ExperimentalControl::~ExperimentalControl()
{
    if(sizeCtrl != 0)
        delete sizeCtrl;
    if(sizeDaq != 0)
        delete sizeDaq;
}


int ExperimentalControl::commitState()
{
    return 0;
}


const ID& ExperimentalControl::getSizeCtrl()
{
    return *sizeCtrl;
}


const ID& ExperimentalControl::getSizeDaq()
{
    return *sizeDaq;
}
