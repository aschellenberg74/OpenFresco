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
// invokes the expTangentStiff command in the interpreter. 

#include <ExperimentalTangentStiff.h>
#include <elementAPI.h>
#include <map>

extern void* OPF_ETBroyden();
extern void* OPF_ETBfgs();
extern void* OPF_ETTranspose();


namespace {
    struct char_cmp {
        bool operator () (const char *a, const char *b) const
        {
            return strcmp(a, b) < 0;
        }
    };
    
    typedef std::map<const char *, void *(*)(void), char_cmp> OPF_ParsingFunctionMap;
    
    static OPF_ParsingFunctionMap expTangentStiffMap;
    
    static int setUpExpTangentStiff(void)
    {
        expTangentStiffMap.insert(std::make_pair("Broyden", &OPF_ETBroyden));
        expTangentStiffMap.insert(std::make_pair("BFGS", &OPF_ETBfgs));
        expTangentStiffMap.insert(std::make_pair("Transpose", &OPF_ETTranspose));
        
        return 0;
    }
}


int OPF_ExperimentalTangentStiff()
{
    static bool initDone = false;
    if (initDone == false) {
        setUpExpTangentStiff();
        initDone = true;
    }
    
    // make sure there is a minimum number of arguments
    if (OPS_GetNumRemainingInputArgs() < 2) {
        opserr << "WARNING insufficient number of experimental tangent stiff arguments\n";
        opserr << "Want: expTangentStiff type tag <specific experimental tangent stiff args>\n";
        return -1;
    }
    
    const char* type = OPS_GetString();
    
    OPF_ParsingFunctionMap::const_iterator iter = expTangentStiffMap.find(type);
    if (iter == expTangentStiffMap.end()) {
        opserr << "WARNING unknown experimental tangent stiff type: "
            << type << ": check the manual\n";
        return -1;
    }
    
    ExperimentalTangentStiff* theTangent = (ExperimentalTangentStiff*)(*iter->second)();
    if (theTangent == 0) {
        opserr << "WARNING could not create experimental tangent stiff " << type << endln;
        return -1;
    }
    
    // now add the site to the modelBuilder
    if (OPF_addExperimentalTangentStiff(theTangent) == false) {
        opserr << "WARNING could not add experimental tangent stiff.\n";
        delete theTangent;
        return -1;
    }
    
    return 0;
}
