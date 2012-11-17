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
** Created: 03/09
** Revision: A

** Description: This file contains the implementation of vuel.
** vuel is an explicit Abaqus (R) client element. The element
** communicates with the OpenFresco middle-tier server through
** a tcp/ip connection.

      subroutine vuel(nblock,rhs,amass,dtimeStable,svars,nsvars,
     *                energy,
     *                nnode,ndofel,props,nprops,jprops,njprops,
     *                coords,mcrd,u,du,v,a,
     *                jtype,jElem,
     *                time,period,dtimeCur,dtimePrev,kstep,kinc,
     *                lflags,
     *                dMassScaleFactor,
     *                predef,npredef,
     *                jdltyp, adlmag)
      
      include 'vaba_param.inc'
      
c     operational code keys
      parameter (jMassCalc            = 1,
     *           jIntForceAndDtStable = 2,
     *           jExternForce         = 3)
      
c     flag indices
      parameter (iProcedure = 1,
     *           iNlgeom    = 2,
     *           iOpCode    = 3,
     *           nFlags     = 3)
      
c     procedure flags
      parameter (jDynExplicit = 17)
      
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
      
c     predefined variables indices
      parameter (iPredValueNew = 1,
     *           iPredValueOld = 2,
     *           nPred         = 2)
      
      dimension rhs(nblock,ndofel),amass(nblock,ndofel,ndofel),
     *          dtimeStable(nblock),
     *          svars(nblock,nsvars),energy(nblock,nElEnergy),
     *          props(nprops),jprops(njprops),
     *          jElem(nblock),time(nTime),lflags(nFlags),
     *          coords(nblock,nnode,mcrd),
     *          u(nblock,ndofel),du(nblock,ndofel),
     *          v(nblock,ndofel),a(nblock, ndofel),
     *          dMassScaleFactor(nblock),  
     *          predef(nblock,nnode,npredef,nPred),
     *          adlmag(nblock)
      
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
c     (jtype = user-defined integer value n in element type VUn)
      if (jtype .le. numSockIDs) then
         socketID = socketIDs(jtype)
      else
         write(*,*) 'ERROR - Only ',numSockIDs,' genericClient_exp ',
     *              'elements supported: consider increasing ',
     *              'numSockIDs parameter in genericClient_exp.for'
         call xplb_exit
      endif
      
c     setup connection with SimAppElemServer
      if (socketID .eq. 0 .and. time(iTotalTime) .gt. 0.0) then
         port = jprops(1)
         sizeMachineInet = 9+1
         call setupconnectionclient(port,
     *                              '127.0.0.1'//char(0),
     *                              sizeMachineInet,
     *                              socketID)
         if (socketID .le. 0) then
            write(*,*) 'ERROR - failed to setup connection'
            call xplb_exit
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
      
c     zero rhs vector
      do kblock = 1, nblock
         do i = 1, ndofel
            rhs(kblock,i) = 0.0
         enddo
      enddo
      
c     dynamic analysis
      if (lflags(iProcedure) .eq. jDynExplicit) then
         
c        mass matrix
         if (lflags(iOpCode) .eq. jMassCalc) then
            do kblock = 1, nblock
               do i = 1, ndofel
                  amass(kblock,i,i) = 1E-12
               enddo
            enddo
         
c        resisting force
         else if (lflags(iOpCode) .eq. jIntForceAndDtStable
     *            .and. time(iTotalTime) .gt. 0.0) then
            do kblock = 1, nblock
c              stable time increment in the element
               dtimeStable(kblock) = 1.0
               
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
                  sData(1+i) = u(kblock,i)
                  sData(1+ndofel+i) = v(kblock,i)
                  sData(1+2*ndofel+i) = a(kblock,i)
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
                  rhs(kblock,i) = rData(i)
               enddo
            enddo
         endif
      endif
      
      return
      end
