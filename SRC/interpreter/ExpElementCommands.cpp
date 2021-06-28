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
// invokes the expElement command in the interpreter. 

#include <ExperimentalElement.h>
#include <Domain.h>
#include <elementAPI.h>
#include <map>

extern void* OPF_EETruss();
extern void* OPF_EETrussCorot();
extern void* OPF_EEBeamColumn2d();
extern void* OPF_EEBeamColumn3d();
extern void* OPF_EETwoNodeLink();
extern void* OPF_EEGeneric();
extern void* OPF_EEInvertedVBrace2d();
extern void* OPF_EEBearing2d();
extern void* OPF_EEBearing3d();


namespace {
    struct char_cmp {
        bool operator () (const char *a, const char *b) const
        {
            return strcmp(a, b) < 0;
        }
    };
    
    typedef std::map<const char *, void *(*)(void), char_cmp> OPF_ParsingFunctionMap;
    
    static OPF_ParsingFunctionMap expElementMap;
    
    static void* OPF_EEBeamColumn()
    {
        int ndm = OPS_GetNDM();
        if (ndm == 2)
            return OPF_EEBeamColumn2d();
        else
            return OPF_EEBeamColumn3d();
    }
    
    static void* OPF_EEInvertedVBrace()
    {
        int ndm = OPS_GetNDM();
        if (ndm == 2)
            return OPF_EEInvertedVBrace2d();
        else {
            opserr << "WARNING expElement invertedVBrace command"
                << " not implemented yet for ndm = 3\n";
            return 0;
        }
    }
    
    static void* OPF_EEBearing()
    {
        int ndm = OPS_GetNDM();
        if (ndm == 2)
            return OPF_EEBearing2d();
        else
            return OPF_EEBearing3d();
    }
    
    static int setUpExpElement(void)
    {
        expElementMap.insert(std::make_pair("truss", &OPF_EETruss));
        expElementMap.insert(std::make_pair("corotTruss", &OPF_EETrussCorot));
        expElementMap.insert(std::make_pair("beamColumn", &OPF_EEBeamColumn));
        expElementMap.insert(std::make_pair("twoNodeLink", &OPF_EETwoNodeLink));
        expElementMap.insert(std::make_pair("generic", &OPF_EEGeneric));
        expElementMap.insert(std::make_pair("invertedVBrace", &OPF_EEInvertedVBrace));
        expElementMap.insert(std::make_pair("bearing", &OPF_EEBearing));
        
        return 0;
    }
}


int OPF_ExperimentalElement()
{
    static bool initDone = false;
    if (initDone == false) {
        setUpExpElement();
        initDone = true;
    }
    
    // make sure there is a minimum number of arguments
    if (OPS_GetNumRemainingInputArgs() < 2) {
        opserr << "WARNING insufficient number of experimental element arguments\n";
        opserr << "Want: expElement type tag <specific experimental element args>\n";
        return -1;
    }
    
    const char* type = OPS_GetString();
    
    OPF_ParsingFunctionMap::const_iterator iter = expElementMap.find(type);
    if (iter == expElementMap.end()) {
        opserr << "WARNING unknown experimental element type: "
            << type << ": check the manual\n";
        return -1;
    }
    
    ExperimentalElement* theElement = (ExperimentalElement*)(*iter->second)();
    if (theElement == 0) {
        opserr << "WARNING could not create experimental element " << type << endln;
        return -1;
    }
    
    // now add the element to the domain
    Domain* theDomain = OPS_GetDomain();
    if (theDomain == 0) {
        opserr << "WARNING failed to get domain\n";
        delete theElement;
        return -1;
    }
    if (theDomain->addElement(theElement) == false) {
        opserr << "WARNING could not add element with tag: "
            << theElement->getTag() << " and of type: "
            << theElement->getClassType() << " to the domain\n";
        delete theElement;
        return -1;
    }
    
    return 0;
}
