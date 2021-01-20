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

#ifndef ExperimentalElement_h
#define ExperimentalElement_h

// Written: Yoshi (yos@catfish.dpri.kyoto-u.ac.jp)
// Created: 09/06
// Revision: A
//
// Description: This file contains the class definition for ExperimentalElement.
// An ExperimentalElement element is an abstract class for conducting experimental tests.
// ExperimentalElement objects are associated with experimental site objects.

#include <ExperimentalSite.h>
#include <ExperimentalTangentStiff.h>

#include <Element.h>
#include <Matrix.h>

class ExperimentalElement : public Element
{
public:
    // constructor
    ExperimentalElement(int tag, 
        int classTag,
        ExperimentalSite *site = 0,
        ExperimentalTangentStiff *tangStiff = 0);
    
    // destructor
    virtual ~ExperimentalElement();
    
    // public method to obtain basic DOF size which is
    // equal to the max num DOF that can be controlled
    virtual int getNumBasicDOF() = 0;
    
    // public methods to set and to obtain the initial stiffness
    virtual int setInitialStiff(const Matrix& stiff) = 0;
    const Matrix &getTangentStiff();
    const Matrix &getInitialStiff();
    
    // public methods to obtain the daq response in global system
    virtual const Vector &getDisp();
    virtual const Vector &getVel();
    virtual const Vector &getAccel();
    virtual const Vector &getTime();

protected:
    // pointer to ExperimentalSite object
    ExperimentalSite* theSite;
    // pointer to ExperimentalTangentStiff object
    ExperimentalTangentStiff* theTangStiff;
    
    // sizeCtrl/Daq
    // [0]:disp, [1]:vel, [2]:accel, [3]:force, [4]:time
    ID* sizeCtrl;
    ID* sizeDaq;
        
    // initial stiffness matrix
    Matrix theInitStiff;

private:
    // the following methods must be defined if the Element object 
    // is inherited, but are meaningless for this element.
    int revertToLastCommit();
    int revertToStart();
    
    bool firstWarning;
};

#endif
