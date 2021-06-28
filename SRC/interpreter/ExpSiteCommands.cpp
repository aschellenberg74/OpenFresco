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
// invokes the expSite command in the interpreter. 

#include <ExperimentalSite.h>
#include <elementAPI.h>
#include <map>

extern void* OPF_LocalExpSite();
extern void* OPF_ShadowExpSite();
extern void* OPF_ActorExpSite();


namespace {
    struct char_cmp {
        bool operator () (const char *a, const char *b) const
        {
            return strcmp(a, b) < 0;
        }
    };
    
    typedef std::map<const char *, void *(*)(void), char_cmp> OPF_ParsingFunctionMap;
    
    static OPF_ParsingFunctionMap expSiteMap;
    
    static int setUpExpSite(void)
    {
        expSiteMap.insert(std::make_pair("LocalSite", &OPF_LocalExpSite));
        expSiteMap.insert(std::make_pair("ShadowSite", &OPF_ShadowExpSite));
        expSiteMap.insert(std::make_pair("RemoteSite", &OPF_ShadowExpSite));
        expSiteMap.insert(std::make_pair("ActorSite", &OPF_ActorExpSite));
        
        return 0;
    }
}


int OPF_ExperimentalSite()
{
    static bool initDone = false;
    if (initDone == false) {
        setUpExpSite();
        initDone = true;
    }
    
    // make sure there is a minimum number of arguments
    if (OPS_GetNumRemainingInputArgs() < 2) {
        opserr << "WARNING insufficient number of experimental site arguments\n";
        opserr << "Want: expSite type tag <specific experimental site args>\n";
        return -1;
    }
    
    const char* type = OPS_GetString();
    
    OPF_ParsingFunctionMap::const_iterator iter = expSiteMap.find(type);
    if (iter == expSiteMap.end()) {
        opserr << "WARNING unknown experimental site type: "
            << type << ": check the manual\n";
        return -1;
    }
    
    ExperimentalSite* theSite = (ExperimentalSite*)(*iter->second)();
    if (theSite == 0) {
        opserr << "WARNING could not create experimental site " << type << endln;
        return -1;
    }
    
    // now add the site to the modelBuilder
    if (OPF_addExperimentalSite(theSite) == false) {
        opserr << "WARNING could not add experimental site.\n";
        delete theSite;
        return -1;
    }
    
    return 0;
}
