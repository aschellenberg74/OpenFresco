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
// $Source: $

// Written: Hong Kim
// Created: 04/10
// Revision: A
//
// Purpose: This file is the C++ sample standalone program for 
// testing experimental tangent object.  The test is done against
// the results produced in matlab

// standard C++ includes
#include <stdlib.h>
#include <iostream>
#include <fstream>
using namespace std;

#include <OPS_Globals.h>
#include <StandardStream.h>

// includes OpenFresco classes
#include <FrescoGlobals.h>
#include <ETBroyden.h>
#include <ETBfgs.h>
#include <ETTranspose.h>

// init the global variabled defined in OPS_Globals.h
StandardStream sserr;
OPS_Stream opserr = &sserr;

// main routine
int main(int argc, char **argv)
{
	int numDOF = 2;
	Vector Ddisp1(numDOF), Dforce1(numDOF),
	Ddisp2(numDOF), Dforce2(numDOF),
	Ddisp3(numDOF), Dforce3(numDOF),
		   Ddisp4(numDOF), Dforce4(numDOF),
		   Ddisp5(numDOF), Dforce5(numDOF),
		   Ddisp6(numDOF), Dforce6(numDOF),
	Ddisp7(numDOF), Dforce7(numDOF);
	
	Matrix tS(numDOF,numDOF), KInit(numDOF,numDOF), K(numDOF, numDOF);
	
	//Ddisp = new Vector(numDOF);
	//Dforce = new Vector(numDOF);
	//theStiff = new Matrix(numDOF,numDOF);
	//K = new Matrix(numDOF,numDOF);
	Ddisp1(0) =  0.0552;
	Ddisp1(1) =  0.0630;
	Ddisp2(0) = -0.0936;
	Ddisp2(1) =  0.0230;	
	Ddisp3(0) = -0.0275;
	Ddisp3(1) = -0.0901;
	Ddisp4(0) = -0.0021;
	Ddisp4(1) = -0.0615;
	Ddisp5(0) = -0.0754;
	Ddisp5(1) = -0.0589;
	Ddisp6(0) = -0.0707;
	Ddisp6(1) = -0.0622;
	Ddisp7(0) = -0.0915;
	Ddisp7(1) =  0.0271;
	
	Dforce1(0) =  2.4684;
	Dforce1(1) =  0.3306;
	Dforce2(0) = -4.4452;
	Dforce2(1) =  0.3482;
	Dforce3(0) = -1.1123;
	Dforce3(1) = -0.5757;
	Dforce4(0) =  0.0243;
	Dforce4(1) = -0.4263;
	Dforce5(0) = -3.4260;
	Dforce5(1) = -0.2615;
	Dforce6(0) = -1.7274;
	Dforce6(1) = -0.2940;
	Dforce7(0) = -0.6490;
	Dforce7(1) =  0.3727;
	
	KInit(0,0) = 47.00;
	KInit(0,1) = -2.00;
	KInit(1,0) = -2.00;
	KInit(1,1) =  7.00;
	
	K(0,0) = 26.546;
	K(0,1) = -2.401;
	K(1,0) = -2.401;
	K(1,1) =  7.4559;
	
	/* Load variables to be tested
	// Load displacement vector
	// OpenFile
	ifstream dispFile ("displ2.txt", ios::in);
	// Check file has opened properly
	if (!dispFile) {
		cerr << "File could not be opened" <<endl;
		exit(1);
	}
	// Load force vector
	// Load initial stiffness vector */

	// Define ExperimentalTangentStiff
	//ETBroyden *theTangentStiff = new ETBroyden(1);
	ETBfgs *theTangentStiff = new ETBfgs(1);
	//ETTranspose *theTangentStiff = new ETTranspose(1, 6);
	
	// Update the Tangent stiffness matrix
	tS = theTangentStiff->updateTangentStiff(&Ddisp1, (Vector*)0, (Vector*)0, &Dforce1, (Vector*)0, &KInit, &K);
	int i, j;
	for (i=0; i<numDOF; i++) {
		for (j=0; j<numDOF; j++) {
			opserr << tS(i,j) << "\t";
			//	opserr << K(i,j) << "\t ";
		}
		opserr << endln;
	}
	tS = theTangentStiff->updateTangentStiff(&Ddisp2, (Vector*)0, (Vector*)0, &Dforce2, (Vector*)0, &KInit, &tS);

	for (i=0; i<numDOF; i++) {
		for (j=0; j<numDOF; j++) {
			opserr << tS(i,j) << "\t";
			//	opserr << K(i,j) << "\t ";
		}
		opserr << endln;
	}
	tS = theTangentStiff->updateTangentStiff(&Ddisp3, (Vector*)0, (Vector*)0, &Dforce3, (Vector*)0, &KInit, &tS);

	for (i=0; i<numDOF; i++) {
		for (j=0; j<numDOF; j++) {
			opserr << tS(i,j) << "\t";
			//	opserr << K(i,j) << "\t ";
		}
		opserr << endln;
	}
	tS = theTangentStiff->updateTangentStiff(&Ddisp4, (Vector*)0, (Vector*)0, &Dforce4, (Vector*)0, &KInit, &tS);

	for (i=0; i<numDOF; i++) {
		for (j=0; j<numDOF; j++) {
			opserr << tS(i,j) << "\t";
			//	opserr << K(i,j) << "\t ";
		}
		opserr << endln;
	}
	tS = theTangentStiff->updateTangentStiff(&Ddisp5, (Vector*)0, (Vector*)0, &Dforce5, (Vector*)0, &KInit, &tS);

	for (i=0; i<numDOF; i++) {
		for (j=0; j<numDOF; j++) {
			opserr << tS(i,j) << "\t";
			//	opserr << K(i,j) << "\t ";
		}
		opserr << endln;
	}
	tS = theTangentStiff->updateTangentStiff(&Ddisp6, (Vector*)0, (Vector*)0, &Dforce6, (Vector*)0, &KInit, &tS);

	for (i=0; i<numDOF; i++) {
		for (j=0; j<numDOF; j++) {
			opserr << tS(i,j) << "\t";
		//	opserr << K(i,j) << "\t ";
		}
		opserr << endln;
	}

	tS = theTangentStiff->updateTangentStiff(&Ddisp7, (Vector*)0, (Vector*)0, &Dforce7, (Vector*)0, &KInit, &tS);
	
	for (i=0; i<numDOF; i++) {
		for (j=0; j<numDOF; j++) {
			opserr << tS(i,j) << "\t";
			//	opserr << K(i,j) << "\t ";
		}
		opserr << endln;
	}
  return 0;
}	
	
