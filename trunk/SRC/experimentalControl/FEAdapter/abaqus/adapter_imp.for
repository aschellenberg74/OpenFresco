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

** Written: Andreas Schellenberg
** Created: 09/09
** Revision: A

** Description: This file contains the implementation of uel.
** uel is an implicit Abaqus (R) adapter element. The element
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
     *     params(3),jdltyp(mdload,*),adlmag(mdload,*),
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
      real*8    pr(ndofel)
      real*8    timePast
      
      save socketIDs
      save sData
      save rData
      save timePast
      
      data socketIDs /numSockIDs*0/
      data timePast /0.0/
      
      
c     extract socketID
c     (jtype = user-defined integer value n in element type Un)
      if (jtype .le. numSockIDs) then
         socketID = socketIDs(jtype)
      else
         write(*,*) 'ERROR - Only ',numSockIDs,' adapter_imp ',
     *              'elements supported: consider increasing ',
     *              'numSockIDs parameter in adapter_imp.for'
         call xit
      endif
      
c     setup connection with ECSimAdapter client
      if (socketID .eq. 0) then
         
c        check number of state variables first
         if (nsvars .lt. 2*ndofel) then
            write(*,*) 'ERROR - ',2*ndofel,' state variables are ',
     *                 'required for adapter element U',jtype
            call xit
         endif
         
c        check number of penalty stiffness terms next
         if (nprops .lt. ndofel) then
            write(*,*) 'ERROR - ',ndofel,' penalty stiffness terms ',
     *                 'are required for adapter element U',jtype
            call xit
         endif
         
c        now setup the connection
         port = jprops(1)
         write(*,*) 'Waiting for ECSimAdapter experimental ',
     *              'control on port',port,'...'
         call setupconnectionserver(port,socketID)
         if (socketID .le. 0) then
            write(*,*) 'ERROR - failed to setup connection'
            call xit
         endif
         socketIDs(jtype) = socketID
         
c        get the data sizes
c        sizes = {ctrlDisp, ctrlVel, ctrlAccel, ctrlForce, ctrlTime,
c                 daqDisp,  daqVel,  daqAccel,  daqForce,  daqTime,  dataSize}
         call recvdata(socketID, sizeInt, iData, 11, stat)
         
         if (iData(1) .ne. 0 .and. iData(1) .ne. ndofel .or.
     *       iData(4) .ne. 0 .and. iData(4) .ne. ndofel .or.
     *       iData(6) .ne. 0 .and. iData(6) .ne. ndofel .or.
     *       iData(9) .ne. 0 .and. iData(9) .ne. ndofel) then
            write(*,*) 'ERROR - wrong data sizes received: ',
     *                 iData(1),iData(4),iData(6),iData(9),
     *                 ' != 0 or ',ndofel
            call closeconnection(socketID, stat)
            call xit
         endif
         write(*,*) 'Adapter element now running...'
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
     *                    'in adapter element U',jtype
               call closeconnection(socketID, stat)
               call xit
            else
c              update response if time has advanced
               if (time(iTotalTime) .gt. timePast) then
c                 receive data
                  call recvdata(socketID, sizeDouble,
     *                          rData, dataSize, stat)
                  
c                 check if force request was received
                  if (rData(1) .eq. 10.) then
c                    send daq displacements and forces
                     call senddata(socketID, sizeDouble,
     *                             sData, dataSize, stat)
                     
c                    receive new trial response
                     call recvdata(socketID, sizeDouble,
     *                             rData, dataSize, stat)
                  endif
                  
                  if (rData(1) .ne. 3.) then
                     if (rData(1) .eq. 99.) then
                        write(*,*) 'The Simulation has successfully ',
     *                             'completed'
                        call closeconnection(socketID, stat)
                        call xit
                     else
                        write(*,*) 'ERROR - Wrong action received: ',
     *                             'expecting 3 but got ',rData(1)
                        call closeconnection(socketID, stat)
                        call xit
                     endif
                  endif
                  
c                 save current time
                  timePast = time(iTotalTime)
               endif
               
c              get resisting force pr = k*u + p0 = k*(u - u0)
               do i = 1, ndofel
                  pr(i) = props(i)*(u(i) - rData(1+i))
                  rhs(i,1) = -pr(i)
               enddo
               
c              assign daq values for feedback
               do i = 1, ndofel
                  sData(i) = u(i)
                  sData(ndofel+i) = -pr(i)
               enddo
               
c              set tangent stiffness matrix
               do i = 1, ndofel
                  amatrx(i,i) = props(i)
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
            
c           update response if time has advanced
            if (time(iTotalTime) .gt. timePast) then
c              receive data
               call recvdata(socketID, sizeDouble,
     *                       rData, dataSize, stat)
               
c              check if force request was received
               if (rData(1) .eq. 10.) then
c                 send daq displacements and forces
                  call senddata(socketID, sizeDouble,
     *                          sData, dataSize, stat)
                  
c                 receive new trial response
                  call recvdata(socketID, sizeDouble,
     *                          rData, dataSize, stat)
               endif
               
               if (rData(1) .ne. 3.) then
                  if (rData(1) .eq. 99.) then
                     write(*,*) 'The Simulation has successfully ',
     *                          'completed'
                     call closeconnection(socketID, stat)
                     call xit
                  else
                     write(*,*) 'ERROR - Wrong action received: ',
     *                          'expecting 3 but got ',rData(1)
                     call closeconnection(socketID, stat)
                     call xit
                  endif
               endif
               
c              save current time
               timePast = time(iTotalTime)
            endif
            
c           get resisting force pr = k*u + p0 = k*(u - u0)
            do i = 1, ndofel
               pr(i) = props(i)*(u(i) - rData(1+i))
            enddo
            
c           assign daq values for feedback
            do i = 1, ndofel
               sData(i) = u(i)
               sData(ndofel+i) = -rhs(i,1)
            enddo
            
c           update resting force for alpha method            
            do i = 1, ndofel
               rhs(i,1) = -(1.0+alpha)*pr(i) + alpha*svars(i)
               svars(ndofel+i) = svars(i)
               svars(i) = pr(i)
            enddo
            
c           set tangent stiffness matrix
            do i = 1, ndofel
               amatrx(i,i) = (1.0+alpha)*props(i)
            enddo
         endif
      
c     stiffness matrix
      else if (lflags(iOpCode) .eq. jCurrentStiff) then
         do i = 1, ndofel
            amatrx(i,i) = props(i)
         enddo
      
c     mass matrix
      else if (lflags(iOpCode) .eq. jCurrentMass) then
         do i = 1, ndofel
            amatrx(i,i) = 0.0
         enddo
      
c     residual calculation
      else if (lflags(iOpCode) .eq. jCurrentResidual) then
         alpha = params(1)
         
         write(*,*) 'ERROR - residual calculation not implemented ',
     *              'yet in adapter element U',jtype
         call closeconnection(socketID, stat)
         call xit
      
c     initial acceleration calculation
      else if (lflags(iOpCode) .eq. jInitialAccel) then
         do i = 1, ndofel
            amatrx(i,i) = 0.0
            rhs(i,1) = 0.0
         enddo
      endif
      
      return
      end
