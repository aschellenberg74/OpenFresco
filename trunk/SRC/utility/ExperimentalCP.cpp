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
// $URL$

// Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
// Created: 02/07
// Revision: A
//
// Description: This file contains the implementation of ExperimentalCP.

#include "ExperimentalCP.h"

#include <Node.h>

#include <stdlib.h>


ExperimentalCP::ExperimentalCP(int tag, const ID &dof,
    const ID &rsptype, const Vector &fact)
    : TaggedObject(tag),
    numSignals(dof.Size()), numDOF(dof.Size()),
    DOF(dof), rspType(rsptype), factor(dof.Size()),
    lowerLim(0), upperLim(0), isRelative(dof.Size()),
    uniqueDOF(dof), sizeRspType(5), dofRspType(5),
    nodeTag(0), nodeNDM(0), nodeNDF(0)
{
    // initialize factors
    if (fact.Size() > 0)  {
        if (rspType.Size() != numSignals || 
            fact.Size() != numSignals)  {
            opserr << "ExperimentalCP::ExperimentalCP() - "
                << "DOF, rspType and factor arrays need to "
                << "have the same size.\n";
            exit(OF_ReturnType_failed);
        }
        factor = fact;
    } else  {
        if (rspType.Size() != numSignals)  {
            opserr << "ExperimentalCP::ExperimentalCP() - "
                << "DOF and rspType arrays need to "
                << "have the same size.\n";
            exit(OF_ReturnType_failed);
        }
        for (int i=0; i<numSignals; i++)
            factor(i) = 1.0;
    }
    
    // find unique DOFs and number thereof
    numDOF = uniqueDOF.unique();
    /*if (numUniqueDir > ndf)  {
        opserr << "ExperimentalCP::ExperimentalCP() - "
            << "more unique directions specified than "
            << "available degrees of freedom\n";
        exit(OF_ReturnType_failed);
    }*/
    
    // set sizes of response types
    sizeRspType.Zero();
    for (int i=0; i<numSignals; i++)  {
        if (rspType(i) < 0 || 4 < rspType(i))  {
            opserr << "ExperimentalCP::ExperimentalCP() - "
                << "wrong response type received.\n";
            exit(OF_ReturnType_failed);
        }
        sizeRspType(rspType(i)) += 1;
    }
    
    // zero the dofRspType vector and the isRelative ID
    dofRspType.Zero();
    isRelative.Zero();
}


ExperimentalCP::ExperimentalCP(const ExperimentalCP& ecp)
    : TaggedObject(ecp)
{    
    numSignals = ecp.numSignals;
    numDOF     = ecp.numDOF;
    
    DOF        = ecp.DOF;
    rspType    = ecp.rspType;
    factor     = ecp.factor;
    lowerLim   = ecp.lowerLim;
    upperLim   = ecp.upperLim;
    isRelative = ecp.isRelative;
    
    uniqueDOF   = ecp.uniqueDOF;
    sizeRspType = ecp.sizeRspType;
    dofRspType  = ecp.dofRspType;
    
    nodeTag  = ecp.nodeTag;
    nodeNDM  = ecp.nodeNDM;
    nodeNDF  = ecp.nodeNDF;
    nodeCrds = ecp.nodeCrds;
}


ExperimentalCP::~ExperimentalCP()
{
    // nothing to destroy
}


ExperimentalCP* ExperimentalCP::getCopy()
{
    ExperimentalCP *theCopy = new ExperimentalCP(*this);

    return theCopy;
}


int ExperimentalCP::setData(const ID &dof,
    const ID &rsptype, const Vector &fact)
{
    // save the number of directions
    numSignals = dof.Size();
    numDOF = dof.Size();
    DOF = dof;
    rspType = rsptype;
    
    // initialize factors
    if (fact.Size() > 0)  {
        if (rspType.Size() != numSignals || 
            fact.Size() != numSignals)  {
            opserr << "ExperimentalCP::setData() - "
                << "DOF, rspType and factor arrays need to "
                << "have the same size.\n";
            return OF_ReturnType_failed;
        }
        factor = fact;
    } else  {
        if (rspType.Size() != numSignals)  {
            opserr << "ExperimentalCP::setData() - "
                << "DOF and rspType arrays need to "
                << "have the same size.\n";
            return OF_ReturnType_failed;
        }
        for (int i=0; i<numSignals; i++)
            factor(i) = 1.0;
    }
    
    // find unique DOFs and number thereof
    numDOF = uniqueDOF.unique();
    /*if (numUniqueDir > ndf)  {
        opserr << "ExperimentalCP::ExperimentalCP() - "
            << "more directions specified than available "
            << "degrees of freedom\n";
        exit(OF_ReturnType_failed);
    }*/
    
    // set sizes of response types
    sizeRspType.Zero();
    for (int i=0; i<numSignals; i++)  {
        if (rspType(i) < 0 || 4 < rspType(i))  {
            opserr << "ExperimentalCP::setData() - "
                << "wrong response type received.\n";
            return OF_ReturnType_failed;
        }
        sizeRspType(rspType(i)) += 1;
    }
    
    // zero the dofRspType vector
    dofRspType.Zero();
    
    return 0;
}


int ExperimentalCP::setLimits(const Vector &lowerlim,
    const Vector &upperlim)
{
    if (lowerlim.Size() != numSignals || 
        upperlim.Size() != numSignals)  {
            opserr << "ExperimentalCP::setLimits() - "
                << "lower and upper limits need to be of "
                << "size: " << numSignals << endln;
            return OF_ReturnType_failed;
    }
    
    lowerLim = lowerlim;
    upperLim = upperlim;
    
    return 0;
}


int ExperimentalCP::setSigRefType(const ID &isrelative)
{
    if (isrelative.Size() != numSignals)  {
            opserr << "ExperimentalCP::setSigRefType() - "
                << "signal reference type ID needs to be of "
                << "size: " << numSignals << endln;
            return OF_ReturnType_failed;
    }
    
    isRelative = isrelative;
    
    return 0;
}


int ExperimentalCP::setNode(Node *node)
{
    // check node input
    if (node == 0)  {
        opserr << "ExperimentalCP::setNode() - "
            << "null node pointer passed.\n";
        exit(OF_ReturnType_failed);
    }
    
    // save nodal data
    theNode = node;
    nodeTag = theNode->getTag();
    nodeNDF = theNode->getNumberDOF();
    nodeCrds = theNode->getCrds();
    nodeNDM = nodeCrds.Size();
    
    return 0;
}


int ExperimentalCP::getNumSignal()
{
    return numSignals;
}


int ExperimentalCP::getNumDOF()
{
    return numDOF;
}


const ID& ExperimentalCP::getSizeRspType()
{
    return sizeRspType;
}


const ID& ExperimentalCP::getDOFRspType(int dir)
{
    /*if (dir < 0 || dir > ndf)  {
        opserr << "ExperimentalCP::getDOFRspType() - "
            << "direction out of bounds, "
            << "direction " << dir << " does not exist\n";
        exit(OF_ReturnType_failed);
    }*/
    
    dofRspType.Zero();
    for (int i=0; i<numSignals; i++)  {
        if (DOF(i) == dir)
            dofRspType(rspType(i)) = 1;
    }
    
    return dofRspType;
}


int ExperimentalCP::getNodeTag()
{
    if (nodeTag == 0)  {
        opserr << "ExperimentalCP::getNodeTag() - "
            << "this control point has no node "
            << "assigned.\n";
        exit(OF_ReturnType_failed);
    }
    
    return nodeTag;
}


int ExperimentalCP::getNodeNDM()
{
    if (nodeTag == 0)  {
        opserr << "ExperimentalCP::getNodeNDM() - "
            << "this control point has no node "
            << "assigned.\n";
        exit(OF_ReturnType_failed);
    }
    
    return nodeNDM;
}


int ExperimentalCP::getNodeNDF()
{
    if (nodeTag == 0)  {
        opserr << "ExperimentalCP::getNodeNDF() - "
            << "this control point has no node "
            << "assigned.\n";
        exit(OF_ReturnType_failed);
    }
    
    return nodeNDF;
}


const Vector& ExperimentalCP::getNodeCrds()
{
    if (nodeTag == 0)  {
        opserr << "ExperimentalCP::getNodeCrds() - "
            << "this control point has no node "
            << "assigned.\n";
        exit(OF_ReturnType_failed);
    }
    
    return nodeCrds;
}


const ID& ExperimentalCP::getDOF()
{
    return DOF;
}


const ID& ExperimentalCP::getUniqueDOF()
{
    return uniqueDOF;
}


const ID& ExperimentalCP::getRspType()
{
    return rspType;
}


const Vector& ExperimentalCP::getFactor()
{
    return factor;
}


const Vector& ExperimentalCP::getLowerLimit()
{
    if (lowerLim == 0)  {
        opserr << "ExperimentalCP::getLowerLimit() - "
            << "this control point has no lower "
            << "limits assigned.\n";
        exit(OF_ReturnType_failed);
    }
    
    return lowerLim;
}


const Vector& ExperimentalCP::getUpperLimit()
{
    if (upperLim == 0)  {
        opserr << "ExperimentalCP::getUpperLimit() - "
            << "this control point has no upper "
            << "limits assigned.\n";
        exit(OF_ReturnType_failed);
    }
    
    return upperLim;
}


const ID& ExperimentalCP::getSigRefType()
{
    return isRelative;
}


int ExperimentalCP::getDOF(int signalID)
{
    if (signalID < 0 || numSignals <= signalID)  {
        opserr << "ExperimentalCP::getDOF() - "
            << "signal ID out of bounds, "
            << "component " << signalID << " does not exist.\n";
        exit(OF_ReturnType_failed);
    }
    
    return DOF(signalID);
}


int ExperimentalCP::getRspType(int signalID)
{
    if (signalID < 0 || numSignals <= signalID)  {
        opserr << "ExperimentalCP::getRspType() - "
            << "signal ID out of bounds, "
            << "component " << signalID << " does not exist.\n";
        exit(OF_ReturnType_failed);
    }
    
    return rspType(signalID);
}


double ExperimentalCP::getFactor(int signalID)
{
    if (signalID < 0 || numSignals <= signalID)  {
        opserr << "ExperimentalCP::getFactor() - "
            << "signal ID out of bounds, "
            << "component " << signalID << " does not exist.\n";
        exit(OF_ReturnType_failed);
    }
    
    return factor(signalID);
}


double ExperimentalCP::getLowerLimit(int signalID)
{
    if (lowerLim == 0)  {
        opserr << "ExperimentalCP::getLowerLimit() - "
            << "this control point has no lower "
            << "limits assigned.\n";
        exit(OF_ReturnType_failed);
    }
    
    if (signalID < 0 || numSignals <= signalID)  {
        opserr << "ExperimentalCP::getLowerLimit() - "
            << "signal ID out of bounds, "
            << "component " << signalID << " does not exist.\n";
        exit(OF_ReturnType_failed);
    }
    
    return lowerLim(signalID);
}


double ExperimentalCP::getUpperLimit(int signalID)
{
    if (upperLim == 0)  {
        opserr << "ExperimentalCP::getUpperLimit() - "
            << "this control point has no upper "
            << "limits assigned.\n";
        exit(OF_ReturnType_failed);
    }
    
    if (signalID < 0 || numSignals <= signalID)  {
        opserr << "ExperimentalCP::getUpperLimit() - "
            << "signal ID out of bounds, "
            << "component " << signalID << " does not exist.\n";
        exit(OF_ReturnType_failed);
    }
    
    return upperLim(signalID);
}


int ExperimentalCP::getSigRefType(int signalID)
{
    if (signalID < 0 || numSignals <= signalID)  {
        opserr << "ExperimentalCP::getSigRefType() - "
            << "signal ID out of bounds, "
            << "component " << signalID << " does not exist.\n";
        exit(OF_ReturnType_failed);
    }
    
    return isRelative(signalID);
}


void ExperimentalCP::Print(OPS_Stream &s, int flag)
{
    s << "ExperimentalCP: " << this->getTag() << endln;
    s << "  numSignal: " << numSignals << ", numDOF: " << numDOF << endln; 
    s << "  DOF     : " << DOF << endln;
    s << "  rspType : " << rspType << endln;
    s << "  factor  : " << factor << endln;
    if (lowerLim != 0)
        s << "  lowerLim: " << lowerLim << endln;
    if (upperLim != 0)
        s << "  upperLim: " << upperLim << endln;
    s << "  isRelative  : " << isRelative << endln;
    if (nodeTag != 0)  {
        s << "  nodeTag: " << nodeTag << endln;
        s << "    ndm: " << nodeNDM << ", ndf: " << nodeNDF << endln;
        s << "    crds: " << nodeCrds << endln;
    }
    s << endln;
}


int ExperimentalCP::hasLimits()
{
    if (lowerLim != 0 && upperLim != 0)
        return 1;
    
    return 0;
}


int ExperimentalCP::operator == (ExperimentalCP& ecp)
{
    // factor value IS NOT checked!
    if (nodeTag == ecp.nodeTag 
        && DOF == ecp.DOF
        && rspType == ecp.rspType) {
        return 1;
    } else {
        return 0;
    }
}


int ExperimentalCP::operator != (ExperimentalCP& ecp)
{
    // factor value IS NOT checked!
    if (nodeTag != ecp.nodeTag 
        || DOF != ecp.DOF
        || rspType != ecp.rspType) {
        return 1;
    } else {
        return 0;
    }
}
