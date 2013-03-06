#define S_FUNCTION_NAME  mtsConnect
#define S_FUNCTION_LEVEL 2

#include "simstruc.h"
#include <winsock2.h>
#include <stdlib.h>
#include <malloc.h>
//#include <windows.h>
//#include "sock.h"

static DWORD	lastError;
static int		sock;
static struct	sockaddr_in	from;

static int errorHandler(int err)
{
	return (err);
}
/*================*
 * Build checking *
 *================*/

#define MDL_CHECK_PARAMETERS
#if defined(MDL_CHECK_PARAMETERS) && defined(MATLAB_MEX_FILE)
/* Function: mdlCheckParameters =============================================
* Abstract:
*    Validate our parameters to verify they are okay.
*/
static void mdlCheckParameters(SimStruct *S)
{
	/* Check 1st parameter: IP String */
	
	if (!mxIsChar(ssGetSFcnParam(S,0)))
	{
		ssSetErrorStatus(S,"1st parameter must be an IP string e.g. 'xx.xx.xx.xx'.");
		return;
	}
	/* Check 2nd parameter: port */
	if (!mxIsNumeric(ssGetSFcnParam(S,1)))
	{
		ssSetErrorStatus(S,"2nd parameter must be an integer port number.");
		return;
	}
	/* Check 3rd parameter: input size of mtsConnector */
	if (!mxIsNumeric(ssGetSFcnParam(S,2)))
	{
		ssSetErrorStatus(S,"3rd parameter must be an integer buffer size.");
		return;
	}
	/* Check 4th parameter: output size of mtsConnector*/
	if (!mxIsNumeric(ssGetSFcnParam(S,3)))
	{
		ssSetErrorStatus(S,"4th parameter must be an integer buffer size.");
		return;
	}    
}
#endif /* MDL_CHECK_PARAMETERS */
 
#define MDL_START  /* Change to #undef to remove function */
#if defined(MDL_START) 
	/* Function: mdlStart =======================================================
	* Abstract:
	*    This function is called once at start of model execution. If you
	*    have states that should be initialized once, this is the place
	*    to do it.
	*/
static void mdlStart(SimStruct *S)
{
	double	*dparam;
	float	*inputBuf;  /* Buffer for input side */
	float   *outputBuf; /* Buffer for output side */
	int		inputBLen;  /* length of input buffer */
	int     outputBLen; /* length of output buffer */
	int		sock;
	int     ReadMode;	/* read mode, 0 for non-blocking, 1 for blocking */	
	int     sockBufferSize;
	
	unsigned short port;	
	WSADATA wsaData;
	struct sockaddr_in local;
	
	/* Get buffer length parameter and allocate a buffer */
	dparam	   = mxGetData(ssGetSFcnParam(S,2));
	outputBLen = (int)(*dparam)+4;   /* four extra words are used to send socket state information, including input/output width and read mode and package ID */	
	dparam	   = mxGetData(ssGetSFcnParam(S,3));
	inputBLen  = (int)(*dparam)+4; 
	inputBuf   = (float *)calloc(inputBLen, sizeof(float));
	outputBuf  = (float *)calloc(outputBLen, sizeof(float));
	
	/* Get socket port parameter */
	dparam = mxGetData(ssGetSFcnParam(S,1));
	port = (int)*dparam;

	/* Start Winsock */
	if (WSAStartup(0x202,&wsaData) == SOCKET_ERROR)
	{
		ssSetErrorStatus(S,"Socket startup error.");
		goto errorExit;
	}

	/* Create a listening socket */
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
	{
		ssSetErrorStatus(S,"Error returned by socket().");
		goto errorExit;
	}

	/*
	bind() associates a local address and port combination with the
	socket just created. This is most useful when the application is a 
	server that has a well-known port that clients know about in advance.
	*/

	local.sin_family      = AF_INET;
	local.sin_addr.s_addr = inet_addr(mxArrayToString(ssGetSFcnParam(S,0))); 
	local.sin_port        = htons(port);
	if (bind(sock, (struct sockaddr*)&local, sizeof(local)) == SOCKET_ERROR)
	{
		ssSetErrorStatus(S,"Error returned by bind().");
		goto errorExit;
	}
	sockBufferSize = inputBLen*sizeof(float);
	/* set the receive buffer size to be the size of one package so incoming package will not be accumulated */	
	setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char *)&sockBufferSize, sizeof(sockBufferSize));  
	ReadMode = 0;

	/* Store buffer pointers, length and sock */
	ssGetPWork(S)[0] = inputBuf;
	ssGetPWork(S)[1] = outputBuf;
	ssGetIWork(S)[0] = sock;
	ssGetIWork(S)[1] = inputBLen;
	ssGetIWork(S)[2] = outputBLen;
	ssGetIWork(S)[3] = ReadMode;
	return;

errorExit:
	
	if (inputBuf)
		free (inputBuf);
	if (outputBuf)
		free (outputBuf);
	WSACleanup();
}
#endif /*  MDL_START */

/* Function: mdlInitializeSizes ===============================================
 * Abstract:
 *   Setup sizes of the various vectors.
 */
static void mdlInitializeSizes(SimStruct *S)
{
	double	*oWidth;
	double  *iWidth;
	
	int     inputWidth;   /* Width of input port */
	int     outputWidth;  /* Width of output port */

	ssSetNumSFcnParams(S, 4);  /* Number of expected parameters */
	#if defined(MATLAB_MEX_FILE)
		if (ssGetNumSFcnParams(S) == ssGetSFcnParamsCount(S)) {
			mdlCheckParameters(S);
			if (ssGetErrorStatus(S) != NULL) {
				return;
			}
		} else {
			return; /* Parameter mismatch will be reported by Simulink */
		}
	#endif
    iWidth = mxGetData(ssGetSFcnParam(S,2));
	oWidth = mxGetData(ssGetSFcnParam(S,3));
	inputWidth = (int)(*iWidth);
	outputWidth = (int)(*oWidth);
	
    if (!ssSetNumInputPorts(S, 1)) return;
    ssSetInputPortWidth(S, 0, inputWidth);  /* set input port width */
    ssSetInputPortDirectFeedThrough(S, 0, 1);

    if (!ssSetNumOutputPorts(S,2)) return;
	ssSetOutputPortWidth(S, 0, 1);			/* function call trigger output port */
	ssSetOutputPortWidth(S, 1, outputWidth);	/* set output port width */

    ssSetNumSampleTimes(S, 1);
    ssSetNumRWork(S, 0);
    ssSetNumIWork(S, 4);	/* Store socket handle here */
    ssSetNumPWork(S, 2);	/* reserve element in the pointers vector to store an object */
    ssSetNumModes(S, 0);
    ssSetNumNonsampledZCs(S, 0);

    /* Take care when specifying exception free code - see sfuntmpl_doc.c */
    ssSetOptions(S, //SS_OPTION_EXCEPTION_FREE_CODE |
                 SS_OPTION_USE_TLC_WITH_ACCELERATOR);
}


/* Function: mdlInitializeSampleTimes =========================================
 * Abstract:
 *    Specifiy that we inherit our sample time from the driving block.
 */
static void mdlInitializeSampleTimes(SimStruct *S)
{
    ssSetSampleTime(S, 0, INHERITED_SAMPLE_TIME);
	ssSetOffsetTime(S, 0, 0.0);
    ssSetCallSystemOutput(S,0);  /* call on 1st element */
}


/* Function: mdlOutputs =======================================================
 */

static void mdlOutputs(SimStruct *S, int_T tid)
{
    int_T				i;
    InputRealPtrsType	uPtrs = ssGetInputPortRealSignalPtrs(S,0);
    real_T				*y    = ssGetOutputPortRealSignal(S,1);
    long				*inputBuf  = ssGetPWork(S)[0];
	long				*outputBuf = ssGetPWork(S)[1];
	int					sock = (int) ssGetIWork(S)[0];
	int					inputBLen  = ssGetIWork(S)[1];
	int					outputBLen = ssGetIWork(S)[2];
	int					ReadMode   = ssGetIWork(S)[3];
	int                 state;	
	union	convert {	float f;
						unsigned long l;
					}	convert;
    unsigned long		mes;	
	int					retval;
	int					fromlen;	

	/* Check for data available */
	
	fromlen = sizeof(from);	
	if (ReadMode == 0)		/* Non-blocking mode */
	{
		mes = 0;
		retval=ioctlsocket (sock, FIONREAD, &mes );
		if (retval == SOCKET_ERROR)
		{	
			ssSetErrorStatus(S,"Socket Error on ioctlsocket().");
			return;
		}

		/* Nothing to read from socket */
		if( mes == 0 )
		{	
			return;
		}
		else
		{
			/* Read from socket */
			retval = recvfrom (sock, (char *)(inputBuf), inputBLen*sizeof(long), 0, (struct sockaddr *)&from, &fromlen);			
			if (retval == SOCKET_ERROR)
			{	
				if (WSAGetLastError()!=WSAECONNRESET)   /* When socket on the other side closed, ignore the error message here */
					ssSetErrorStatus(S,"Socket Error on recvfrom1().");
				return;
			}
			
			/* Check if the output port width of COOPS matches the input port width of Simulink */			
			if ((outputBLen - 4)!= (int)ntohl(inputBuf[1]))
			{
				ssSetErrorStatus(S,"The output port width of COOPS doesn't match the input port width of Simulink");
				return;
			}
			
			/* Check if the input port width of COOPS matches the output port width of Simulink */	
			if ((inputBLen - 4)!= (int)ntohl(inputBuf[0]))
			{
				ssSetErrorStatus(S,"The input port width of COOPS doesn't match the output port width of Simulink");
				return;
			}
            
			/* Read mode is the third word from the socket */
			state = (int)ntohl(inputBuf[2]);
			if (state == 1)
				ReadMode = 1;
			else
				ReadMode = 0;
			ssGetIWork(S)[3] = ReadMode;			
		}
	}
	else	/* Blocking read mode (lock-step) */
	{
		retval = recvfrom (sock, (char *)inputBuf, inputBLen*sizeof(float), 0, (struct sockaddr *)&from, &fromlen);
		if (retval == SOCKET_ERROR)
		{
			ssSetErrorStatus(S,"Socket Error on recvfrom2().");
			return;
		}		
		state = (int)ntohl(inputBuf[2]);
		if (state == 1)
			ReadMode = 1;
		else
			ReadMode = 0;
		ssGetIWork(S)[3] = ReadMode;
	}
	
	/* pass data received from COOPS to output port of mtsConnect model */
	for (i=4; i < inputBLen; i++)
	{	convert.l = ntohl(inputBuf[i]);
		y[i-4] = convert.f;
	}			
    
	/* clock the simulation */
	if (!ssCallSystemWithTid(S, 0, tid))
		return; /* error handled by Simulink */

	/* set sock state header, incluing input/output port width and read mode */	
	outputBuf[0] = htonl(inputBLen - 1);
	outputBuf[1] = htonl(outputBLen -1);
	outputBuf[2] = htonl(1);
	outputBuf[3] = inputBuf[3];
 
    /* pass data from input port of mtsConnect model to output buffer, ready to send to COOPS */
	for (i=4; i < outputBLen; i++)
	{
		convert.f = (float)*uPtrs[i-4];
		outputBuf[i] = htonl(convert.l);
    }

	/* Send outputs back to COOPS */
	retval = sendto(sock, (const char *)outputBuf, outputBLen*sizeof(float), 0, (struct sockaddr *)&from, fromlen);

	if (retval == SOCKET_ERROR)
	{
		ssSetErrorStatus(S,"Socket Error on sendto().");
		return;
	}	
}


/* Function: mdlTerminate =====================================================
 * Abstract:
 *    No termination needed, but we are required to have this routine.
 */
static void mdlTerminate(SimStruct *S)
{
	int		sock;
	float   *inputBuf = ssGetPWork(S)[0];
	float   *outputBuf = ssGetPWork(S)[1];
	int    errorCode;
	int	   retval;
	int	   fromlen;
	int    i;
	int		outputBLen = ssGetIWork(S)[2];
	sock = (int) ssGetIWork(S)[0];

	for (i=0;i<outputBLen;i++)
		outputBuf[i] = (float)htonl(0);	
	fromlen = sizeof(from);
	retval = sendto(sock, (const char *)outputBuf, outputBLen*sizeof(float), 0, (struct sockaddr *)&from, fromlen);
	/* free inputBuf and outputBuf pointer */	
	free(ssGetPWorkValue(S,0));
	free(ssGetPWorkValue(S,1));
	errorCode = WSAGetLastError();	
	closesocket(sock) ;
	WSACleanup();
}



#ifdef  MATLAB_MEX_FILE    /* Is this file being compiled as a MEX-file? */
#include "simulink.c"      /* MEX-file interface mechanism */
#else
#include "cg_sfun.h"       /* Code generation registration function */
#endif

#ifdef __cplusplus
} /* end of extern "C" scope */
#endif

