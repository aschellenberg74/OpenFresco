/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 2001, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision$
// $Date$
// $Source: /usr/local/cvs/OpenSees/SRC/domain/component/Parameter.h,v $

#ifndef Parameter_h
#define Parameter_h

#include <Information.h>
#include <TaggedObject.h>
#include <string.h>

class DomainComponent;
class MovableObject;
class Channel;
class FEM_ObjectBroker;
class Domain;

class Parameter : public TaggedObject, public MovableObject
{
 public:
  Parameter(int tag, DomainComponent *theObject,
	    const char **argv, int argc);
  Parameter(const Parameter &param);
  Parameter(int tag, int classTag = PARAMETER_TAG_Parameter);
  Parameter();
  virtual ~Parameter();
  
  virtual void Print(OPS_Stream &s, int flag =0);
  
  virtual int update(int newValue); 
  virtual int update(double newValue); 
  virtual int activate(bool active);
  virtual double getValue(void) {return theInfo.theDouble;}
  virtual void setValue(double newValue) {theInfo.theDouble = newValue;}

  virtual int addComponent(DomainComponent *theObject, const char **argv, int argc);  
  virtual int addObject(int parameterID, MovableObject *object);

  virtual int clean(void);

  void setGradIndex(int gradInd) {gradIndex = gradInd;}
  int getGradIndex(void) {return gradIndex;}

  // To make it compile -- MHS 9/28/2011
  bool isImplicit(void) {return true;}
  double getSensitivity(int index) {return 0.0;}
  double getPerturbation(void) {return 0.1;}

  virtual void setDomain(Domain *theDomain);
  virtual int sendSelf(int commitTag, Channel &theChannel);  
  virtual int recvSelf(int commitTag, Channel &theChannel, FEM_ObjectBroker &theBroker);

 protected:
  
 private:
  Information theInfo;
  double currentValue;

  enum {initialSize = 64};
  enum {expandSize = 128};

  DomainComponent **theComponents;
  int numComponents;
  int maxNumComponents;

  MovableObject **theObjects;
  int numObjects;
  int maxNumObjects;

  int *parameterID;

  int gradIndex; // 0,...,nparam-1
};

#endif
