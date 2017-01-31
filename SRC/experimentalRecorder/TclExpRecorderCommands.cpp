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
// $URL$

// Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
// Created: 08/08
// Revision: A
//
// Description: This file contains the function invoked when the user
// invokes the expRecorder command in the interpreter. 

#include <tcl.h>
#include <ID.h>
#include <Domain.h>
#include <SimulationInformation.h>
#include <elementAPI.h>
#include <ExperimentalSite.h>

// recorders
#include <ExpSiteRecorder.h>
#include <ExpSetupRecorder.h>
#include <ExpControlRecorder.h>
#include <ExpSignalFilterRecorder.h>
#include <ExpTangentStiffRecorder.h>

// output streams
#include <StandardStream.h>
#include <DataFileStream.h>
#include <XmlFileStream.h>
#include <BinaryFileStream.h>
#include <TCP_Stream.h>
#include <DatabaseStream.h>

enum outputMode {STANDARD_STREAM, DATA_STREAM, XML_STREAM, DATABASE_STREAM, BINARY_STREAM, DATA_STREAM_CSV, TCP_STREAM};

extern SimulationInformation *theSimulationInfo;
extern ExperimentalSite *getExperimentalSite(int tag);
extern ExperimentalSite *getExperimentalSiteFirst();
extern ExperimentalSetup *getExperimentalSetup(int tag);
extern ExperimentalControl *getExperimentalControl(int tag);
extern ExperimentalSignalFilter *getExperimentalSignalFilter(int tag);
extern ExperimentalTangentStiff *getExperimentalTangentStiff(int tag);


int TclCreateExpRecorder(ClientData clientData, Tcl_Interp *interp, int argc,
    TCL_Char **argv, Domain *theDomain, Recorder **theRecorder)
{
    // make sure there is a minimum number of arguments
    if (argc < 2)  {
        opserr << "WARNING insufficient number of experimental recorder arguments\n";
        opserr << "Want: expRecorder type <specific recorder args>\n";
        return TCL_ERROR;
    }
    
    FE_Datastore *theRecorderDatabase = 0;
    OPS_Stream *theOutputStream = 0;
    
    TCL_Char *fileName = 0;
    TCL_Char *tableName = 0;
    
    // ----------------------------------------------------------------------------	
    if (strcmp(argv[1],"Site") == 0)  {
        int numSites = 0;
        ID *siteTags = 0;
        ExperimentalSite **theSites = 0;
        bool echoTime = false;
        outputMode eMode = STANDARD_STREAM;
        double deltaT = 0.0;
        int precision = 6;
        bool doScientific = false;
        bool closeOnWrite = false;
        const char *inetAddr = 0;
        int inetPort;
        int i, j, argi = 2;
        int flags = 0;
        int sizeData = 0;
        
        while (flags == 0 && argi < argc)  {
            
            if (strcmp(argv[argi],"-site") == 0)  {
                if (argc < argi+2)  {
                    opserr << "WARNING expRecorder Site .. -site tags .. - no site tags specified\n";
                    return TCL_ERROR;
                }
                
                // read in a list of sites until end of command or other flag
                argi++;
                int siteTag;
                siteTags = new ID(0,32);
                while (argi < argc && Tcl_GetInt(interp, argv[argi], &siteTag) == TCL_OK)  {
                    (*siteTags)[numSites] = siteTag;
                    numSites++;
                    argi++;
                }
                Tcl_ResetResult(interp);
                
                if (argi == argc)  {
                    opserr << "WARNING no response type specified for experimental site recorder\n";
                    delete siteTags;
                    return TCL_ERROR;
                }
                
                if (strcmp(argv[argi],"all") == 0)  {
                    siteTags = 0;
                    argi++;
                }
            }
            
            else if (strcmp(argv[argi],"-siteRange") == 0)  {
                if (argc < argi+3)  {
                    opserr << "WARNING expRecorder Site .. -siteRange start end  .. - no site tags specified\n";
                    return TCL_ERROR;
                }
                
                // read in start and end tags of two sites & add set [start,end]
                int start, end;
                if (Tcl_GetInt(interp, argv[argi+1], &start) != TCL_OK)  {
                    opserr << "WARNING expRecorder Site -siteRange start end - invalid start " << argv[argi+1] << endln;
                    return TCL_ERROR;
                }      
                if (Tcl_GetInt(interp, argv[argi+2], &end) != TCL_OK)  {
                    opserr << "WARNING expRecorder Site -siteRange start end - invalid end " << argv[argi+2] << endln;
                    return TCL_ERROR;
                }      
                if (start > end)  {
                    int swap = end;
                    end = start;
                    start = swap;
                }
                
                siteTags = new ID(end-start);
                if (siteTags == 0)  {
                    opserr << "WARNING expRecorder Site -siteRange start end - out of memory\n";
                    return TCL_ERROR;
                }
                for (i=start; i<=end; i++)
                    (*siteTags)[numSites++] = i;
                argi += 3;
            }
            
            else if ((strcmp(argv[argi],"-time") == 0) || (strcmp(argv[argi],"-load") == 0))  {
                echoTime = true;
                argi++;
            }
            
            else if ((strcmp(argv[argi],"-dT") == 0) || (strcmp(argv[argi],"-dt") == 0))  {
                argi++;
                if (Tcl_GetDouble(interp, argv[argi], &deltaT) != TCL_OK)
                    return TCL_ERROR;
                argi++;
            }
            
            else if (strcmp(argv[argi],"-precision") == 0)  {
                argi++;
                if (Tcl_GetInt(interp, argv[argi], &precision) != TCL_OK)
                    return TCL_ERROR;
                argi++;
            }
            
            else if (strcmp(argv[argi],"-scientific") == 0)  {
                doScientific = true;
                argi++;
            }
            
            else if (strcmp(argv[argi],"-closeOnWrite") == 0)  {
                closeOnWrite = true;
                argi++;
            }
            
            else if (strcmp(argv[argi],"-file") == 0)  {
                fileName = argv[argi+1];
                const char *pwd = OPS_GetInterpPWD();
                theSimulationInfo->addOutputFile(fileName,pwd);
                eMode = DATA_STREAM;
                argi += 2;
            }
            
            else if ((strcmp(argv[argi],"-csv") == 0) || (strcmp(argv[argi],"-fileCSV") == 0))  {
                fileName = argv[argi+1];
                const char *pwd = OPS_GetInterpPWD();
                theSimulationInfo->addOutputFile(fileName,pwd);
                eMode = DATA_STREAM_CSV;
                argi += 2;
            }
            
            else if ((strcmp(argv[argi],"-xml") == 0) || (strcmp(argv[argi],"-nees") == 0))  {
                fileName = argv[argi+1];
                const char *pwd = OPS_GetInterpPWD();
                theSimulationInfo->addOutputFile(fileName,pwd);
                eMode = XML_STREAM;
                argi += 2;
            }
            
            else if ((strcmp(argv[argi],"-binary") == 0))  {
                fileName = argv[argi+1];
                const char *pwd = OPS_GetInterpPWD();
                theSimulationInfo->addOutputFile(fileName,pwd);
                eMode = BINARY_STREAM;
                argi += 2;
            }
            
            else if ((strcmp(argv[argi],"-tcp") == 0) || (strcmp(argv[argi],"-TCP") == 0))  {
                inetAddr = argv[argi+1];
                if (Tcl_GetInt(interp, argv[argi+2], &inetPort) != TCL_OK)
                    return TCL_ERROR;
                eMode = TCP_STREAM;
                argi += 3;
            }
            
            else if (strcmp(argv[argi],"-database") == 0)  {
                theRecorderDatabase = OPS_GetFEDatastore();
                if (theRecorderDatabase != 0)  {
                    tableName = argv[argi+1];
                    eMode = DATABASE_STREAM;
                } else {
                    opserr << "WARNING expRecorder Site .. -database &lt;fileName&gt; - NO CURRENT DATABASE, results to File instead\n";
                    fileName = argv[argi+1];
                }
                argi += 2;
            }
            
            else  {
                // first unknown string then is assumed to start 
                // experimental site response requests
                sizeData = argc-argi;
                flags = 1;
            }
        }
        
        if (sizeData <= 0)
            opserr << "WARNING no data response type specified for experimental site recorder\n";
        
        const char **data = new const char *[sizeData];
        
        for (i=argi, j=0; i<argc; i++, j++)
            data[j] = argv[i];
        
        // construct the theOutputStream
        if (eMode == DATA_STREAM && fileName != 0)  {
            theOutputStream = new DataFileStream(fileName, OVERWRITE, 2, 0, closeOnWrite, precision, doScientific);
        } else if (eMode == DATA_STREAM_CSV && fileName != 0)  {
            theOutputStream = new DataFileStream(fileName, OVERWRITE, 2, 1, closeOnWrite, precision, doScientific);
        } else if (eMode == XML_STREAM && fileName != 0)  {
            theOutputStream = new XmlFileStream(fileName);
        } else if (eMode == BINARY_STREAM && fileName != 0)  {
            theOutputStream = new BinaryFileStream(fileName);
        } else if (eMode == TCP_STREAM && inetAddr != 0)  {
            theOutputStream = new TCP_Stream(inetPort, inetAddr);
        } else if (eMode == DATABASE_STREAM && tableName != 0)  {
            theOutputStream = new DatabaseStream(OPS_GetFEDatastore(), tableName);
        } else
            theOutputStream = new StandardStream();
        
        // set precision for stream
        theOutputStream->setPrecision(precision);
        
        // construct array of experimental sites
        theSites = new ExperimentalSite* [numSites];
        if (theSites == 0)  {
            opserr << "WARNING out of memory creating experimental site recorder\n";
            return TCL_ERROR;
        }
        for (int i=0; i<numSites; i++)  {
            theSites[i] = getExperimentalSite((*siteTags)(i));
            if (theSites[i] == 0)  {
                opserr << "WARNING experimental site with tag "
                    << (*siteTags)(i) << " not found\n";
                return TCL_ERROR;
            }
        }
        
        // now create the ExpSiteRecorder
        (*theRecorder) = new ExpSiteRecorder(numSites, theSites, data, sizeData, echoTime, *theOutputStream, deltaT);
        
        // cleanup dynamic memory
        if (siteTags != 0)
            delete siteTags;
        if (data != 0)
            delete [] data;
    }
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"Setup") == 0)  {
        int numSetups = 0;
        ID *setupTags = 0;
        ExperimentalSetup **theSetups = 0;
        bool echoTime = false;
        outputMode eMode = STANDARD_STREAM;
        double deltaT = 0.0;
        int precision = 6;
        bool doScientific = false;
        bool closeOnWrite = false;
        const char *inetAddr = 0;
        int inetPort;
        int i, j, argi = 2;
        int flags = 0;
        int sizeData = 0;
        
        while (flags == 0 && argi < argc)  {
            
            if (strcmp(argv[argi],"-setup") == 0)  {
                if (argc < argi+2)  {
                    opserr << "WARNING expRecorder Setup .. -setup tags .. - no setup tags specified\n";
                    return TCL_ERROR;
                }
                
                // read in a list of setups until end of command or other flag
                argi++;
                int setupTag;
                setupTags = new ID(0,32);
                while (argi < argc && Tcl_GetInt(interp, argv[argi], &setupTag) == TCL_OK)  {
                    (*setupTags)[numSetups] = setupTag;
                    numSetups++;
                    argi++;
                }
                Tcl_ResetResult(interp);
                
                if (argi == argc)  {
                    opserr << "WARNING no response type specified for experimental setup recorder\n";
                    delete setupTags;
                    return TCL_ERROR;
                }
                
                if (strcmp(argv[argi],"all") == 0)  {
                    setupTags = 0;
                    argi++;
                }
            }
            
            else if (strcmp(argv[argi],"-setupRange") == 0)  {
                if (argc < argi+3)  {
                    opserr << "WARNING expRecorder Setup .. -setupRange start end  .. - no setup tags specified\n";
                    return TCL_ERROR;
                }
                
                // read in start and end tags of two setups & add set [start,end]
                int start, end;
                if (Tcl_GetInt(interp, argv[argi+1], &start) != TCL_OK)  {
                    opserr << "WARNING expRecorder Setup -setupRange start end - invalid start " << argv[argi+1] << endln;
                    return TCL_ERROR;
                }
                if (Tcl_GetInt(interp, argv[argi+2], &end) != TCL_OK)  {
                    opserr << "WARNING expRecorder Setup -setupRange start end - invalid end " << argv[argi+2] << endln;
                    return TCL_ERROR;
                }
                if (start > end)  {
                    int swap = end;
                    end = start;
                    start = swap;
                }
                
                setupTags = new ID(end-start);
                if (setupTags == 0)  {
                    opserr << "WARNING expRecorder Setup -setupRange start end - out of memory\n";
                    return TCL_ERROR;
                }
                for (i=start; i<=end; i++)
                    (*setupTags)[numSetups++] = i;
                argi += 3;
            }
            
            else if ((strcmp(argv[argi],"-time") == 0) || (strcmp(argv[argi],"-load") == 0))  {
                echoTime = true;
                argi++;
            }
            
            else if ((strcmp(argv[argi],"-dT") == 0) || (strcmp(argv[argi],"-dt") == 0))  {
                argi++;
                if (Tcl_GetDouble(interp, argv[argi], &deltaT) != TCL_OK)
                    return TCL_ERROR;
                argi++;
            }
            
            else if (strcmp(argv[argi],"-precision") == 0)  {
                argi++;
                if (Tcl_GetInt(interp, argv[argi], &precision) != TCL_OK)
                    return TCL_ERROR;
                argi++;
            }
            
            else if (strcmp(argv[argi],"-scientific") == 0)  {
                doScientific = true;
                argi++;
            }
            
            else if (strcmp(argv[argi],"-closeOnWrite") == 0)  {
                closeOnWrite = true;
                argi++;
            }
            
            else if (strcmp(argv[argi],"-file") == 0)  {
                fileName = argv[argi+1];
                const char *pwd = OPS_GetInterpPWD();
                theSimulationInfo->addOutputFile(fileName,pwd);
                eMode = DATA_STREAM;
                argi += 2;
            }
            
            else if ((strcmp(argv[argi],"-csv") == 0) || (strcmp(argv[argi],"-fileCSV") == 0))  {
                fileName = argv[argi+1];
                const char *pwd = OPS_GetInterpPWD();
                theSimulationInfo->addOutputFile(fileName,pwd);
                eMode = DATA_STREAM_CSV;
                argi += 2;
            }
            
            else if ((strcmp(argv[argi],"-xml") == 0) || (strcmp(argv[argi],"-nees") == 0))  {
                fileName = argv[argi+1];
                const char *pwd = OPS_GetInterpPWD();
                theSimulationInfo->addOutputFile(fileName,pwd);
                eMode = XML_STREAM;
                argi += 2;
            }
            
            else if ((strcmp(argv[argi],"-binary") == 0))  {
                fileName = argv[argi+1];
                const char *pwd = OPS_GetInterpPWD();
                theSimulationInfo->addOutputFile(fileName,pwd);
                eMode = BINARY_STREAM;
                argi += 2;
            }
            
            else if ((strcmp(argv[argi],"-tcp") == 0) || (strcmp(argv[argi],"-TCP") == 0))  {
                inetAddr = argv[argi+1];
                if (Tcl_GetInt(interp, argv[argi+2], &inetPort) != TCL_OK)
                    return TCL_ERROR;
                eMode = TCP_STREAM;
                argi += 3;
            }
            
            else if (strcmp(argv[argi],"-database") == 0)  {
                theRecorderDatabase = OPS_GetFEDatastore();
                if (theRecorderDatabase != 0)  {
                    tableName = argv[argi+1];
                    eMode = DATABASE_STREAM;
                } else {
                    opserr << "WARNING expRecorder Setup .. -database &lt;fileName&gt; - NO CURRENT DATABASE, results to File instead\n";
                    fileName = argv[argi+1];
                }
                argi += 2;
            }
            
            else  {
                // first unknown string then is assumed to start
                // experimental setup response requests
                sizeData = argc-argi;
                flags = 1;
            }
        }
        
        if (sizeData <= 0)
            opserr << "WARNING no data response type specified for experimental setup recorder\n";
        
        const char **data = new const char *[sizeData];
        
        for (i=argi, j=0; i<argc; i++, j++)
            data[j] = argv[i];
        
        // construct the theOutputStream
        if (eMode == DATA_STREAM && fileName != 0)  {
            theOutputStream = new DataFileStream(fileName, OVERWRITE, 2, 0, closeOnWrite, precision, doScientific);
        } else if (eMode == DATA_STREAM_CSV && fileName != 0)  {
            theOutputStream = new DataFileStream(fileName, OVERWRITE, 2, 1, closeOnWrite, precision, doScientific);
        } else if (eMode == XML_STREAM && fileName != 0)  {
            theOutputStream = new XmlFileStream(fileName);
        } else if (eMode == BINARY_STREAM && fileName != 0)  {
            theOutputStream = new BinaryFileStream(fileName);
        } else if (eMode == TCP_STREAM && inetAddr != 0)  {
            theOutputStream = new TCP_Stream(inetPort, inetAddr);
        } else if (eMode == DATABASE_STREAM && tableName != 0)  {
            theOutputStream = new DatabaseStream(OPS_GetFEDatastore(), tableName);
        } else
            theOutputStream = new StandardStream();
        
        // set precision for stream
        theOutputStream->setPrecision(precision);
        
        // construct array of experimental setups
        theSetups = new ExperimentalSetup* [numSetups];
        if (theSetups == 0)  {
            opserr << "WARNING out of memory creating experimental setup recorder\n";
            return TCL_ERROR;
        }
        for (int i=0; i<numSetups; i++)  {
            theSetups[i] = getExperimentalSetup((*setupTags)(i));
            if (theSetups[i] == 0)  {
                opserr << "WARNING experimental setup with tag "
                    << (*setupTags)(i) << " not found\n";
                return TCL_ERROR;
            }
        }
        
        // now create the ExpSetupRecorder
        (*theRecorder) = new ExpSetupRecorder(numSetups, theSetups, data, sizeData, echoTime, *theOutputStream, deltaT);
        
        // cleanup dynamic memory
        if (setupTags != 0)
            delete setupTags;
        if (data != 0)
            delete [] data;
    }
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"Control") == 0)  {
        int numControls = 0;
        ID *controlTags = 0;
        ExperimentalControl **theControls = 0;
        bool echoTime = false;
        outputMode eMode = STANDARD_STREAM;
        double deltaT = 0.0;
        int precision = 6;
        bool doScientific = false;
        bool closeOnWrite = false;
        const char *inetAddr = 0;
        int inetPort;
        int i, j, argi = 2;
        int flags = 0;
        int sizeData = 0;
        
        while (flags == 0 && argi < argc)  {
            
            if (strcmp(argv[argi],"-control") == 0)  {
                if (argc < argi+2)  {
                    opserr << "WARNING expRecorder Control .. -control tags .. - no control tags specified\n";
                    return TCL_ERROR;
                }
                
                // read in a list of controls until end of command or other flag
                argi++;
                int controlTag;
                controlTags = new ID(0,32);
                while (argi < argc && Tcl_GetInt(interp, argv[argi], &controlTag) == TCL_OK)  {
                    (*controlTags)[numControls] = controlTag;
                    numControls++;
                    argi++;
                }
                Tcl_ResetResult(interp);
                
                if (argi == argc)  {
                    opserr << "WARNING no response type specified for experimental control recorder\n";
                    delete controlTags;
                    return TCL_ERROR;
                }
                
                if (strcmp(argv[argi],"all") == 0)  {
                    controlTags = 0;
                    argi++;
                }
            }
            
            else if (strcmp(argv[argi],"-controlRange") == 0)  {
                if (argc < argi+3)  {
                    opserr << "WARNING expRecorder Control .. -controlRange start end  .. - no control tags specified\n";
                    return TCL_ERROR;
                }
                
                // read in start and end tags of two controls & add set [start,end]
                int start, end;
                if (Tcl_GetInt(interp, argv[argi+1], &start) != TCL_OK)  {
                    opserr << "WARNING expRecorder Control -controlRange start end - invalid start " << argv[argi+1] << endln;
                    return TCL_ERROR;
                }
                if (Tcl_GetInt(interp, argv[argi+2], &end) != TCL_OK)  {
                    opserr << "WARNING expRecorder Control -controlRange start end - invalid end " << argv[argi+2] << endln;
                    return TCL_ERROR;
                }
                if (start > end)  {
                    int swap = end;
                    end = start;
                    start = swap;
                }
                
                controlTags = new ID(end-start);
                if (controlTags == 0)  {
                    opserr << "WARNING expRecorder Control -controlRange start end - out of memory\n";
                    return TCL_ERROR;
                }
                for (i=start; i<=end; i++)
                    (*controlTags)[numControls++] = i;
                argi += 3;
            }
            
            else if ((strcmp(argv[argi],"-time") == 0) || (strcmp(argv[argi],"-load") == 0))  {
                echoTime = true;
                argi++;
            }
            
            else if ((strcmp(argv[argi],"-dT") == 0) || (strcmp(argv[argi],"-dt") == 0))  {
                argi++;
                if (Tcl_GetDouble(interp, argv[argi], &deltaT) != TCL_OK)
                    return TCL_ERROR;
                argi++;
            }
            
            else if (strcmp(argv[argi],"-precision") == 0)  {
                argi++;
                if (Tcl_GetInt(interp, argv[argi], &precision) != TCL_OK)
                    return TCL_ERROR;
                argi++;
            }
            
            else if (strcmp(argv[argi],"-scientific") == 0)  {
                doScientific = true;
                argi++;
            }
            
            else if (strcmp(argv[argi],"-closeOnWrite") == 0)  {
                closeOnWrite = true;
                argi++;
            }
            
            else if (strcmp(argv[argi],"-file") == 0)  {
                fileName = argv[argi+1];
                const char *pwd = OPS_GetInterpPWD();
                theSimulationInfo->addOutputFile(fileName,pwd);
                eMode = DATA_STREAM;
                argi += 2;
            }
            
            else if ((strcmp(argv[argi],"-csv") == 0) || (strcmp(argv[argi],"-fileCSV") == 0))  {
                fileName = argv[argi+1];
                const char *pwd = OPS_GetInterpPWD();
                theSimulationInfo->addOutputFile(fileName,pwd);
                eMode = DATA_STREAM_CSV;
                argi += 2;
            }
            
            else if ((strcmp(argv[argi],"-xml") == 0) || (strcmp(argv[argi],"-nees") == 0))  {
                fileName = argv[argi+1];
                const char *pwd = OPS_GetInterpPWD();
                theSimulationInfo->addOutputFile(fileName,pwd);
                eMode = XML_STREAM;
                argi += 2;
            }
            
            else if ((strcmp(argv[argi],"-binary") == 0))  {
                fileName = argv[argi+1];
                const char *pwd = OPS_GetInterpPWD();
                theSimulationInfo->addOutputFile(fileName,pwd);
                eMode = BINARY_STREAM;
                argi += 2;
            }
            
            else if ((strcmp(argv[argi],"-tcp") == 0) || (strcmp(argv[argi],"-TCP") == 0))  {
                inetAddr = argv[argi+1];
                if (Tcl_GetInt(interp, argv[argi+2], &inetPort) != TCL_OK)
                    return TCL_ERROR;
                eMode = TCP_STREAM;
                argi += 3;
            }
            
            else if (strcmp(argv[argi],"-database") == 0)  {
                theRecorderDatabase = OPS_GetFEDatastore();
                if (theRecorderDatabase != 0)  {
                    tableName = argv[argi+1];
                    eMode = DATABASE_STREAM;
                } else {
                    opserr << "WARNING expRecorder Control .. -database &lt;fileName&gt; - NO CURRENT DATABASE, results to File instead\n";
                    fileName = argv[argi+1];
                }
                argi += 2;
            }
            
            else  {
                // first unknown string then is assumed to start
                // experimental control response requests
                sizeData = argc-argi;
                flags = 1;
            }
        }
        
        if (sizeData <= 0)
            opserr << "WARNING no data response type specified for experimental control recorder\n";
        
        const char **data = new const char *[sizeData];
        
        for (i=argi, j=0; i<argc; i++, j++)
            data[j] = argv[i];
        
        // construct theOutputStream
        if (eMode == DATA_STREAM && fileName != 0)  {
            theOutputStream = new DataFileStream(fileName, OVERWRITE, 2, 0, closeOnWrite, precision, doScientific);
        } else if (eMode == DATA_STREAM_CSV && fileName != 0)  {
            theOutputStream = new DataFileStream(fileName, OVERWRITE, 2, 1, closeOnWrite, precision, doScientific);
        } else if (eMode == XML_STREAM && fileName != 0)  {
            theOutputStream = new XmlFileStream(fileName);
        } else if (eMode == BINARY_STREAM && fileName != 0)  {
            theOutputStream = new BinaryFileStream(fileName);
        } else if (eMode == TCP_STREAM && inetAddr != 0)  {
            theOutputStream = new TCP_Stream(inetPort, inetAddr);
        } else if (eMode == DATABASE_STREAM && tableName != 0)  {
            theOutputStream = new DatabaseStream(OPS_GetFEDatastore(), tableName);
        } else
            theOutputStream = new StandardStream();
        
        // set precision for stream
        theOutputStream->setPrecision(precision);
        
        // construct array of experimental controls
        theControls = new ExperimentalControl* [numControls];
        if (theControls == 0)  {
            opserr << "WARNING out of memory creating experimental control recorder\n";
            return TCL_ERROR;
        }
        for (int i=0; i<numControls; i++)  {
            theControls[i] = getExperimentalControl((*controlTags)(i));
            if (theControls[i] == 0)  {
                opserr << "WARNING experimental control with tag "
                    << (*controlTags)(i) << " not found\n";
                return TCL_ERROR;
            }
        }
        
        // now create the ExpControlRecorder
        (*theRecorder) = new ExpControlRecorder(numControls, theControls, data, sizeData, echoTime, *theOutputStream, deltaT);
        
        // cleanup dynamic memory
        if (controlTags != 0)
            delete controlTags;
        if (data != 0)
            delete [] data;
    }
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"SignalFilter") == 0)  {
        int numFilters = 0;
        ID *filterTags = 0;
        ExperimentalSignalFilter **theFilters = 0;
        bool echoTime = false;
        outputMode eMode = STANDARD_STREAM;
        double deltaT = 0.0;
        int precision = 6;
        bool doScientific = false;
        bool closeOnWrite = false;
        const char *inetAddr = 0;
        int inetPort;
        int i, j, argi = 2;
        int flags = 0;
        int sizeData = 0;
        
        while (flags == 0 && argi < argc)  {
            
            if (strcmp(argv[argi],"-filter") == 0)  {
                if (argc < argi+2)  {
                    opserr << "WARNING expRecorder SignalFilter .. -filter tags .. - no signal filter tags specified\n";
                    return TCL_ERROR;
                }
                
                // read in a list of signal filters until end of command or other flag
                argi++;
                int filterTag;
                filterTags = new ID(0,32);
                while (argi < argc && Tcl_GetInt(interp, argv[argi], &filterTag) == TCL_OK)  {
                    (*filterTags)[numFilters] = filterTag;
                    numFilters++;
                    argi++;
                }
                Tcl_ResetResult(interp);
                
                if (argi == argc)  {
                    opserr << "WARNING no response type specified for experimental signal filter recorder\n";
                    delete filterTags;
                    return TCL_ERROR;
                }
                
                if (strcmp(argv[argi],"all") == 0)  {
                    filterTags = 0;
                    argi++;
                }
            }
            
            else if (strcmp(argv[argi],"-filterRange") == 0)  {
                if (argc < argi+3)  {
                    opserr << "WARNING expRecorder SignalFilter .. -filterRange start end  .. - no signal filter tags specified\n";
                    return TCL_ERROR;
                }
                
                // read in start and end tags of two signal filters & add set [start,end]
                int start, end;
                if (Tcl_GetInt(interp, argv[argi+1], &start) != TCL_OK)  {
                    opserr << "WARNING expRecorder SignalFilter -filterRange start end - invalid start " << argv[argi+1] << endln;
                    return TCL_ERROR;
                }
                if (Tcl_GetInt(interp, argv[argi+2], &end) != TCL_OK)  {
                    opserr << "WARNING expRecorder SignalFilter -filterRange start end - invalid end " << argv[argi+2] << endln;
                    return TCL_ERROR;
                }
                if (start > end)  {
                    int swap = end;
                    end = start;
                    start = swap;
                }
                
                filterTags = new ID(end-start);
                if (filterTags == 0)  {
                    opserr << "WARNING expRecorder SignalFilter -filterRange start end - out of memory\n";
                    return TCL_ERROR;
                }
                for (i=start; i<=end; i++)
                    (*filterTags)[numFilters++] = i;
                argi += 3;
            }
            
            else if ((strcmp(argv[argi],"-time") == 0) || (strcmp(argv[argi],"-load") == 0))  {
                echoTime = true;
                argi++;
            }
            
            else if ((strcmp(argv[argi],"-dT") == 0) || (strcmp(argv[argi],"-dt") == 0))  {
                argi++;
                if (Tcl_GetDouble(interp, argv[argi], &deltaT) != TCL_OK)
                    return TCL_ERROR;
                argi++;
            }
            
            else if (strcmp(argv[argi],"-precision") == 0)  {
                argi++;
                if (Tcl_GetInt(interp, argv[argi], &precision) != TCL_OK)
                    return TCL_ERROR;
                argi++;
            }
            
            else if (strcmp(argv[argi],"-scientific") == 0)  {
                doScientific = true;
                argi++;
            }
            
            else if (strcmp(argv[argi],"-closeOnWrite") == 0)  {
                closeOnWrite = true;
                argi++;
            }
            
            else if (strcmp(argv[argi],"-file") == 0)  {
                fileName = argv[argi+1];
                const char *pwd = OPS_GetInterpPWD();
                theSimulationInfo->addOutputFile(fileName,pwd);
                eMode = DATA_STREAM;
                argi += 2;
            }
            
            else if ((strcmp(argv[argi],"-csv") == 0) || (strcmp(argv[argi],"-fileCSV") == 0))  {
                fileName = argv[argi+1];
                const char *pwd = OPS_GetInterpPWD();
                theSimulationInfo->addOutputFile(fileName,pwd);
                eMode = DATA_STREAM_CSV;
                argi += 2;
            }
            
            else if ((strcmp(argv[argi],"-xml") == 0) || (strcmp(argv[argi],"-nees") == 0))  {
                fileName = argv[argi+1];
                const char *pwd = OPS_GetInterpPWD();
                theSimulationInfo->addOutputFile(fileName,pwd);
                eMode = XML_STREAM;
                argi += 2;
            }
            
            else if ((strcmp(argv[argi],"-binary") == 0))  {
                fileName = argv[argi+1];
                const char *pwd = OPS_GetInterpPWD();
                theSimulationInfo->addOutputFile(fileName,pwd);
                eMode = BINARY_STREAM;
                argi += 2;
            }
            
            else if ((strcmp(argv[argi],"-tcp") == 0) || (strcmp(argv[argi],"-TCP") == 0))  {
                inetAddr = argv[argi+1];
                if (Tcl_GetInt(interp, argv[argi+2], &inetPort) != TCL_OK)
                    return TCL_ERROR;
                eMode = TCP_STREAM;
                argi += 3;
            }
            
            else if (strcmp(argv[argi],"-database") == 0)  {
                theRecorderDatabase = OPS_GetFEDatastore();
                if (theRecorderDatabase != 0)  {
                    tableName = argv[argi+1];
                    eMode = DATABASE_STREAM;
                } else {
                    opserr << "WARNING expRecorder SignalFilter .. -database &lt;fileName&gt; - NO CURRENT DATABASE, results to File instead\n";
                    fileName = argv[argi+1];
                }
                argi += 2;
            }
            
            else  {
                // first unknown string then is assumed to start
                // experimental signal filter response requests
                sizeData = argc-argi;
                flags = 1;
            }
        }
        
        if (sizeData <= 0)
            opserr << "WARNING no data response type specified for experimental signal filter recorder\n";
        
        const char **data = new const char *[sizeData];
        
        for (i=argi, j=0; i<argc; i++, j++)
            data[j] = argv[i];
        
        // construct the theOutputStream
        if (eMode == DATA_STREAM && fileName != 0)  {
            theOutputStream = new DataFileStream(fileName, OVERWRITE, 2, 0, closeOnWrite, precision, doScientific);
        } else if (eMode == DATA_STREAM_CSV && fileName != 0)  {
            theOutputStream = new DataFileStream(fileName, OVERWRITE, 2, 1, closeOnWrite, precision, doScientific);
        } else if (eMode == XML_STREAM && fileName != 0)  {
            theOutputStream = new XmlFileStream(fileName);
        } else if (eMode == BINARY_STREAM && fileName != 0)  {
            theOutputStream = new BinaryFileStream(fileName);
        } else if (eMode == TCP_STREAM && inetAddr != 0)  {
            theOutputStream = new TCP_Stream(inetPort, inetAddr);
        } else if (eMode == DATABASE_STREAM && tableName != 0)  {
            theOutputStream = new DatabaseStream(OPS_GetFEDatastore(), tableName);
        } else
            theOutputStream = new StandardStream();
        
        // set precision for stream
        theOutputStream->setPrecision(precision);
        
        // construct array of experimental signal filters
        theFilters = new ExperimentalSignalFilter* [numFilters];
        if (theFilters == 0)  {
            opserr << "WARNING out of memory creating experimental signal filter recorder\n";
            return TCL_ERROR;
        }
        for (int i=0; i<numFilters; i++)  {
            theFilters[i] = getExperimentalSignalFilter((*filterTags)(i));
            if (theFilters[i] == 0)  {
                opserr << "WARNING experimental signal filter with tag "
                    << (*filterTags)(i) << " not found\n";
                return TCL_ERROR;
            }
        }
        
        // now create the ExpSignalFilterRecorder
        (*theRecorder) = new ExpSignalFilterRecorder(numFilters, theFilters, data, sizeData, echoTime, *theOutputStream, deltaT);
        
        // cleanup dynamic memory
        if (filterTags != 0)
            delete filterTags;
        if (data != 0)
            delete [] data;
    }
    // ----------------------------------------------------------------------------	
    else if (strcmp(argv[1],"TangentStiff") == 0)  {
        int numTangStif = 0;
        ID *tangStifTags = 0;
        ExperimentalTangentStiff **theTangStiffs = 0;
        bool echoTime = false;
        outputMode eMode = STANDARD_STREAM;
        double deltaT = 0.0;
        int precision = 6;
        bool doScientific = false;
        bool closeOnWrite = false;
        const char *inetAddr = 0;
        int inetPort;
        int i, j, argi = 2;
        int flags = 0;
        int sizeData = 0;
        
        while (flags == 0 && argi < argc)  {
            
            if (strcmp(argv[argi],"-tangStif") == 0)  {
                if (argc < argi+2)  {
                    opserr << "WARNING expRecorder TangentStiff .. -tangStif tags .. - no tangent stiffness tags specified\n";
                    return TCL_ERROR;
                }
                
                // read in a list of tangent stiffnesses until end of command or other flag
                argi++;
                int tangStifTag;
                tangStifTags = new ID(0,32);
                while (argi < argc && Tcl_GetInt(interp, argv[argi], &tangStifTag) == TCL_OK)  {
                    (*tangStifTags)[numTangStif] = tangStifTag;
                    numTangStif++;
                    argi++;
                }
                Tcl_ResetResult(interp);
                
                if (argi == argc)  {
                    opserr << "WARNING no response type specified for experimental tangent stiffness recorder\n";
                    delete tangStifTags;
                    return TCL_ERROR;
                }
                
                if (strcmp(argv[argi],"all") == 0)  {
                    tangStifTags = 0;
                    argi++;
                }
            }
            
            else if (strcmp(argv[argi],"-tangStifRange") == 0)  {
                if (argc < argi+3)  {
                    opserr << "WARNING expRecorder TangentStiff .. -tangStifRange start end  .. - no tangent stiffness tags specified\n";
                    return TCL_ERROR;
                }
                
                // read in start and end tags of two tangent stiffnesses & add set [start,end]
                int start, end;
                if (Tcl_GetInt(interp, argv[argi+1], &start) != TCL_OK)  {
                    opserr << "WARNING expRecorder TangentStiff -tangStifRange start end - invalid start " << argv[argi+1] << endln;
                    return TCL_ERROR;
                }
                if (Tcl_GetInt(interp, argv[argi+2], &end) != TCL_OK)  {
                    opserr << "WARNING expRecorder TangentStiff -tangStifRange start end - invalid end " << argv[argi+2] << endln;
                    return TCL_ERROR;
                }
                if (start > end)  {
                    int swap = end;
                    end = start;
                    start = swap;
                }
                
                tangStifTags = new ID(end-start);
                if (tangStifTags == 0)  {
                    opserr << "WARNING expRecorder TangentStiff -tangStifRange start end - out of memory\n";
                    return TCL_ERROR;
                }
                for (i=start; i<=end; i++)
                    (*tangStifTags)[numTangStif++] = i;
                argi += 3;
            }
            
            else if ((strcmp(argv[argi],"-time") == 0) || (strcmp(argv[argi],"-load") == 0))  {
                echoTime = true;
                argi++;
            }
            
            else if ((strcmp(argv[argi],"-dT") == 0) || (strcmp(argv[argi],"-dt") == 0))  {
                argi++;
                if (Tcl_GetDouble(interp, argv[argi], &deltaT) != TCL_OK)
                    return TCL_ERROR;
                argi++;
            }
            
            else if (strcmp(argv[argi],"-precision") == 0)  {
                argi++;
                if (Tcl_GetInt(interp, argv[argi], &precision) != TCL_OK)
                    return TCL_ERROR;
                argi++;
            }
            
            else if (strcmp(argv[argi],"-scientific") == 0)  {
                doScientific = true;
                argi++;
            }
            
            else if (strcmp(argv[argi],"-closeOnWrite") == 0)  {
                closeOnWrite = true;
                argi++;
            }
            
            else if (strcmp(argv[argi],"-file") == 0)  {
                fileName = argv[argi+1];
                const char *pwd = OPS_GetInterpPWD();
                theSimulationInfo->addOutputFile(fileName,pwd);
                eMode = DATA_STREAM;
                argi += 2;
            }
            
            else if ((strcmp(argv[argi],"-csv") == 0) || (strcmp(argv[argi],"-fileCSV") == 0))  {
                fileName = argv[argi+1];
                const char *pwd = OPS_GetInterpPWD();
                theSimulationInfo->addOutputFile(fileName,pwd);
                eMode = DATA_STREAM_CSV;
                argi += 2;
            }
            
            else if ((strcmp(argv[argi],"-xml") == 0) || (strcmp(argv[argi],"-nees") == 0))  {
                fileName = argv[argi+1];
                const char *pwd = OPS_GetInterpPWD();
                theSimulationInfo->addOutputFile(fileName,pwd);
                eMode = XML_STREAM;
                argi += 2;
            }
            
            else if ((strcmp(argv[argi],"-binary") == 0))  {
                fileName = argv[argi+1];
                const char *pwd = OPS_GetInterpPWD();
                theSimulationInfo->addOutputFile(fileName,pwd);
                eMode = BINARY_STREAM;
                argi += 2;
            }
            
            else if ((strcmp(argv[argi],"-tcp") == 0) || (strcmp(argv[argi],"-TCP") == 0))  {
                inetAddr = argv[argi+1];
                if (Tcl_GetInt(interp, argv[argi+2], &inetPort) != TCL_OK)
                    return TCL_ERROR;
                eMode = TCP_STREAM;
                argi += 3;
            }
            
            else if (strcmp(argv[argi],"-database") == 0)  {
                theRecorderDatabase = OPS_GetFEDatastore();
                if (theRecorderDatabase != 0)  {
                    tableName = argv[argi+1];
                    eMode = DATABASE_STREAM;
                } else {
                    opserr << "WARNING expRecorder TangentStiff .. -database &lt;fileName&gt; - NO CURRENT DATABASE, results to File instead\n";
                    fileName = argv[argi+1];
                }
                argi += 2;
            }
            
            else  {
                // first unknown string then is assumed to start
                // experimental tangent stiffness response requests
                sizeData = argc-argi;
                flags = 1;
            }
        }
        
        if (sizeData <= 0)
            opserr << "WARNING no data response type specified for experimental tangent stiffness recorder\n";
        
        const char **data = new const char *[sizeData];
        
        for (i=argi, j=0; i<argc; i++, j++)
            data[j] = argv[i];
        
        // construct the theOutputStream
        if (eMode == DATA_STREAM && fileName != 0)  {
            theOutputStream = new DataFileStream(fileName, OVERWRITE, 2, 0, closeOnWrite, precision, doScientific);
        } else if (eMode == DATA_STREAM_CSV && fileName != 0)  {
            theOutputStream = new DataFileStream(fileName, OVERWRITE, 2, 1, closeOnWrite, precision, doScientific);
        } else if (eMode == XML_STREAM && fileName != 0)  {
            theOutputStream = new XmlFileStream(fileName);
        } else if (eMode == BINARY_STREAM && fileName != 0)  {
            theOutputStream = new BinaryFileStream(fileName);
        } else if (eMode == TCP_STREAM && inetAddr != 0)  {
            theOutputStream = new TCP_Stream(inetPort, inetAddr);
        } else if (eMode == DATABASE_STREAM && tableName != 0)  {
            theOutputStream = new DatabaseStream(OPS_GetFEDatastore(), tableName);
        } else
            theOutputStream = new StandardStream();
        
        // set precision for stream
        theOutputStream->setPrecision(precision);
        
        // construct array of experimental tangent stiffnesses
        theTangStiffs = new ExperimentalTangentStiff* [numTangStif];
        if (theTangStiffs == 0)  {
            opserr << "WARNING out of memory creating experimental tangent stiffness recorder\n";
            return TCL_ERROR;
        }
        for (int i=0; i<numTangStif; i++)  {
            theTangStiffs[i] = getExperimentalTangentStiff((*tangStifTags)(i));
            if (theTangStiffs[i] == 0)  {
                opserr << "WARNING experimental tangent stiffness with tag "
                    << (*tangStifTags)(i) << " not found\n";
                return TCL_ERROR;
            }
        }
        
        // now create the ExpTangentStiffRecorder
        (*theRecorder) = new ExpTangentStiffRecorder(numTangStif, theTangStiffs, data, sizeData, echoTime, *theOutputStream, deltaT);
        
        // cleanup dynamic memory
        if (tangStifTags != 0)
            delete tangStifTags;
        if (data != 0)
            delete [] data;
    }
    // ----------------------------------------------------------------------------
    else  {
        opserr << "WARNING no recorder exists ";
        opserr << "for recorder of type:" << argv[1];
        return TCL_ERROR;
    }
    
    // check we instantiated a recorder .. if not ran out of memory
    if ((*theRecorder) == 0)  {
        opserr << "WARNING ran out of memory - expRecorder " << argv[1] << endln;
        return TCL_ERROR;
    }
    
    // operation successfull
    return TCL_OK;
}


int TclAddExpRecorder(ClientData clientData, Tcl_Interp *interp,
    int argc, TCL_Char **argv, Domain *theDomain)
{
    Recorder *theRecorder = 0;
    
    TclCreateExpRecorder(clientData, interp, argc, argv, theDomain, &theRecorder);
    
    if (theRecorder != 0)  {
        ExperimentalSite *theSite = getExperimentalSiteFirst();
        if (theSite == 0)  {
            opserr << "WARNING failed to get first experimental site\n";
            delete theRecorder;
            return TCL_ERROR;
        }
        if ((theSite->addRecorder(*theRecorder)) < 0)  {
            opserr << "WARNING could not add to experimental site - expRecorder " << argv[1] << endln;
            delete theRecorder;
            return TCL_ERROR;
        }
        
        int recorderTag = theRecorder->getTag();
        sprintf(interp->result,"%d",recorderTag);
    }
    
    return TCL_OK;
}


int TclRemoveExpRecorder(ClientData clientData, Tcl_Interp *interp,
    int argc, TCL_Char **argv, Domain *theDomain)
{
    ExperimentalSite *theSite = getExperimentalSiteFirst();
    if (theSite == 0)  {
        opserr << "WARNING failed to get first experimental site\n";
        return TCL_ERROR;
    }
    
    if (strcmp(argv[1],"recorder") == 0)  {
        int tag;
        if (Tcl_GetInt(interp, argv[2], &tag) != TCL_OK)  {
            opserr << "WARNING removeExp recorder tag? failed to read tag: " << argv[2] << endln;
            return TCL_ERROR;
        }
        if ((theSite->removeRecorder(tag)) < 0)  {
            opserr << "WARNING could not remove expRecorder with tag " << argv[2] << endln;
            return TCL_ERROR;
        }
    }
    else if (strcmp(argv[1],"recorders") == 0)  {
        if ((theSite->removeRecorders()) < 0)  {
            opserr << "WARNING could not remove expRecorders\n";
            return TCL_ERROR;
        }
    }
    
    return TCL_OK;
}


int TclExpRecord(ClientData clientData, Tcl_Interp *interp,
    int argc, TCL_Char **argv, Domain *theDomain)
{
    ExperimentalSite *theSite = getExperimentalSiteFirst();
    if (theSite == 0)  {
        opserr << "WARNING failed to get first experimental site\n";
        return TCL_ERROR;
    }
    if ((theSite->record()) < 0)  {
        opserr << "WARNING could not record\n";
        return TCL_ERROR;
    }
    
    return TCL_OK;
}
