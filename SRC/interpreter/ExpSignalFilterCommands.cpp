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
// Created: 01/21
// Revision: A
//
// Description: This file contains the function invoked when the user
// invokes the expSignalFilter command in the interpreter. 

#include <ExperimentalSignalFilter.h>
#include <elementAPI.h>
#include <map>

extern void* OPF_ESFErrorSimRandomGauss();
extern void* OPF_ESFErrorSimUndershoot();
extern void* OPF_ESFKrylovForceConverter();
extern void* OPF_ESFTangForceConverter();


namespace {
    struct char_cmp {
        bool operator () (const char *a, const char *b) const
        {
            return strcmp(a, b) < 0;
        }
    };
    
    typedef std::map<const char *, void *(*)(void), char_cmp> OPF_ParsingFunctionMap;
    
    static OPF_ParsingFunctionMap expSignalFilterMap;
    
    static int setUpExpSignalFilter(void)
    {
        expSignalFilterMap.insert(std::make_pair("ErrorSimRandomGauss", &OPF_ESFErrorSimRandomGauss));
        expSignalFilterMap.insert(std::make_pair("ErrorSimUndershoot", &OPF_ESFErrorSimUndershoot));
        expSignalFilterMap.insert(std::make_pair("KrylovForceConverter", &OPF_ESFKrylovForceConverter));
        expSignalFilterMap.insert(std::make_pair("TangentForceConverter", &OPF_ESFTangForceConverter));
        
        return 0;
    }
}


int OPF_ExperimentalSignalFilter()
{
    static bool initDone = false;
    if (initDone == false) {
        setUpExpSignalFilter();
        initDone = true;
    }
    
    // make sure there is a minimum number of arguments
    if (OPS_GetNumRemainingInputArgs() < 2) {
        opserr << "WARNING insufficient number of experimental signal filter arguments\n";
        opserr << "Want: expSignalFilter type tag <specific experimental signal filter args>\n";
        return -1;
    }
    
    const char* type = OPS_GetString();
    
    OPF_ParsingFunctionMap::const_iterator iter = expSignalFilterMap.find(type);
    if (iter == expSignalFilterMap.end()) {
        opserr << "WARNING unknown experimental signal filter type: "
            << type << ": check the manual\n";
        return -1;
    }
    
    ExperimentalSignalFilter* theFilter = (ExperimentalSignalFilter*)(*iter->second)();
    if (theFilter == 0) {
        opserr << "WARNING could not create experimental signal filter " << type << endln;
        return -1;
    }
    
    // now add the signal filter to the modelBuilder
    if (OPF_addExperimentalSignalFilter(theFilter) == false) {
        opserr << "WARNING could not add experimental signal filter.\n";
        delete theFilter;
        return -1;
    }
    
    return 0;
}
