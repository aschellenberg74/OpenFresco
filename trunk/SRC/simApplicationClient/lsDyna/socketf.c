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

/* ****************************************************************** **
** Fortran interface                                                  **
**                                                                    **
** $Revision: $                                                       **
** $Date: $                                                           **
** $Source: $                                                         **
**                                                                    **
** Written: Yuli Huang (yulee@berkeley.edu)                           **
** Created: 11/06                                                     **
**                                                                    **
** ****************************************************************** */


#define G77

#ifdef _WIN32
#  ifndef G77
#    define F77_NAME_UPPER
#  else
     /* use lower case function name*/
#    define F77_NAME_LOWER_USCORE
     /* use upper case function name*/
     /* #define F77_NAME_LOWER_2USCORE */
#  endif
#else
/* use lower case function name*/
#define F77_NAME_LOWER_USCORE
/* use upper case function name*/
/* #define F77_NAME_LOWER_2USCORE */
#endif

/* Support Windows extension to specify calling convention */
#ifdef USE_FORT_CDECL
#define FORT_CALL __cdecl
#elif defined (USE_FORT_STDCALL)
#define FORT_CALL __stdcall
#else
#define FORT_CALL
#endif

#ifdef F77_NAME_UPPER
#define senddata_ SENDDATA
#define recvdata_ RECVDATA
#define establishconnection_ ESTABLISHCONNECTION
#define closeconnection_ CLOSECONNECTION
#elif defined(F77_NAME_LOWER_2USCORE)
#define senddata_ senddata__
#define recvdata_ recvdata__
#define establishconnection_ establishconnection__
#define closeconnection_ closeconnection__
#elif !defined(F77_NAME_LOWER_USCORE)
#define senddata_ senddata
#define recvdata_ recvdata
#define establishconnection_ establishconnection
#define closeconnection_ closeconnection
/* Else leave name alone */
#endif

int sendData(int *socketID, int *dataTypeSize, char data[], int *lenData);
int recvData(int *socketID, int *dataTypeSize, char data[], int *lenData);
int establishConnection(unsigned int *port, const char machineInetAddr[], int *lengthInet);
int closeConnection(int *socketID);

#ifdef  __cplusplus
extern "C" {
#endif

/* Prototypes for the Fortran interfaces */
void FORT_CALL senddata_ (int *socketID, int *dataTypeSize, char data[], int *lenData, int *ierr) {
    *ierr = sendData(socketID, dataTypeSize, data, lenData);
}

void FORT_CALL recvdata_ (int *socketID, int *dataTypeSize, char data[], int *lenData, int *ierr) {
    *ierr = recvData(socketID, dataTypeSize, data, lenData);
}

void FORT_CALL establishconnection_ (unsigned int *port, const char machineInetAddr[], int *lengthInet, int *socketID) {
    *socketID = establishConnection(port, machineInetAddr, lengthInet);
}

void FORT_CALL closeconnection_ (int *socketID, int *ierr) {
    *ierr = closeConnection(socketID);
}

#ifdef  __cplusplus
}
#endif
