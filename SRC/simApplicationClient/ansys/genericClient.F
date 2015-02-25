** ****************************************************************** **
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
** ****************************************************************** **

** $Revision:$
** $Date:$
** $Source:$

** Written: Andreas Schellenberg
** Created: 12/10
** Revision: A

** Description: This file contains the implementation of UserElem.
** UserElem is an implicit ANSYS (R) client element. The element
** communicates with the OpenFresco middle-tier server through
** a tcp/ip connection.

      subroutine UserElem (elId, matId, keyMtx, lumpm, nDim, nNodes,
     *                     Nodes, nIntPnts, nUsrDof, kEStress, 
     *                     keyAnsMat, keySym, nKeyOpt, KeyOpt,
     *                     temper, temperB, tRef, kTherm, 
     *                     nPress, Press, kPress, nReal, RealConst, 
     *                     nSaveVars, saveVars, xRef, xCur, 
     *                     TotValDofs, IncValDofs, ItrValDofs,
     *                     VelValDofs, AccValDofs,
     *                     kfstps, nlgeom, nrkey, outkey, elPrint, iott,
     *                     keyHisUpd, ldstep, isubst, ieqitr, timval, 
     *                     keyEleErr, keyEleCnv,
     *                     eStiff, eMass, eDamp, eSStiff,
     *                     fExt, fInt, elVol, elMass, elCG, 
     *                     nRsltBsc, RsltBsc, nRsltVar, RsltVar, 
     *                     nElEng, elEnergy) 
     *                     

c******************************************************************************
c
c     elId      (int,sc,in)        element number
c     matId     (int,sc,in)        material number of this element
c     keyMtx    (int,ar(10),in)    matrix and load vector form requests
c                                     0 = not requested, 1 = requested
c                                     see below for more details
c     lumpm     (int,sc,in)        mass matrix format
c                                    = 0 no lumped mass matrix
c                                    = 1 lumped mass matrix
c     nDim      (int,sc,in)        number of dimensions of the problem
c                                       (defined on USRELEM command as NDIM)
c                                    = 2 2D
c                                    = 3 3D
c     nNodes    (int,sc,in)        number of nodes of the element
c                                       (defined on USRELEM command as NNODES)
c     Nodes     (int,ar(nNodes),in)node list of this element 
c     nIntPnts  (int,sc,in)        maximum number of integration points
c                                       (defined on USRELEM command as NINTPNTS)
c     nUsrDof   (int,sc,in)        number of DOFs of this element (matrix and 
c                                     load vector size)
c     kEStress  (int,sc,in)        kEStress 
c                                       (defined on USRELEM command as KESTRESS)
c     keyAnsMat (int,sc,in)        key to indicate if ANSYS material
c                                     routine is going to be called
c                                     (defined on USRELEM command as KEYANSMAT)
c                                     = 0, No
c                                     = 1, Yes
c     keySym    (int,sc,in)        key to indicate if element matrices
c                                     is symmetric
c                                       (defined on USRELEM command as KEYSYM)
c                                     = 0, symmetric
c                                     = 1, unsymmetric
c     nKeyOpt   (int,sc,in)        number of element key options able to be
c                                     used in this routine
c     KeyOpt    (int,ar(nKeyOpt),in) values of element key option defined
c                                     by et or keyopt command for the
c                                     user elements, only the first
c                                     nKeyOpt values are passed in and can
c                                     be used to branch the routine for
c                                     different formulations
c     temper    (dp,ar(nNodes),in) nodal temperatures at current time
c     temperB   (dp,ar(nNodes),in) nodal temperatures at the beginning of this
c                                     incremental step (substep)
c     tRef      (dp,sc,in)         reference temperature
c     kTherm    (int,sc,inout)     input:  flag for thermal loading 
c                                      = 1, Temperatures at nodes are different 
c                                      from the reference temperature, 
c                                      thermal loading might be needed.
c                                      = 0, Temperatures at nodes are the same
c                                      as the reference temperature, 
c                                      thermal loading is not needed.
c                                  output:  flag for thermal strains
c     nPress    (int,sc,in)        number of pressure values for this element
c     Press     (dp,ar(nPress),in) applied elemental face load (pressure)
c     kPress    (int,sc,in)        flag for pressure loading 
c                                      = 1, pressure load is applied and 
c                                      equivalent nodal forces should be 
c                                      calculated
c                                      = 0, no pressure loading
c     nReal     (int,sc,in)        number of real constants
c                                       (defined on USRELEM command as NREAL)
c     RealConst (dp,ar(nReal),in)  user defined real constants 
c     nSaveVars (int,sc,in)        number of saved variables
c                                      (defined on USRELEM command as NSAVEVARS)
c     saveVars  (dp,ar(nSaveVars),inout) user saved variables
c     xRef      (dp,ar(nDim,nNodes),in)
c                                  nodal coordinates in initial configuration
c     xCur      (dp,ar(nDim,nNodes),in)
c                                  nodal coordinates in current configuration
c     TotValDofs (dp,ar(nUsrDof),in) total values of DOFs (displacements) 
c                                     from time = 0
c     IncValDofs (dp,ar(nUsrDof),in) incremental values of DOFs (displacements) 
c                                     for the current step
c     ItrValDofs (dp,ar(nUsrDof),in) iterative values of DOFs (displacements)
c                                     for the current iteration
c                                     (normally needed for debug only)
c     VelValDofs (dp,ar(nUsrDof),in) first time derivatives of DOFs 
c                                             (velocities) (normally not needed)
c     AccValDofs (dp,ar(nUsrDof),in) second time derivatives of DOFs 
c                                          (accelerations) (normally not needed)
c     kfstps    (int,sc,in)        key for the first iteration of first 
c                                     substep of the first load step
c                                     = 1 yes
c                                     = 0 no
c     nlgeom    (int,sc,in)        large deformation key [from nlgeom command]
c                                     = 0 NLGEOM,OFF
c                                     = 1 NLGEOM, ON
c     nrkey     (int,sc,in)        key to indicate a newton-raphson
c                                     (incremental) procedure
c                                     = 0 No
c                                     = 1 Yes
c     outkey    (int,sc,in)        key to indicate if any element output is
c                                     to be placed on the print file or the 
c                                     result file
c                                     = 0 No
c                                     = 1 Yes
c     elPrint   (int,sc,in)        key to indicate if any element output is 
c                                     to be placed on the print file
c                                     = 0 No
c                                     = 1 Yes
c     iott      (int,sc,in)        print output file unit number
c     keyHisUpd (int,sc,in)        key to indicate if history-dependent
c                                    variables need to be updated, like
c                                    equivalent plastic strain, back stress
c                                    etc. since the iteration is already
c                                    converged
c                                     = 0 not converged, don't need to update
c                                         history dependent variables
c                                     = 1 yes, converged, need to update
c                                         history dependent variables
c
c     --- The following 7 variable group can usually be ignored.
c     --- The variables are used for debug, timing, and convergence control.
c     ldstep    (int,sc,in)        current load step number
c     isubst    (int,sc,in)        current substep number
c     ieqitr    (int,sc,in)        current equilibium iteration  number
c     timval    (int,sc,in)        current time value
c     keyEleErr (int,sc,inout)     key to indicate if there is any element 
c                                     formulation error, like negative Jacobian.
c                                     The error could be caused by too
c                                     large incremental step, illegal model.
c                                     = 0 no error (preset value before calling)
c                                     = 1 some error happens. ANSYS will
c                                     decide to stop the analysis or cutback
c                                     the substep (bi-section) based on other
c                                     user input and information at higher
c                                     level.
c     keyEleCnv (int,sc,inout)     key to flag if this element satisfies
c                                     the user defined element convergence
c                                     criterion. 
c                                     = 1, yes, the criterion is satisfied
c                                       or don't have any criterion at all
c                                       it is preset value before calling
c                                     = 0, no, the element doesn't satisfy
c                                       element convergence criterion. If
c                                       this is the case, the iteration will
c                                       not converge even when both force
c                                       and displacement converge 
c       ---- end of 7 variable group -----
c
c                                                                  requested if
c     eStiff(dp,ar(nUsrDof,nUsrDof),inout) stiffness matrix         keyMtx(1)=1
c     eMass (dp,ar(nUsrDof,nUsrDof),inout) mass matrix              keyMtx(2)=1
c     eDamp (dp,ar(nUsrDof,nUsrDof),inout) damping matrix           keyMtx(3)=1
c     eSStiff(dp,ar(nUsrDof,nUsrDof),inout)stress stiffness matrix  keyMtx(4)=1
c     fExt      (dp,ar(nUsrDof),out)       applied f vector         keyMtx(5)=1
c     fInt      (dp,ar(nUsrDof),out)       internal force vector    keyMtx(6)=1

c     elVol     (dp,sc,out)        element volume
c     elMass    (dp,sc,out)        element mass
c     elCG      (dp,ar(3),out)     element centroid coordinates in current
c                                     configuration
c     nRsltBsc  (dp,sc,in)         number of basic elemental results saved in
c                                   result files 
c     RsltBsc   (dp,ar(nRsltBsc),out) basic elemental results 
c                                       (see EXPLANATION below)
c     nRsltVar  (int,sc,in)        number of elemental results saved in 
c                                     result file as non-summable miscellaneous
c                                     variables 
c                                       (defined on USRELEM command as NRSLTVAR)
c     RsltVar   (dp,ar(nRsltVar),out) variables to saved in result files as
c                                      non-summable miscellaneous variables 
c                                      requested when outkey = 1
c
c     nElEng    (int,sc,in)        number of energies (fixed at 3)
c
c     elEnergy  (dp,ar(nElEng),out) elemental energy
c                                     elEnergy(1), element strain energy
c                                     elEnergy(2), element plastic strain energy
c                                     elEnergy(3), element creep strain energy
c
c     EXPLANATION OF RsltBsc
c     
c       Basic element results are saved and total number of result 
c     quantities is nRsltBsc, where:
c            nRsltBsc = (7+7)* number of corner nodes at one element.
c       To process the quantites by post processing properly, the results 
c     must be in the following order:
c       1.) Stresses: Sx Sy Sz Sxy Syz Sxz Seqv at all corner points,
c     followed by:
c       2.) Strains : Ex Ey Ez Exy Eyz Exz Eeqv at all corner points
c     where Seqv and Eeqv = equivalent stress and strain respectively
c
c
c******************************************************************************
c
#include "impcom.inc"
#include "locknm.inc"
c
      integer          elId, matId, keyMtx(10), lumpm, nDim, nNodes,
     *                 Nodes(nNodes), nIntPnts, nUsrDof, kEStress, 
     *                 keyAnsMat, keySym, nKeyOpt, KeyOpt(nKeyOpt),
     *                 kTherm, nPress, kPress, nReal, nSaveVars, 
     *                 kfstps, nlgeom, nrkey, outkey, 
     *                 elPrint, iott, keyHisUpd, 
     *                 ldstep, isubst, ieqitr, keyEleErr, keyEleCnv,
     *                 nRsltBsc, nRsltVar, nElEng
      
      double precision temper(nNodes), temperB(nNodes), tRef,
     *                 Press(nPress), RealConst(nReal),
     *                 saveVars(nSaveVars),
     *                 xRef(nDim,nNodes), xCur(nDim,nNodes),
     *                 TotValDofs(nUsrDof), IncValDofs(nUsrDof),
     *                 ItrValDofs(nUsrDof), VelValDofs(nUsrDof),
     *                 AccValDofs(nUsrDof),      timval,
     *                 eStiff(nUsrDof,nUsrDof), eMass(nUsrDof,nUsrDof),
     *                 eDamp(nUsrDof,nUsrDof), eSStiff(nUsrDof,nUsrDof),
     *                 fExt(nUsrDof), fInt(nUsrDof), 
     *                 elVol, elMass, elCG(3),
     *                 RsltBsc(nRsltBsc), RsltVar(nRsltVar), 
     *                 elEnergy(nElEng)
      
      external         vzero, erhandler

c     local variables
      integer*4 dataSize
      parameter (dataSize = 256)
      
      integer*4 sizeInt, sizeDouble
      parameter (sizeInt = 4, sizeDouble = 8)
      
      integer*4 port
      integer*4 sizeMachineInet
      integer*4 socketID
      integer*4 stat
      
      integer*4 iData(11)
      real*8    sData(dataSize)
      real*8    rData(dataSize)
      
      write(*,*) 'Before init save var'
c     initialize user saved variables
      if (kfstps .eq. 1) then
         if (nSaveVars .lt. 1) then
            write(*,*) 'ERROR - need at least 1 user saved variable'
c            call erhandler ('UserElem', 1000, 3, 'Need at least 1
c     *                       user saved variable.',dperr(1),
c     *                       ' ')
         endif
         saveVars(1) = 0.0
      endif
      
c     extract socketID
      socketID = int(saveVars(1))
      write(*,*) 'Before init socket'
c     setup connection with SimAppElemServer
      if (socketID .eq. 0) then
         
c        get port number and initialize connection
         port = int(RealConst(1))
         sizeMachineInet = 9+1
         write(*,*) 'port = ',port
         call setupconnectionclient(port,
     *                              '127.0.0.1'//char(0),
     *                              sizeMachineInet,
     *                              socketID)
         if (socketID .le. 0) then
            write(*,*) 'ERROR - failed to setup connection'
c            call erhandler ('UserElem', 1000, 3, 'failed to
c     *                       setup connection.',dperr(1),
c     *                       ' ')
         endif
         saveVars(1) = socketID
         
c        set the data size for the experimental element
c        sizeCtrl(disp)
         iData(1)  = nUsrDof
c        sizeCtrl(vel)
         iData(2)  = nUsrDof
c        sizeCtrl(accel)
         iData(3)  = nUsrDof
c        sizeCtrl(force)
         iData(4)  = 0
c        sizeCtrl(time)
         iData(5)  = 1
c        sizeDaq(disp)
         iData(6)  = 0
c        sizeDaq(vel)
         iData(7)  = 0
c        sizeDaq(accel)
         iData(8)  = 0
c        sizeDaq(force)
         iData(9)  = nUsrDof
c        sizeDaq(time)
         iData(10) = 0
c        dataSize
         iData(11) = dataSize
         
         call senddata(socketID, sizeInt,
     *                 iData, 11, stat)
      endif
      
c     commit state (update history variables)
      if (keyHisUpd .eq. 1) then
         sData(1) = 5
         call senddata(socketID, sizeDouble,
     *                 sData, dataSize, stat)
      endif
      
c     create internal force vector
      if (keyMtx(6) .eq. 1) then
c        send trial response to experimental element
         sData(1) = 3
         do i = 1, nUsrDof
            sData(1+i) = TotValDofs(i)
            sData(1+nUsrDof+i) = VelValDofs(i)
            sData(1+2*nUsrDof+i) = AccValDofs(i)
         enddo
         sData(1+3*nUsrDof+1) = timval
         
         call senddata(socketID, sizeDouble,
     *                 sData, dataSize, stat)
         
c        get measured resisting forces
         sData(1) = 10
         call senddata(socketID, sizeDouble,
     *                 sData, dataSize, stat)
         
         call recvdata(socketID, sizeDouble,
     *                 rData, dataSize, stat)
         
c        zero vector first then assign values
         call vzero(fInt(1),nUsrDof)
         do i = 1, nUsrDof
            fInt(i) = rData(i)
         enddo
      endif
      
c     create stiffness matrix
      if (keyMtx(1) .eq. 1) then
c        get tangent stiffness matrix
         sData(1) = 13
         call senddata(socketID, sizeDouble,
     *                 sData, dataSize, stat)
         
         call recvdata(socketID, sizeDouble,
     *                 rData, dataSize, stat)
         
c        zero matrix first then assign values
         call vzero(eStiff(1,1),nUsrDof*nUsrDof)
         k = 1
         do i = 1, nUsrDof
            do j = 1, nUsrDof
               eStiff(i,j) = rData(k)
               k = k + 1
            enddo 
         enddo
      endif
      
c     create mass matrix
      if (keyMtx(2) .eq. 1) then
c        get mass matrix
         sData(1) = 15
         call senddata(socketID, sizeDouble,
     *                 sData, dataSize, stat)
         
         call recvdata(socketID, sizeDouble,
     *                 rData, dataSize, stat)
         
c        zero matrix first then assign values
         call vzero(eMass(1,1),nUsrDof*nUsrDof)
         k = 1
         do i = 1, nUsrDof
            do j = 1, nUsrDof
               eMass(i,j) = rData(k)
               if (i .eq. j .and. eMass(i,j) .le. 0.0) then
                  eMass(i,j) = 1E-12
               endif
               k = k + 1
            enddo
         enddo
      endif
      
      return
      end
