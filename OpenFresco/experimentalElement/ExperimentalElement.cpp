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
// Description: This file contains the implementation of the ExperimentalElement class.

#include <Domain.h>
#include <Information.h>
#include <ElementResponse.h>
#include <Node.h>
#include <Renderer.h>

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <ExperimentalElement.h>
#include <ExperimentalSite.h>
#include <Matrix.h>


ExperimentalElement::ExperimentalElement(int tag, 
    int classTag,
    ExperimentalSite *site)
    : Element(tag,classTag),
    theSite(site),
    sizeCtrl(0), sizeDaq(0),
    cpsCtrl(0), cpsDaq(0),
    theInitStif(1,1),
    firstWarning(true)
{
    // does nothing
}


ExperimentalElement::~ExperimentalElement()
{
    if(theSite)
        delete theSite;
    
    if(sizeCtrl != 0)
        delete sizeCtrl;
    if(sizeDaq != 0)
        delete sizeDaq;
    
    if(cpsCtrl != 0)
        delete cpsCtrl;
    if(cpsDaq != 0)
        delete cpsDaq;
}


const Matrix& ExperimentalElement::getInitialStiff(void)
{
    return theInitStif;
}


int ExperimentalElement::revertToLastCommit()
{
    opserr << "ExperimentalElement::revertToLastCommit() - "
        << "Element: " << this->getTag() << endln
        << "Can't revert to last commit. This is an experimental element." 
        << endln;
    
    return -1;
}


int ExperimentalElement::revertToStart()
{   
    opserr << "ExperimentalElement::revertToStart() - "
        << "Element: " << this->getTag() << endln
        << "Can't revert to start. This is an experimental element." 
        << endln;
    
    return -1;
}


const Matrix& ExperimentalElement::getTangentStiff(void)
{
    if(firstWarning == true)  {
        opserr << "WARNING ExperimentalElement::getTangentStiff() - "
            << "Element: " << this->getTag() << endln
            << "TangentStiff cannot be calculated. Return InitialStiff instead." 
            << endln;
        opserr << "Subsequent getTangentStiff warnings will be suppressed." 
            << endln;
        
        firstWarning = false;
    }
    
    return  this->getInitialStiff();
}

