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

** $Revision: $
** $Date: $
** $Source: $

** Written: Yuli Huang (yulee@berkeley.edu)
** Created: 11/06
** Revision: A

** Description: This file contains the class definition for genericClient.
** genericClient is a generic element defined by any number of nodes and 
** the degrees of freedom at those nodes. The element communicates with 
** an OpenFresco element trough a tcp/ip connection.
c
      subroutine genericClient(d,ul,uldot,uldotdot,xl,ix,tl,s,r,
     1                         ndf,ndm,nst,isw,ierr)

      implicit none

      integer*4 sizeSendData
      parameter (sizeSendData=256)

      integer*4 sizeInt, sizeDouble
      parameter (sizeInt=4, sizeDouble=8)
      
      integer*4 socketID
      common    //socketID

      integer*4 ix(*),ndf,ndm,nst,isw,ierr
      real*8    d(*),ul(*),uldot(*),uldotdot(*),xl(*),tl,s(*),r(*)

      integer*4 i,port

      integer*4 iData(11)
      real*8    sData(sizeSendData)

      integer*4 nleft, dataTypeSize
      integer*4 sizeMachineInet;

      integer*4 stat


      if     (isw .eq. 0) then
c-----------------------------------------------------------------------
c       output element type
c-----------------------------------------------------------------------
        write(*,*) "genericClient numNodes:", d(1), "port:", d(2)
c
      elseif (isw .eq. 1) then
c-----------------------------------------------------------------------
c       check for valid input args
c-----------------------------------------------------------------------
        if (ndf .lt. 1 .or. ndf .gt. 6) then
          ierr = -1;
          go to 55
        endif
c
        if (ndm .lt. 1 .or. ndm .gt. 3) then
          ierr = -1;
          go to 55
        endif
c
        if (nst .ne. d(1)*ndf) then
          ierr = -1;
          go to 55
        endif
c
        if (d(1) .eq. 0.d0) then
          ierr = -1;
          go to 55
        endif
c
c ...   setup the connection
c
        port = nint(d(2))
        sizeMachineInet = 9+1
        call setupconnectionclient(port,
     1                             '127.0.0.1'//char(0),
     2                             sizeMachineInet,
     3                             socketID)
        if (socketID .le. 0) then
          ierr = -1;
          go to 55
        endif
c
c ...   set the data size for the experimental site
c
c ...   sizeCtrl
c       disp
        iData(1)  = nst
c       vel
        iData(2)  = nst
c       accel
        iData(3)  = nst
c       force
        iData(4)  = 0
c       time
        iData(5)  = 0
c ...   sizeCtrl
c       disp
        iData(6)  = 0
c       vel
        iData(7)  = 0
c       accel
        iData(8)  = 0
c       force
        iData(9)  = nst
c       time
        iData(10) = 0
c ...   dataSize
        iData(11) = sizeSendData
c
        dataTypeSize = sizeInt
        nleft        = 11
        call senddata(socketID, dataTypeSize, iData, nleft, stat)
c
      elseif (isw .eq. 2) then
c-----------------------------------------------------------------------
c       check element for errors
c-----------------------------------------------------------------------
        ierr = 0;
        go to 55
c
      elseif (isw .eq. 3 .or. isw .eq. 4 .or.isw .eq. 6) then
c-----------------------------------------------------------------------
c       compute element residual and tangent matrix
c-----------------------------------------------------------------------
c
c ...   send trial response to experimental site
c
        sData(1) = 3
        do 5, i = 1,nst
          sData(1      +i) = ul(i)
          sData(1+  nst+i) = uldot(i)
          sData(1+2*nst+i) = uldotdot(i)
    5   continue
c
        dataTypeSize = sizeDouble
        nleft        = sizeSendData
        call senddata(socketID, dataTypeSize, sData, nleft, stat)
c
        if (isw .eq. 3) then
c
c ...     add stiffness portion to matrix
c
          sData(1) = 13
c
          dataTypeSize = sizeDouble
          nleft        = sizeSendData
          call senddata(socketID, dataTypeSize, sData, nleft, stat)
c
          nleft        = nst*nst
          call recvdata(socketID, dataTypeSize, s, nleft, stat)
        endif
c
c ...   get measured resisting forces
c
        sData(1) = 10
        dataTypeSize = sizeDouble
        nleft        = sizeSendData
        call sendData(socketID, dataTypeSize, sData, nleft, stat)

        dataTypeSize = sizeDouble
        nleft        = nst
        call recvData(socketID, dataTypeSize, r, nleft, stat)
c
        if (isw .eq. 6) write(*,*) r(1), r(2), r(3), r(4)
c
      elseif (isw .eq. 5) then
c-----------------------------------------------------------------------
c       compute element mass matrix
c-----------------------------------------------------------------------
        sData(1) = 15
c
        dataTypeSize = sizeDouble
        nleft        = sizeSendData
        call senddata(socketID, dataTypeSize, sData, nleft, stat)
c
        nleft        = nst*nst
        call senddata(socketID, dataTypeSize, s, nleft, stat)
      elseif (isw .eq. 7) then
c-----------------------------------------------------------------------
c       output surface loading
c-----------------------------------------------------------------------
c
      elseif (isw .eq. 8) then
c-----------------------------------------------------------------------
c       compute stress projections at nodes
c-----------------------------------------------------------------------
c
      elseif (isw .eq. 10) then
c-----------------------------------------------------------------------
c       disconnect from experimental site
c-----------------------------------------------------------------------
        sData(1) = 99
        dataTypeSize = sizeDouble
        nleft        = sizeSendData
        call senddata(socketID, dataTypeSize, sData, nleft, stat)
c
        call closeconnection(socketID, stat)
      endif
c
      ierr=0
c
   55 end
c
c
      program gElem
c
      integer*4 ndm, ndf, nst
      parameter (ndm=2, ndf=2, nst=4)
c
      integer*4 i, j
      real*8    d(2), disp(4), vel(4), accel(4)
      real*8    s(4,4), r(4)
c
      integer*4 nodeNum(2)
      real*8    coord(4)
      real*8    temp(2)
c
      integer*4 stat
c
      d(1)       = 2
      d(2)       = 8090
      nodeNum(1) = 3
      nodeNum(2) = 4
      coord(1)   = 168
      coord(2)   = 0
      coord(3)   = 72
      coord(4)   = 96
      temp(1)    = 0
      temp(2)    = 0
c
c ... setup
c
      call genericClient(d,disp,vel,accel,coord,nodeNum,temp,s,r,
     1                   ndf,ndm,nst,1,stat)
c
c ... disp, stiff, force
c
      write(*,*) "DISP:"
c
      disp(1) = 0.d0
      disp(2) = 0.d0
      disp(3) = 1.06017d0
      disp(4) = -0.355778d0
c
      do 70, i = 1,4
        vel(i)   = 0.d0
        accel(i) = 0.d0
        do 60, j = 1,4
          s(i,j) = 0.d0
   60   continue
        r(i)     = 0.d0
        write(*,*) disp(i), vel(i), accel(i)
   70 continue
c
      call genericClient(d,disp,vel,accel,coord,nodeNum,temp,s,r,
     1                   ndf,ndm,nst,3,stat)
      do 80, i = 1,4
        write(*,*) disp(i), vel(i), accel(i)
   80 enddo
c
      write(*,*) "FORCE:"
      do 90, i = 1,4
        write(*,*) r(i)
   90 continue
c
c ... shutdown
c
      call genericClient(d,disp,vel,accel,coord,nodeNum,temp,s,r,
     1                   ndf,ndm,nst,10,stat)
c
      stop
      end
