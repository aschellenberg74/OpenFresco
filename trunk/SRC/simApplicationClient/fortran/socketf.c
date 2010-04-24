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

/* $Revision: $
/* $Date: $
/* $Source: $

/* Written: Yuli Huang (yulee@berkeley.edu)
/* Created: 11/06
/* Revision: A

/* Description: This file contains the Fortran interface for the socket.*/


/* Adjust calling conventions */
#ifdef USE_FORT_CDECL
#define FORT_CALL __cdecl
#elif defined (USE_FORT_STDCALL)
#define FORT_CALL __stdcall
#else
#define FORT_CALL
#endif

/* Adjust naming conventions */
#ifdef F77_NAME_UPPER
#define setupconnectionserver_ SETUPCONNECTIONSERVER
#define setupconnectionclient_ SETUPCONNECTIONCLIENT
#define closeconnection_ CLOSECONNECTION
#define senddata_ SENDDATA
#define sendnbdata_ SENDNBDATA
#define recvdata_ RECVDATA
#define recvnbdata_ RECVNBDATA
#define getsocketid_ GETSOCKETID
#elif defined(F77_NAME_LOWER_2USCORE)
#define setupconnectionserver_ setupconnectionserver__
#define setupconnectionclient_ setupconnectionclient__
#define closeconnection_ closeconnection__
#define senddata_ senddata__
#define sendnbdata_ sendnbdata__
#define recvdata_ recvdata__
#define recvnbdata_ recvnbdata__
#define getsocketid_ getsocketid__
#elif !defined(F77_NAME_LOWER_USCORE)
#define setupconnectionserver_ setupconnectionserver
#define setupconnectionclient_ setupconnectionclient
#define closeconnection_ closeconnection
#define senddata_ senddata
#define sendnbdata_ sendnbdata
#define recvdata_ recvdata
#define recvnbdata_ recvnbdata
#define getsocketid_ getsocketid
/* F77_NAME_LOWER_USCORE */
/* Else leave name alone */
#endif

/* Adjust string and hidden length conventions */
#ifdef MIXED_STRING_LEN
#define CHNAME(id) const char id ## [], const int id ## _len
#define CHLEN(id)
#else
#define CHNAME(id) const char id ## []
#define CHLEN(id) , const int id ## _len
#endif

/* Functions defined in socket.c */
void setupconnectionserver(unsigned int *port, int *socketID);
void setupconnectionclient(unsigned int *other_Port, const char other_InetAddr[], int *lengthInet, int *socketID);
void closeconnection(int *socketID, int *ierr);
void senddata(int *socketID, int *dataTypeSize, char data[], int *lenData, int *ierr);
void sendnbdata(int *socketID, int *dataTypeSize, char data[], int *lenData, int *ierr);
void recvdata(int *socketID, int *dataTypeSize, char data[], int *lenData, int *ierr);
void recvnbdata(int *socketID, int *dataTypeSize, char data[], int *lenData, int *ierr);
void getsocketid(unsigned int *port, const char machineInetAddr[], int *lengthInet, int *socketID);

#ifdef  __cplusplus
extern "C" {
#endif

/* Prototypes for the Fortran interfaces */
void FORT_CALL setupconnectionserver_ (unsigned int *port, int *socketID) {
    setupconnectionserver(port, socketID);
}

void FORT_CALL setupconnectionclient_ (unsigned int *other_Port, CHNAME(other_InetAddr), int *lengthInet, int *socketID CHLEN(other_InetAddr)) {
    setupconnectionclient (other_Port, other_InetAddr, lengthInet, socketID);
}

void FORT_CALL closeconnection_ (int *socketID, int *ierr) {
    closeconnection(socketID, ierr);
}

void FORT_CALL senddata_ (int *socketID, int *dataTypeSize, char data[], int *lenData, int *ierr) {
    senddata(socketID, dataTypeSize, data, lenData, ierr);
}

void FORT_CALL sendnbdata_ (int *socketID, int *dataTypeSize, char data[], int *lenData, int *ierr) {
    sendnbdata(socketID, dataTypeSize, data, lenData, ierr);
}

void FORT_CALL recvdata_ (int *socketID, int *dataTypeSize, char data[], int *lenData, int *ierr) {
    recvdata(socketID, dataTypeSize, data, lenData, ierr);
}

void FORT_CALL recvnbdata_ (int *socketID, int *dataTypeSize, char data[], int *lenData, int *ierr) {
    recvnbdata(socketID, dataTypeSize, data, lenData, ierr);
}

void FORT_CALL getsocketid_ (unsigned int *port, CHNAME(machineInetAddr), int *lengthInet, int *socketID CHLEN(machineInetAddr)) {
	  getsocketid(port, machineInetAddr, lengthInet, socketID);
}

#ifdef  __cplusplus
}
#endif
