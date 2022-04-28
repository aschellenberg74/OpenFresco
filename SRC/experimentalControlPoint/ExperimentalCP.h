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

#ifndef ExperimentalCP_h
#define ExperimentalCP_h

// Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
// Created: 02/07
// Revision: A
//
// Description: This file contains the class definition for 
// ExperimentalCP. This object possess control point data, i.e.
// DOF-IDs and response types for each DOF. Optionally the
// control point object can store scaling factors, response
// limits, signal reference types and be associated with a node.

#include <FrescoGlobals.h>

#include <DomainComponent.h>
#include <ID.h>

class Node;

class ExperimentalCP : public TaggedObject
{
public:
    // constructors
    ExperimentalCP(int tag, const ID &DOF,
        const ID &rspType, const Vector &factor = 0);
    ExperimentalCP(const ExperimentalCP &ecp);
    
    // destructor
    virtual ~ExperimentalCP();
    
    virtual ExperimentalCP *getCopy();
    virtual void Print(OPS_Stream &s, int flag);
    
    // methods to set control point data
    int setData(const ID &DOF,
        const ID &rspType, const Vector &factor = 0);
    int setLimits(const Vector &lowerLimit,
        const Vector &upperLimit);
    int setTrialSigRefType(const ID &isRelative);
    int setOutSigRefType(const ID &isRelative);
    int setCtrlSigRefType(const ID &isRelative);
    int setDaqSigRefType(const ID &isRelative);
    int setNode(Node *theNode);
    
    // methods to get derived control point data
    int getNumSignal();
    int getNumDOF();
    const ID &getSizeRspType();
    const ID &getDOFRspType(int dof);
    
    // methods to get optional nodal data
    int getNodeTag();
    int getNodeNDM();
    int getNodeNDF();
    const Vector &getNodeCrds();
    
    // methods to get basic control point data
    const ID &getDOF();
    const ID &getRspType();
    const Vector &getFactor();
    const Vector &getLowerLimit();
    const Vector &getUpperLimit();
    const ID &getTrialSigRefType();
    const ID &getOutSigRefType();
    const ID &getCtrlSigRefType();
    const ID &getDaqSigRefType();
    
    const ID &getUniqueDOF();  // DO I STILL NEED THIS ???
    
    int getDOF(int signalID);
    int getRspType(int signalID);
    double getFactor(int signalID);
    double getLowerLimit(int signalID);
    double getUpperLimit(int signalID);
    int getTrialSigRefType(int signalID);
    int getOutSigRefType(int signalID);
    int getCtrlSigRefType(int signalID);
    int getDaqSigRefType(int signalID);
    
    // methods to query information
    int hasLimits();
    int operator == (ExperimentalCP &ecp);
    int operator != (ExperimentalCP &ecp);
    
protected:
    int numSignals;     // number of signals assigned to CP
    int numDOF;         // number of unique DOF assigned to CP
    
    ID DOF;             // DOF IDs (size = numSignals) 
    ID rspType;         // response types (size = numSignals)
    Vector factor;      // scaling factors (size = numSignals)
    Vector lowerLim;    // lower limits (size = numSignals)
    Vector upperLim;    // upper limits (size = numSignals)
    ID isRelTrial;      // absolute or relative trial signal(size = numSignals)
    ID isRelOut;        // absolute or relative out signal (size = numSignals)
    ID isRelCtrl;       // absolute or relative ctrl signal (size = numSignals)
    ID isRelDaq;        // absolute or relative daq signal (size = numSignals)
    
    ID uniqueDOF;       // unique DOFs
    ID sizeRspType;     // sizes of response types
    ID dofRspType;      // response types for each unique direction
    
    int nodeTag;        // node tag with which CP is associated
    Node *theNode;      // pointer to node with which CP is associated
    int nodeNDM;        // number of dimensions
    int nodeNDF;        // number of degrees of freedom
    Vector nodeCrds;    // vector of nodal coordinates
};

extern bool OPF_addExperimentalCP(ExperimentalCP* newComponent);
extern ExperimentalCP* OPF_getExperimentalCP(int tag);
extern bool OPF_removeExperimentalCP(int tag);
extern void OPF_clearExperimentalCPs();
int OPF_ExperimentalCP();

#endif
