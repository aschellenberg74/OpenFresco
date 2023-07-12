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

// Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
// Created: 02/07
// Revision: A
//
// Description: This file contains the implementation of ExperimentalCP.

#include "ExperimentalCP.h"

#include <stdlib.h>
#include <elementAPI.h>

#include <TaggedObject.h>
#include <MapOfTaggedObjects.h>
#include <Domain.h>
#include <Node.h>

static MapOfTaggedObjects theExperimentalCPs;


bool OPF_addExperimentalCP(ExperimentalCP* newComponent)
{
    return theExperimentalCPs.addComponent(newComponent);
}


bool OPF_removeExperimentalCP(int tag)
{
    TaggedObject* obj = theExperimentalCPs.removeComponent(tag);
    if (obj != 0) {
        delete obj;
        return true;
    }
    return false;
}


ExperimentalCP* OPF_getExperimentalCP(int tag)
{
    TaggedObject* theResult = theExperimentalCPs.getComponentPtr(tag);
    if (theResult == 0) {
        opserr << "OPF_GetExperimentalCP() - "
            << "none found with tag: " << tag << endln;
        return 0;
    }
    ExperimentalCP* theControl = (ExperimentalCP*)theResult;
    
    return theControl;
}


void OPF_clearExperimentalCPs()
{
    theExperimentalCPs.clearAll();
}


int OPF_ExperimentalCP()
{
    // pointer to experimental control point that will be returned
    ExperimentalCP* theCP = 0;
    
    if (OPS_GetNumRemainingInputArgs() < 3) {
        opserr << "WARNING invalid number of arguments\n";
        opserr << "Want: expControlPoint tag <-node nodeTag> dof rspType "
            << "<-fact f> <-lim l u> <-relTrial> <-relOut> <-relCtrl> <-relDaq> ...\n";
        return -1;
    }
    
    // control point tag
    int tag;
    int numdata = 1;
    if (OPS_GetIntInput(&numdata, &tag) < 0) {
        opserr << "WARNING invalid expControlPoint tag\n";
        return -1;
    }
    
    // node (optional)
    int ndf = 0;
    Domain* theDomain = 0;
    Node* theNode = 0;
    const char* type = OPS_GetString();
    if (strcmp(type, "-node") == 0) {
        int nodeTag;
        numdata = 1;
        if (OPS_GetIntInput(&numdata, &nodeTag) < 0) {
            opserr << "WARNING invalid nodeTag for control point: " << tag << endln;
            return -1;
        }
        theDomain = OPS_GetDomain();
        theNode = theDomain->getNode(nodeTag);
        ndf = theNode->getNumberDOF();
    }
    else {
        // move current arg back by one
        OPS_ResetCurrentInputArg(-1);
    }
    
    // get the dof and associated properties
    int dofID = 0;
    ID dof(32);
    ID rspType(32);
    Vector factor(32);
    Vector lowerLim(32);
    Vector upperLim(32);
    ID isRelTrial(32);
    ID isRelOut(32);
    ID isRelCtrl(32);
    ID isRelDaq(32);
    int numSignals = 0, numLimits = 0;
    int numRelTrial = 0, numRelOut = 0;
    int numRelCtrl = 0, numRelDaq = 0;
    while (OPS_GetNumRemainingInputArgs() > 0) {
        
        // dof ID
        type = OPS_GetString();
        if (sscanf(type, "%d", &dofID) != 1) {
            if (sscanf(type, "%*[dfouDFOU]%d", &dofID) != 1) {
                opserr << "WARNING invalid dof for control point: " << tag << endln;
                opserr << "Want: expControlPoint tag <-node nodeTag> dof rspType "
                    << "<-fact f> <-lim l u> <-relTrial> <-relOut> <-relCtrl> <-relDaq> ...\n";
                return -1;
            }
        }
        if (dofID < 1 || (ndf != 0 && ndf < dofID)) {
            opserr << "WARNING out-of-bound dof for control point: " << tag << endln;
            opserr << "Want: expControlPoint tag <-node nodeTag> dof rspType "
                << "<-fact f> <-lim l u> <-relTrial> <-relOut> <-relCtrl> <-relDaq> ...\n";
            return -1;
        }
        dof(numSignals) = dofID - 1;
        
        // response type
        type = OPS_GetString();
        if (strcmp(type, "1") == 0 || strcmp(type, "dsp") == 0 ||
            strcmp(type, "disp") == 0 || strcmp(type, "displacement") == 0)
            rspType(numSignals) = OF_Resp_Disp;
        else if (strcmp(type, "2") == 0 || strcmp(type, "vel") == 0 ||
            strcmp(type, "velocity") == 0)
            rspType(numSignals) = OF_Resp_Vel;
        else if (strcmp(type, "3") == 0 || strcmp(type, "acc") == 0 ||
            strcmp(type, "accel") == 0 || strcmp(type, "acceleration") == 0)
            rspType(numSignals) = OF_Resp_Accel;
        else if (strcmp(type, "4") == 0 || strcmp(type, "frc") == 0 ||
            strcmp(type, "force") == 0)
            rspType(numSignals) = OF_Resp_Force;
        else if (strcmp(type, "5") == 0 || strcmp(type, "t") == 0 ||
            strcmp(type, "tme") == 0 || strcmp(type, "time") == 0)
            rspType(numSignals) = OF_Resp_Time;
        else {
            opserr << "WARNING invalid rspType for control point: " << tag << endln;
            opserr << "Want: expControlPoint tag <-node nodeTag> dof rspType "
                << "<-fact f> <-lim l u> <-relTrial> <-relOut> <-relCtrl> <-relDaq> ...\n";
            return -1;
        }
        
        // read optional parameters until next dof ID is specified
        type = OPS_GetString();
        while (type != 0 && sscanf(type, "%d", &dofID) != 1 &&
             sscanf(type, "%*[dfouDFOU]%d", &dofID) != 1) {
            
            // scale factor
            if (type != 0 && (strcmp(type, "-fact") == 0 || strcmp(type, "-factor") == 0)) {
                numdata = 1;
                if (OPS_GetDoubleInput(&numdata, &factor(numSignals)) < 0) {
                    opserr << "WARNING invalid factor for control point: " << tag << endln;
                    opserr << "Want: expControlPoint tag <-node nodeTag> dof rspType "
                        << "<-fact f> <-lim l u> <-relTrial> <-relOut> <-relCtrl> <-relDaq> ...\n";
                    return -1;
                }
                // read next argument
                type = OPS_GetString();
            }
            else {
                factor(numSignals) = 1.0;
            }
            
            // response limits
            if (type != 0 && (strcmp(type, "-lim") == 0 || strcmp(type, "-limit") == 0)) {
                numdata = 1;
                if (OPS_GetDoubleInput(&numdata, &lowerLim(numSignals)) < 0) {
                    opserr << "WARNING invalid lower limit for control point: " << tag << endln;
                    opserr << "Want: expControlPoint tag <-node nodeTag> dof rspType "
                        << "<-fact f> <-lim l u> <-relTrial> <-relOut> <-relCtrl> <-relDaq> ...\n";
                    return -1;
                }
                if (OPS_GetDoubleInput(&numdata, &upperLim(numSignals)) < 0) {
                    opserr << "WARNING invalid upper limit for control point: " << tag << endln;
                    opserr << "Want: expControlPoint tag <-node nodeTag> dof rspType "
                        << "<-fact f> <-lim l u> <-relTrial> <-relOut> <-relCtrl> <-relDaq> ...\n";
                    return -1;
                }
                numLimits++;
                // read next argument
                type = OPS_GetString();
            }
            
            // relative signal references
            if (type != 0 && (strcmp(type, "-relTrial") == 0 ||
                strcmp(type, "-relativeTrial") == 0 ||
                strcmp(type, "-useRelTrial") == 0 ||
                strcmp(type, "-useRelativeTrial") == 0)) {
                isRelTrial(numSignals) = 1;
                numRelTrial++;
                // read next argument
                type = OPS_GetString();
            }
            if (type != 0 && (strcmp(type, "-relOut") == 0 ||
                strcmp(type, "-relativeOut") == 0 ||
                strcmp(type, "-useRelOut") == 0 ||
                strcmp(type, "-useRelativeOut") == 0)) {
                isRelOut(numSignals) = 1;
                numRelOut++;
                // read next argument
                type = OPS_GetString();
            }
            if (type != 0 && (strcmp(type, "-relCtrl") == 0 ||
                strcmp(type, "-relativeCtrl") == 0 ||
                strcmp(type, "-useRelCtrl") == 0 ||
                strcmp(type, "-useRelativeCtrl") == 0)) {
                isRelCtrl(numSignals) = 1;
                numRelCtrl++;
                // read next argument
                type = OPS_GetString();
            }
            if (type != 0 && (strcmp(type, "-relDaq") == 0 ||
                strcmp(type, "-relativeDaq") == 0 ||
                strcmp(type, "-useRelDaq") == 0 ||
                strcmp(type, "-useRelativeDaq") == 0)) {
                isRelDaq(numSignals) = 1;
                numRelDaq++;
                // read next argument
                type = OPS_GetString();
            }
        }
        
        numSignals++;
        if (OPS_GetNumRemainingInputArgs() > 0) {
            // move current arg back by one
            OPS_ResetCurrentInputArg(-1);
        }
    }
    dof.resize(numSignals);
    rspType.resize(numSignals);
    factor.resize(numSignals);
    lowerLim.resize(numSignals);
    upperLim.resize(numSignals);
    isRelTrial.resize(numSignals);
    isRelOut.resize(numSignals);
    isRelCtrl.resize(numSignals);
    isRelDaq.resize(numSignals);
    
    // parsing was successful, allocate the control point
    theCP = new ExperimentalCP(tag, dof, rspType, factor);
    
    // add limits if available
    if (numLimits > 0)
        theCP->setLimits(lowerLim, upperLim);
    
    // add signal reference types if available
    if (numRelTrial > 0)
        theCP->setTrialSigRefType(isRelTrial);
    if (numRelOut > 0)
        theCP->setOutSigRefType(isRelOut);
    if (numRelCtrl > 0)
        theCP->setCtrlSigRefType(isRelCtrl);
    if (numRelDaq > 0)
        theCP->setDaqSigRefType(isRelDaq);
    
    // add node if available
    if (theNode != 0)
        theCP->setNode(theNode);
    
    // now add the control point to the modelBuilder
    if (OPF_addExperimentalCP(theCP) == false) {
        delete theCP; // invoke the destructor, otherwise mem leak
        return -1;
    }
    
    return 0;
}


ExperimentalCP::ExperimentalCP(int tag, const ID& dof,
    const ID& rsptype, const Vector& fact)
    : TaggedObject(tag),
    numSignals(dof.Size()), numDOF(dof.Size()),
    DOF(dof), rspType(rsptype), factor(dof.Size()),
    lowerLim(0), upperLim(0),
    isRelTrial(dof.Size()), isRelOut(dof.Size()),
    isRelCtrl(dof.Size()), isRelDaq(dof.Size()),
    uniqueDOF(dof), sizeRspType(5), dofRspType(5),
    nodeTag(0), theNode(0), nodeNDM(0), nodeNDF(0)
{
    // initialize factors
    if (fact.Size() > 0) {
        if (rspType.Size() != numSignals ||
            fact.Size() != numSignals) {
            opserr << "ExperimentalCP::ExperimentalCP() - "
                << "DOF, rspType and factor arrays need to "
                << "have the same size.\n";
            exit(OF_ReturnType_failed);
        }
        factor = fact;
    }
    else {
        if (rspType.Size() != numSignals) {
            opserr << "ExperimentalCP::ExperimentalCP() - "
                << "DOF and rspType arrays need to "
                << "have the same size.\n";
            exit(OF_ReturnType_failed);
        }
        for (int i = 0; i < numSignals; i++)
            factor(i) = 1.0;
    }
    
    // find unique DOFs and number thereof
    numDOF = uniqueDOF.unique();
    /*if (numUniqueDir > ndf) {
        opserr << "ExperimentalCP::ExperimentalCP() - "
            << "more unique directions specified than "
            << "available degrees of freedom\n";
        exit(OF_ReturnType_failed);
    }*/
    
    // set sizes of response types
    sizeRspType.Zero();
    for (int i = 0; i < numSignals; i++) {
        if (rspType(i) < 0 || 4 < rspType(i)) {
            opserr << "ExperimentalCP::ExperimentalCP() - "
                << "wrong response type received.\n";
            exit(OF_ReturnType_failed);
        }
        sizeRspType(rspType(i)) += 1;
    }
    
    // zero the dofRspType vector and the isRelative IDs
    dofRspType.Zero();
    isRelTrial.Zero();
    isRelOut.Zero();
    isRelCtrl.Zero();
    isRelDaq.Zero();
}


ExperimentalCP::ExperimentalCP(const ExperimentalCP& ecp)
    : TaggedObject(ecp)
{
    numSignals = ecp.numSignals;
    numDOF = ecp.numDOF;
    
    DOF = ecp.DOF;
    rspType = ecp.rspType;
    factor = ecp.factor;
    lowerLim = ecp.lowerLim;
    upperLim = ecp.upperLim;
    isRelTrial = ecp.isRelTrial;
    isRelOut = ecp.isRelOut;
    isRelCtrl = ecp.isRelCtrl;
    isRelDaq = ecp.isRelDaq;
    
    uniqueDOF = ecp.uniqueDOF;
    sizeRspType = ecp.sizeRspType;
    dofRspType = ecp.dofRspType;
    
    nodeTag = ecp.nodeTag;
    theNode = ecp.theNode;
    nodeNDM = ecp.nodeNDM;
    nodeNDF = ecp.nodeNDF;
    nodeCrds = ecp.nodeCrds;
}


ExperimentalCP::~ExperimentalCP()
{
    // nothing to destroy
}


ExperimentalCP* ExperimentalCP::getCopy()
{
    ExperimentalCP* theCopy = new ExperimentalCP(*this);
    
    return theCopy;
}


int ExperimentalCP::setData(const ID& dof,
    const ID& rsptype, const Vector& fact)
{
    // save the number of directions
    numSignals = dof.Size();
    numDOF = dof.Size();
    DOF = dof;
    rspType = rsptype;
    
    // initialize factors
    if (fact.Size() > 0) {
        if (rspType.Size() != numSignals ||
            fact.Size() != numSignals) {
            opserr << "ExperimentalCP::setData() - "
                << "DOF, rspType and factor arrays need to "
                << "have the same size.\n";
            return OF_ReturnType_failed;
        }
        factor = fact;
    }
    else {
        if (rspType.Size() != numSignals) {
            opserr << "ExperimentalCP::setData() - "
                << "DOF and rspType arrays need to "
                << "have the same size.\n";
            return OF_ReturnType_failed;
        }
        for (int i = 0; i < numSignals; i++)
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
    for (int i = 0; i < numSignals; i++) {
        if (rspType(i) < 0 || 4 < rspType(i)) {
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


int ExperimentalCP::setLimits(const Vector& lowerlim,
    const Vector& upperlim)
{
    if (lowerlim.Size() != numSignals ||
        upperlim.Size() != numSignals) {
        opserr << "ExperimentalCP::setLimits() - "
            << "lower and upper limits need to be of "
            << "size: " << numSignals << endln;
        return OF_ReturnType_failed;
    }
    
    lowerLim = lowerlim;
    upperLim = upperlim;
    
    return 0;
}


int ExperimentalCP::setTrialSigRefType(const ID& isrelative)
{
    if (isrelative.Size() != numSignals) {
        opserr << "ExperimentalCP::setTrailSigRefType() - "
            << "trial signal reference type ID needs to be of "
            << "size: " << numSignals << endln;
        return OF_ReturnType_failed;
    }
    
    isRelTrial = isrelative;
    
    return 0;
}


int ExperimentalCP::setOutSigRefType(const ID& isrelative)
{
    if (isrelative.Size() != numSignals) {
        opserr << "ExperimentalCP::setOutSigRefType() - "
            << "out signal reference type ID needs to be of "
            << "size: " << numSignals << endln;
        return OF_ReturnType_failed;
    }
    
    isRelOut = isrelative;
    
    return 0;
}


int ExperimentalCP::setCtrlSigRefType(const ID& isrelative)
{
    if (isrelative.Size() != numSignals) {
        opserr << "ExperimentalCP::setCtrlSigRefType() - "
            << "ctrl signal reference type ID needs to be of "
            << "size: " << numSignals << endln;
        return OF_ReturnType_failed;
    }
    
    isRelCtrl = isrelative;
    
    return 0;
}


int ExperimentalCP::setDaqSigRefType(const ID& isrelative)
{
    if (isrelative.Size() != numSignals) {
        opserr << "ExperimentalCP::setDaqSigRefType() - "
            << "daq signal reference type ID needs to be of "
            << "size: " << numSignals << endln;
        return OF_ReturnType_failed;
    }
    
    isRelDaq = isrelative;
    
    return 0;
}


int ExperimentalCP::setNode(Node* node)
{
    // check node input
    if (node == 0) {
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
    dofRspType.Zero();
    for (int i = 0; i < numSignals; i++) {
        if (DOF(i) == dir)
            dofRspType(rspType(i)) = 1;
    }
    
    return dofRspType;
}


int ExperimentalCP::getNodeTag()
{
    if (nodeTag == 0) {
        opserr << "ExperimentalCP::getNodeTag() - "
            << "this control point has no node "
            << "assigned.\n";
        exit(OF_ReturnType_failed);
    }
    
    return nodeTag;
}


int ExperimentalCP::getNodeNDM()
{
    if (nodeTag == 0) {
        opserr << "ExperimentalCP::getNodeNDM() - "
            << "this control point has no node "
            << "assigned.\n";
        exit(OF_ReturnType_failed);
    }
    
    return nodeNDM;
}


int ExperimentalCP::getNodeNDF()
{
    if (nodeTag == 0) {
        opserr << "ExperimentalCP::getNodeNDF() - "
            << "this control point has no node "
            << "assigned.\n";
        exit(OF_ReturnType_failed);
    }
    
    return nodeNDF;
}


const Vector& ExperimentalCP::getNodeCrds()
{
    if (nodeTag == 0) {
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
    if (lowerLim == 0) {
        opserr << "ExperimentalCP::getLowerLimit() - "
            << "this control point has no lower "
            << "limits assigned.\n";
        exit(OF_ReturnType_failed);
    }
    
    return lowerLim;
}


const Vector& ExperimentalCP::getUpperLimit()
{
    if (upperLim == 0) {
        opserr << "ExperimentalCP::getUpperLimit() - "
            << "this control point has no upper "
            << "limits assigned.\n";
        exit(OF_ReturnType_failed);
    }
    
    return upperLim;
}


const ID& ExperimentalCP::getTrialSigRefType()
{
    return isRelTrial;
}


const ID& ExperimentalCP::getOutSigRefType()
{
    return isRelOut;
}


const ID& ExperimentalCP::getCtrlSigRefType()
{
    return isRelCtrl;
}


const ID& ExperimentalCP::getDaqSigRefType()
{
    return isRelDaq;
}


int ExperimentalCP::getDOF(int signalID)
{
    if (signalID < 0 || numSignals <= signalID) {
        opserr << "ExperimentalCP::getDOF() - "
            << "signal ID out of bounds, "
            << "component " << signalID << " does not exist.\n";
        exit(OF_ReturnType_failed);
    }
    
    return DOF(signalID);
}


int ExperimentalCP::getRspType(int signalID)
{
    if (signalID < 0 || numSignals <= signalID) {
        opserr << "ExperimentalCP::getRspType() - "
            << "signal ID out of bounds, "
            << "component " << signalID << " does not exist.\n";
        exit(OF_ReturnType_failed);
    }
    
    return rspType(signalID);
}


double ExperimentalCP::getFactor(int signalID)
{
    if (signalID < 0 || numSignals <= signalID) {
        opserr << "ExperimentalCP::getFactor() - "
            << "signal ID out of bounds, "
            << "component " << signalID << " does not exist.\n";
        exit(OF_ReturnType_failed);
    }
    
    return factor(signalID);
}


double ExperimentalCP::getLowerLimit(int signalID)
{
    if (lowerLim == 0) {
        opserr << "ExperimentalCP::getLowerLimit() - "
            << "this control point has no lower "
            << "limits assigned.\n";
        exit(OF_ReturnType_failed);
    }
    
    if (signalID < 0 || numSignals <= signalID) {
        opserr << "ExperimentalCP::getLowerLimit() - "
            << "signal ID out of bounds, "
            << "component " << signalID << " does not exist.\n";
        exit(OF_ReturnType_failed);
    }
    
    return lowerLim(signalID);
}


double ExperimentalCP::getUpperLimit(int signalID)
{
    if (upperLim == 0) {
        opserr << "ExperimentalCP::getUpperLimit() - "
            << "this control point has no upper "
            << "limits assigned.\n";
        exit(OF_ReturnType_failed);
    }
    
    if (signalID < 0 || numSignals <= signalID) {
        opserr << "ExperimentalCP::getUpperLimit() - "
            << "signal ID out of bounds, "
            << "component " << signalID << " does not exist.\n";
        exit(OF_ReturnType_failed);
    }
    
    return upperLim(signalID);
}


int ExperimentalCP::getTrialSigRefType(int signalID)
{
    if (signalID < 0 || numSignals <= signalID) {
        opserr << "ExperimentalCP::getTrialSigRefType() - "
            << "trial signal ID out of bounds, "
            << "component " << signalID << " does not exist.\n";
        exit(OF_ReturnType_failed);
    }
    
    return isRelTrial(signalID);
}


int ExperimentalCP::getOutSigRefType(int signalID)
{
    if (signalID < 0 || numSignals <= signalID) {
        opserr << "ExperimentalCP::getOutSigRefType() - "
            << "out signal ID out of bounds, "
            << "component " << signalID << " does not exist.\n";
        exit(OF_ReturnType_failed);
    }
    
    return isRelOut(signalID);
}


int ExperimentalCP::getCtrlSigRefType(int signalID)
{
    if (signalID < 0 || numSignals <= signalID) {
        opserr << "ExperimentalCP::getCtrlSigRefType() - "
            << "ctrl signal ID out of bounds, "
            << "component " << signalID << " does not exist.\n";
        exit(OF_ReturnType_failed);
    }
    
    return isRelCtrl(signalID);
}


int ExperimentalCP::getDaqSigRefType(int signalID)
{
    if (signalID < 0 || numSignals <= signalID) {
        opserr << "ExperimentalCP::getDaqSigRefType() - "
            << "daq signal ID out of bounds, "
            << "component " << signalID << " does not exist.\n";
        exit(OF_ReturnType_failed);
    }
    
    return isRelDaq(signalID);
}


void ExperimentalCP::Print(OPS_Stream& s, int flag)
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
    s << "  isRelTrial  : " << isRelTrial << endln;
    s << "  isRelOut  : " << isRelOut << endln;
    s << "  isRelCtrl  : " << isRelCtrl << endln;
    s << "  isRelDaq  : " << isRelDaq << endln;
    if (nodeTag != 0) {
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
    }
    else {
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
    }
    else {
        return 0;
    }
}
