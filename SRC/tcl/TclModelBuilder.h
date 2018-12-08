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

#ifndef TclModelBuilder_h
#define TclModelBuilder_h

// Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
// Created: 04/11
// Revision: A
//
// Description: This file contains the class definition for TclModelBuilder.
// A TclModelBuilder adds the commands to create the model for the standard
// models that can be generated using the elements released with the OpenSees 
// framework.

#include <tcl.h>
#include <ModelBuilder.h>

class TaggedObjectStorage;
//class SectionForceDeformation;
//class SectionRepres;
//class NDMaterial;
//class YieldSurface_BC;
//class YS_Evolution;
//class PlasticHardeningMaterial;
//class CyclicModel;
//class HystereticBackbone;
//class StiffnessDegradation;
//class UnloadingRule;
//class StrengthDegradation;

class TclModelBuilder : public ModelBuilder
{
public:
    // constructor & destructor
    TclModelBuilder(Domain &theDomain, Tcl_Interp *interp, int ndm, int ndf);
    ~TclModelBuilder();

    int buildFE_Model(void);
    int getNDM() const;
    int getNDF() const;

    /* methods needed for the continuum elements and generic section
    // models to add/get ND material models
    int addNDMaterial(NDMaterial &theMaterial);
    NDMaterial *getNDMaterial(int tag);

    // methods needed for the nonlinear beam column elements to
    // add/get section objects
    int addSection(SectionForceDeformation &theSection);
    SectionForceDeformation *getSection(int tag);
    int addSectionRepres(SectionRepres &theSectionRepres);
    SectionRepres *getSectionRepres(int tag);

#ifdef OO_HYSTERETIC
    // methods needed to add/get material state relationships
    int addStiffnessDegradation(StiffnessDegradation &theDegr);
    StiffnessDegradation *getStiffnessDegradation(int tag);
    int addUnloadingRule(UnloadingRule &theDegr);
    UnloadingRule *getUnloadingRule(int tag);
    int addStrengthDegradation(StrengthDegradation &theDegr);
    StrengthDegradation *getStrengthDegradation(int tag);
#endif
    int addHystereticBackbone(HystereticBackbone &theBackbone);
    HystereticBackbone *getHystereticBackbone(int tag);*/

protected:
    Tcl_Interp *theInterp;

private:
    int ndm;	// space dimension of the mesh
    int ndf;	// number of degrees of freedom per node

    /*TaggedObjectStorage *theNDMaterials;
    //TaggedObjectStorage *theSections;
    //TaggedObjectStorage *theSectionRepresents;

#ifdef OO_HYSTERETIC
    TaggedObjectStorage *theStiffnessDegradations;
    TaggedObjectStorage *theUnloadingRules;
    TaggedObjectStorage *theStrengthDegradations;
#endif
    TaggedObjectStorage *theHystereticBackbones;*/
};

#endif
