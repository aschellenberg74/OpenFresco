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
// $Source$

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
#include <FEM_ObjectBroker.h>

#include <Vector.h>
#include <string.h>

extern ExperimentalControl *getExperimentalControl(int tag);
extern ExperimentalSetup *getExperimentalSetup(int tag);
static ArrayOfTaggedObjects *theExperimentalSites(0);


int addExperimentalSite(ExperimentalSite &theTestSite)
{
    bool result = theExperimentalSites->addComponent(&theTestSite);
    if (result == true)
        return 0;
    else {
        opserr << "TclModelBuilder::addExperimentalSite() - "
            << "failed to add experimental site: " << theTestSite;
        return -1;
    }
}


extern ExperimentalSite *getExperimentalSite(int tag)
{
    TaggedObject *mc = theExperimentalSites->getComponentPtr(tag);
    if (mc == 0) 
        return 0;

    // otherweise we do a cast and return
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
    TCL_Char **argv, TclModelBuilder *theTclBuilder, Domain *theDomain)
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
			opserr << "WARNING invalid LocalSite tag\n";
			return TCL_ERROR;		
		}
		if (Tcl_GetInt(interp, argv[3], &setupTag) != TCL_OK)  {
			opserr << "WARNING invalid setupTag\n";
			opserr << "LocalSite site: " << tag << endln;
			return TCL_ERROR;	
		}
		ExperimentalSetup *theSetup = getExperimentalSetup(setupTag);
		if (theSetup == 0)  {
			opserr << "WARNING experimental setup not found\n";
			opserr << "expSetup: " << setupTag << endln;
			opserr << "LocalSite site: " << tag << endln;
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
            opserr << "WARNING could not add experimental site to the domain\n";
            opserr << *theSite << endln;
            delete theSite; // invoke the destructor, otherwise mem leak
            return TCL_ERROR;
        }
    }
	
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"RemoteSite") == 0)  {
		if (argc != 5 && argc != 7)  {
			opserr << "WARNING invalid number of arguments\n";
			printCommand(argc,argv);
			opserr << "Want: expSite RemoteSite tag <-setup setupTag> ipAddr ipPort\n";
			return TCL_ERROR;
		}    
		
		int tag, setupTag, ipPort;
		char *ipAddr;
        ExperimentalSite *theSite = 0;
		
		if (Tcl_GetInt(interp, argv[2], &tag) != TCL_OK)  {
			opserr << "WARNING invalid RemoteSite tag\n";
			return TCL_ERROR;		
		}

        if (argc == 5)  {
            ipAddr = (char *)malloc((strlen(argv[3]) + 1)*sizeof(char));
            strcpy(ipAddr,argv[3]);
            if (Tcl_GetInt(interp, argv[4], &ipPort) != TCL_OK)  {
                opserr << "WARNING invalid RemoteSite ipPort\n";
                return TCL_ERROR;		
            }
            
            startup_socket();
            TCP_Socket *theChannel = new TCP_Socket(ipPort,ipAddr);
            //FEM_ObjectBroker *theBroker = new FEM_ObjectBroker();
            
            // parsing was successful, allocate the site
            theSite = new RemoteExpSite(tag, *theChannel);
        }
        if (argc == 7)  {
            if (Tcl_GetInt(interp, argv[4], &setupTag) != TCL_OK)  {
                opserr << "WARNING invalid setupTag\n";
                opserr << "RemoteSite site: " << tag << endln;
                return TCL_ERROR;	
            }
            ExperimentalSetup *theSetup = getExperimentalSetup(setupTag);
            if (theSetup == 0)  {
                opserr << "WARNING experimental setup not found\n";
                opserr << "expSetup: " << setupTag << endln;
                opserr << "RemoteSite site: " << tag << endln;
                return TCL_ERROR;
            }
            ipAddr = (char *)malloc((strlen(argv[5]) + 1)*sizeof(char));
            strcpy(ipAddr,argv[5]);
            if (Tcl_GetInt(interp, argv[6], &ipPort) != TCL_OK)  {
                opserr << "WARNING invalid RemoteSite ipPort\n";
                return TCL_ERROR;		
            }
            
            startup_socket();
            TCP_Socket *theChannel = new TCP_Socket(ipPort,ipAddr);
            //FEM_ObjectBroker *theBroker = new FEM_ObjectBroker();
            
            // parsing was successful, allocate the site
            theSite = new RemoteExpSite(tag, theSetup, *theChannel);
        }

        if (theSite == 0)  {
            opserr << "WARNING could not create experimental site " << argv[1] << endln;
            return TCL_ERROR;
        }
        
        // now add the site to the modelBuilder
        if (addExperimentalSite(*theSite) < 0)  {
            opserr << "WARNING could not add experimental site to the domain\n";
            opserr << *theSite << endln;
            delete theSite; // invoke the destructor, otherwise mem leak
            return TCL_ERROR;
        }
    }
	
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"ActorSite") == 0)  {
		if (argc != 6)  {
			opserr << "WARNING invalid number of arguments\n";
			printCommand(argc,argv);
			opserr << "Want: expSite ActorSite tag -setup setupTag ipPort\n"
                << "  or: expSite ActorSite tag -control ctrlTag ipPort\n";
			return TCL_ERROR;
		}    
		
		int tag, setupTag, ctrlTag, ipPort;
        ActorExpSite *theSite = 0;
		
		if (Tcl_GetInt(interp, argv[2], &tag) != TCL_OK)  {
			opserr << "WARNING invalid ActorSite tag\n";
			return TCL_ERROR;		
		}

        if (strcmp(argv[3], "-setup") == 0)  {
            if (Tcl_GetInt(interp, argv[4], &setupTag) != TCL_OK)  {
                opserr << "WARNING invalid setupTag\n";
                opserr << "ActorSite site: " << tag << endln;
                return TCL_ERROR;	
            }
            ExperimentalSetup *theSetup = getExperimentalSetup(setupTag);
            if (theSetup == 0)  {
                opserr << "WARNING experimental setup not found\n";
                opserr << "expSetup: " << setupTag << endln;
                opserr << "ActorSite site: " << tag << endln;
                return TCL_ERROR;
            }
            if (Tcl_GetInt(interp, argv[5], &ipPort) != TCL_OK)  {
                opserr << "WARNING invalid ActorSite ipPort\n";
                return TCL_ERROR;		
            }
            
            startup_socket();
            TCP_Socket *theChannel = new TCP_Socket(ipPort);
            //FEM_ObjectBroker *theBroker = new FEM_ObjectBroker();
            
            // parsing was successful, allocate the site
            theSite = new ActorExpSite(tag, theSetup, *theChannel);
        }
        if (strcmp(argv[3], "-control") == 0)  {
            if (Tcl_GetInt(interp, argv[4], &ctrlTag) != TCL_OK)  {
                opserr << "WARNING invalid ctrlTag\n";
                opserr << "ActorSite site: " << tag << endln;
                return TCL_ERROR;	
            }
            ExperimentalControl *theControl = getExperimentalControl(ctrlTag);
            if (theControl == 0)  {
                opserr << "WARNING experimental control not found\n";
                opserr << "expControl: " << ctrlTag << endln;
                opserr << "ActorSite site: " << tag << endln;
                return TCL_ERROR;
            }
            if (Tcl_GetInt(interp, argv[5], &ipPort) != TCL_OK)  {
                opserr << "WARNING invalid ActorSite ipPort\n";
                return TCL_ERROR;		
            }
            
            startup_socket();
            TCP_Socket *theChannel = new TCP_Socket(ipPort);
            //FEM_ObjectBroker *theBroker = new FEM_ObjectBroker();
            
            // parsing was successful, allocate the site
            theSite = new ActorExpSite(tag, theControl, *theChannel);
        }

        if (theSite == 0)  {
            opserr << "WARNING could not create experimental site " << argv[1] << endln;
            return TCL_ERROR;
        }
        
        // now add the site to the modelBuilder
        if (addExperimentalSite(*theSite) < 0)  {
            opserr << "WARNING could not add experimental site to the domain\n";
            opserr << *theSite << endln;
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
