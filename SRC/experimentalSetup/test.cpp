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
// $Source: /cvsroot/openfresco/OpenFresco/experimentalSetup/test.cpp,v $

// Written: Yoshi
// Created: 09/06
// Revision: A
//
// Purpose: This file is the C++ sample standalone program for 
// ExperimentalSetup objects.

// standard C++ includes
#include <stdlib.h>
#include <iostream>
#include <fstream>
using namespace std;

#include <OPS_Globals.h>
#include <StandardStream.h>

// includes the domain classes
#include <ElasticMaterial.h>
#include <Steel01.h>

// includes OpenFresco classes
#include <FrescoGlobals.h>
#include <ExperimentalCP.h>
#include <ESOneActuator.h>
#include <ESNoTransformation.h>
#include <ECSimUniaxialMaterials.h>

// init the global variabled defined in OPS_Globals.h
StandardStream sserr;
OPS_Stream opserr = &sserr;

// main routine
int main(int argc, char **argv)
{
  // Define force-deformation relationship for spring

  // dummy specimen
  UniaxialMaterial *theMaterial1 = new ElasticMaterial(1, 5.0);
  UniaxialMaterial *theMaterial2 = new Steel01(2, 5.0, 5.0, 0.1);
  UniaxialMaterial *theMaterial3 = new Steel01(3, 5.0, 5.0, 0.1);

    
  // set ctrlData, daqData
  int numMat = 1;
  /*
  ExperimentalCP **cCP = new ExperimentalCP* [2];
  ArrayOfTaggedObjects *ctrlData = new ArrayOfTaggedObjects(1);
  cCP[0] = new ExperimentalCP(1, 1, OF_Dir_X, OF_Resp_Disp);
  cCP[1] = new ExperimentalCP(2, 1, OF_Dir_X, OF_Resp_Vel);
  for(int i=0; i<2; i++) 
    ctrlData->addComponent(cCP[i]);

  ExperimentalCP **dCP = new ExperimentalCP* [3];
  ArrayOfTaggedObjects *daqData = new ArrayOfTaggedObjects(1);
  dCP[0] = new ExperimentalCP(1, 1, OF_Dir_X, OF_Resp_Disp);
  dCP[1] = new ExperimentalCP(2, 1, OF_Dir_X, OF_Resp_Force);
  dCP[2] = new ExperimentalCP(3, 1, OF_Dir_X, OF_Resp_Vel);
  for(int i=0; i<3; i++) 
    daqData->addComponent(dCP[i]);
  */
  ID sizeTrial(OF_Resp_All), sizeOut(OF_Resp_All);
  sizeTrial[0] = numMat;
  sizeTrial[1] = numMat;
  sizeOut[0] = numMat;
  sizeOut[1] = numMat;
  sizeOut[3] = numMat;
  
  // Define ExperimentalControl
  ECSimUniaxialMaterials *theController 
    = new ECSimUniaxialMaterials(1, sizeTrial, sizeOut);
  //    = new ECSimUniaxialMaterials(1, *ctrlData, *daqData);
  //  theController->addDummySpecimen(theMaterial1);
  theController->addDummySpecimen(theMaterial2);
  //  theController->addDummySpecimen(theMaterial3);

  // Define ExperimentalSetup
  ExperimentalSetup *theSetup
    = new ESOneActuator(1, sizeTrial, sizeOut, OF_Dir_X, theController);
    //    = new ESNoTransformation(1, sizeTrial, sizeOut, theController);
  theSetup->setup();

  int numDOF = 3;

  Vector *disp, *vel, *force;
  Vector *Tdisp, *Tvel;
  disp = new Vector(numDOF);
  vel = new Vector(numDOF);
  force = new Vector(numDOF);
  Tdisp = new Vector(numDOF);
  Tvel = new Vector(numDOF);
 
  ofstream OUTD("outD.dat");
  ofstream OUTF("outF.dat");
  for(int i=0; i<100; i++) {
    for(int j=0; j<numDOF; j++) {
      (*Tdisp)[j] = 3.0*sin(2.*3.141592*i/100);
    }

    theSetup->setTrialResponse(Tdisp, Tvel, (Vector*)0, (Vector*)0, (Vector*)0);
    theSetup->getDaqResponse(disp, vel, (Vector*)0, force, (Vector*)0);

    opserr << "Tdisp = " << *Tdisp;
    opserr << "Tvel = " << *Tvel << endln;
    opserr << "disp = " << *disp;
    opserr << "vel = " << *vel;
    opserr << "force = " << *force << endln;

    for(int j=0; j<numDOF; j++) {
      OUTD << ' ' << (*disp)[j];
      OUTF << ' ' << (*force)[j];
    }
    OUTD << endl; OUTF << endl;

    theSetup->commitState();
  }
 
  return 0;
}	
	
