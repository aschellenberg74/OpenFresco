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

#ifndef ExperimentalCP_h
#define ExperimentalCP_h

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 02/07
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
    ExperimentalCP(int tag, int nodeTag,
        const ID &direction, const ID &response,
        const Vector &factor = 0);
    ExperimentalCP(const ExperimentalCP &ecp);
    
    // destructor
    virtual ~ExperimentalCP();
    
    virtual ExperimentalCP *getCopy (void);
    virtual void Print(OPS_Stream &s, int flag);
    
    int setData(int nodeTag, const ID &direction,
        const ID &response, const Vector &factor = 0);
    int setLimits(const Vector &lowerLimit,
        const Vector &upperLimit);
    
    int getNodeTag();
    int getNumParameters();
    const ID &getSizeRespType();

    const ID &getDir();
    const ID &getResponseType();
    const Vector &getFactor();
    const Vector *getLowerLimit();
    const Vector *getUpperLimit();

    int getDir(int dirID);
    int getResponseType(int dirID);
    double getFactor(int dirID);
    double getLowerLimit(int dirID);
    double getUpperLimit(int dirID);
        
    int operator == (ExperimentalCP &ecp);
    int operator != (ExperimentalCP &ecp);
    
protected:
    int nodeTag;        // control node tag
    ID direction;       // directions
    ID response;        // response types
    Vector factor;      // factors
    Vector *lowerLim;   // lower limits
    Vector *upperLim;   // upper limits

    int numParam;       // number of parameters
    ID sizeRespType;    // sizes of response types

};

#endif
