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

// $Revision: 314 $
// $Date: 2011-05-22 14:17:07 -0700 (Sun, 22 May 2011) $
// $Source: $

// Written: Andreas Schellenberg & Frank McKenna
// Created: 02/13
// Revision: A
//
// Description: This file contains the implementation of the udp_socket.

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#ifdef _WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
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
static int numSockets = 0;
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
    struct sockaddr otherAddr;
    struct socketConnection *next;
} SocketConnection;

static SocketConnection *theSockets = NULL;
static int socketIDs = 0;


void CALL_CONV startupsockets(int *ierr)
{
#ifdef _WIN32
    WSADATA wsaData;
    if (numSockets == 0)
        *ierr = WSAStartup(0x0002, &wsaData);
    else
        *ierr = 0;
    numSockets++;
#else
    *ierr = 0;
#endif
} 


void CALL_CONV cleanupsockets()
{
#ifdef _WIN32
    numSockets--;
    if (numSockets == 0)
        WSACleanup();
#endif
}


/*
* setupconnectionserver() - function to setup a connection from server
*
* input: unsigned int *port - the port number
*
* return: int *socketID - negative number if failed to setup connection
*/
void CALL_CONV setupconnectionserver(unsigned int *port, int *socketID)
{
    union {
        struct sockaddr    addr;
        struct sockaddr_in addr_in;
    } my_Addr;
    union {
        struct sockaddr    addr;
        struct sockaddr_in addr_in;
    } other_Addr;
    
    SocketConnection *theSocket = theSockets;
    socket_type sockfd;
    socklen_type addrLength;
    unsigned int other_Port;
    char *other_InetAddr;
    char data;
    int ierr;
    
    // initialize sockets
    startupsockets(&ierr);
    if (ierr != 0) {
        fprintf(stderr,"udp_socket::setupconnectionserver() - could not startup server socket\n");
        *socketID = -1;
        return;
    }
    
    // set up my_Addr.addr_in with address given by port and internet address of
    // machine on which the process that uses this routine is running.
    
    // set up my_Addr
    bzero((char *) &my_Addr, sizeof(my_Addr));
    my_Addr.addr_in.sin_family = AF_INET;
    my_Addr.addr_in.sin_port = htons(*port);
    
#ifdef _WIN32
    my_Addr.addr_in.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
#else
    my_Addr.addr_in.sin_addr.s_addr = htonl(INADDR_ANY);
#endif
    
    // open a socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        fprintf(stderr,"udp_socket::setupconnectionserver() - could not open server socket\n");
        *socketID = -2;
        return;
    }
    
    // bind local address to it
    if (bind(sockfd, &my_Addr.addr, sizeof(my_Addr.addr)) < 0) {
        fprintf(stderr,"udp_socket::setupconnectionserver() - could not bind local address\n");
        *socketID = -3;
        return;
    }
    
    // wait for remote process to send message
    // the remote address and its length is also received and saved
    ierr = recvfrom(sockfd, &data, 1, 0, &other_Addr.addr, &addrLength);    
    
    // then send a message back
    data = 'b';
    ierr = sendto(sockfd, &data, 1, 0, &other_Addr.addr, addrLength);        
    
    // get other_Port and other_InetAddr
    other_Port = ntohs(other_Addr.addr_in.sin_port);
    other_InetAddr = inet_ntoa(other_Addr.addr_in.sin_addr);
    
    // add a new socket connection
    theSocket = (SocketConnection *)malloc(sizeof(SocketConnection));
    
    socketIDs++;
    theSocket->port = other_Port;
    theSocket->machineInetAddr = (char *)malloc((strlen(other_InetAddr)+1)*sizeof(char));
    strcpy(theSocket->machineInetAddr, other_InetAddr);
    theSocket->socketID = socketIDs;
    theSocket->sockfd = sockfd;
    theSocket->otherAddr = other_Addr.addr;
    theSocket->next = theSockets;
    theSockets = theSocket;
    
    // set up return values & return
    *socketID = theSocket->socketID;
}


/*
* setupconnectionclient() - function to setup a connection from client
*
* input: unsigned int *other_Port - the port number
*        const char *other_InetAddr - the machine inet address
*        int *lengthInet - length of machine inet address
*
* return: int *socketID - negative number if failed to setup connection
*/
void CALL_CONV setupconnectionclient(unsigned int *other_Port,
    const char other_InetAddr[], int *lengthInet, int *socketID)
{
    union {
        struct sockaddr    addr;
        struct sockaddr_in addr_in;
    } my_Addr;
    union {
        struct sockaddr    addr;
        struct sockaddr_in addr_in;
    } other_Addr;
    
    SocketConnection *theSocket = theSockets;
    socket_type sockfd;
    socklen_type addrLength;
    char data;    
    int ierr;
    
    // check inputs
    if (other_InetAddr == 0) {
        fprintf(stderr,"udp_socket::setupconnectionclient() - missing other_InetAddr\n");
        *socketID = -1;
        return;
    }
    
    // initialize sockets
    startupsockets(&ierr);
    if (ierr != 0) {
        fprintf(stderr,"udp_socket::setupconnectionclient() - could not startup client socket\n");
        *socketID = -2;
        return;
    }
    
    while (theSocket != 0) {
        if (theSocket->port == *other_Port) {
            if (strcmp(theSocket->machineInetAddr, other_InetAddr) == 0) {
                *socketID = theSocket->socketID;
                return;
            }
        }
        theSocket = theSocket->next;
    }
    
    // set up other_Addr.addr_in with address given by port and internet
    // address of remote machine to which the connection is made.
    
    // set up remote address
    bzero((char *) &other_Addr, sizeof(other_Addr));
    other_Addr.addr_in.sin_family = AF_INET;
    other_Addr.addr_in.sin_port = htons(*other_Port);
    
#ifdef _WIN32
    other_Addr.addr_in.sin_addr.S_un.S_addr = inet_addr(other_InetAddr);
#else
    other_Addr.addr_in.sin_addr.s_addr = inet_addr(other_InetAddr);
#endif
    
    // set up my_Addr.addr_in
    bzero((char *) &my_Addr, sizeof(my_Addr));
    my_Addr.addr_in.sin_family = AF_INET;
    my_Addr.addr_in.sin_port = htons(0);
    
#ifdef _WIN32
    my_Addr.addr_in.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
#else
    my_Addr.addr_in.sin_addr.s_addr = htonl(INADDR_ANY);
#endif
    
    // open a socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        fprintf(stderr,"udp_socket::setupconnectionclient() - could not open client socket\n");
        *socketID = -3;
        return;
    }
    
    // bind local address to it
    if (bind(sockfd, &my_Addr.addr, sizeof(my_Addr.addr)) < 0) {
        fprintf(stderr,"udp_socket::setupconnectionclient() - could not bind local address\n");
        *socketID = -4;
        return;
    }
    
    // send a message to address
    data = 'a';
    addrLength = sizeof(other_Addr.addr);
    ierr = sendto(sockfd, &data, 1, 0, &other_Addr.addr, addrLength);
    
    // receive a message from other
    ierr = recvfrom(sockfd, &data, 1, 0, &other_Addr.addr, &addrLength);
    
    // add a new socket connection
    theSocket = (SocketConnection *)malloc(sizeof(SocketConnection));
    
    socketIDs++;
    theSocket->port = *other_Port;
    theSocket->machineInetAddr = (char *)malloc((strlen(other_InetAddr)+1)*sizeof(char));
    strcpy(theSocket->machineInetAddr, other_InetAddr);
    theSocket->socketID = socketIDs;
    theSocket->sockfd = sockfd;
    theSocket->otherAddr = other_Addr.addr;
    theSocket->next = theSockets;
    theSockets = theSocket;
    
    // set up return values & return
    *socketID = theSocket->socketID;
}


/*
* closeconnection() - function to terminate a connection
*
* input: int *socketID - socket identifier
*
* return: int *ierr - 0 if successfull, negative number if not
*/
void CALL_CONV closeconnection(int *socketID, int *ierr)
{
    SocketConnection *theSocket = theSockets;
    *ierr = 0;
    
    // find the socket
    while (theSocket->socketID != *socketID) 
        theSocket = theSocket->next;
    if (theSocket == 0) {
        fprintf(stderr,"udp_socket::closeconnection() - could not find socket to close\n");
        *ierr = -1;
        return;
    }
    
#ifdef _WIN32
    closesocket(theSocket->sockfd);
#else
    close(theSocket->sockfd);
#endif
    
    // cleanup sockets
    cleanupsockets();    
}


/*
* senddata() - function to send data in blocking mode
*
* input: int *socketID - socket identifier
*        int *dataTypeSize - size of data type
*        char *data - pointer to data to send
*        int *lenData - length of data
*        
* return: int *ierr - 0 if successfull, negative number if not
*/
void CALL_CONV senddata(int *socketID, int *dataTypeSize, char data[], int *lenData, int *ierr)
{
    SocketConnection *theSocket = theSockets;
    socket_type sockfd;
    struct sockaddr otherAddr;
    socklen_type addrLength;
    int size;
    unsigned long nbMode = 0;
    char *gMsg = data;
    *ierr = 0;
    
    // find the socket
    while (theSocket != 0 && theSocket->socketID != *socketID)
        theSocket = theSocket->next;
    if (theSocket == 0) {
        fprintf(stderr,"udp_socket::senddata() - could not find socket to send data\n");
        *ierr = -1;
        return;
    }
    sockfd = theSocket->sockfd;
    otherAddr = theSocket->otherAddr;
    addrLength = sizeof(otherAddr);
    
    // turn on blocking mode
#ifdef _WIN32
    if (ioctlsocket(sockfd, FIONBIO, &nbMode) != 0) {
        fprintf(stderr,"udp_socket::senddata() - could not turn on blocking mode\n");
        *ierr = -2;
        return;
    }
#else
    if (ioctl(sockfd, FIONBIO, &nbMode) != 0) {
        fprintf(stderr,"udp_socket::senddata() - could not turn on blocking mode\n");
        *ierr = -2;
        return;
    }
#endif
    
    // send the data
    // if o.k. get a pointer to the data in the message and
    // place the incoming data there
    size = *lenData * *dataTypeSize;
    
    while (size > 0) {
        if (size <= MAX_UDP_DATAGRAM) {
            *ierr = sendto(sockfd, gMsg, size, 0, &otherAddr, addrLength);
            size = 0;
        }
        else {	
            *ierr = sendto(sockfd, gMsg, MAX_UDP_DATAGRAM, 0, &otherAddr, addrLength);
            gMsg += MAX_UDP_DATAGRAM;
            size -= MAX_UDP_DATAGRAM;
        }
    }
}


/*
* sendnbdata() - function to send data in nonblocking mode
*
* input: int *socketID - socket identifier
*        int *dataTypeSize - size of data type
*        char *data - pointer to data to send
*        int *lenData - length of data
*        
* return: int *ierr - 0 if successfull, negative number if not
*/
void CALL_CONV sendnbdata(int *socketID, int *dataTypeSize, char data[], int *lenData, int *ierr)
{
    SocketConnection *theSocket = theSockets;
    socket_type sockfd;
    struct sockaddr otherAddr;
    socklen_type addrLength;    
    int size;
    unsigned long nbMode = 1;
    char *gMsg = data;
    *ierr = 0;
    
    // find the socket
    while (theSocket != 0 && theSocket->socketID != *socketID)
        theSocket = theSocket->next;
    if (theSocket == 0) {
        fprintf(stderr,"udp_socket::sendnbdata() - could not find socket to send data\n");
        *ierr = -1;
        return;
    }
    sockfd = theSocket->sockfd;
    otherAddr = theSocket->otherAddr;
    addrLength = sizeof(otherAddr);
    
    // turn on nonblocking mode
#ifdef _WIN32
    if (ioctlsocket(sockfd, FIONBIO, &nbMode) != 0) {
        fprintf(stderr,"udp_socket::sendnbdata() - could not turn on nonblocking mode\n");
        *ierr = -2;
        return;
    }
#else
    if (ioctl(sockfd, FIONBIO, &nbMode) != 0) {
        fprintf(stderr,"udp_socket::sendnbdata() - could not turn on nonblocking mode\n");
        *ierr = -2;
        return;
    }
#endif
    
    // send the data
    // if o.k. get a pointer to the data in the message and
    // place the incoming data there
    size = *lenData * *dataTypeSize;
    
    while (size > 0) {
        if (size <= MAX_UDP_DATAGRAM) {
            *ierr = sendto(sockfd, gMsg, size, 0, &otherAddr, addrLength);
            if (*ierr < 0) {
                *ierr = -3;
                return;
            }
            size = 0;
        }
        else {	
            *ierr = sendto(sockfd, gMsg, MAX_UDP_DATAGRAM, 0, &otherAddr, addrLength);
            if (*ierr < 0) {
                *ierr = -3;
                return;
            }
            gMsg += MAX_UDP_DATAGRAM;
            size -= MAX_UDP_DATAGRAM;
        }
    }
}


/*
* recvdata() - function to receive data in blocking mode
*
* input: int *socketID - socket identifier
*        int *dataTypeSize - size of data type
*        char *data - pointer to data to receive
*        int *lenData - length of data
*        
* return: int *ierr - 0 if successfull, negative number if not
*/
void CALL_CONV recvdata(int *socketID, int *dataTypeSize, char data[], int *lenData, int *ierr)
{
    SocketConnection *theSocket = theSockets;
    socket_type sockfd;
    struct sockaddr otherAddr;
    socklen_type addrLength;
    int size;
    unsigned long nbMode = 0;
    char *gMsg = data;
    *ierr = 0;
    
    // find the socket
    while (theSocket != 0 && theSocket->socketID != *socketID)
        theSocket=theSocket->next;
    if (theSocket == 0) {
        fprintf(stderr,"udp_socket::recvdata() - could not find socket to receive data\n");
        *ierr = -1;
        return;
    }
    sockfd = theSocket->sockfd;
    otherAddr = theSocket->otherAddr;
    addrLength = sizeof(otherAddr);
    
    // turn on blocking mode
#ifdef _WIN32
    if (ioctlsocket(sockfd, FIONBIO, &nbMode) != 0) {
        fprintf(stderr,"udp_socket::recvdata() - could not turn on blocking mode\n");
        *ierr = -2;
        return;
    }
#else
    if (ioctl(sockfd, FIONBIO, &nbMode) != 0) {
        fprintf(stderr,"udp_socket::recvdata() - could not turn on blocking mode\n");
        *ierr = -2;
        return;
    }
#endif
    
    // receive the data
    // if o.k. get a pointer to the data in the message and
    // place the incoming data there
    size = *lenData * *dataTypeSize;
    
    while (size > 0) {
        if (size <= MAX_UDP_DATAGRAM) {
            *ierr = recvfrom(sockfd, gMsg, size, 0, &otherAddr, &addrLength);
            size = 0;
        }
        else {
            *ierr = recvfrom(sockfd, gMsg, MAX_UDP_DATAGRAM, 0, &otherAddr, &addrLength);
            gMsg += MAX_UDP_DATAGRAM;
            size -= MAX_UDP_DATAGRAM;
        }
    }
}


/*
* recvnbdata() - function to receive data in nonblocking mode
*
* input: int *socketID - socket identifier
*        int *dataTypeSize - size of data type
*        char *data - pointer to data to receive
*        int *lenData - length of data
*        
* return: int *ierr - 0 if successfull, negative number if not
*/
void CALL_CONV recvnbdata(int *socketID, int *dataTypeSize, char data[], int *lenData, int *ierr)
{
    SocketConnection *theSocket = theSockets;
    socket_type sockfd;
    struct sockaddr otherAddr;
    socklen_type addrLength;    
    int size;
    unsigned long nbMode = 1;
    char *gMsg = data;
    *ierr = 0;
    
    // find the socket
    while (theSocket != 0 && theSocket->socketID != *socketID)
        theSocket=theSocket->next;
    if (theSocket == 0) {
        fprintf(stderr,"udp_socket::recvnbdata() - could not find socket to receive data\n");
        *ierr = -1;
        return;
    }
    sockfd = theSocket->sockfd;
    otherAddr = theSocket->otherAddr;
    addrLength = sizeof(otherAddr);
    
    // turn on nonblocking mode
#ifdef _WIN32
    if (ioctlsocket(sockfd, FIONBIO, &nbMode) != 0) {
        fprintf(stderr,"udp_socket::recvnbdata() - could not turn on nonblocking mode\n");
        *ierr = -2;
        return;
    }
#else
    if (ioctl(sockfd, FIONBIO, &nbMode) != 0) {
        fprintf(stderr,"udp_socket::recvnbdata() - could not turn on nonblocking mode\n");
        *ierr = -2;
        return;
    }
#endif
    
    // receive the data
    // if o.k. get a pointer to the data in the message and
    // place the incoming data there
    size = *lenData * *dataTypeSize;
    
    while (size > 0) {
        if (size <= MAX_UDP_DATAGRAM) {
            *ierr = recvfrom(sockfd, gMsg, size, 0, &otherAddr, &addrLength);
            if (*ierr < 0) {
                *ierr = -3;
                return;
            }
            size = 0;
        }
        else {
            *ierr = recvfrom(sockfd, gMsg, MAX_UDP_DATAGRAM, 0, &otherAddr, &addrLength);
            if (*ierr < 0) {
                *ierr = -3;
                return;
            }
            gMsg += MAX_UDP_DATAGRAM;
            size -= MAX_UDP_DATAGRAM;
        }
    }
}


/*
* getsocketid() - function to find an open connection
*
* input: unsigned int *port - the port number
*        char *machineInetAddr - the machine inet address
*        int *lengthInet - length of machine inet address
*
* return: int *socketID - negative number if failed to get open connection
*/
void CALL_CONV getsocketid(unsigned int *port, const char machineInetAddr[], int *lengthInet, int *socketID)
{
    SocketConnection *theSocket = theSockets;
    
    // check inputs
    if (machineInetAddr == 0) {
        fprintf(stderr,"udp_socket::getsocketid() - missing machineInetAddr\n");
        *socketID = -1;
        return;
    }
    
    // search for open socket with socketID
    while (theSocket != 0) {
        if (theSocket->port == *port) {
            if (strcmp(theSocket->machineInetAddr, machineInetAddr) == 0) {
                *socketID = theSocket->socketID;
                return;
            }
        }
        theSocket = theSocket->next;
    }
    fprintf(stderr,"udp_socket::getsocketid() - could not find socket to get socketID\n");
    *socketID = -2;
}
