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

#ifndef ExperimentalCP_h
#define ExperimentalCP_h

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 02/07
// Revision: A
//
// Description: This file contains the class definition for 
// ExperimentalCP. This object possess control point data, i.e.
// control node tag, direction, and response.

#include <FrescoGlobals.h>

#include <DomainComponent.h>
#include <ID.h>

class ExperimentalCP : public TaggedObject
{
public:
    // constructors
    ExperimentalCP(int tag, int ndm, int ndf,
        int nodeTag, const ID &direction,
        const ID &response, const Vector &factor = 0);
    ExperimentalCP(const ExperimentalCP &ecp);
    
    // destructor
    virtual ~ExperimentalCP();
    
    virtual ExperimentalCP *getCopy();
    virtual void Print(OPS_Stream &s, int flag);
    
    // methods to set control point data
    int setNDM(int ndm);
    int setNDF(int ndf);
    int setData(int nodeTag, const ID &direction,
        const ID &response, const Vector &factor = 0);
    int setLimits(const Vector &lowerLimit,
        const Vector &upperLimit);
    
    // methods to get derived control point data
    int getNDM();
    int getNDF();
    int getNodeTag();
    int getNumDirection();
    int getNumUniqueDir();
    const ID &getSizeRespType();
    const ID &getDirRespType(int dir);

    // methods to get basic control point data
    const ID &getDirection();
    const ID &getUniqueDir();
    const ID &getResponseType();
    const Vector &getFactor();
    const Vector &getLowerLimit();
    const Vector &getUpperLimit();

    int getDirection(int dirID);
    int getResponseType(int dirID);
    double getFactor(int dirID);
    double getLowerLimit(int dirID);
    double getUpperLimit(int dirID);
    
    // methods to query information
    int hasLimits();
    int operator == (ExperimentalCP &ecp);
    int operator != (ExperimentalCP &ecp);
    
protected:
    int ndm;            // number of dimensions
    int ndf;            // number of degrees of freedom
    int nodeTag;        // control node tag
    ID direction;       // numDirection directions
    ID uniqueDir;       // numUniqueDir unique directions
    ID response;        // numDirection response types
    Vector factor;      // numDirection factors
    Vector lowerLim;    // lower limits
    Vector upperLim;    // upper limits

    int numDirection;   // number of directions
    int numUniqueDir;   // number of unique directions
    ID sizeRespType;    // sizes of response types
    ID dirRespType;     // response types for each unique direction
};

#endif
