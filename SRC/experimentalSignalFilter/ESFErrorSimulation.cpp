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

// Written: Yoshi
// Created: 09/06
// Revision: A
//
// Description: This file contains the implementation of ESFErrorSimulation.

#include "ESFErrorSimulation.h"


ESFErrorSimulation::ESFErrorSimulation(int tag)
    : ExperimentalSignalFilter(tag)
{
    // does nothing
}


ESFErrorSimulation::ESFErrorSimulation(const ESFErrorSimulation& esf)
    : ExperimentalSignalFilter(esf)
{
    // does nothing
}


ESFErrorSimulation::~ESFErrorSimulation()
{
    // does nothing
}


Vector& ESFErrorSimulation::converting(Vector* td)
{
    opserr << "\nWARNING ESFErrorSimulation::converting(Vector* td) - " << endln
    << "No conversion performed. The input vector is returned instead." << endln;
    return *td;
}


Vector& ESFErrorSimulation::converting(Vector* td, Vector* tf)
{
    opserr << "\nWARNING ESFErrorSimulation::converting(Vector* td, const Vector* tf) - " << endln
    << "No conversion performed. The input vector is returned instead." << endln;
    return *tf;
}


int ESFErrorSimulation::setSize(const int sz)
{
    // does nothing
    return OF_ReturnType_completed;
}
