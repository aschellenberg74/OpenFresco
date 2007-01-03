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

// $Revision: $
// $Date: $
// $Source: $

// Written: Frank McKenna (fmckenna@ce.berkeley.edu)
// Created: 10/06
// Revision: A
//
// Description: This file contains the implementation of the EETruss.

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#ifdef _WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <strings.h>
#endif

#define MAX_UDP_DATAGRAM 9126
#define MAX_INET_ADDR 28

#ifdef _WIN32
typedef SOCKET socket_type;
typedef int socklen_type;
#define bzero(s,n) memset((s),0,(n))
#define bcmp(s1,s2,n) memcmp((s1),(s2),(n))
#else
typedef int socket_type;
typedef socklen_t socklen_type;
#endif

#define CALL_CONV 

typedef struct socketConnection {
    unsigned int port;
    char *machineInetAddr;
    int socketID;
    socket_type sockfd;
    struct socketConnection *next;
} SocketConnection;

static SocketConnection *theSockets = NULL;
static int numSockets = 0;
static int socketIDs  = 0;


void CALL_CONV startupsockets(int *ierr)
{
#ifdef _WIN32
    if (numSockets == 0) {
        WSADATA wsaData;
        *ierr = WSAStartup(0x0002, &wsaData);
    } else
        *ierr = 0;
#else
    *ierr = 0;
#endif
} 


void CALL_CONV cleanupsockets()
{
    numSockets--;
#ifdef _WIN32
    if (numSockets == 0)
        WSACleanup();
#endif
}


/*
* getSocketID() - function to find an open connection
*
* input: unsigned int port - the port number
*        char *machineInetAddr - the machine inet address
*        int lengthInet - length of machine inet address
*
* return: int socketID, a negative number if failed to establish connection
*/
void CALL_CONV getsocketid(unsigned int *port, const char machineInetAddr[], int *lengthInet, int *socketID)
{
    SocketConnection *theSocket = theSockets;

    // check inputs
    if (machineInetAddr == 0) {
        *socketID = -1;
        return;
    }
    // search for open socket with sockID
    while (theSocket != 0) {
        if (theSocket->port == *port) {
            if (strcmp(theSocket->machineInetAddr, machineInetAddr) == 0) {
                *socketID = theSocket->socketID;
                return;
            }
        }
        theSocket = theSocket->next;
    }

    *socketID = -1;
}


/*
* sendData() - function to send data
*
* input: int socketID - socket identifier
*        int dataTypeSize - size of data type
*        char *data - pointer to data to send
*        int lenData - length of data
*        
* return: 0 if successfull, negative number if not
*/
void CALL_CONV senddata(int *socketID, int *dataTypeSize, char data[], int *lenData, int *ierr)
{
    int nwrite, nleft;    
    char *gMsg = data;
    SocketConnection *theSocket = theSockets;  

    // find the socket
    while (theSocket->socketID != *socketID) 
        theSocket = theSocket->next;
    if (theSocket == 0) {
        *ierr = -1;
        return;
    }

    // send the data
    // if o.k. get a pointer to the data in the message and 
    // place the incoming data there
    nleft = *lenData * *dataTypeSize;

    while (nleft > 0) {
        nwrite = send(theSocket->sockfd, gMsg, nleft, 0);
        nleft -= nwrite;
        gMsg +=  nwrite;
    }

    *ierr = 0;
}


/*
* recvData() - function to receive data
*
* input: int socketID - socket identifier
*        int dataTypeSize - size of data type
*        char *data - pointer to data to receive
*        int lenData - length of data
*        
* return: 0 if successfull, negative number if not
*/
void CALL_CONV recvdata(int *socketID, int *dataTypeSize, char data[], int *lenData, int *ierr)
{
    int nread, nleft;    
    char *gMsg = data;
    SocketConnection *theSocket = theSockets;  

    // find the socket
    while (theSocket->socketID != *socketID) 
        theSocket=theSocket->next;
    if (theSocket == 0) {
        *ierr = -1;
        return;
    }

    // receive the data
    // if o.k. get a pointer to the data in the message and 
    // place the incoming data there
    nleft = *lenData * *dataTypeSize;

    while (nleft > 0) {
        nread = recv(theSocket->sockfd, gMsg, nleft, 0);
        nleft -= nread;
        gMsg +=  nread;
    }

    *ierr = 0;
}


/*
* establishConnection() - function to create a connection
*
* input: unsigned int port - the port number
*        char *machineInetAddr - the machine inet address
*        int lengthInet - length of machine inet address
*
* return: int socketID, a negative number if failed to establish connection
*/
void CALL_CONV establishconnection(unsigned int *port, const char machineInetAddr[], int *lengthInet, int *socketID)
{
    union {
        struct sockaddr    addr;
        struct sockaddr_in addr_in;
    } my_Addr;
    union {
        struct sockaddr    addr;
        struct sockaddr_in addr_in;
    } other_Addr;

    socket_type sockfd;
    socklen_type addrLength;

    int ierr;
    unsigned int myPort;
    SocketConnection *theSocket = theSockets;

    // check inputs
    if (machineInetAddr == 0) {
        fprintf(stderr,"socket.c - missing machineInetAddr\n");
        *socketID = -1;
        return;
    }

    startupsockets(&ierr);
    if (ierr != 0) {
        fprintf(stderr,"socket.c - could not startup socket\n");
        *socketID = -1;
        return;
    }

    while (theSocket != 0) {
        if (theSocket->port == *port) {
            if (strcmp(theSocket->machineInetAddr, machineInetAddr) == 0) {
                *socketID = theSocket->socketID;
                return;
            }
        }
        theSocket = theSocket->next;
    }

    /*   
    * connect to remote socket
    */

    // set up remote address
    bzero((char *) &other_Addr.addr_in, sizeof(other_Addr.addr_in));
    other_Addr.addr_in.sin_family = AF_INET;
    other_Addr.addr_in.sin_port = htons(*port);

#ifdef _WIN32
    other_Addr.addr_in.sin_addr.S_un.S_addr = inet_addr(machineInetAddr);
#else
    other_Addr.addr_in.sin_addr.s_addr = inet_addr(machineInetAddr);
#endif

    // set up my_Addr.addr_in
    bzero((char *) &my_Addr.addr_in, sizeof(my_Addr.addr_in));    
    my_Addr.addr_in.sin_family = AF_INET;
    my_Addr.addr_in.sin_port = htons(0);

#ifdef _WIN32
    my_Addr.addr_in.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
#else
    my_Addr.addr_in.sin_addr.s_addr = htonl(INADDR_ANY);
#endif

    addrLength = sizeof(my_Addr.addr_in);

    // open a socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr,"socket.c - could not open socket\n");
        *socketID = -2;
        return;
    }

    // bind local address to it
    if (bind(sockfd, (struct sockaddr *) &my_Addr.addr_in,sizeof(my_Addr.addr_in)) < 0) {
        fprintf(stderr,"socket.c - could not bind local address\n");
        *socketID = -3;
        return;
    }
    myPort = ntohs(my_Addr.addr_in.sin_port);    


    // now try to connect to socket with remote address
    if (connect(sockfd, (struct sockaddr *) &other_Addr.addr_in, 
        sizeof(other_Addr.addr_in))< 0) {
            fprintf(stderr,"socket.c - could not connect\n");
            *socketID = -4;
            return;
    }

    // get my_address info
    getsockname(sockfd, &my_Addr.addr, &addrLength);

    // add a new socket connection
    theSocket = (SocketConnection *)malloc(sizeof(SocketConnection));

    numSockets++;
    socketIDs++;
    theSocket->port = *port;
    theSocket->machineInetAddr = (char *)malloc((strlen(machineInetAddr)+1)*sizeof(char));
    strcpy(theSocket->machineInetAddr, machineInetAddr);
    theSocket->socketID = socketIDs;
    theSocket->sockfd = sockfd;
    theSocket->next = theSockets;
    theSockets = theSocket;

    // set up return values & return
    *socketID = theSocket->socketID;
}


/*
* closeConnection() - function to terminate a connection
*
* input: int socketID - socket identifier
*
* return: 0 if successfull, negative number if not
*/
void CALL_CONV closeconnection(int *socketID, int *ierr)
{
    SocketConnection *theSocket = theSockets;

    while (theSocket->socketID != *socketID) 
        theSocket = theSocket->next;
    if (theSocket == 0) {
        *ierr = -1;
        return;
    }

#ifdef _WIN32
    closesocket(theSocket->sockfd);
#else
    close(theSocket->sockfd);
#endif

    numSockets--;
    if (numSockets == 0)
        cleanupsockets();

    *ierr = 0;
}
