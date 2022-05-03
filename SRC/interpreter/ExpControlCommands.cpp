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
// invokes the expControl command in the interpreter. 

#include <ExperimentalControl.h>
#include <ExperimentalCP.h>
#include <ExperimentalSignalFilter.h>
#include <elementAPI.h>
#include <map>

extern void* OPF_ECSimUniaxialMaterial();
extern void* OPF_ECSimDomain();
extern void* OPF_ECSimFEAdapter();
extern void* OPF_ECSimSimulink();
extern void* OPF_ECGenericTCP();
extern void* OPF_ECLabVIEW();
extern void* OPF_ECxPCtarget();
extern void* OPF_ECSCRAMNet();
extern void* OPF_ECSCRAMNetGT();
#ifndef _WIN64
extern void* OPF_ECdSpace();
extern void* OPF_ECMtsCsi();
//extern void* OPF_ECNIEseries();
#endif


namespace {
    struct char_cmp {
        bool operator () (const char *a, const char *b) const
        {
            return strcmp(a, b) < 0;
        }
    };
    
    typedef std::map<const char *, void *(*)(void), char_cmp> OPF_ParsingFunctionMap;
    
    static OPF_ParsingFunctionMap expControlMap;
    
    static int setUpExpControl(void)
    {
        expControlMap.insert(std::make_pair("SimUniaxialMaterials", &OPF_ECSimUniaxialMaterial));
        expControlMap.insert(std::make_pair("SimDomain", &OPF_ECSimDomain));
        expControlMap.insert(std::make_pair("SimFEAdapter", &OPF_ECSimFEAdapter));
        expControlMap.insert(std::make_pair("SimSimulink", &OPF_ECSimSimulink));
        expControlMap.insert(std::make_pair("GenericTCP", &OPF_ECGenericTCP));
        expControlMap.insert(std::make_pair("LabVIEW", &OPF_ECLabVIEW));
        expControlMap.insert(std::make_pair("xPCtarget", &OPF_ECxPCtarget));
        expControlMap.insert(std::make_pair("SCRAMNet", &OPF_ECSCRAMNet));
        expControlMap.insert(std::make_pair("SCRAMNetGT", &OPF_ECSCRAMNetGT));
#ifndef _WIN64
        expControlMap.insert(std::make_pair("dSpace", &OPF_ECdSpace));
        expControlMap.insert(std::make_pair("MTSCsi", &OPF_ECMtsCsi));
        //expControlMap.insert(std::make_pair("NIEseries", &OPF_ECNIEseries));
#endif
        return 0;
    }
}


int OPF_ExperimentalControl()
{
    static bool initDone = false;
    if (initDone == false) {
        setUpExpControl();
        initDone = true;
    }
    
    // make sure there is a minimum number of arguments
    if (OPS_GetNumRemainingInputArgs() < 2) {
        opserr << "WARNING insufficient number of experimental control arguments\n";
        opserr << "Want: expControl type tag <specific experimental control args>\n";
        return -1;
    }
    
    const char* type = OPS_GetString();
    
    OPF_ParsingFunctionMap::const_iterator iter = expControlMap.find(type);
    if (iter == expControlMap.end()) {
        opserr << "WARNING unknown experimental control type: "
            << type << ": check the manual\n";
        return -1;
    }
    
	ExperimentalControl* theControl = (ExperimentalControl*)(*iter->second)();
    if (theControl == 0) {
        opserr << "WARNING could not create experimental control " << type << endln;
        return -1;
    }
    
    // finally check for signal filters
    int filterTag;
    int numdata = 1;
    ExperimentalSignalFilter* theFilter = 0;
    while (OPS_GetNumRemainingInputArgs() > 0) {
        type = OPS_GetString();
        if (strcmp(type, "-ctrlFilter") == 0 ||
            strcmp(type, "-ctrlFilters") == 0) {
            for (int j = 0; j < 5; j++) {
                if (OPS_GetIntInput(&numdata, &filterTag) < 0) {
                    opserr << "WARNING invalid ctrlFilter tag\n";
                    opserr << "expControl: " << theControl->getTag() << endln;
                    return -1;
                }
                if (filterTag > 0) {
                    theFilter = OPF_getExperimentalSignalFilter(filterTag);
                    if (theFilter == 0) {
                        opserr << "WARNING experimental signal filter not found\n";
                        opserr << "expSignalFilter: " << filterTag << endln;
                        opserr << "expControl: " << theControl->getTag() << endln;
                        return -1;
                    }
                    theControl->setCtrlFilter(theFilter, j);
                }
            }
        }
        if (strcmp(type, "-daqFilter") == 0 ||
            strcmp(type, "-daqFilters") == 0) {
            for (int j = 0; j < 5; j++) {
                if (OPS_GetIntInput(&numdata, &filterTag) < 0) {
                    opserr << "WARNING invalid daqFilter tag\n";
                    opserr << "expControl: " << theControl->getTag() << endln;
                    return -1;
                }
                if (filterTag > 0) {
                    theFilter = OPF_getExperimentalSignalFilter(filterTag);
                    if (theFilter == 0) {
                        opserr << "WARNING experimental signal filter not found\n";
                        opserr << "expSignalFilter: " << filterTag << endln;
                        opserr << "expControl: " << theControl->getTag() << endln;
                        return -1;
                    }
                    theControl->setDaqFilter(theFilter, j);
                }
            }
        }
    }
    
    // now add the control to the modelBuilder
    if (OPF_addExperimentalControl(theControl) == false) {
        opserr << "WARNING could not add experimental control.\n";
        delete theControl;
        return -1;
    }
    
    return 0;
}
