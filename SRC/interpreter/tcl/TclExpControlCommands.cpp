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
// Created: 09/06
// Revision: A
//
// Description: This file contains the function invoked when the user
// invokes the expControl command in the interpreter. 

#include <string.h>
#include <tcl.h>
#include <ArrayOfTaggedObjects.h>
#include <Vector.h>
#include <elementAPI.h>

#include <ExperimentalControl.h>
#include <ExperimentalCP.h>

extern void* OPF_ECSimUniaxialMaterial();
//extern void* OPF_ECSimUniaxialMaterialForce();
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
extern void* OPF_ECNIEseries();
#endif


static void printCommand(int argc, TCL_Char **argv)
{
    opserr << "Input command: ";
    for (int i=0; i<argc; i++)
        opserr << argv[i] << " ";
    opserr << endln;
}


int TclExpControlCommand(ClientData clientData, Tcl_Interp *interp,
    int argc, TCL_Char **argv, Domain *theDomain)
{
    // make sure there is a minimum number of arguments
    if (argc < 3)  {
        opserr << "WARNING insufficient number of experimental control arguments\n";
        opserr << "Want: expControl type tag <specific experimental control args>\n";
        return TCL_ERROR;
    }
    
    // reset the input args
    OPS_ResetInputNoBuilder(clientData, interp, 2, argc, argv, theDomain);
    
    // pointer to control that will be added
    ExperimentalControl *theControl = 0;
    
    // ----------------------------------------------------------------------------	
    if (strcmp(argv[1],"SimUniaxialMaterials") == 0)  {
        void* theEC = OPF_ECSimUniaxialMaterial();
        if (theEC != 0)
            theControl = (ExperimentalControl*)theEC;
        else
            return TCL_ERROR;
    }
    
    /* ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"SimUniaxialMaterialsForce") == 0)  {
        if (argc < 4)  {
            opserr << "WARNING invalid number of arguments\n";
            printCommand(argc,argv);
            opserr << "Want: expControl SimUniaxialMaterialsForce tag matTags "
                << "<-ctrlFilters (5 filterTag)> <-daqFilters (5 filterTag)>\n";
            return TCL_ERROR;
        }
        
        int i, argi, tag, matTag, numMats = 0;
        
        argi = 2;
        if (Tcl_GetInt(interp, argv[argi], &tag) != TCL_OK)  {
            opserr << "WARNING invalid expControl SimUniaxialMaterialsForce tag\n";
            return TCL_ERROR;
        }
        argi++;
        // now read the number of materials
        while (argi+numMats < argc &&
            strcmp(argv[argi+numMats],"-ctrlFilters") != 0 &&
            strcmp(argv[argi+numMats],"-daqFilters") != 0)  {
                numMats++;
        }
        if (numMats == 0)  {
            opserr << "WARNING no uniaxial materials specified\n";
            opserr << "expControl SimUniaxialMaterialsForce " << tag << endln;
            return TCL_ERROR;
        }
        // create the array to hold the uniaxial materials
        UniaxialMaterial **theSpecimen = new UniaxialMaterial* [numMats];
        if (theSpecimen == 0)  {
            opserr << "WARNING out of memory\n";
            opserr << "expControl SimUniaxialMaterialsForce " << tag << endln;
            return TCL_ERROR;
        }
        for (i=0; i<numMats; i++)  {
            theSpecimen[i] = 0;
            if (Tcl_GetInt(interp, argv[argi], &matTag) != TCL_OK)  {
                opserr << "WARNING invalid matTag\n";
                opserr << "expControl SimUniaxialMaterialsForce " << tag << endln;
                return TCL_ERROR;
            }
            theSpecimen[i] = OPS_GetUniaxialMaterial(matTag);
            if (theSpecimen[i] == 0)  {
                opserr << "WARNING uniaxial material not found\n";
                opserr << "uniaxialMaterial " << matTag << endln;
                opserr << "expControl SimUniaxialMaterialsForce " << tag << endln;
                return TCL_ERROR;
            }
            argi++;
        }
        
        // parsing was successful, allocate the control
        theControl = new ECSimUniaxialMaterialsForce(tag, numMats, theSpecimen);
        
        // cleanup dynamic memory
        if (theSpecimen != 0)
            delete [] theSpecimen;
    }*/
    
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"SimDomain") == 0)  {
        void* theCtrl = OPF_ECSimDomain();
        if (theCtrl != 0)
            theControl = (ExperimentalControl*)theCtrl;
        else
            return TCL_ERROR;
    }
    
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"SimFEAdapter") == 0)  {
        void* theCtrl = OPF_ECSimFEAdapter();
        if (theCtrl != 0)
            theControl = (ExperimentalControl*)theCtrl;
        else
            return TCL_ERROR;
    }
    
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"SimSimulink") == 0)  {
        void* theCtrl = OPF_ECSimSimulink();
        if (theCtrl != 0)
            theControl = (ExperimentalControl*)theCtrl;
        else
            return TCL_ERROR;
    }
    
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"GenericTCP") == 0)  {
        void* theCtrl = OPF_ECGenericTCP();
        if (theCtrl != 0)
            theControl = (ExperimentalControl*)theCtrl;
        else
            return TCL_ERROR;
    }
    
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1], "LabVIEW") == 0) {
        void* theCtrl = OPF_ECLabVIEW();
        if (theCtrl != 0)
            theControl = (ExperimentalControl*)theCtrl;
        else
            return TCL_ERROR;
    }
    
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"xPCtarget") == 0)  {
        void* theCtrl = OPF_ECxPCtarget();
        if (theCtrl != 0)
            theControl = (ExperimentalControl*)theCtrl;
        else
            return TCL_ERROR;
    }
    
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"SCRAMNet") == 0)  {
        void* theCtrl = OPF_ECSCRAMNet();
        if (theCtrl != 0)
            theControl = (ExperimentalControl*)theCtrl;
        else
            return TCL_ERROR;
    }
    
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"SCRAMNetGT") == 0)  {
        void* theCtrl = OPF_ECSCRAMNetGT();
        if (theCtrl != 0)
            theControl = (ExperimentalControl*)theCtrl;
        else
            return TCL_ERROR;
    }
    
#ifndef _WIN64
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1], "dSpace") == 0) {
        void* theCtrl = OPF_ECdSpace();
        if (theCtrl != 0)
            theControl = (ExperimentalControl*)theCtrl;
        else
            return TCL_ERROR;
    }
    
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1], "MTSCsi") == 0) {
        void* theCtrl = OPF_ECMtsCsi();
        if (theCtrl != 0)
            theControl = (ExperimentalControl*)theCtrl;
        else
            return TCL_ERROR;
    }
    
    /* ----------------------------------------------------------------------------
    else if (strcmp(argv[1],"NIEseries") == 0)  {
        if (argc < 4)  {
            opserr << "WARNING invalid number of arguments\n";
            printCommand(argc,argv);
            opserr << "Want: expControl NIEseries tag device "
                << "<-ctrlFilters (5 filterTag)> <-daqFilters (5 filterTag)>\n";
            return TCL_ERROR;
        }

        int argi, tag, device;

        argi = 2;
        if (Tcl_GetInt(interp, argv[argi], &tag) != TCL_OK)  {
            opserr << "WARNING invalid expControl NIEseries tag\n";
            return TCL_ERROR;
        }
        argi++;
        if (Tcl_GetInt(interp, argv[argi], &device) != TCL_OK)  {
            opserr << "WARNING invalid device\n";
            opserr << "expControl NIEseries " << tag << endln;
            return TCL_ERROR;
        }
        argi++;

        // parsing was successful, allocate the control
        theControl = new ECNIEseries(tag, device);
    }*/
#endif
    
    // ----------------------------------------------------------------------------	
    else  {
        // experimental control type not recognized
        opserr << "WARNING unknown experimental control type: "
            <<  argv[1] << ": check the manual\n";
        return TCL_ERROR;
    }
    
    if (theControl == 0)  {
        opserr << "WARNING could not create experimental control " << argv[1] << endln;
        return TCL_ERROR;
    }
    
    // finally check for filters
    int i;
    for (i=3; i<argc; i++)  {
        if (i+5 < argc && strcmp(argv[i], "-ctrlFilters") == 0)  {
            int filterTag;
            ExperimentalSignalFilter *theFilter = 0;
            for (int j=0; j<5; j++)  {
                if (Tcl_GetInt(interp, argv[i+1+j], &filterTag) != TCL_OK)  {
                    opserr << "WARNING invalid ctrlFilter tag\n";
                    opserr << "expControl: " << theControl->getTag() << endln;
                    return TCL_ERROR;
                }
                if (filterTag > 0)  {
                    theFilter = OPF_GetExperimentalSignalFilter(filterTag);
                    if (theFilter == 0)  {
                        opserr << "WARNING experimental signal filter not found\n";
                        opserr << "expSignalFilter: " << filterTag << endln;
                        opserr << "expControl: " << theControl->getTag() << endln;
                        return TCL_ERROR;
                    }
                    theControl->setCtrlFilter(theFilter,j);
                }
            }
        }
    }
    for (i=3; i<argc; i++)  {
        if (i+5 < argc && strcmp(argv[i], "-daqFilters") == 0)  {
            int filterTag;
            ExperimentalSignalFilter *theFilter = 0;
            for (int j=0; j<5; j++)  {
                if (Tcl_GetInt(interp, argv[i+1+j], &filterTag) != TCL_OK)  {
                    opserr << "WARNING invalid daqFilter tag\n";
                    opserr << "expControl: " << theControl->getTag() << endln;
                    return TCL_ERROR;
                }
                if (filterTag > 0)  {
                    theFilter = OPF_GetExperimentalSignalFilter(filterTag);
                    if (theFilter == 0)  {
                        opserr << "WARNING experimental signal filter not found\n";
                        opserr << "expSignalFilter: " << filterTag << endln;
                        opserr << "expControl: " << theControl->getTag() << endln;
                        return TCL_ERROR;
                    }
                    theControl->setDaqFilter(theFilter,j);
                }
            }
        }
    }
    
    // now add the control to the modelBuilder
    if (OPF_AddExperimentalControl(theControl) == false)  {
        delete theControl; // invoke the destructor, otherwise mem leak
        return TCL_ERROR;
    }
    
    return TCL_OK;
}


int TclRemoveExpControl(ClientData clientData, Tcl_Interp *interp,
    int argc, TCL_Char **argv)
{
    if (strcmp(argv[1], "control") == 0)  {
        if (argc != 3)  {
            opserr << "WARNING invalid number of arguments\n";
            printCommand(argc, argv);
            opserr << "Want: removeExp control tag\n";
            return TCL_ERROR;
        }
        int tag;
        if (Tcl_GetInt(interp, argv[2], &tag) != TCL_OK)  {
            opserr << "WARNING invalid removeExp control tag\n";
            return TCL_ERROR;
        }
        if (OPF_RemoveExperimentalControl(tag) == false) {
            opserr << "WARNING could not remove expControl with tag " << tag << endln;
            return TCL_ERROR;
        }
        //ExperimentalControl *theControl = removeExperimentalControl(tag);
        //if (theControl != 0)  {
        //    delete theControl;
        //    theControl = 0;
        //} else  {
        //    opserr << "WARNING could not remove expControl with tag " << tag << endln;
        //    return TCL_ERROR;
        //}
    }
    else if (strcmp(argv[1], "controls") == 0)  {
        OPF_ClearExperimentalControls();
    }
    
    return TCL_OK;
}
