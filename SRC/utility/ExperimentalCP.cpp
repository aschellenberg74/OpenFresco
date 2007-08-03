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
    : TaggedObject(0), ndm(0), ndf(0), nodeTag(0), 
    direction(0), uniqueDir(0), response(0),
    factor(0), lowerLim(0), upperLim(0),
    numDirection(0), numUniqueDir(0),
    sizeRespType(5), dirRespType(5)
{
    // does nothing
}


ExperimentalCP::ExperimentalCP(int tag, int NDM, int NDF,
    int nodetag, const ID &dir, const ID &resp, const Vector &fact)
    : TaggedObject(tag), ndm(NDM), ndf(NDF), nodeTag(nodetag),
    direction(dir), uniqueDir(dir), response(resp),
    factor(dir.Size()), lowerLim(0), upperLim(0),
    numDirection(dir.Size()), numUniqueDir(dir.Size()),
    sizeRespType(5), dirRespType(5)
{
    // initialize factors
    if (fact != 0)  {
        if (resp.Size() != numDirection || 
            fact.Size() != numDirection)  {
            opserr << "ExperimentalCP::ExperimentalCP() - "
                << "direction, response and factor need to "
                << "have same size\n";
            exit(OF_ReturnType_failed);
        }
        factor = fact;
    } else  {
        if (resp.Size() != numDirection)  {
            opserr << "ExperimentalCP::ExperimentalCP() - "
                << "direction and response need to "
                << "have same size\n";
            exit(OF_ReturnType_failed);
        }
        for (int i=0; i<numDirection; i++)
            factor(i) = 1.0;
    }

    // find unique directions and number thereof
    numUniqueDir = uniqueDir.unique();
    if (numUniqueDir > ndf)  {
        opserr << "ExperimentalCP::ExperimentalCP() - "
            << "more unique directions specified than "
            << "available degrees of freedom\n";
        exit(OF_ReturnType_failed);
    }

    // set sizes of response types
    sizeRespType.Zero();
    for (int i=0; i<numDirection; i++)
        sizeRespType(response(i))++;
}


ExperimentalCP::ExperimentalCP(const ExperimentalCP& ecp)
    : TaggedObject(ecp)
{
    ndm       = ecp.ndm;
    ndf       = ecp.ndf;
    nodeTag   = ecp.nodeTag;
    direction = ecp.direction;
    uniqueDir = ecp.uniqueDir;
    response  = ecp.response;
    factor    = ecp.factor;
    if (ecp.lowerLim != 0)
        (*lowerLim)  = *(ecp.lowerLim);
    if (ecp.upperLim != 0)
        (*upperLim)  = *(ecp.upperLim);
    numDirection = ecp.numDirection;
    numUniqueDir = ecp.numUniqueDir;
    sizeRespType = ecp.sizeRespType;
    dirRespType  = ecp.dirRespType;
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


int ExperimentalCP::setNDM(int NDM)
{
    ndm = NDM;

    return 0;
}


int ExperimentalCP::setNDF(int NDF)
{
    ndf = NDF;

    return 0;
}


int ExperimentalCP::setData(int nodetag, const ID &dir,
    const ID &resp, const Vector &fact)
{
    // save the number of directions
    numDirection = dir.Size();

    // initialize factors
    if (fact != 0)  {
        if (resp.Size() != numDirection || 
            fact.Size() != numDirection)  {
            opserr << "ExperimentalCP::setData() - "
                << "direction, response and factor need to "
                << "have same size\n";
            return OF_ReturnType_failed;
        }
        factor = fact;
    } else  {
        if (resp.Size() != numDirection)  {
            opserr << "ExperimentalCP::setData() - "
                << "direction and response need to "
                << "have same size\n";
            return OF_ReturnType_failed;
        }
        for (int i=0; i<numDirection; i++)
            factor(i) = 1.0;
    }

    // assign the remaining variables
    nodeTag   = nodetag;
    direction = dir;
    uniqueDir = dir;
    response  = resp;
   
    // find unique directions and number thereof
    numUniqueDir = uniqueDir.unique();
    if (numUniqueDir > ndf)  {
        opserr << "ExperimentalCP::ExperimentalCP() - "
            << "more directions specified than available "
            << "degrees of freedom\n";
        exit(OF_ReturnType_failed);
    }

    // set sizes of response types
    sizeRespType.Zero();
    for (int i=0; i<numDirection; i++)
        sizeRespType(response(i))++;

    return 0;
}


int ExperimentalCP::setLimits(const Vector &lowerlim,
    const Vector &upperlim)
{
    if (lowerLim == 0 || upperLim == 0)  {
        // create the new limits
        lowerLim = new Vector(numDirection);
        upperLim = new Vector(numDirection);
    }
    else if (lowerLim->Size() != numDirection
        || upperLim->Size() != numDirection)  {
        // delete the old limits
        if (lowerLim != 0)
            delete lowerLim;
        if (upperLim != 0)
            delete upperLim;
        // create the new limits
        lowerLim = new Vector(numDirection);
        upperLim = new Vector(numDirection);
    }

    if (lowerlim.Size() != numDirection || 
        upperlim.Size() != numDirection)  {
            opserr << "ExperimentalCP::setLimits() - "
                << "lower and upper limits need to be of "
                << "size: " << numDirection << endln;
            return OF_ReturnType_failed;
    }

    (*lowerLim) = lowerlim;
    (*upperLim) = upperlim;

    return 0;
}


int ExperimentalCP::getNDM()
{
    return ndm;
}


int ExperimentalCP::getNDF()
{
    return ndf;
}


int ExperimentalCP::getNodeTag()
{
    return nodeTag;
}


int ExperimentalCP::getNumDirection()
{
    return numDirection;
}


int ExperimentalCP::getNumUniqueDir()
{
    return numUniqueDir;
}


const ID& ExperimentalCP::getSizeRespType()
{
    return sizeRespType;
}


const ID& ExperimentalCP::getDirRespType(int dir)
{
    if (dir<0 || dir>ndf)  {
        opserr << "ExperimentalCP::getDirRespType() - "
            << "direction out of bounds, "
            << "direction " << dir << " does not exist\n";
        exit(OF_ReturnType_failed);
    }

    dirRespType.Zero();
    for (int i=0; i<numDirection; i++)  {
        if (direction(i) == dir)
            dirRespType(response(i)) = 1;
    }

    return dirRespType;
}


const ID& ExperimentalCP::getDirection()
{
    return direction;
}


const ID& ExperimentalCP::getUniqueDir()
{
    return uniqueDir;
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


int ExperimentalCP::getDirection(int dirID)
{
    if (dirID<0 || dirID>=numDirection)  {
        opserr << "ExperimentalCP::getDir() - "
            << "direction ID out of bounds, "
            << "component " << dirID << " does not exist\n";
        exit(OF_ReturnType_failed);
    }

    return direction(dirID);
}


int ExperimentalCP::getResponseType(int dirID)
{
    if (dirID<0 || dirID>=numDirection)  {
        opserr << "ExperimentalCP::getResponseType() - "
            << "direction ID out of bounds, "
            << "component " << dirID << " does not exist\n";
        exit(OF_ReturnType_failed);
    }

    return response(dirID);
}


double ExperimentalCP::getFactor(int dirID)
{
    if (dirID<0 || dirID>=numDirection)  {
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

    if (dirID<0 || dirID>=numDirection)  {
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

    if (dirID<0 || dirID>=numDirection)  {
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
        << ", ndm: " << ndm << ", ndf: " << ndf << endln;
    s << "    node tag: " << nodeTag
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
