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
**  LS-DYNA (R) is a registered trademark of                          **
**  Livermore Software Technology Corporation in the United States.   **
**                                                                    **
** ****************************************************************** **

** $Revision: $
** $Date: $
** $Source: $

** Written: Yuli Huang (yulee@berkeley.edu)
** Created: 11/06
** Revision: A

** Description: This file contains the implementation of e101.
** e101 is a LS-DYNA (R) client element. The element communicates with
** the OpenFresco middle-tier server trough a tcp/ip connection.
c
      subroutine ushl_e101(force,stiff,ndtot,istif,
     .     x1,x2,x3,x4,y1,y2,y3,y4,z1,z2,z3,z4,
     .     fx1,fx2,fx3,fx4,
     .     fy1,fy2,fy3,fy4,
     .     fz1,fz2,fz3,fz4,
     .     xdof,
     .     dx1,dx2,dx3,dx4,dy1,dy2,dy3,dy4,dz1,dz2,dz3,dz4,
     .     wx1,wx2,wx3,wx4,wy1,wy2,wy3,wy4,wz1,wz2,wz3,wz4,
     .     dxdof,
     .     thick,thck1,thck2,thck3,thck4,
     .     hsv,nhsv,
     .     cm,lmc,
     .     gl11,gl21,gl31,gl12,gl22,gl32,gl13,gl23,gl33,
     .     cmtrx,lft,llt)

      include 'nlqparm'
      dimension force(nlq,ndtot),stiff(nlq,ndtot,ndtot)
      dimension x1(nlq),x2(nlq),x3(nlq),x4(nlq)
      dimension y1(nlq),y2(nlq),y3(nlq),y4(nlq)
      dimension z1(nlq),z2(nlq),z3(nlq),z4(nlq)
      dimension fx1(nlq),fx2(nlq),fx3(nlq),fx4(nlq)
      dimension fy1(nlq),fy2(nlq),fy3(nlq),fy4(nlq)
      dimension fz1(nlq),fz2(nlq),fz3(nlq),fz4(nlq)
      dimension xdof(nlq,8,3)
      dimension dx1(nlq),dx2(nlq),dx3(nlq),dx4(nlq)
      dimension dy1(nlq),dy2(nlq),dy3(nlq),dy4(nlq)
      dimension dz1(nlq),dz2(nlq),dz3(nlq),dz4(nlq)
      dimension wx1(nlq),wx2(nlq),wx3(nlq),wx4(nlq)
      dimension wy1(nlq),wy2(nlq),wy3(nlq),wy4(nlq)
      dimension wz1(nlq),wz2(nlq),wz3(nlq),wz4(nlq)
      dimension dxdof(nlq,8,3)
      dimension thick(nlq),thck1(nlq),thck2(nlq),thck3(nlq),thck4(nlq)
      dimension hsv(nlq,nhsv),cm(lmc)
      dimension gl11(nlq),gl21(nlq),gl31(nlq),
     .     gl12(nlq),gl22(nlq),gl32(nlq),
     .     gl13(nlq),gl23(nlq),gl33(nlq)
      dimension cmtrx(nlq,15,3)
      dimension x(2),y(2),z(2),dx(2),dy(2),dz(2),cs(3)

      integer*4 sizeSendData
      parameter (sizeSendData=256)

      integer*4 sizeInt, sizeDouble
      parameter (sizeInt=4, sizeDouble=8)

      integer*4 port
      integer*4 nleft, dataTypeSize
      integer*4 sizeMachineInet;
      integer*4 socketID
      integer*4 stat

      integer*4 iData(11)
      real*8    sData(sizeSendData)
      real*8    r(4)

      do i=lft,llt
         if (nint(hsv(i,1)) .eq. 0) then
            port=nint(cm(1))
            sizeMachineInet = 9+1
            call setupconnectionclient(port,
     1                                 '127.0.0.1'//char(0),
     2                                 sizeMachineInet,
     3                                 socketID)
            if (socketID .le. 0) then
               write(*,*) 'Warning: cannot connect'
            else
               hsv(i,1)=1
               hsv(i,2)=socketID
c
c ...          set the data size for the experimental site
c
c ...          sizeCtrl
c              disp
               iData(1)  = 4
c              vel
               iData(2)  = 0
c              accel
               iData(3)  = 0
c              force
               iData(4)  = 0
c              time
               iData(5)  = 0
c ...          sizeDaq
c              disp
               iData(6)  = 4
c              vel
               iData(7)  = 0
c              accel
               iData(8)  = 0
c              force
               iData(9)  = 4
c              time
               iData(10) = 0
c ...          dataSize
               iData(11) = sizeSendData
c
               dataTypeSize = sizeInt
               nleft        = 11
               call senddata(socketID, dataTypeSize, iData, nleft, stat)
            endif
         else
            socketID = nint(hsv(i,2))
         endif
c
         hsv(i,3)=hsv(i,3)+dx1(i)
         hsv(i,4)=hsv(i,4)+dy1(i)
         hsv(i,5)=hsv(i,5)+dx2(i)
         hsv(i,6)=hsv(i,6)+dy2(i)
c
c ...    send trial response to experimental site
c
         sData(1) = 3
         do 5, j = 1,4
           sData(1+j) = hsv(i,2+j)
    5    continue
c
         dataTypeSize = sizeDouble
         nleft        = sizeSendData
         call senddata(socketID, dataTypeSize, sData, nleft, stat)
c
c ...   get measured resisting forces
c
         sData(1) = 10
         dataTypeSize = sizeDouble
         nleft        = sizeSendData
         call senddata(socketID, dataTypeSize, sData, nleft, stat)

         dataTypeSize = sizeDouble
         nleft        = 4
         call recvdata(socketID, dataTypeSize, r, nleft, stat)
c
         do j=1,2
            force(i,j)   = r(j)
            force(i,j+6) = r(j+2)
         enddo
      enddo
      return 
      end