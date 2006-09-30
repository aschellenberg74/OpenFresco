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
// Description: This file contains the implementation of the
// ESChevronBrace2d class.

#include "ESChevronBrace2d.h"

#include <Matrix.h>
#include <math.h>


ESChevronBrace2d::ESChevronBrace2d(int tag,
    int nlGeomFlag,
    double actLength0, double actLength1, double actLength2,
    double rigidLength0, double rigidLength1,
    ExperimentalControl* control)
    : ExperimentalSetup(tag, control), nlFlag(nlGeomFlag),
    La0(actLength0), La1(actLength1), La2(actLength2),
    L0(rigidLength0), L1(rigidLength1)
{
    this->setup();
}


ESChevronBrace2d::ESChevronBrace2d(const ESChevronBrace2d& es)
    : ExperimentalSetup(es)
{
    nlFlag = es.nlFlag;
    La0    = es.La0;
    La1    = es.La1;
    La2    = es.La2;
    L0     = es.L0;
    L1     = es.L1;
}


ESChevronBrace2d::~ESChevronBrace2d()
{
    // does nothing
}


int ESChevronBrace2d::setSize(ID sizeT, ID sizeO)
{
    // check sizeTrial and sizeOut
    // for ESChevronBrace2d object
    
    // a component of sizeT must be equal to 3
    // and a component of sizeO must be equal
    // to 6 if it is non-zero.
    
    int i;
    for(i=0; i<OF_Resp_Force; i++) {
        if((sizeT[i] != 0 && sizeT[i] != 3) ||
            (sizeO[i] != 0 && sizeO[i] != 3)) {
            opserr << "ESChevronBrace2d::setSize - wrong sizeTrial/Out\n"; 
            opserr << "see User Manual.\n";
            opserr << "sizeT = " << sizeT;
            opserr << "sizeO = " << sizeO;
            exit(1);
        }
    }
    if((sizeT[OF_Resp_Force] != 0 && sizeT[OF_Resp_Force] != 3) ||
        (sizeO[OF_Resp_Force] != 0 && sizeO[OF_Resp_Force] != 6)) {
        opserr << "ESChevronBrace2d::setSize - wrong sizeTrial/Out\n"; 
        opserr << "see User Manual.\n";
        opserr << "sizeT = " << sizeT;
        opserr << "sizeO = " << sizeO;
        exit(1);
    }
    if((sizeT[OF_Resp_Time] != 0 && sizeT[OF_Resp_Time] != 1) ||
        (sizeO[OF_Resp_Time] != 0 && sizeO[OF_Resp_Time] != 1)) {
        opserr << "ESChevronBrace2d::setSize - wrong sizeTrial/Out\n"; 
        opserr << "see User Manual.\n";
        opserr << "sizeT = " << sizeT;
        opserr << "sizeO = " << sizeO;
        exit(1);
    }
    
    return OF_ReturnType_completed;
}


int ESChevronBrace2d::commitState()
{
    return theControl->commitState();
}


int ESChevronBrace2d::setup()
{
    // setup for ctrl/daq vectors of ESChevronBrace2d
    sizeCtrl->Zero();
    sizeDaq->Zero();
    for(int i=0; i<OF_Resp_Force; i++) {
        (*sizeCtrl)[i] = 3;
        (*sizeDaq)[i] = 3;
    }
    (*sizeCtrl)[OF_Resp_Force] = 3;
    (*sizeDaq)[OF_Resp_Force] = 6;
    (*sizeCtrl)[OF_Resp_Time] = 1;
    (*sizeDaq)[OF_Resp_Time] = 1;
    
    this->setCtrlDaqSize();
    
    return OF_ReturnType_completed;
}


int ESChevronBrace2d::transfTrialResponse(const Vector* disp, 
    const Vector* vel,
    const Vector* accel,
    const Vector* force,
    const Vector* time)
{
    // transform data
    if(disp != 0) {
        this->transfTrialDisp(disp);
        for(int i=0; i<(*sizeCtrl)[OF_Resp_Disp]; i++)
            (*cDisp)[i] *= (*cDispFact)[i];
    }
    if(disp != 0 && vel != 0) {
        this->transfTrialVel(disp,vel);
        for(int i=0; i<(*sizeCtrl)[OF_Resp_Vel]; i++)
            (*cVel)[i] *= (*cVelFact)[i];
    }
    if(disp != 0 && vel != 0 && accel != 0) {
        this->transfTrialAccel(disp,vel,accel);
        for(int i=0; i<(*sizeCtrl)[OF_Resp_Accel]; i++)
            (*cAccel)[i] *= (*cAccelFact)[i];
    }
    if(force != 0) {
        this->transfTrialForce(force);
        for(int i=0; i<(*sizeCtrl)[OF_Resp_Force]; i++)
            (*cForce)[i] *= (*cForceFact)[i];
    }
    if(time != 0) {
        this->transfTrialTime(time);
        for(int i=0; i<(*sizeCtrl)[OF_Resp_Time]; i++)
            (*cTime)[i] *= (*cTimeFact)[i];
    }
    
    return OF_ReturnType_completed;
}


int ESChevronBrace2d::transfDaqResponse(Vector* disp,
    Vector* vel,
    Vector* accel,
    Vector* force,
    Vector* time)
{
    // transform data
    if(disp != 0) {
        for(int i=0; i<(*sizeDaq)[OF_Resp_Disp]; i++)
            (*dDisp)[i] /= (*dDispFact)[i];
        this->transfDaqDisp(disp);
    }
    if(disp != 0 && vel != 0) {
        for(int i=0; i<(*sizeDaq)[OF_Resp_Vel]; i++)
            (*dVel)[i]  /= (*dVelFact)[i];
        this->transfDaqVel(vel);
    }
    if(disp != 0 && vel != 0 && accel != 0) {
        for(int i=0; i<(*sizeDaq)[OF_Resp_Accel]; i++)
            (*dAccel)[i] /= (*dAccelFact)[i];
        this->transfDaqAccel(accel);
    }
    if(force != 0) {
        for(int i=0; i<(*sizeDaq)[OF_Resp_Force]; i++)
            (*dForce)[i] /= (*dForceFact)[i];
        this->transfDaqForce(force);
    }
    if(time != 0) {
        for(int i=0; i<(*sizeDaq)[OF_Resp_Time]; i++)
            (*dTime)[i] /= (*dTimeFact)[i];
        this->transfDaqTime(time);
    }
    
    return OF_ReturnType_completed;
}


ExperimentalSetup* ESChevronBrace2d::getCopy()
{
    ESChevronBrace2d *theCopy = new ESChevronBrace2d(*this);
    
    return theCopy;
}


void ESChevronBrace2d::Print(OPS_Stream &s, int flag)
{
    s << "ExperimentalSetup: " << this->getTag(); 
    s << " type: ESChevronBrace2d\n";
    s << " nlGeomFlag  : " << nlFlag << endln;
    s << " actLength0  : " << La0 << endln;
    s << " actLength1  : " << La1 << endln;
    s << " actLength2  : " << La2 << endln;
    s << " rigidLength0: " << L0 << endln;
    s << " rigidLength1: " << L1 << endln;
    if(theControl != 0)  {
        s << "\tExperimentalControl tag: " << theControl->getTag();
        s << *theControl;
    }
}


int ESChevronBrace2d::transfTrialDisp(const Vector* disp)
{  
    // linear geometry, horizontal actuator left
    if (nlFlag == 0)  {
        // actuator 0
        (*cDisp)(0) = (*disp)(0);
        // actuator 1
        (*cDisp)(1) = (*disp)(1) - L0*(*disp)(2);
        // actuator 2
        (*cDisp)(2) = (*disp)(1) + L1*(*disp)(2);
    }
    // linear geometry, horizontal actuator right
    else if (nlFlag == 1)  {
        // actuator 0
        (*cDisp)(0) = -(*disp)(0);
        // actuator 1
        (*cDisp)(1) = (*disp)(1) - L0*(*disp)(2);
        // actuator 2
        (*cDisp)(2) = (*disp)(1) + L1*(*disp)(2);
    }
    // nonlinear geometry, horizontal actuator left
    else if (nlFlag == 2)  {
        // actuator 0
        (*cDisp)(0) = pow(pow((*disp)(0)+L0*(1-cos((*disp)(2)))+La0,2)+pow((*disp)(1)-L0*sin((*disp)(2)),2),0.5)-La0;
        // actuator 1
        (*cDisp)(1) = pow(pow((*disp)(0)+L0*(1-cos((*disp)(2))),2)+pow((*disp)(1)-L0*sin((*disp)(2))+La1,2),0.5)-La1;
        // actuator 2
        (*cDisp)(2) = pow(pow((*disp)(0)-L1*(1-cos((*disp)(2))),2)+pow((*disp)(1)+L1*sin((*disp)(2))+La2,2),0.5)-La2;
    }
    // nonlinear geometry, horizontal actuator right
    else if (nlFlag == 3)  {
        opserr << "ESChevronBrace2d::transfTrialDisp() - "
            << "nonlinear geometry with horizontal actuator right "
            << "not implemented yet";
        return OF_ReturnType_failed;
    }
    
    return OF_ReturnType_completed;
}


int ESChevronBrace2d::transfTrialVel(const Vector* vel)
{
    return OF_ReturnType_completed;
}


int ESChevronBrace2d::transfTrialVel(const Vector* disp,
    const Vector* vel)
{  
    // linear geometry, horizontal actuator left
    if (nlFlag == 0)  {
        // actuator 0
        (*cVel)(0) = (*vel)(0);
        // actuator 1
        (*cVel)(1) = (*vel)(1) - L0*(*vel)(2);
        // actuator 2
        (*cVel)(2) = (*vel)(1) + L1*(*vel)(2);
    }
    // linear geometry, horizontal actuator right
    else if (nlFlag == 1)  {
        // actuator 0
        (*cVel)(0) = -(*vel)(0);
        // actuator 1
        (*cVel)(1) = (*vel)(1) - L0*(*vel)(2);
        // actuator 2
        (*cVel)(2) = (*vel)(1) + L1*(*vel)(2);
    }
    // nonlinear geometry, horizontal actuator left
    else if (nlFlag == 2)  {
        // actuator 0
        (*cVel)(0) = 1/2*(2*((*disp)(0)+L0*(1-cos((*disp)(2)))+La0)*((*vel)(0)+L0*sin((*disp)(2))*(*vel)(2))+2*((*disp)(1)-L0*sin((*disp)(2)))*((*vel)(1)-L0*cos((*disp)(2))*(*vel)(2)))/pow(pow((*disp)(0)+L0*(1-cos((*disp)(2)))+La0,2)+pow((*disp)(1)-L0*sin((*disp)(2)),2),0.5);
        // actuator 1
        (*cVel)(1) = 1/2*(2*((*disp)(0)+L0*(1-cos((*disp)(2))))*((*vel)(0)+L0*sin((*disp)(2))*(*vel)(2))+2*((*disp)(1)-L0*sin((*disp)(2))+La1)*((*vel)(1)-L0*cos((*disp)(2))*(*vel)(2)))/pow(pow((*disp)(0)+L0*(1-cos((*disp)(2))),2)+pow((*disp)(1)-L0*sin((*disp)(2))+La1,2),0.5);
        // actuator 2
        (*cVel)(2) = 1/2*(2*((*disp)(0)-L1*(1-cos((*disp)(2))))*((*vel)(0)-L1*sin((*disp)(2))*(*vel)(2))+2*((*disp)(1)+L1*sin((*disp)(2))+La2)*((*vel)(1)+L1*cos((*disp)(2))*(*vel)(2)))/pow(pow((*disp)(0)-L1*(1-cos((*disp)(2))),2)+pow((*disp)(1)+L1*sin((*disp)(2))+La2,2),0.5);
    }
    // nonlinear geometry, horizontal actuator right
    else if (nlFlag == 3)  {
        opserr << "ESChevronBrace2d::transfTrialVel() - "
            << "nonlinear geometry with horizontal actuator right "
            << "not implemented yet";
        return OF_ReturnType_failed;
    }
    
    return OF_ReturnType_completed;
}


int ESChevronBrace2d::transfTrialAccel(const Vector* accel)
{
    return OF_ReturnType_completed;
}


int ESChevronBrace2d::transfTrialAccel(const Vector* disp,
    const Vector* vel,
    const Vector* accel)
{  
    // linear geometry, horizontal actuator left
    if (nlFlag == 0)  {
        // actuator 0
        (*cAccel)(0) = (*accel)(0);
        // actuator 1
        (*cAccel)(1) = (*accel)(1) - L0*(*accel)(2);
        // actuator 2
        (*cAccel)(2) = (*accel)(1) + L1*(*accel)(2);
    }
    // linear geometry, horizontal actuator right
    else if (nlFlag == 1)  {
        // actuator 0
        (*cAccel)(0) = -(*accel)(0);
        // actuator 1
        (*cAccel)(1) = (*accel)(1) - L0*(*accel)(2);
        // actuator 2
        (*cAccel)(2) = (*accel)(1) + L1*(*accel)(2);
    }
    // nonlinear geometry, horizontal actuator left
    else if (nlFlag == 2)  {
        // actuator 0
        (*cAccel)(0) = -1/4*pow(2*((*disp)(0)+L0*(1-cos((*disp)(2)))+La0)*((*vel)(0)+L0*sin((*disp)(2))*(*vel)(2))+2*((*disp)(1)-L0*sin((*disp)(2)))*((*vel)(1)-L0*cos((*disp)(2))*(*vel)(2)),2)/pow(pow((*disp)(0)+L0*(1-cos((*disp)(2)))+La0,2)+pow((*disp)(1)-L0*sin((*disp)(2)),2),1.5)+1/2*(2*pow((*vel)(0)+L0*sin((*disp)(2))*(*vel)(2),2)+2*((*disp)(0)+L0*(1-cos((*disp)(2)))+La0)*((*accel)(0)+L0*cos((*disp)(2))*pow((*vel)(2),2)+L0*sin((*disp)(2))*(*accel)(2))+2*pow((*vel)(1)-L0*cos((*disp)(2))*(*vel)(2),2)+2*((*disp)(1)-L0*sin((*disp)(2)))*((*accel)(1)+L0*sin((*disp)(2))*pow((*vel)(2),2)-L0*cos((*disp)(2))*(*accel)(2)))/pow(pow((*disp)(0)+L0*(1-cos((*disp)(2)))+La0,2)+pow((*disp)(1)-L0*sin((*disp)(2)),2),0.5);
        // actuator 1
        (*cAccel)(1) = -1/4*pow(2*((*disp)(0)+L0*(1-cos((*disp)(2))))*((*vel)(0)+L0*sin((*disp)(2))*(*vel)(2))+2*((*disp)(1)-L0*sin((*disp)(2))+La1)*((*vel)(1)-L0*cos((*disp)(2))*(*vel)(2)),2)/pow(pow((*disp)(0)+L0*(1-cos((*disp)(2))),2)+pow((*disp)(1)-L0*sin((*disp)(2))+La1,2),1.5)+1/2*(2*pow((*vel)(0)+L0*sin((*disp)(2))*(*vel)(2),2)+2*((*disp)(0)+L0*(1-cos((*disp)(2))))*((*accel)(0)+L0*cos((*disp)(2))*pow((*vel)(2),2)+L0*sin((*disp)(2))*(*accel)(2))+2*pow((*vel)(1)-L0*cos((*disp)(2))*(*vel)(2),2)+2*((*disp)(1)-L0*sin((*disp)(2))+La1)*((*accel)(1)+L0*sin((*disp)(2))*pow((*vel)(2),2)-L0*cos((*disp)(2))*(*accel)(2)))/pow(pow((*disp)(0)+L0*(1-cos((*disp)(2))),2)+pow((*disp)(1)-L0*sin((*disp)(2))+La1,2),0.5);
        // actuator 2
        (*cAccel)(2) = -1/4*pow(2*((*disp)(0)-L1*(1-cos((*disp)(2))))*((*vel)(0)-L1*sin((*disp)(2))*(*vel)(2))+2*((*disp)(1)+L1*sin((*disp)(2))+La2)*((*vel)(1)+L1*cos((*disp)(2))*(*vel)(2)),2)/pow(pow((*disp)(0)-L1*(1-cos((*disp)(2))),2)+pow((*disp)(1)+L1*sin((*disp)(2))+La2,2),1.5)+1/2*(2*pow((*vel)(0)-L1*sin((*disp)(2))*(*vel)(2),2)+2*((*disp)(0)-L1*(1-cos((*disp)(2))))*((*accel)(0)-L1*cos((*disp)(2))*pow((*vel)(2),2)-L1*sin((*disp)(2))*(*accel)(2))+2*pow((*vel)(1)+L1*cos((*disp)(2))*(*vel)(2),2)+2*((*disp)(1)+L1*sin((*disp)(2))+La2)*((*accel)(1)-L1*sin((*disp)(2))*pow((*vel)(2),2)+L1*cos((*disp)(2))*(*accel)(2)))/pow(pow((*disp)(0)-L1*(1-cos((*disp)(2))),2)+pow((*disp)(1)+L1*sin((*disp)(2))+La2,2),0.5);
    }
    // nonlinear geometry, horizontal actuator right
    else if (nlFlag == 3)  {
        opserr << "ESChevronBrace2d::transfTrialAccel() - "
            << "nonlinear geometry with horizontal actuator right "
            << "not implemented yet";
        return OF_ReturnType_failed;
    }
    
    return OF_ReturnType_completed;
}


int ESChevronBrace2d::transfTrialForce(const Vector* force)
{  
    // linear geometry, horizontal actuator left
    if (nlFlag == 0)  {
        // actuator 0
        (*cForce)(0) = (*force)(0);
        // actuator 1
        (*cForce)(1) = 1.0/(L0+L1)*(L1*(*force)(1) - (*force)(2));
        // actuator 2
        (*cForce)(2) = 1.0/(L0+L1)*(L0*(*force)(1) + (*force)(2));
    }
    // linear geometry, horizontal actuator right
    else if (nlFlag == 1)  {
        // actuator 0
        (*cForce)(0) = -(*force)(0);
        // actuator 1
        (*cForce)(1) = 1.0/(L0+L1)*(L1*(*force)(1) - (*force)(2));
        // actuator 2
        (*cForce)(2) = 1.0/(L0+L1)*(L0*(*force)(1) + (*force)(2));
    }
    // nonlinear geometry, horizontal actuator left
    else if (nlFlag == 2)  {
        opserr << "ESChevronBrace2d::transfTrialForce() - "
            << "nonlinear geometry with horizontal actuator left "
            << "not implemented yet";
        return OF_ReturnType_failed;
    }
    // nonlinear geometry, horizontal actuator right
    else if (nlFlag == 3)  {
        opserr << "ESChevronBrace2d::transfTrialForce() - "
            << "nonlinear geometry with horizontal actuator right "
            << "not implemented yet";
        return OF_ReturnType_failed;
    }
    
    return OF_ReturnType_completed;
}


int ESChevronBrace2d::transfTrialTime(const Vector* time)
{
    *cTime = *time;
    
    return OF_ReturnType_completed;
}


int ESChevronBrace2d::transfDaqDisp(Vector* disp)
{ 
    // linear geometry, horizontal actuator left
    if (nlFlag == 0)  {
        (*disp)(0) = (*dDisp)(0);
        (*disp)(1) = 1.0/(L0+L1)*(L1*(*dDisp)(1) + L0*(*dDisp)(2));
        (*disp)(2) = 1.0/(L0+L1)*(-(*dDisp)(1) + (*dDisp)(2));
    }
    // linear geometry, horizontal actuator right
    else if (nlFlag == 1)  {
        (*disp)(0) = -(*dDisp)(0);
        (*disp)(1) = 1.0/(L0+L1) * (L1*(*dDisp)(1) + L0*(*dDisp)(2));
        (*disp)(2) = 1.0/(L0+L1) * (-(*dDisp)(1) + (*dDisp)(2));
    }
    // nonlinear geometry, horizontal actuator left
    else if (nlFlag == 2)  {
        Vector F(2), theta(2), dTheta(2);
        Matrix DF(2,2);
        int iter = 0;
        int maxIter = 15;
        double tol = 1E-9;
        
        double d0 = La0 + (*dDisp)(0);
        double d1 = La1 + (*dDisp)(1);
        double d2 = La2 + (*dDisp)(2);
        
        theta(0) = (d0-La0)/La1;
        theta(1) = (d0-La0)/La2;
        
        do  {
            F(0) = pow(d0,2) - pow(d1*sin(theta(0))+La0,2) - pow(d1*cos(theta(0))-La1,2);
            F(1) = pow(L0+L1,2) - pow(d2*sin(theta(1))+L0+L1-d1*sin(theta(0)),2) - pow(d2*cos(theta(1))-d1*cos(theta(0)),2);
            
            DF(0,0) = -2*d1*(La0*cos(theta(0))+La1*sin(theta(0)));
            DF(0,1) = 0;
            DF(1,0) = 2*d1*((L0+L1)*cos(theta(0))-d2*sin(theta(0)-theta(1)));
            DF(1,1) = 2*d2*(-(L0+L1)*cos(theta(1))+d1*sin(theta(0)-theta(1)));
            
            // Newton's method
            dTheta = F/DF;
            theta -= dTheta;
            iter++;
        }  while ((dTheta.Norm() >= tol) && (iter <= maxIter));
        
        // issue warning if iteration did not converge
        if (iter >= maxIter)   {
            opserr << "WARNING: ESChevronBrace2d::transfDaqDisp() - "
                << "did not find the angle theta after "
                << iter << " iterations and norm: " << dTheta.Norm() << endln;
            return OF_ReturnType_failed;
        }

        (*disp)(2) = atan((d2*cos(theta(1))-d1*cos(theta(0)))/(d2*sin(theta(1))+L0+L1-d1*sin(theta(0))));
        (*disp)(0) = d1*sin(theta(0))+L0*cos((*disp)(2))-L0;
        (*disp)(1) = d1*cos(theta(0))+L0*sin((*disp)(2))-La1;
    }
    // nonlinear geometry, horizontal actuator right
    else if (nlFlag == 3)  {
        Vector F(2), theta(2), dTheta(2);
        Matrix DF(2,2);
        int iter = 0;
        int maxIter = 15;
        double tol = 1E-9;
        
        double d0 = La0 + (*dDisp)(0);
        double d1 = La1 + (*dDisp)(1);
        double d2 = La2 + (*dDisp)(2);
        
        theta(0) = (d0-La0)/La1;
        theta(1) = (d0-La0)/La2;
        
        do  {
            F(0) = pow(d0,2) - pow(-d2*sin(theta(1))-La0,2) - pow(d2*cos(theta(1))-La1,2);
            F(1) = pow(L0+L1,2) - pow(-d2*sin(theta(1))+L0+L1+d1*sin(theta(0)),2) - pow(d2*cos(theta(1))-d1*cos(theta(0)),2);
            
            DF(0,0) = 0;
            DF(0,1) = -2*d2*(La0*cos(theta(1))+La1*sin(theta(1)));
            DF(1,0) = 2*d1*(-(L0+L1)*cos(theta(0))-d2*sin(theta(0)-theta(1)));
            DF(1,1) = 2*d2*((L0+L1)*cos(theta(1))+d1*sin(theta(0)-theta(1)));
            
            // Newton’s method
            dTheta = F/DF;
            theta -= dTheta;
            iter++;
        }  while ((dTheta.Norm() >= tol) && (iter <= maxIter));
        
        // issue warning if iteration did not converge
        if (iter >= maxIter)   {
            opserr << "WARNING: ESChevronBrace2d::transfDaqDisp() - "
                << "did not find the angle theta after "
                << iter << " iterations and norm: " << dTheta.Norm() << endln;
            return OF_ReturnType_failed;
        }

        (*disp)(2) = atan((d2*cos(theta(1))-d1*cos(theta(0)))/(-d2*sin(theta(1))+L0+L1+d1*sin(theta(0))));
        (*disp)(0) = -d1*sin(theta(0))+L0*cos((*disp)(2))-L0;
        (*disp)(1) = d1*cos(theta(0))+L0*sin((*disp)(2))-La1;
    }
    
    return OF_ReturnType_completed;
}


int ESChevronBrace2d::transfDaqVel(Vector* vel)
{
    // linear geometry, horizontal actuator left
    if (nlFlag == 0)  {
        (*vel)(0) = (*dVel)(0);
        (*vel)(1) = 1.0/(L0+L1)*(L1*(*dVel)(1) + L0*(*dVel)(2));
        (*vel)(2) = 1.0/(L0+L1)*(-(*dVel)(1) + (*dVel)(2));
    }
    // linear geometry, horizontal actuator right
    else if (nlFlag == 1)  {
        (*vel)(0) = -(*dVel)(0);
        (*vel)(1) = 1.0/(L0+L1) * (L1*(*dVel)(1) + L0*(*dVel)(2));
        (*vel)(2) = 1.0/(L0+L1) * (-(*dVel)(1) + (*dVel)(2));
    }
    // nonlinear geometry, horizontal actuator left
    else if (nlFlag == 2)  {
        opserr << "ESChevronBrace2d::transfDaqVel() - "
            << "nonlinear geometry with horizontal actuator left "
            << "not implemented yet";
        return OF_ReturnType_failed;
    }
    // nonlinear geometry, horizontal actuator right
    else if (nlFlag == 3)  {
        opserr << "ESChevronBrace2d::transfDaqVel() - "
            << "nonlinear geometry with horizontal actuator right "
            << "not implemented yet";
        return OF_ReturnType_failed;
    }
    
    return OF_ReturnType_completed;
}


int ESChevronBrace2d::transfDaqAccel(Vector* accel)
{
    // linear geometry, horizontal actuator left
    if (nlFlag == 0)  {
        (*accel)(0) = (*dAccel)(0);
        (*accel)(1) = 1.0/(L0+L1)*(L1*(*dAccel)(1) + L0*(*dAccel)(2));
        (*accel)(2) = 1.0/(L0+L1)*(-(*dAccel)(1) + (*dAccel)(2));
    }
    // linear geometry, horizontal actuator right
    else if (nlFlag == 1)  {
        (*accel)(0) = -(*dAccel)(0);
        (*accel)(1) = 1.0/(L0+L1) * (L1*(*dAccel)(1) + L0*(*dAccel)(2));
        (*accel)(2) = 1.0/(L0+L1) * (-(*dAccel)(1) + (*dAccel)(2));
    }
    // nonlinear geometry, horizontal actuator left
    else if (nlFlag == 2)  {
        opserr << "ESChevronBrace2d::transfDaqAccel() - "
            << "nonlinear geometry with horizontal actuator left "
            << "not implemented yet";
        return OF_ReturnType_failed;
    }
    // nonlinear geometry, horizontal actuator right
    else if (nlFlag == 3)  {
        opserr << "ESChevronBrace2d::transfDaqAccel() - "
            << "nonlinear geometry with horizontal actuator right "
            << "not implemented yet";
        return OF_ReturnType_failed;
    }
    
    return OF_ReturnType_completed;
}


int ESChevronBrace2d::transfDaqForce(Vector* force)
{
    *force = *dForce;
    
    return OF_ReturnType_completed;
}


int ESChevronBrace2d::transfDaqTime(Vector* time)
{  
    *time = *dTime;
    
    return OF_ReturnType_completed;
}
