/* ****************************************************************** **
**    OpenFRESCO - Open Framework                                     **
**                 for Experimental Setup and Control                 **
**                                                                    **
**                                                                    **
** Copyright (c) 2006, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited. See    **
** file 'COPYRIGHT_UCB' in main directory for information on usage    **
** and redistribution, and for a DISCLAIMER OF ALL WARRANTIES.        **
**                                                                    **
** Developed by:                                                      **
**   Andreas Schellenberg (andreas.schellenberg@gmx.net)              **
**   Yoshikazu Takahashi (yos@catfish.dpri.kyoto-u.ac.jp)             **
**   Gregory L. Fenves (fenves@berkeley.edu)                          **
**   Stephen A. Mahin (mahin@berkeley.edu)                            **
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
    numParam(0), sizeRespType(5)
{
    // does nothing
}


ExperimentalCP::ExperimentalCP(int tag, int nodetag,
    const ID &dir, const ID &resp, const Vector &fact)
    : TaggedObject(tag),
    nodeTag(nodetag), direction(dir), response(resp),
    factor(dir.Size()), lowerLim(0), upperLim(0),
    numParam(dir.Size()), sizeRespType(5)
{
    // initialize factors
    if (fact != 0)  {
        if (resp.Size() != numParam || 
            fact.Size() != numParam)  {
            opserr << "ExperimentalCP::ExperimentalCP() - "
                << "direction, response and factor need to "
                << "have same size\n";
            exit(OF_ReturnType_failed);
        }
        factor = fact;
    } else  {
        if (resp.Size() != numParam)  {
            opserr << "ExperimentalCP::ExperimentalCP() - "
                << "direction and response need to "
                << "have same size\n";
            exit(OF_ReturnType_failed);
        }
        for (int i=0; i<numParam; i++)
            factor(i) = 1.0;
    }

    // set sizes of response types
    sizeRespType.Zero();
    for (int i=0; i<numParam; i++)  {
        if (response(i) == 0)
            sizeRespType(0)++;
        else if (response(i) == 1)
            sizeRespType(1)++;
        else if (response(i) == 2)
            sizeRespType(2)++;
        else if (response(i) == 3)
            sizeRespType(3)++;
        else if (response(i) == 4)
            sizeRespType(4)++;
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
    numParam     = ecp.numParam;
    sizeRespType = ecp.sizeRespType;
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
    // initialize factors
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
        for (int i=0; i<numParam; i++)
            factor(i) = 1.0;
    }

    nodeTag   = nodetag;
    direction = dir;
    response  = resp;
    numParam  = dir.Size();
   
    // set sizes of response types
    sizeRespType.Zero();
    for (int i=0; i<numParam; i++)  {
        if (response(i) == 0)
            sizeRespType(0)++;
        else if (response(i) == 1)
            sizeRespType(1)++;
        else if (response(i) == 2)
            sizeRespType(2)++;
        else if (response(i) == 3)
            sizeRespType(3)++;
        else if (response(i) == 4)
            sizeRespType(4)++;
    }

    return 0;
}


int ExperimentalCP::setLimits(const Vector &lowerlim,
    const Vector &upperlim)
{
    if (lowerLim == 0 || upperLim == 0)  {
        // create the new limits
        lowerLim = new Vector(numParam);
        upperLim = new Vector(numParam);
    }
    else if (lowerLim->Size() != numParam
        || upperLim->Size() != numParam)  {
        // delete the old limits
        if (lowerLim != 0)
            delete lowerLim;
        if (upperLim != 0)
            delete upperLim;
        // create the new limits
        lowerLim = new Vector(numParam);
        upperLim = new Vector(numParam);
    }

    if (lowerlim.Size() != numParam || 
        upperlim.Size() != numParam)  {
            opserr << "ExperimentalCP::setLimits() - "
                << "lower and upper limits need to be of "
                << "size: " << numParam << endln;
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


int ExperimentalCP::getNumParameters()
{
    return numParam;
}


const ID& ExperimentalCP::getSizeRespType()
{
    return sizeRespType;
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
    if (dirID<0 || dirID>=numParam)  {
        opserr << "ExperimentalCP::getDir() - "
            << "direction ID out of bounds, "
            << "component " << dirID << " does not exist\n";
        exit(OF_ReturnType_failed);
    }

    return direction(dirID);
}


int ExperimentalCP::getResponseType(int dirID)
{
    if (dirID<0 || dirID>=numParam)  {
        opserr << "ExperimentalCP::getResponseType() - "
            << "direction ID out of bounds, "
            << "component " << dirID << " does not exist\n";
        exit(OF_ReturnType_failed);
    }

    return response(dirID);
}


double ExperimentalCP::getFactor(int dirID)
{
    if (dirID<0 || dirID>=numParam)  {
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

    if (dirID<0 || dirID>=numParam)  {
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

    if (dirID<0 || dirID>=numParam)  {
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
