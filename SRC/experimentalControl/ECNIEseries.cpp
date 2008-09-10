/* ****************************************************************** **
**    OpenFRESCO - Open Framework                                     **
**                 for Experimental Setup and Control                 **
**                                                                    **
**                                                                    **
** Copyright (c) 2006, Yoshikazu Takahashi, Kyoto University          **
** All rights reserved.                                               **
**                                                                    **
** Licensed under the modified BSD License (the "License");           **
** you may not use this file except in compliance with the License.   **
** You may obtain a copy of the License in main directory.            **
** Unless required by applicable law or agreed to in writing,         **
** software distributed under the License is distributed on an        **
** "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,       **
** either express or implied. See the License for the specific        **
** language governing permissions and limitations under the License.  **
**                                                                    **
** Developed by:                                                      **
**   Yoshikazu Takahashi (yos@catfish.dpri.kyoto-u.ac.jp)             **
**   Andreas Schellenberg (andreas.schellenberg@gmx.net)              **
**   Gregory L. Fenves (fenves@berkeley.edu)                          **
**                                                                    **
** ****************************************************************** */

// $Revision: $
// $Date: $
// $URL: $

// Written: Yoshi (yos@catfish.dpri.kyoto-u.ac.jp)
// Created: 11/06
// Revision: A
//
// Description: This file contains the implementation of ECNIEseries.

#include "ECNIEseries.h"


ECNIEseries::ECNIEseries(int tag ,int device)//, bool rtest)
    : ExperimentalControl(tag),//realtest(rtest),
    cDispV(0), /*cVelV(0),*/  dDispV(0), /*dVelV(0),*/ dForceV(0), actForce(0), actDisp(0)//,iDevice(0)
{
    // NI PCI-6036E setting
    iDevice = device;
    V_da = 5.0;  // (V)
    r_da = 1;
    nbit_da = 16;
    iChan_da = 0;
    iChan_da = new i16 [1];
    piChanVect = new i16 [1];
    for(int i=0; i<1; i++)
        piChanVect[i] = 0;
    iGroup = 1;
    // must be changed
    factor = 1.;

    // transformation coef (unit -> Voltage)
    unit2Vol = 0;
    unit2Vol = new f64 [1];

    if(iChan_da == 0 || unit2Vol == 0) {
        opserr << "FATAL ECNIEseries::ECNIEseries - fail to create arrays for DA."
            << endln;
        exit(-1);
    }

    iChan_da[0] = 0;

    unit2Vol[0] = (V_da/(f64)r_da)/(15.0);

    // Control rate is defined by the setting of DA Channel 0.
    //   rate = 60.0;    // displ. rate (unit/s)
    //  rate = 120.0;    // displ. rate (unit/s)
    //  rate = 480.0;    // displ. rate (unit/s)

    // control width (unit/pt)
    ctrl_w = (15.0)/pow(2.0,nbit_da-7)*(f64)factor*(f64)(1.0);

    opserr << "Control width : " << ctrl_w << '(' << "mm" << ")\n"; 
    //nbit_da-1 -> nbit_da-4
    //dupdaterate=10000.0->1000.0
    //  dUpdateRate = rate/ctrl_w; // (pts/s)
    //  dUpdateRate = 10000.;
    dUpdateRate = 20.;
    rate = dUpdateRate*ctrl_w;
    if(dUpdateRate < WFM_MIN_PTS_IN_PERIOD) {
        opserr << "ctrl_w is too small" << endln;
        while (dUpdateRate < WFM_MIN_PTS_IN_PERIOD) {
            factor++;
            ctrl_w = (15.0)/pow(2.0,nbit_da-7)*(f64)factor*1.0;
            dUpdateRate = rate/ctrl_w;
        }
        opserr << "factor is changed to " << factor << endln;
        opserr << "New Control width : " << ctrl_w << '(' << "mm" << ")\n";
    }
    opserr << "dUpdateRate : " << dUpdateRate << endln;

    V_ad = 5.0;  // (V)
    nbit_ad = 16;
    iChan_ad = 0;
    dVoltage = Vol2unit = 0;
    iChan_ad = new i16 [3];
    dVoltage = new f64 [3];
    Vol2unit = new f64 [3];
    if(iChan_ad == 0 || dVoltage == 0 || Vol2unit == 0) {
        opserr << "FATAL ECNIEseries::ECNIEseries - fail to create arrays for AD." << endln;
        exit(-1);
    }

    iChan_ad[0] = 0;
    iChan_ad[1] = 1;
    iChan_ad[2] = 2;

    Vol2unit[0] = (15.0)/V_ad;
    Vol2unit[1] = (100.0)/V_ad;
    Vol2unit[2] = (15.0)/V_ad;


    ulIterations = 1;
    iFIFOMode = 0;
    iDelayMode = 0;
    iUpdateTB = 0;
    ulUpdateInt = 0;
    iWhichClock = 0;
    iUnits = 0;
    iWFMstopped = 0;
    ulItersDone = 0;
    ulPtsDone = 0;
    iOpSTART = 1;
    iOpCLEAR = 0;
    iIgnoreWarning = 0;
    iYieldON = 1;

    // NI board configuration
    int iStatus;
    for(int i=0; i<1; i++) {
        iStatus = AO_Configure(iDevice, iChan_da[i], 0, 0, V_da, 0);
    }
    for(int i=0; i<3; i++) {
        // diff mode
        iStatus = AI_Configure(iDevice, iChan_ad[i], 0, 0, 0, 0);
    }
}


ECNIEseries::ECNIEseries(const ECNIEseries& ast)
    : ExperimentalControl(ast)
{
    // realtest = ast.realtest;

    cDispV = new Vector((*sizeCtrl)(OF_Resp_Disp));
    //cVelV = new double [(*sizeCtrl)(OF_Resp_Vel)];

    dDispV = new Vector((*sizeDaq)(OF_Resp_Disp));
    dForceV = new Vector((*sizeDaq)(OF_Resp_Force));
    /*dummySpecimen = 0;
    if(ast.dummySpecimen != 0) 
    dummySpecimen = ast.dummySpecimen->getCopy();*/

    //
    iDevice = ast.iDevice;

    V_da = ast.V_da;
    r_da = ast.r_da;
    nbit_da = ast.nbit_da;

    iChan_da = 0;
    iChan_da = new i16 [1];
    piChanVect = new i16 [1];
    for(int i=0; i<1; i++)
        piChanVect[i] = (ast.piChanVect)[i];
    iGroup = ast.iGroup;

    // transformation coef (unit -> Voltage)
    unit2Vol = 0;
    unit2Vol = new f64 [1];
    if(iChan_da == 0 || unit2Vol == 0) {
        opserr << "FATAL ECNIEseries::ECNIEseries - fail to create arrays for DA."
            << endln;
        exit(-1);
    }
    for(int i=0; i<1; i++) {
        iChan_da[i] = (ast.iChan_da)[i];
        unit2Vol[i] = (ast.unit2Vol)[i];
    }

    factor = ast.factor;
    rate = ast.rate;
    ctrl_w = ast.ctrl_w;
    dUpdateRate = ast.dUpdateRate;

    V_ad = ast.V_ad;
    nbit_ad = ast.nbit_ad;
    iChan_ad = 0;
    dVoltage = Vol2unit = 0;
    iChan_ad = new i16 [3];
    dVoltage = new f64 [3];
    Vol2unit = new f64 [3];
    if(iChan_ad == 0 || dVoltage == 0 || Vol2unit == 0) {
        opserr << "FATAL ECNIEseries::ECNIEseries - fail to create arrays for AD." << endln;
        exit(-1);
    }

    for(int i=0; i<3; i++) {
        iChan_ad[i] = ast.iChan_ad[i];
        dVoltage[i] = ast.dVoltage[i];
        Vol2unit[i] = ast.Vol2unit[i];
    }

    ulIterations = ast.ulIterations;
    iFIFOMode = ast.iFIFOMode;
    iDelayMode = ast.iDelayMode;
    iUpdateTB = ast.iUpdateTB;
    ulUpdateInt = ast.ulUpdateInt;
    iWhichClock = ast.iWhichClock;
    iUnits = ast.iUnits;
    iWFMstopped = ast.iWFMstopped;
    ulItersDone = ast.ulItersDone;
    ulPtsDone = ast.ulPtsDone;
    iOpSTART = ast.iOpSTART;
    iOpCLEAR = ast.iOpCLEAR;
    iIgnoreWarning = ast.iIgnoreWarning;
    iYieldON = ast.iYieldON;
}


ECNIEseries::~ECNIEseries()
{
    if(iChan_da != 0)
        delete [] iChan_da;
    if(piChanVect != 0)
        delete [] piChanVect;
    if(unit2Vol != 0)
        delete [] unit2Vol;

    if(iChan_ad != 0)
        delete [] iChan_ad;
    if(dVoltage != 0)
        delete [] dVoltage;
    if(Vol2unit != 0)
        delete [] Vol2unit;

    // delete memory of target vectors
    if (cDispV != 0)
        delete cDispV;
    /*if (targVel != 0)
    delete targVel;
    if (targAccel != 0)
    delete targAccel;*/

    // delete memory of measured vectors
    if (dDispV != 0)
        delete dDispV;
    if (dForceV != 0)
        delete dForceV;

    // delete memory of 1d material

    /*if(dummySpecimen != 0)
    delete dummySpecimen;*/
}


int ECNIEseries::setup()
{  //opserr << (*sizeDaq)(OF_Resp_Force);
    cDispV = new Vector((*sizeCtrl)(OF_Resp_Disp));
    //cVelV = new double [(*sizeCtrl)(OF_Resp_Vel)];

    dDispV = new Vector((*sizeDaq)(OF_Resp_Disp));
    dForceV = new Vector((*sizeDaq)(OF_Resp_Force));  

    //cDispV->Zero();
    //dDispV->Zero();
    //dForceV->Zero(); 

    //cVelV->Zero();
    opserr << (*cDispV)(0);
    int code = 0, i, iStatus;
    double targetVal = 0.0;

    this->Print(opserr);

    opserr << "*********************************************************\n";
    opserr << "*\n";
    opserr << "* IMPORTANT !!!   IMPORTANT !!!   IMPORTANT !!!\n";
    opserr << "*\n";
    opserr << "*    Make sure that\n";
    opserr << "*      offset values of controller set to ZEROs\n";
    opserr << "*      because DA values is used the initial AD data\n";
    opserr << "*      as the DA offset values\n";
    opserr << "*\n";
    opserr << "*    Hit any key to proceed the initialization\n";
    opserr << "*\n";
    opserr << "*********************************************************\n";

    NIDAQWaitForKey(0.0);

    acquire();

    for(int i=0; i<1; i++)
    {iStatus = AO_VWrite(iDevice, iChan_da[i], targetVal*unit2Vol[i]);}

    acquire();

    opserr << "*\n";
    opserr << "* INITIAL VALUES OF DAQ\n";
    opserr << "*\n";

    /*opserr << "*   Ch. " << 1 << ": " << (*cDispV)(0) 
    << " (" << "mm"
    << ") <" << (*cDispV)(0)/Vol2unit[0] << " (Volt)>\n";*/

    opserr << "*   Ch. " << 2 << ": " << (*dForceV)(0)
    << " (" << "N"
        << ") <" << (*dForceV)(0)/Vol2unit[1] << " (Volt)>\n";

    opserr << "*   Ch. " << 3 << ": " << (*dDispV)(0) 
    << " (" << "mm"
        << ") <" << (*dDispV)(0)/Vol2unit[2] << " (Volt)>\n";


    opserr << "*\n";
    opserr << "*********************************************************\n";
    opserr << "*\n";
    opserr << "*    If okay, Hit any key to start\n";
    opserr << "*\n";
    opserr << "*********************************************************\n";

    NIDAQWaitForKey(0.0);

    targetVal = (*cDispV)(0);
    //targetVal = 0.0;

    opserr << "*\n";
    opserr << "*    Running.....\n";

    /*if(theFilter != 0) {
    targetVal = theFilter->filtering(targetVal);
    }*/

    //if(realtest == true) {
    //actForce = (*dForceV)(0);
    /*} else {
    code = dummySpecimen->setTrialStrain(targetVal);
    actForce = dummySpecimen->getStress();
    }*/
    //actDisp = 0.0;//?????

    return code;
}


int ECNIEseries::setSize(ID sizeT, ID sizeO)
{
    // check sizeTrial and sizeOut
    // for ECNIEseries object

    // ECNIEseries object only use 
    // disp or disp and vel or disp, vel and accel for trial
    // disp and force for output
    // check these are available in sizeT/sizeO.

    if(sizeT(OF_Resp_Disp) !=1 || sizeO(OF_Resp_Disp) !=1 ||
        sizeO(OF_Resp_Force) !=1)
    {
        opserr << "ECNIEseries::setSize - wrong sizeTrial/Out\n"; 
        opserr << "see User Manual.\n";
        exit(1);
    }

    *sizeCtrl = sizeT;
    *sizeDaq = sizeO;

    return OF_ReturnType_completed;
}


int ECNIEseries::setTrialResponse(const Vector* disp,
    const Vector* vel,
    const Vector* accel,
    const Vector* force,
    const Vector* time)
{	
    *cDispV = *disp;
    //*cVelV = *vel;
    this->control();

    return OF_ReturnType_completed;
}


int ECNIEseries::getDaqResponse(Vector* disp,
    Vector* vel,
    Vector* accel,
    Vector* force,
    Vector* time)
{
    this->acquire();

    *disp = *dDispV;
    //*vel = *dVelV;
    *force = *dForceV;

    return OF_ReturnType_completed;
}


int ECNIEseries::commitState()
{
    return OF_ReturnType_completed;

    /*int code = 0;
    code = ExperimentalControl::commitState();
    if(realtest == false)
    code = dummySpecimen->commitState();
    return code;*/
}


ExperimentalControl* ECNIEseries::getCopy()
{
    return new ECNIEseries(*this);
}


Response* ECNIEseries::setResponse(const char **argv, int argc,
    OPS_Stream &output)
{
    int i;
    char outputData[15];
    Response *theResponse = 0;
    
    output.tag("ExpControlOutput");
    output.attr("ctrlType",this->getClassType());
    output.attr("ctrlTag",this->getTag());
        
    // target displacements
    if (strcmp(argv[0],"targDisp") == 0 ||
        strcmp(argv[0],"targetDisp") == 0 ||
        strcmp(argv[0],"targetDisplacement") == 0 ||
        strcmp(argv[0],"targetDisplacements") == 0)
    {
        for (i=0; i<(*sizeCtrl)(OF_Resp_Disp); i++)  {
            sprintf(outputData,"targDisp%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpControlResponse(this, 1,
            Vector((*sizeCtrl)(OF_Resp_Disp)));
    }
    
    // measured displacements
    if (strcmp(argv[0],"measDisp") == 0 ||
        strcmp(argv[0],"measuredDisp") == 0 ||
        strcmp(argv[0],"measuredDisplacement") == 0 ||
        strcmp(argv[0],"measuredDisplacements") == 0)
    {
        for (i=0; i<(*sizeDaq)(OF_Resp_Disp); i++)  {
            sprintf(outputData,"measDisp%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpControlResponse(this, 2,
            Vector((*sizeDaq)(OF_Resp_Disp)));
    }
    
    // measured forces
    if (strcmp(argv[0],"measForce") == 0 ||
        strcmp(argv[0],"measuredForce") == 0 ||
        strcmp(argv[0],"measuredForces") == 0)
    {
        for (i=0; i<(*sizeDaq)(OF_Resp_Force); i++)  {
            sprintf(outputData,"measForce%d",i+1);
            output.tag("ResponseType",outputData);
        }
        theResponse = new ExpControlResponse(this, 3,
            Vector((*sizeDaq)(OF_Resp_Force)));
    }
    
    output.endTag();
    
    return theResponse;
}


int ECNIEseries::getResponse(int responseID, Information &info)
{
    switch (responseID)  {
    case 1:  // target displacements
        return info.setVector(*cDispV);
        
    case 2:  // measured displacements
        return info.setVector(*dDispV);
        
    case 3:  // measured forces
        return info.setVector(*dForceV);
        
    default:
        return -1;
    }
}


void ECNIEseries::Print(OPS_Stream &s, int flag)
{
    s << "****************************************************************\n";
    s << "* ExperimentalControl: " << this->getTag() << endln; 
    s << "*   type: ECNIEseries\n";
    s << "*   <Equipment for DA>" << endln;
    for(int i=0; i<ndim; i++) {
    s << "\t";
    ctrl_equip[i]->Print(s);//?????
    }
    s << "*   <Equipment for AD>" << endln;
    for(int i=0; i<ndim; i++) {
    s << "\t";
    daq_equip[i]->Print(s);//?????
    }
    s << "*   ctrlFilters:";
    for (int i=0; i<OF_Resp_All; i++)  {
        if (theCtrlFilters[i] != 0)
            s << " " << theCtrlFilters[i]->getTag();
        else
            s << " 0";
    }
    s << "\n*   daqFilters:";
    for (int i=0; i<OF_Resp_All; i++)  {
        if (theCtrlFilters[i] != 0)
            s << " " << theCtrlFilters[i]->getTag();
        else
            s << " 0";
    }
    s << endln;
    s << "****************************************************************\n";
    s << endln;
}


int ECNIEseries::control()
{
    int code = 0;
    //double targetDisp = (*cDispV)(1);

    // opserr << "targetDisp = " << targetDisp << endln;

    i16 iStatus, iRetVal;

    iUpdateTB = 0;
    ulUpdateInt = 0;

    iWFMstopped = 0;
    ulItersDone = 0;
    ulPtsDone = 0;

    // read current value
    acquire();

    f64 **pdBuffer = new f64* [1];
    i16 **piBuffer = new i16* [1];

    u32 ulCount;
    for (int i=0; i<1; i++){
        f64 mm_s = (*dDispV)(0);
        f64 mm_t = (*cDispV)(0);//targetDisp;
        opserr << "\nfrom " << mm_s << " to " << mm_t << " with ctrl_w " << ctrl_w;

        ulCount = (u32)(fabs(mm_t - mm_s)/ctrl_w);

        if(ulCount == 0) {
            // must check again!!!!!!!!!!!!!!!!!
            //targetDisp = (*dDispV)(1);
            /*if(targetDisp != actDisp) {
            if(theFilter != 0)
            targetDisp = theFilter->filtering(targetDisp);
            }*/
            /*if(realtest == false) {
            dummySpecimen->setTrialStrain(targetDisp);
            //        dummySpecimen->setTrialStrain(mm_t);
            }*/
            //actDisp = (*dDispV)(1);
            //      actDisp = mm_t;

            return code;
        }

        pdBuffer[i] = new f64 [ulCount];
        piBuffer[i] = new i16 [ulCount];
        if(pdBuffer[i] == NULL || piBuffer[i] == NULL) {
            opserr << "FATAL ECNIEseries::control - failed to create pdBuffer and piBuffer." << endln;
            exit(-1);
        }

        iStatus = NIDAQMakeRampBuffer(pdBuffer[i], ulCount,
            mm_s, mm_t, unit2Vol[i]);

        //  opserr << "\n" << "pdBuffer[ulCount] = " << pdBuffer[0][ulCount-1]*Vol2unit[0] << ", mm_t = " << mm_t << endln;
        /*if(realtest == false) {
        dummySpecimen->setTrialStrain(pdBuffer[0][ulCount-1]*Vol2unit[0]);
        //      dummySpecimen->setTrialStrain(mm_t);
        }*/
    }

    //  opserr << "\n" << "ulCount = " << (int)ulCount << endln;
    if (iStatus == 0) {
        /* If buffer was made correctly, then output it. */
        iStatus = WFM_Group_Setup(iDevice, 1, piChanVect,
            iGroup);

        iRetVal = NIDAQErrorHandler(iStatus, "WFM_Group_Setup",
            iIgnoreWarning);

        for(int i=0; i<1; i++) {
            iStatus = WFM_Scale(iDevice, iChan_da[i], ulCount, 1.0,
                pdBuffer[i], piBuffer[i]);
            iRetVal = NIDAQErrorHandler(iStatus, "WFM_Scale", iIgnoreWarning);

            iStatus = WFM_Load(iDevice, 1, piChanVect, piBuffer[i],
                ulCount, ulIterations, iFIFOMode);
            iRetVal = NIDAQErrorHandler(iStatus, "WFM_Load", iIgnoreWarning);
        }
        iStatus = WFM_Rate(dUpdateRate, iUnits, &iUpdateTB, &ulUpdateInt);
        iRetVal = NIDAQErrorHandler(iStatus, "WFM_Rate", iIgnoreWarning);

        iStatus = WFM_ClockRate(iDevice, iGroup, iWhichClock,
            iUpdateTB, ulUpdateInt, iDelayMode);
        iRetVal = NIDAQErrorHandler(iStatus, "WFM_ClockRate", iIgnoreWarning);

        //   opserr << "\n" << " A " << (int)ulCount << " point waveform should be output at a rate of " << dUpdateRate << " updates/sec.\n";

        iStatus = WFM_Group_Control(iDevice, iGroup, iOpSTART);
        iRetVal = NIDAQErrorHandler(iStatus,
            "WFM_Group_Control/START", iIgnoreWarning);


        // check the signal generation of Channel 0
        while ((iWFMstopped == 0) && (iStatus == 0)) {
            iStatus = WFM_Check(iDevice, iChan_da[0], &iWFMstopped,
                &ulItersDone, &ulPtsDone);
            iRetVal = NIDAQYield(iYieldON);
            // opserr << "\n" << int(iStatus) << int(iWFMstopped);
        }

        iRetVal = NIDAQErrorHandler(iStatus, "WFM_Check", iIgnoreWarning);


        /* CLEANUP - Don't check for errors on purpose. */
        /* Set group back to initial state. */
        iStatus = WFM_Group_Control(iDevice, iGroup, iOpCLEAR);
        for(int i=0; i<1; i++)
            iStatus = AO_Write(iDevice, iChan_da[i], piBuffer[i][ulCount-1]);
    } 
    else {
        opserr << " The buffer was not made correctly. Check the parameters for NIDAQMakeRampBuffer.\n";
    }
    this->sleep(200);
    acquire();
    //targetDisp = (*dDispV)(1);

    /*  if(targetDisp != actDisp) {
    if(theFilter != 0)
    targetDisp = theFilter->filtering(targetDisp);*/

    //  if(realtest == false) {
    //    dummySpecimen->setTrialStrain(s_c(0));
    //    dummySpecimen->setTrialStrain(targetDisp);
    //  }

    //  actDisp = s_c(0);
    //actDisp = (*dDispV)(1);

    for(int i=0; i<1; i++) {
        delete pdBuffer[i];
        delete piBuffer[i];
    }
    delete [] pdBuffer;
    delete [] piBuffer;

    return code;
}


int ECNIEseries::acquire()
{
    i16 iStatus, iRetVal;

    for(int i=0; i<3; i++) {
        // read voltage of iChan (5V, bipolar)
        iStatus = AI_VRead(iDevice, iChan_ad[i], 1.0, &(dVoltage[i]));//
        iRetVal = NIDAQErrorHandler(iStatus, "AI_VRead", iIgnoreWarning);
    }

    //(*cDispV)(1) = dVoltage[0]*Vol2unit[0];
    // opserr << "\ndVoltage(1) = " << dVoltage[1];// << ", daqData = " << (*daqData)(0);
    // opserr <<  Vol2unit[1] ;
    //  if(realtest == true) {
    (*dForceV)(0) = dVoltage[1]*Vol2unit[1];

    /*else {
    (*daqData)(0) = dummySpecimen->getStrain();
    (*daqData)(1) = dummySpecimen->getStress();
    }*/
    (*dDispV)(0) = dVoltage[2]*Vol2unit[2];

    // for(int i = 0; i<3; i++) opserr << "i = " << i << ", daqData = " << (*daqData)(i) << endln;

    return 0;
}


/*int ECNIEseries::setDummySpecimen(UniaxialMaterial &specimen)
{
// get a copy of the material and check we obtained a valid copy
dummySpecimen = specimen.getCopy();
if(dummySpecimen == 0) {
opserr << "FATAL ECNIEseries::setDummySpecimen - failed to get a copy of a 1d material" << specimen.getTag() << endln;
exit(-1);
}
return 0;
}*/


int ECNIEseries::NIDAQMakeRampBuffer(void* pvBuffer, u32 ulNumPts, 
                                     f64 mm_s, f64 mm_t, f64 u2V)
{
    u32 ulIndex  = 0;   /* index into the data array           */

    if(mm_s < mm_t) {
        for (ulIndex = 0; ulIndex < ulNumPts; ulIndex++) {
            *((f64*)pvBuffer + ulIndex) = (mm_s+(f64)(ulIndex)*ctrl_w)*u2V;
        }
    } else {
        for (ulIndex = 0; ulIndex < ulNumPts; ulIndex++) {
            *((f64*)pvBuffer + ulIndex) = (mm_s-(f64)(ulIndex)*ctrl_w)*u2V;
        }
    }
    return (NoError);
}


void ECNIEseries::sleep(const clock_t wait)
{
    clock_t goal;
    goal = wait + clock();
    while (goal>clock());
}
