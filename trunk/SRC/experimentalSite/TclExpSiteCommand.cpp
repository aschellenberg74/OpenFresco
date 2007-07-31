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

// $Revision$
// $Date$
// $URL: $

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 09/06
// Revision: A
//
// Description: This file contains the function invoked when the user
// invokes the expSite command in the interpreter. 

#include <TclModelBuilder.h>
#include <ArrayOfTaggedObjects.h>

#include <LocalExpSite.h>
#include <RemoteExpSite.h>
#include <ActorExpSite.h>

#include <TCP_Socket.h>
#include <TCP_SocketSSL.h>

#include <Vector.h>
#include <string.h>

extern ExperimentalControl *getExperimentalControl(int tag);
extern ExperimentalSetup *getExperimentalSetup(int tag);
static ArrayOfTaggedObjects *theExperimentalSites(0);


int addExperimentalSite(ExperimentalSite &theSite)
{
    bool result = theExperimentalSites->addComponent(&theSite);
    if (result == true)
        return 0;
    else {
        opserr << "addExperimentalSite() - "
            << "failed to add experimental site: " << theSite;
        return -1;
    }
}


extern ExperimentalSite *getExperimentalSite(int tag)
{
    if (theExperimentalSites == 0) {
        opserr << "getExperimentalSite() - "
            << "failed to get experimental site: " << tag << endln
            << "no experimental site objects have been defined\n";
        return 0;
    }

    TaggedObject *mc = theExperimentalSites->getComponentPtr(tag);
    if (mc == 0) 
        return 0;

    // otherwise we do a cast and return
    ExperimentalSite *result = (ExperimentalSite *)mc;
    return result;
}


static void printCommand(int argc, TCL_Char **argv)
{
    opserr << "Input command: ";
    for (int i=0; i<argc; i++)
		opserr << argv[i] << " ";
    opserr << endln;
} 


int TclExpSiteCommand(ClientData clientData, Tcl_Interp *interp, int argc,
    TCL_Char **argv, Domain *theDomain, TclModelBuilder *theTclBuilder)
{
    if (theExperimentalSites == 0)
        theExperimentalSites = new ArrayOfTaggedObjects(32);

    // make sure there is a minimum number of arguments
    if (argc < 3)  {
		opserr << "WARNING insufficient number of experimental site arguments\n";
		opserr << "Want: expSite type tag <specific experimental site args>\n";
		return TCL_ERROR;
    }    
	
    // ----------------------------------------------------------------------------	
    if (strcmp(argv[1],"LocalSite") == 0)  {
		if (argc != 4)  {
			opserr << "WARNING invalid number of arguments\n";
			printCommand(argc,argv);
			opserr << "Want: expSite LocalSite tag setupTag\n";
			return TCL_ERROR;
		}    
		
		int tag, setupTag;
        ExperimentalSite *theSite = 0;
		
		if (Tcl_GetInt(interp, argv[2], &tag) != TCL_OK)  {
			opserr << "WARNING invalid expSite LocalSite tag\n";
			return TCL_ERROR;		
		}
		if (Tcl_GetInt(interp, argv[3], &setupTag) != TCL_OK)  {
			opserr << "WARNING invalid setupTag\n";
			opserr << "expSite LocalSite " << tag << endln;
			return TCL_ERROR;	
		}
		ExperimentalSetup *theSetup = getExperimentalSetup(setupTag);
		if (theSetup == 0)  {
			opserr << "WARNING experimental setup not found\n";
			opserr << "expSetup: " << setupTag << endln;
			opserr << "expSite LocalSite " << tag << endln;
			return TCL_ERROR;
		}
		
		// parsing was successful, allocate the site
		theSite = new LocalExpSite(tag, theSetup);

        if (theSite == 0)  {
            opserr << "WARNING could not create experimental site " << argv[1] << endln;
            return TCL_ERROR;
        }
        
        // now add the site to the modelBuilder
        if (addExperimentalSite(*theSite) < 0)  {
            delete theSite; // invoke the destructor, otherwise mem leak
            return TCL_ERROR;
        }
    }
	
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"RemoteSite") == 0)  {
		if (5 > argc && argc > 9)  {
			opserr << "WARNING invalid number of arguments\n";
			printCommand(argc,argv);
			opserr << "Want: expSite RemoteSite tag <-setup setupTag> ipAddr ipPort <-ssl> <-dataSize size>\n";
			return TCL_ERROR;
		}
		
		int tag, setupTag, ipPort, argi;
		char *ipAddr;
        int ssl = 0;
        int dataSize = OF_Network_dataSize;
        ExperimentalSetup *theSetup = 0;
        Channel *theChannel = 0;
        RemoteExpSite *theSite = 0;
		
		if (Tcl_GetInt(interp, argv[2], &tag) != TCL_OK)  {
			opserr << "WARNING invalid expSite RemoteSite tag\n";
			return TCL_ERROR;		
		}
        argi = 3;
        // get the experimental setup if provided
        if (strcmp(argv[argi], "-setup") == 0)  {
            argi++;
            if (Tcl_GetInt(interp, argv[argi], &setupTag) != TCL_OK)  {
                opserr << "WARNING invalid setupTag\n";
                opserr << "expSite RemoteSite " << tag << endln;
                return TCL_ERROR;	
            }
            theSetup = getExperimentalSetup(setupTag);
            if (theSetup == 0)  {
                opserr << "WARNING experimental setup not found\n";
                opserr << "expSetup: " << setupTag << endln;
                opserr << "expSite RemoteSite " << tag << endln;
                return TCL_ERROR;
            }
            argi++;
        }
        // get ip-address and ip-port
        ipAddr = (char *)malloc((strlen(argv[argi]) + 1)*sizeof(char));
        strcpy(ipAddr,argv[argi]);
        argi++;
        if (Tcl_GetInt(interp, argv[argi], &ipPort) != TCL_OK)  {
            opserr << "WARNING invalid RemoteSite ipPort\n";
            opserr << "expSite RemoteSite " << tag << endln;
            return TCL_ERROR;		
        }
        argi++;
        // check for optional arguments
        for (int i = argi; i < argc; i++)  {
            if (strcmp(argv[i], "-ssl") == 0)  {
                ssl = 1;
            }
            else if (strcmp(argv[i], "-dataSize") == 0)  {
		        if (Tcl_GetInt(interp, argv[i+1], &dataSize) != TCL_OK)  {
			        opserr << "WARNING invalid RemoteSite dataSize\n";
                    opserr << "expSite RemoteSite " << tag << endln;
			        return TCL_ERROR;		
		        }
            }
        }

        // setup the connection
        if (!ssl)
            theChannel = new TCP_Socket(ipPort,ipAddr,1);
        else
            theChannel = new TCP_SocketSSL(ipPort,ipAddr,1);

        // parsing was successful, allocate the site
        if (theSetup == 0)
            theSite = new RemoteExpSite(tag, *theChannel, dataSize);
        else
            theSite = new RemoteExpSite(tag, theSetup, *theChannel, dataSize);

        if (theSite == 0)  {
            opserr << "WARNING could not create experimental site " << argv[1] << endln;
            return TCL_ERROR;
        }
        
        // now add the site to the modelBuilder
        if (addExperimentalSite(*theSite) < 0)  {
            delete theSite; // invoke the destructor, otherwise mem leak
            return TCL_ERROR;
        }
    }
	
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"ActorSite") == 0)  {
		if (6 > argc || argc > 8)  {
			opserr << "WARNING invalid number of arguments\n";
			printCommand(argc,argv);
			opserr << "Want: expSite ActorSite tag -setup setupTag ipPort <-ssl>\n"
                << "  or: expSite ActorSite tag -control ctrlTag ipPort <-ssl>\n";
			return TCL_ERROR;
		}    
		
		int tag, setupTag, ctrlTag, ipPort, argi;
        int ssl = 0;
        ExperimentalSetup *theSetup = 0;
        ExperimentalControl *theControl = 0;
        Channel *theChannel = 0;
        ActorExpSite *theSite = 0;
		
		if (Tcl_GetInt(interp, argv[2], &tag) != TCL_OK)  {
			opserr << "WARNING invalid expSite ActorSite tag\n";
			return TCL_ERROR;		
		}
        argi = 3;
        if (strcmp(argv[argi], "-setup") == 0)  {
            argi++;
            if (Tcl_GetInt(interp, argv[argi], &setupTag) != TCL_OK)  {
                opserr << "WARNING invalid setupTag\n";
                opserr << "expSite ActorSite " << tag << endln;
                return TCL_ERROR;	
            }
            theSetup = getExperimentalSetup(setupTag);
            if (theSetup == 0)  {
                opserr << "WARNING experimental setup not found\n";
                opserr << "expSetup: " << setupTag << endln;
                opserr << "expSite ActorSite " << tag << endln;
                return TCL_ERROR;
            }
            argi++;
        }
        else if (strcmp(argv[argi], "-control") == 0)  {
            argi++;
            if (Tcl_GetInt(interp, argv[argi], &ctrlTag) != TCL_OK)  {
                opserr << "WARNING invalid ctrlTag\n";
                opserr << "expSite ActorSite " << tag << endln;
                return TCL_ERROR;	
            }
            theControl = getExperimentalControl(ctrlTag);
            if (theControl == 0)  {
                opserr << "WARNING experimental control not found\n";
                opserr << "expControl: " << ctrlTag << endln;
                opserr << "expSite ActorSite " << tag << endln;
                return TCL_ERROR;
            }
            argi++;
        }
        if (Tcl_GetInt(interp, argv[argi], &ipPort) != TCL_OK)  {
            opserr << "WARNING invalid ActorSite ipPort\n";
            opserr << "expSite ActorSite " << tag << endln;
            return TCL_ERROR;		
        }
        argi++;
        // check for optional arguments
        for (int i = argi; i < argc; i++)  {
            if (strcmp(argv[i], "-ssl") == 0)  {
                ssl = 1;
            }
        }

        // parsing was successful, setup the connection and allocate the site
        if (!ssl)  {
            theChannel = new TCP_Socket(ipPort,1);
            if (theChannel != 0) {
                opserr << "\nChannel successfully created: "
                    << "Waiting for RemoteExpSite...\n";
            } else {
                opserr << "WARNING could not create channel\n";
                opserr << "expSite ActorSite " << tag << endln;
                return TCL_ERROR;
            }
        }
        else  {
            theChannel = new TCP_SocketSSL(ipPort,1);
            if (theChannel != 0) {
                opserr << "\nSSL Channel successfully created: "
                    << "Waiting for RemoteExpSite...\n";
            } else {
                opserr << "WARNING could not create SSL channel\n";
                opserr << "expSite ActorSite " << tag << endln;
                return TCL_ERROR;
            }
        }

        // parsing was successful, allocate the site
        if (theControl == 0)
            theSite = new ActorExpSite(tag, theSetup, *theChannel);
        else if (theSetup == 0)
            theSite = new ActorExpSite(tag, theControl, *theChannel);
        
        if (theSite == 0)  {
            opserr << "WARNING could not create experimental site " << argv[1] << endln;
            return TCL_ERROR;
        }
        
        // now add the site to the modelBuilder
        if (addExperimentalSite(*theSite) < 0)  {
            delete theSite; // invoke the destructor, otherwise mem leak
            return TCL_ERROR;
        }
    }

    // ----------------------------------------------------------------------------	
    else  {
        // experimental site type not recognized
        opserr << "WARNING unknown experimental site type: "
            <<  argv[1] << ": check the manual\n";
        return TCL_ERROR;
    }

	return TCL_OK;
}
