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

// Written: Yoshi (yos@catfish.dpri.kyoto-u.ac.jp)
// Created: 09/06
// Revision: A
//
// Description: This file contains the implementation of the ExperimentalElement class.

#include "ExperimentalElement.h"

#include <Domain.h>
#include <Information.h>
#include <ElementResponse.h>
#include <Node.h>
#include <Renderer.h>

#include <math.h>
#include <stdlib.h>
#include <string.h>


ExperimentalElement::ExperimentalElement(int tag, 
    int classTag,
    ExperimentalSite *site)
    : Element(tag,classTag),
    theSite(site),
    sizeCtrl(0), sizeDaq(0),
    theInitStiff(1,1),
    firstWarning(true)
{
    // does nothing
}


ExperimentalElement::~ExperimentalElement()
{
    if (theSite != 0)
        delete theSite;
    
    if (sizeCtrl != 0)
        delete sizeCtrl;
    if (sizeDaq != 0)
        delete sizeDaq;    
}


const Matrix& ExperimentalElement::getTangentStiff()
{
    if (firstWarning == true)  {
        opserr << "\nWARNING ExperimentalElement::getTangentStiff() - "
            << "Element: " << this->getTag() << endln
            << "TangentStiff cannot be calculated. Return InitialStiff instead." 
            << endln;
        opserr << "Subsequent getTangentStiff warnings will be suppressed." 
            << endln;
        
        firstWarning = false;
    }
    
    return  this->getInitialStiff();
}


const Matrix& ExperimentalElement::getInitialStiff()
{
    return theInitStiff;
}


const Vector& ExperimentalElement::getDisp()
{	
    // return global vector with all components zero
    static Vector dg(this->getNumDOF());

    return dg;
}


const Vector& ExperimentalElement::getVel()
{	
    // return global vector with all components zero
    static Vector vg(this->getNumDOF());

    return vg;
}


const Vector& ExperimentalElement::getAccel()
{	
    // return global vector with all components zero
    static Vector ag(this->getNumDOF());

    return ag;
}


const Vector& ExperimentalElement::getTime()
{	
    // return global vector with all components zero
    static Vector time(1);

    return time;
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
