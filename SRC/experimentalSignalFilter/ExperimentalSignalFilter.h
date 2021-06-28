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

#ifndef ExperimentalSignalFilter_h
#define ExperimentalSignalFilter_h

// Written: Yoshi (yos@catfish.dpri.kyoto-u.ac.jp)
// Created: 09/06
// Revision: A
//
// Description: This file contains the class definition for 
// ExperimentalSignalFilter.

#include <FrescoGlobals.h>
#include <ExpSignalFilterResponse.h>

#include <TaggedObject.h>
#include <ID.h>
#include <Vector.h>

class Response;

class ExperimentalSignalFilter : public TaggedObject
{
public:
    // constructors
    ExperimentalSignalFilter(int tag);
    ExperimentalSignalFilter(const ExperimentalSignalFilter& esf);
    
    // method to get class type
    virtual const char *getClassType() const;
    
    // destructor
    virtual ~ExperimentalSignalFilter();
    
    virtual double filtering(double data) = 0;
    virtual Vector& converting(Vector* td) = 0;
    virtual Vector& converting(Vector* dd, Vector* df) = 0;
    virtual int setSize(const int sz) = 0;
    virtual void update() = 0;
    
    virtual ExperimentalSignalFilter *getCopy() = 0;
    
    // public methods for experimental signal filter recorder
    virtual Response *setResponse(const char **argv, int argc,
        OPS_Stream &output);
    virtual int getResponse(int responseID, Information &info);
};

extern bool OPF_addExperimentalSignalFilter(ExperimentalSignalFilter* newComponent);
extern bool OPF_removeExperimentalSignalFilter(int tag);
extern ExperimentalSignalFilter* OPF_getExperimentalSignalFilter(int tag);
extern void OPF_clearExperimentalSignalFilters();

#endif
