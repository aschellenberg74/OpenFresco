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

#ifndef ExperimentalCP_h
#define ExperimentalCP_h

// Written: Yoshi (yos@catfish.dpri.kyoto-u.ac.jp)
// Created: 09/06
// Revision: A
//
// Description: This file contains the class definition for 
// ExperimentalCP. This object possess control point data, i.e.
// control node tag, direction, and response.

#include <DomainComponent.h>
#include <ID.h>

#include <FrescoGlobals.h>

class ExperimentalCP : public TaggedObject
{
public:
    // constructors
    ExperimentalCP();
    ExperimentalCP(int tag, 
        int nodeid,
        int dir,
        int response,
        double fact = 1.0);
    ExperimentalCP(int tag, Vector cp);
    ExperimentalCP(const ExperimentalCP& ecp);
    
    // destructor
    virtual ~ExperimentalCP();
    
    virtual ExperimentalCP *getCopy (void);
    virtual void Print(OPS_Stream &s, int flag);
    
    int setData(const Vector& cp);
    const Vector* getData();
    
    int getNodeTag();
    int getDir();
    int getResponseType();
    double getFactor();
    
    bool operator == (ExperimentalCP& ecp);
    bool operator != (ExperimentalCP& ecp);
    
protected:
    int nodeTag;    // control node tag
    int direction;  // direction
    int response;   // response type
    double factor;  // factor
};

#endif
