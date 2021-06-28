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
// invokes the expRecorder command in the interpreter. 

#include <Recorder.h>
#include <ExperimentalSite.h>
#include <elementAPI.h>
#include <map>

extern void* OPF_ExpSiteRecorder();
extern void* OPF_ExpSetupRecorder();
extern void* OPF_ExpControlRecorder();
extern void* OPF_ExpSignalFilterRecorder();
extern void* OPF_ExpTangentStiffRecorder();


namespace {
    struct char_cmp {
        bool operator () (const char *a, const char *b) const
        {
            return strcmp(a, b) < 0;
        }
    };
    
    typedef std::map<const char *, void *(*)(void), char_cmp> OPF_ParsingFunctionMap;
    
    static OPF_ParsingFunctionMap expRecorderMap;
    
    static int setUpExpRecorder(void)
    {
        expRecorderMap.insert(std::make_pair("Site", &OPF_ExpSiteRecorder));
        expRecorderMap.insert(std::make_pair("Setup", &OPF_ExpSetupRecorder));
        expRecorderMap.insert(std::make_pair("Control", &OPF_ExpControlRecorder));
        expRecorderMap.insert(std::make_pair("SignalFilter", &OPF_ExpSignalFilterRecorder));
        expRecorderMap.insert(std::make_pair("TangentStiff", &OPF_ExpTangentStiffRecorder));
        
        return 0;
    }
}


int OPF_ExperimentalRecorder()
{
    static bool initDone = false;
    if (initDone == false) {
        setUpExpRecorder();
        initDone = true;
    }
    
    // make sure there is a minimum number of arguments
    if (OPS_GetNumRemainingInputArgs() < 2) {
        opserr << "WARNING insufficient number of experimental recorder arguments\n";
        opserr << "Want: expRecorder type <specific recorder args>\n";
        return -1;
    }
    
    const char* type = OPS_GetString();
    
    OPF_ParsingFunctionMap::const_iterator iter = expRecorderMap.find(type);
    if (iter == expRecorderMap.end()) {
        opserr << "WARNING unknown experimental recorder type: "
            << type << ": check the manual\n";
        return -1;
    }
    
    Recorder* theRecorder = (Recorder*)(*iter->second)();
    if (theRecorder == 0) {
        opserr << "WARNING could not create experimental recorder " << type << endln;
        return -1;
    }
    
    // now add the recorder to the first experimental site
    int size = 1;
    int recTag = -1;
    ExperimentalSite* theSite = OPF_getExperimentalSiteFirst();
    if (theSite == 0) {
        opserr << "WARNING failed to get first experimental site\n";
        delete theRecorder;
        OPS_SetIntOutput(&size, &recTag, true);
        return -1;
    }
    if ((theSite->addRecorder(*theRecorder)) < 0) {
        opserr << "WARNING could not add to experimental site - expRecorder "
            << theRecorder->getTag() << endln;
        delete theRecorder;
        OPS_SetIntOutput(&size, &recTag, true);
        return -1;
    }
    
    // set recorder tag as result
    recTag = theRecorder->getTag();
    if (OPS_SetIntOutput(&size, &recTag, true) < 0) {
        opserr << "WARNING failed to return recorder tag\n";
        return -1;
    }
    
    return 0;
}


int OPF_recordExp()
{
    ExperimentalSite* theSite = OPF_getExperimentalSiteFirst();
    if (theSite == 0) {
        opserr << "WARNING failed to get first experimental site\n";
        return -1;
    }
    if ((theSite->record()) < 0) {
        opserr << "WARNING could not record\n";
        return -1;
    }

    return 0;
}