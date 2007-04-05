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

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 02/07
// Revision: A
//
// Description: This file contains the implementation of ExperimentalCP.

#include <stdlib.h>

#include "ExperimentalCP.h"


ExperimentalCP::ExperimentalCP()
    : TaggedObject(0),
    nodeTag(0), direction(0), response(0),
    factor(0), lowerLim(0), upperLim(0),
    numDir(0)
{
    // does nothing
}


ExperimentalCP::ExperimentalCP(int tag, int nodetag,
    const ID &dir, const ID &resp, const Vector &fact)
    : TaggedObject(tag),
    nodeTag(nodetag), direction(dir), response(resp),
    factor(dir.Size()), lowerLim(0), upperLim(0),
    numDir(dir.Size())
{
    if (fact != 0)  {
        if (resp.Size() != numDir || 
            fact.Size() != numDir)  {
            opserr << "ExperimentalCP::ExperimentalCP() - "
                << "direction, response and factor need to "
                << "have same size\n";
            exit(OF_ReturnType_failed);
        }
        factor = fact;
    } else  {
        if (resp.Size() != numDir)  {
            opserr << "ExperimentalCP::ExperimentalCP() - "
                << "direction and response need to "
                << "have same size\n";
            exit(OF_ReturnType_failed);
        }
        for (int i=0; i<numDir; i++)
            factor(i) = 1.0;
    }
}


ExperimentalCP::ExperimentalCP(const ExperimentalCP& ecp)
    : TaggedObject(ecp)
{
    nodeTag   = ecp.nodeTag;
    direction = ecp.direction;
    response  = ecp.response;
    factor    = ecp.factor;
    if (ecp.lowerLim != 0)
        (*lowerLim)  = *(ecp.lowerLim);
    if (ecp.upperLim != 0)
        (*upperLim)  = *(ecp.upperLim);
    numDir    = ecp.numDir;
}


ExperimentalCP::~ExperimentalCP()
{
    if (lowerLim != 0)
        delete lowerLim;
    if (upperLim != 0)
        delete upperLim;
}


ExperimentalCP* ExperimentalCP::getCopy()
{
    ExperimentalCP *theCopy = new ExperimentalCP(*this);

    return theCopy;
}


int ExperimentalCP::setData(int nodetag, const ID &dir,
    const ID &resp, const Vector &fact)
{
    if (fact != 0)  {
        if (resp.Size() != dir.Size() || 
            fact.Size() != dir.Size())  {
            opserr << "ExperimentalCP::setData() - "
                << "direction, response and factor need to "
                << "have same size\n";
            return OF_ReturnType_failed;
        }
        factor = fact;
    } else  {
        if (resp.Size() != dir.Size())  {
            opserr << "ExperimentalCP::setData() - "
                << "direction and response need to "
                << "have same size\n";
            return OF_ReturnType_failed;
        }
        for (int i=0; i<numDir; i++)
            factor(i) = 1.0;
    }

    nodeTag   = nodetag;
    direction = dir;
    response  = resp;
    numDir    = dir.Size();
   
    return 0;
}


int ExperimentalCP::setLimits(const Vector &lowerlim,
    const Vector &upperlim)
{
    if (lowerLim == 0 || upperLim == 0)  {
        // create the new limits
        lowerLim = new Vector(numDir);
        upperLim = new Vector(numDir);
    }
    else if (lowerLim->Size() != numDir
        || upperLim->Size() != numDir)  {
        // delete the old limits
        if (lowerLim != 0)
            delete lowerLim;
        if (upperLim != 0)
            delete upperLim;
        // create the new limits
        lowerLim = new Vector(numDir);
        upperLim = new Vector(numDir);
    }

    if (lowerlim.Size() != numDir || 
        upperlim.Size() != numDir)  {
            opserr << "ExperimentalCP::setLimits() - "
                << "lower and upper limits need to be of "
                << "size: " << numDir << endln;
            return OF_ReturnType_failed;
    }

    (*lowerLim) = lowerlim;
    (*upperLim) = upperlim;

    return 0;
}


int ExperimentalCP::getNodeTag()
{
    return nodeTag;
}


int ExperimentalCP::getNumDir()
{
    return numDir;
}

const ID& ExperimentalCP::getDir()
{
    return direction;
}


const ID& ExperimentalCP::getResponseType()
{
    return response;
}


const Vector& ExperimentalCP::getFactor()
{
    return factor;
}


const Vector* ExperimentalCP::getLowerLimit()
{
    return lowerLim;
}


const Vector* ExperimentalCP::getUpperLimit()
{
    return upperLim;
}


int ExperimentalCP::getDir(int dirID)
{
    if (dirID<0 || dirID>=numDir)  {
        opserr << "ExperimentalCP::getDir() - "
            << "direction ID out of bounds, "
            << "component " << dirID << " does not exist\n";
        exit(OF_ReturnType_failed);
    }

    return direction(dirID);
}


int ExperimentalCP::getResponseType(int dirID)
{
    if (dirID<0 || dirID>=numDir)  {
        opserr << "ExperimentalCP::getResponseType() - "
            << "direction ID out of bounds, "
            << "component " << dirID << " does not exist\n";
        exit(OF_ReturnType_failed);
    }

    return response(dirID);
}


double ExperimentalCP::getFactor(int dirID)
{
    if (dirID<0 || dirID>=numDir)  {
        opserr << "ExperimentalCP::getFactor() - "
            << "direction ID out of bounds, "
            << "component " << dirID << " does not exist\n";
        exit(OF_ReturnType_failed);
    }

    return factor(dirID);
}


double ExperimentalCP::getLowerLimit(int dirID)
{
    if (lowerLim == 0)  {
        opserr << "ExperimentalCP::getLowerLimit() - "
            << "this control point has no lower "
            << "limits assigned\n";
        exit(OF_ReturnType_failed);
    }

    if (dirID<0 || dirID>=numDir)  {
        opserr << "ExperimentalCP::getFactor() - "
            << "direction ID out of bounds, "
            << "component " << dirID << " does not exist\n";
        exit(OF_ReturnType_failed);
    }

    return (*lowerLim)(dirID);
}


double ExperimentalCP::getUpperLimit(int dirID)
{
    if (upperLim == 0)  {
        opserr << "ExperimentalCP::getUpperLimit() - "
            << "this control point has no upper "
            << "limits assigned\n";
        exit(OF_ReturnType_failed);
    }

    if (dirID<0 || dirID>=numDir)  {
        opserr << "ExperimentalCP::getFactor() - "
            << "direction ID out of bounds, "
            << "component " << dirID << " does not exist\n";
        exit(OF_ReturnType_failed);
    }

    return (*upperLim)(dirID);
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


int ExperimentalCP::operator == (ExperimentalCP& ecp)
{
    // factor value IS NOT checked!
    if (nodeTag == ecp.nodeTag 
        && direction == ecp.direction
        && response == ecp.response) {
        return 1;
    } else {
        return 0;
    }
}


int ExperimentalCP::operator != (ExperimentalCP& ecp)
{
    // factor value IS NOT checked!
    if (nodeTag != ecp.nodeTag 
        || direction != ecp.direction
        || response != ecp.response) {
        return 1;
    } else {
        return 0;
    }
}
