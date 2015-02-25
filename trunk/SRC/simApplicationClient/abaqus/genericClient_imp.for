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

** ****************************************************************** **
**  Abaqus (R) is a registered trademark of                           **
**  ABAQUS, Inc in the United States.                                 **
**                                                                    **
** ****************************************************************** **

** $Revision$
** $Date$
** $Source: $

** Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
** Created: 03/09
** Revision: A

** Description: This file contains the implementation of uel.
** uel is an implicit Abaqus (R) client element. The element
** communicates with the OpenFresco middle-tier server through
** a tcp/ip connection.

      subroutine uel(rhs,amatrx,svars,energy,
     *               ndofel,nrhs,nsvars,props,nprops,
     *               coords,mcrd,nnode,u,du,v,a,
     *               jtype,time,dtime,kstep,kinc,jelem,
     *               params,ndload,jdltyp,adlmag,predef,npredf,
     *               lflags,mlvarx,ddlmag,mdload,pnewdt,
     *               jprops,njprop,period)
      
      include 'aba_param.inc'
      
c     operational code keys
      parameter (jNormalTimeIncr  = 1,
     *           jCurrentStiff    = 2,
     *           jCurrentDamp     = 3,
     *           jCurrentMass     = 4,
     *           jCurrentResidual = 5,
     *           jInitialAccel    = 6,
     *           jPerturbation    = 100)
      
c     flag indices
      parameter (iProcedure = 1,
     *           iNlgeom    = 2,
     *           iOpCode    = 3,
     *           iStep      = 4,
     *           cApprox    = 5,
     *           nFlags     = 5)
      
c     procedure flags
      parameter (jStaticAutoIncr      =  1,
     *           jStaticDirectIncr    =  2,
     *           jDynImpHalfStepRes   = 11,
     *           jDynImpFixedTimeIncr = 12,
     *           jDynImpSubspaceProj  = 13)
      
c     time indices
      parameter (iStepTime  = 1,
     *           iTotalTime = 2,
     *           nTime      = 2)
      
c     energy array indices
      parameter (iElPd     = 1,
     *           iElCd     = 2,
     *           iElIe     = 3,
     *           iElTs     = 4,
     *           iElDd     = 5,
     *           iElBv     = 6,
     *           iElDe     = 7,
     *           iElHe     = 8,
     *           iElKe     = 9,
     *           iElTh     = 10,
     *           iElDmd    = 11,
     *           iElDc     = 12,
     *           nElEnergy = 12)
      
      dimension rhs(mlvarx,*),amatrx(ndofel,ndofel),
     *     svars(nsvars),energy(8),props(*),coords(mcrd,nnode),
     *     u(ndofel),du(mlvarx,*),v(ndofel),a(ndofel),time(2),
     *     params(*),jdltyp(mdload,*),adlmag(mdload,*),
     *     ddlmag(mdload,*),predef(2,npredf,nnode),lflags(*),
     *     jprops(*)
      
c     local variables
      integer*4 dataSize
      parameter (dataSize = 256)
      
      integer*4 numSockIDs
      parameter (numSockIDs = 32)
      
      integer*4 sizeInt, sizeDouble
      parameter (sizeInt = 4, sizeDouble = 8)
      
      integer*4 port
      integer*4 sizeMachineInet
      integer*4 socketIDs(numSockIDs)
      integer*4 socketID
      integer*4 stat
      
      integer*4 iData(11)
      real*8    sData(dataSize)
      real*8    rData(dataSize)
      real*8    timePast
      
      save socketIDs
      save timePast
      
      data socketIDs /numSockIDs*0/
      data timePast /0.0/
      
      
c     extract socketID
c     (jtype = user-defined integer value n in element type Un)
      if (jtype .le. numSockIDs) then
         socketID = socketIDs(jtype)
         write(*,*) 'INFO - socketID = ',socketID,'.'
      else
         write(*,*) 'ERROR - Only ',numSockIDs,' genericClient_imp ',
     *              'elements supported: consider increasing ',
     *              'numSockIDs parameter in genericClient_imp.for'
         call xit
      endif
      
c     setup connection with SimAppElemServer
      if (socketID .eq. 0) then
         
c        check number of state variables first
         if (nsvars .lt. 2*ndofel) then
            write(*,*) 'ERROR - ',2*ndofel,' state variables are ',
     *                 'required for genericClient element U',jtype
            call xit
         endif
         
c        now setup the connection
         port = jprops(1)
         sizeMachineInet = 9+1
         call setupconnectionclient(port,
     *                              '127.0.0.1'//char(0),
     *                              sizeMachineInet,
     *                              socketID)
         if (socketID .le. 0) then
            write(*,*) 'ERROR - failed to setup connection'
            call xit
         endif
         socketIDs(jtype) = socketID
         
c        set the data size for the experimental element
c        sizeCtrl(disp)
         iData(1)  = ndofel
c        sizeCtrl(vel)
         iData(2)  = ndofel
c        sizeCtrl(accel)
         iData(3)  = ndofel
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
         iData(9)  = ndofel
c        sizeDaq(time)
         iData(10) = 0
c        dataSize
         iData(11) = dataSize
         
         call senddata(socketID, sizeInt,
     *                 iData, 11, stat)
      endif
      
c     zero rhs vector and A matrix
      do i = 1, ndofel
         do j = 1, nrhs
            rhs(i,j) = 0.0
         enddo
         do j = 1, ndofel
            amatrx(i,j) = 0.0
         enddo
      enddo
      
c     normal incrementation
      if (lflags(iOpCode) .eq. jNormalTimeIncr) then
         
c        static analysis
         if (lflags(iProcedure) .eq. jStaticAutoIncr .or.
     *       lflags(iProcedure) .eq. jStaticDirectIncr) then
            if (lflags(iStep) .ne. 0) then
               write(*,*) 'ERROR - Linear perturbation not supported ',
     *                    'in genericClient element U',jtype
               call closeconnection(socketID, stat)
               call xit
            else
c              commit state
               if (time(iTotalTime) .gt. timePast) then
                  sData(1) = 5
                  call senddata(socketID, sizeDouble,
     *                          sData, dataSize, stat)
                  timePast = time(iTotalTime)
               endif
               
c              send trial response to experimental element
               sData(1) = 3
               do i = 1, ndofel
                  sData(1+i) = u(i)
                  sData(1+ndofel+i) = v(i)
                  sData(1+2*ndofel+i) = a(i)
               enddo
               sData(1+3*ndofel+1) = time(iTotalTime)
               
               call senddata(socketID, sizeDouble,
     *                       sData, dataSize, stat)
               
c              get measured resisting forces
               sData(1) = 10
               call senddata(socketID, sizeDouble,
     *                       sData, dataSize, stat)
               
               call recvdata(socketID, sizeDouble,
     *                       rData, dataSize, stat)
               
               do i = 1, ndofel
                  rhs(i,1) = -rData(i)
               enddo
               
c              get tangent stiffness matrix
               sData(1) = 13
               call senddata(socketID, sizeDouble,
     *                       sData, dataSize, stat)
               
               call recvdata(socketID, sizeDouble,
     *                       rData, dataSize, stat)
               
               k = 1
               do i = 1, ndofel
                  do j = 1, ndofel
                     amatrx(i,j) = rData(k)
                     k = k + 1
                  enddo
               enddo
            endif
         
c        dynamic analysis
         else if (lflags(iProcedure) .eq. jDynImpHalfStepRes .or.
     *            lflags(iProcedure) .eq. jDynImpFixedTimeIncr) then
            alpha = params(1)
            beta  = params(2)
            gamma = params(3)
            
            dadu = 1.0/(beta*dtime**2)
            dvdu = gamma/(beta*dtime)
            
c           commit state
            if (time(iTotalTime) .gt. timePast) then
               sData(1) = 5
               call senddata(socketID, sizeDouble,
     *                       sData, dataSize, stat)
               timePast = time(iTotalTime)
            endif
            
c           send trial response to experimental element
            sData(1) = 3
            do i = 1, ndofel
               sData(1+i) = u(i)
               sData(1+ndofel+i) = v(i)
               sData(1+2*ndofel+i) = a(i)
            enddo
            sData(1+3*ndofel+1) = time(iTotalTime)
            
            call senddata(socketID, sizeDouble,
     *                    sData, dataSize, stat)
            
c           get measured resisting forces
            sData(1) = 10
            call senddata(socketID, sizeDouble,
     *                    sData, dataSize, stat)
            
            call recvdata(socketID, sizeDouble,
     *                    rData, dataSize, stat)
            
            do i = 1, ndofel
               rhs(i,1) = -(1.0+alpha)*rData(i) + alpha*svars(i)
               svars(ndofel+i) = svars(i)
               svars(i) = rData(i)
            enddo
            
c           get tangent stiffness matrix
            sData(1) = 13
            call senddata(socketID, sizeDouble,
     *                    sData, dataSize, stat)
            
            call recvdata(socketID, sizeDouble,
     *                    rData, dataSize, stat)
            
            k = 1
            do i = 1, ndofel
               do j = 1, ndofel
                  amatrx(i,j) = (1.0+alpha)*rData(k)
                  k = k + 1
               enddo
            enddo
         endif
      
c     stiffness matrix
      else if (lflags(iOpCode) .eq. jCurrentStiff) then
c        get tangent stiffness matrix
         sData(1) = 13
         call senddata(socketID, sizeDouble,
     *                 sData, dataSize, stat)
         
         call recvdata(socketID, sizeDouble,
     *                 rData, dataSize, stat)
         
         k = 1
         do i = 1, ndofel
            do j = 1, ndofel
               amatrx(i,j) = rData(k)
               k = k + 1
            enddo 
         enddo
      
c     mass matrix
      else if (lflags(iOpCode) .eq. jCurrentMass) then
c        get mass matrix
         sData(1) = 15
         call senddata(socketID, sizeDouble,
     *                 sData, dataSize, stat)
         
         call recvdata(socketID, sizeDouble,
     *                 rData, dataSize, stat)
         
         k = 1
         do i = 1, ndofel
            do j = 1, ndofel
               amatrx(i,j) = rData(k)
               if (i .eq. j .and. amatrx(i,j) .le. 0.0) then
                  amatrx(i,j) = 1E-12
               endif
               k = k + 1
            enddo
         enddo
      
c     residual calculation
      else if (lflags(iOpCode) .eq. jCurrentResidual) then
         alpha = params(1)
         
c        send trial response to experimental element
         sData(1) = 3
         do i = 1, ndofel
            sData(1+i) = u(i)
            sData(1+ndofel+i) = v(i)
            sData(1+2*ndofel+i) = a(i)
         enddo
         sData(1+3*ndofel+1) = time(iTotalTime)
         
         call senddata(socketID, sizeDouble,
     *                 sData, dataSize, stat)
         
c        get measured resisting forces
         sData(1) = 10
         call senddata(socketID, sizeDouble,
     *                 sData, dataSize, stat)
         
         call recvdata(socketID, sizeDouble,
     *                 rData, dataSize, stat)
         
         do i = 1, ndofel
            rhs(i,1) = -(1.0+alpha)*rData(i)
     *                 + 0.5*alpha*(svars(i)+svars(ndofel+i))
         enddo
      
c     initial acceleration calculation
      else if (lflags(iOpCode) .eq. jInitialAccel) then
c        get mass matrix
         sData(1) = 15
         call senddata(socketID, sizeDouble,
     *                 sData, dataSize, stat)
               
         call recvdata(socketID, sizeDouble,
     *                 rData, dataSize, stat)
               
         k = 1
         do i = 1, ndofel
            do j = 1, ndofel
               amatrx(i,j) = rData(k)
               if (i .eq. j .and. amatrx(i,j) .le. 0.0) then
                  amatrx(i,j) = 1E-12
               endif
               k = k + 1
            enddo
            rhs(i,1) = 0.0
         enddo
      endif
      
      return
      end
