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
// $URL: $

// Written: Yoshi (yos@catfish.dpri.kyoto-u.ac.jp)
// Created: 09/06
// Revision: A
//
// Description: This file contains the implementation of ExperimentalCP.

#include <stdlib.h>

#include "ExperimentalCP.h"


ExperimentalCP::ExperimentalCP()
    : TaggedObject(0),
    nodeTag(0),
    direction(OF_Dir_None),
    response(OF_Resp_All),
    factor(1.0)
{
    // does nothing
}


ExperimentalCP::ExperimentalCP(int tag, int nodeid, int dir, 
    int resp, double fact)
    : TaggedObject(tag),
    nodeTag(nodeid),
    direction(dir),
    response(resp),
    factor(fact)
{
    // does nothing
}


ExperimentalCP::ExperimentalCP(int tag, Vector cp)
    : TaggedObject(tag),
    nodeTag((int)(cp[0])),
    direction((int)(cp[1])),
    response((int)(cp[2])),
    factor(cp[3])
{
    // does nothing
}


ExperimentalCP::ExperimentalCP(const ExperimentalCP& ecp)
    : TaggedObject(ecp)
{
    nodeTag = ecp.nodeTag;
    direction = ecp.direction;
    response = ecp.response;
    factor = ecp.factor;
}


ExperimentalCP::~ExperimentalCP()
{
    // does nothing
}


ExperimentalCP* ExperimentalCP::getCopy()
{
    ExperimentalCP *theCopy = new ExperimentalCP(*this);
    return theCopy;
}


int ExperimentalCP::setData(const Vector& cp)
{
    if(cp.Size() != 4) {
        opserr << "FATAL ExperimentalCP::setData - "
            << "wrong size of ID" << endln;
    }
    nodeTag = (int)(cp[0]);
    direction = (int)(cp[1]);
    response = (int)(cp[2]);
    factor = cp[3];
    
    return 0;
}


const Vector* ExperimentalCP::getData()
{
    Vector *data = new Vector(4);
    (*data)[0] = nodeTag;
    (*data)[1] = direction;
    (*data)[2] = response;
    (*data)[3] = factor;
    
    return data;
}


int ExperimentalCP::getNodeTag()
{
    return nodeTag;
}


int ExperimentalCP::getDir()
{
    return direction;
}


int ExperimentalCP::getResponseType()
{
    return response;
}


double ExperimentalCP::getFactor()
{
    return factor;
}


void ExperimentalCP::Print(OPS_Stream &s, int flag)
{
    s << "ExperimentalCP: tag: " << this->getTag() 
        << ", node tag: " << nodeTag
        << ", dir: " << direction
        << ", resp: " << response
        << ", fact: " << factor
        << endln;
}


bool ExperimentalCP::operator == (ExperimentalCP& ecp)
{
    // factor value IS NOT checked!
    if(nodeTag == ecp.nodeTag 
        && direction == ecp.direction
        && response == ecp.response) {
        return true;
    } else {
        return false;
    }
}


bool ExperimentalCP::operator != (ExperimentalCP& ecp)
{
    return !(*this==ecp);
}

