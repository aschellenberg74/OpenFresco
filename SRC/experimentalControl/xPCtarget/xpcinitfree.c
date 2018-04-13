/* File:     xpcinitfree.c
 * Abstract: This file contains functions for loading and unloading the
 *           functions provided by the xPC Target C API.
 */
/* Copyright 1996-2010 The MathWorks, Inc. */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define EXPORT /**/
#define NOMANGLE /**/


#ifdef _WIN64
#define XPCAPICALLCONV __fastcall
#else
#define XPCAPICALLCONV __stdcall
#endif

#define XPCAPIDLL "xpcapi.dll"

#undef XPCAPIFUNC
#define XPCAPIFUNC(type, fun) NOMANGLE EXPORT type (XPCAPICALLCONV *##fun)
#include "xpcapi.h"

static HMODULE dllHandle;

#define QUOTE1(x)   #x
#define QUOTE(x)    QUOTE1(x)

#define GETFCNPTR(name)      \
    if ((name = (void *) \
         GetProcAddress(dllHandle, QUOTE(name))) == NULL) \
        return -1;

int xPCInitAPI(void) {
    /* Call GetModuleHandle first in case xpcapi.dll is already loaded */
    if ((dllHandle = GetModuleHandleA(XPCAPIDLL)) == NULL)
        /* If the dll is not already loaded (dllHandle == NULL), *
         * call LoadLibrary to load it.                          */
        if ((dllHandle = LoadLibraryA(XPCAPIDLL)) == NULL) {
            return -1;
        }
    return (xPCResolveAPI(dllHandle));
}

int xPCResolveAPI(HMODULE dllHandle) {

    /* Repeating the GetProcAddress calls is benign. */
    if ((xPCErrorMsg = (void *)
         GetProcAddress(dllHandle, "xPCErrorMsg")) == NULL)
        return -1;
    if ((xPCSetLoadTimeOut = (void *)
         GetProcAddress(dllHandle, "xPCSetLoadTimeOut")) == NULL)
        return -1;
    if ((xPCGetLoadTimeOut = (void *)
         GetProcAddress(dllHandle, "xPCGetLoadTimeOut")) == NULL)
        return -1;
    if ((xPCScGetType = (void *)
         GetProcAddress(dllHandle, "xPCScGetType")) == NULL)
        return -1;
    if ((xPCTgScSetYLimits = (void *)
         GetProcAddress(dllHandle, "xPCTgScSetYLimits")) == NULL)
        return -1;
    if ((xPCTgScSetViewMode = (void *)
         GetProcAddress(dllHandle, "xPCTgScSetViewMode")) == NULL)
        return -1;
    if ((xPCTgScSetMode = (void *)
         GetProcAddress(dllHandle, "xPCTgScSetMode")) == NULL)
        return -1;
    if ((xPCTgScSetGrid = (void *)
         GetProcAddress(dllHandle, "xPCTgScSetGrid")) == NULL)
        return -1;
    if ((xPCTgScGetYLimits = (void *)
         GetProcAddress(dllHandle, "xPCTgScGetYLimits")) == NULL)
        return -1;
    if ((xPCTgScGetViewMode = (void *)
         GetProcAddress(dllHandle, "xPCTgScGetViewMode")) == NULL)
        return -1;
    if ((xPCTgScGetMode = (void *)
         GetProcAddress(dllHandle, "xPCTgScGetMode")) == NULL)
        return -1;
    if ((xPCTgScGetGrid = (void *)
         GetProcAddress(dllHandle, "xPCTgScGetGrid")) == NULL)
        return -1;
    if ((xPCTgScSetSignalFormat = (void *)
         GetProcAddress(dllHandle, "xPCTgScSetSignalFormat")) == NULL)
        return -1;
    if ((xPCTgScGetSignalFormat = (void *)
         GetProcAddress(dllHandle, "xPCTgScGetSignalFormat")) == NULL)
        return -1;
    if ((xPCGetSignalName = (void *)
         GetProcAddress(dllHandle, "xPCGetSignalName")) == NULL)
        return -1;
    if ((xPCGetParamName = (void *)
         GetProcAddress(dllHandle, "xPCGetParamName")) == NULL)
        return -1;
    if ((xPCGetParamType = (void *)
         GetProcAddress(dllHandle, "xPCGetParamType")) == NULL)
        return -1;
    if ((xPCGetParamIdx = (void *)
         GetProcAddress(dllHandle, "xPCGetParamIdx")) == NULL)
        return -1;
    if ((xPCGetSignalIdx = (void *)
         GetProcAddress(dllHandle, "xPCGetSignalIdx")) == NULL)
        return -1;
    if ((xPCGetSignalWidth = (void *)
         GetProcAddress(dllHandle, "xPCGetSignalWidth")) == NULL)
        return -1;
    if ((xPCGetParamDims = (void *)
         GetProcAddress(dllHandle, "xPCGetParamDims")) == NULL)
        return -1;
    if ((xPCGetParamDimsSize = (void *)
         GetProcAddress(dllHandle, "xPCGetParamDimsSize")) == NULL)
        return -1;
    if ((xPCLoadApp = (void *)
         GetProcAddress(dllHandle, "xPCLoadApp")) == NULL)
        return -1;
    if ((xPCSetScope = (void *)
         GetProcAddress(dllHandle, "xPCSetScope")) == NULL)
        return -1;
    if ((xPCGetScope = (void *)
         GetProcAddress(dllHandle, "xPCGetScope")) == NULL)
        return -1;
    if ((xPCScSetNumPrePostSamples = (void *)
         GetProcAddress(dllHandle, "xPCScSetNumPrePostSamples")) == NULL)
        return -1;
    if ((xPCScGetNumPrePostSamples = (void *)
         GetProcAddress(dllHandle, "xPCScGetNumPrePostSamples")) == NULL)
        return -1;
    if ((xPCIsScFinished = (void *)
         GetProcAddress(dllHandle, "xPCIsScFinished")) == NULL)
        return -1;
    if ((xPCScStop = (void *)
         GetProcAddress(dllHandle, "xPCScStop")) == NULL)
        return -1;
    if ((xPCScStart = (void *)
         GetProcAddress(dllHandle, "xPCScStart")) == NULL)
        return -1;
    if ((xPCScSoftwareTrigger = (void *)
         GetProcAddress(dllHandle, "xPCScSoftwareTrigger")) == NULL)
        return -1;
    if ((xPCScGetTriggerSlope = (void *)
         GetProcAddress(dllHandle, "xPCScGetTriggerSlope")) == NULL)
        return -1;
    if ((xPCScSetTriggerSlope = (void *)
         GetProcAddress(dllHandle, "xPCScSetTriggerSlope")) == NULL)
        return -1;
    if ((xPCScGetTriggerSignal = (void *)
         GetProcAddress(dllHandle, "xPCScGetTriggerSignal")) == NULL)
        return -1;
    if ((xPCScSetTriggerSignal = (void *)
         GetProcAddress(dllHandle, "xPCScSetTriggerSignal")) == NULL)
        return -1;
    if ((xPCScGetTriggerScope = (void *)
         GetProcAddress(dllHandle, "xPCScGetTriggerScope")) == NULL)
        return -1;
    if ((xPCScSetTriggerScope = (void *)
         GetProcAddress(dllHandle, "xPCScSetTriggerScope")) == NULL)
        return -1;
    if ((xPCScGetTriggerScopeSample = (void *)
         GetProcAddress(dllHandle, "xPCScGetTriggerScopeSample")) == NULL)
        return -1;
    if ((xPCScSetTriggerScopeSample = (void *)
         GetProcAddress(dllHandle, "xPCScSetTriggerScopeSample")) == NULL)
        return -1;
    if ((xPCScGetTriggerMode = (void *)
         GetProcAddress(dllHandle, "xPCScGetTriggerMode")) == NULL)
        return -1;
    if ((xPCScSetTriggerMode = (void *)
         GetProcAddress(dllHandle, "xPCScSetTriggerMode")) == NULL)
        return -1;
    if ((xPCScGetTriggerLevel = (void *)
         GetProcAddress(dllHandle, "xPCScGetTriggerLevel")) == NULL)
        return -1;
    if ((xPCScSetTriggerLevel = (void *)
         GetProcAddress(dllHandle, "xPCScSetTriggerLevel")) == NULL)
        return -1;
    if ((xPCScGetState = (void *)
         GetProcAddress(dllHandle, "xPCScGetState")) == NULL)
        return -1;
    if ((xPCScGetStartTime = (void *)
         GetProcAddress(dllHandle, "xPCScGetStartTime")) == NULL)
        return -1;
    if ((xPCScGetNumSamples = (void *)
         GetProcAddress(dllHandle, "xPCScGetNumSamples")) == NULL)
        return -1;
    if ((xPCScSetNumSamples = (void *)
         GetProcAddress(dllHandle, "xPCScSetNumSamples")) == NULL)
        return -1;
    if ((xPCScGetDecimation = (void *)
         GetProcAddress(dllHandle, "xPCScGetDecimation")) == NULL)
        return -1;
    if ((xPCScSetDecimation = (void *)
         GetProcAddress(dllHandle, "xPCScSetDecimation")) == NULL)
        return -1;
    if ((xPCScGetSignals = (void *)
         GetProcAddress(dllHandle, "xPCScGetSignals")) == NULL)
        return -1;
    if ((xPCGetScopes = (void *)
         GetProcAddress(dllHandle, "xPCGetScopes")) == NULL)
        return -1;
    if ((xPCGetHiddenScopes = (void *)
         GetProcAddress(dllHandle, "xPCGetHiddenScopes")) == NULL)
        return -1;
    if ((xPCScRemSignal = (void *)
         GetProcAddress(dllHandle, "xPCScRemSignal")) == NULL)
        return -1;
    if ((xPCScAddSignal = (void *)
         GetProcAddress(dllHandle, "xPCScAddSignal")) == NULL)
        return -1;
    if ((xPCRemScope = (void *)
         GetProcAddress(dllHandle, "xPCRemScope")) == NULL)
        return -1;
    if ((xPCAddScope = (void *)
         GetProcAddress(dllHandle, "xPCAddScope")) == NULL)
        return -1;
    if ((xPCGetSignal = (void *)
         GetProcAddress(dllHandle, "xPCGetSignal")) == NULL)
        return -1;
    if ((xPCGetSignals = (void *)
         GetProcAddress(dllHandle, "xPCGetSignals")) == NULL)
        return -1;
    if ((xPCMaximumTET = (void *)
         GetProcAddress(dllHandle, "xPCMaximumTET")) == NULL)
        return -1;
    if ((xPCMinimumTET = (void *)
         GetProcAddress(dllHandle, "xPCMinimumTET")) == NULL)
        return -1;
    if ((xPCScGetData = (void *)
         GetProcAddress(dllHandle, "xPCScGetData")) == NULL)
        return -1;
    if ((xPCScGetAutoRestart = (void *)
         GetProcAddress(dllHandle, "xPCScGetAutoRestart")) == NULL)
        return -1;
    if ((xPCScSetAutoRestart = (void *)
         GetProcAddress(dllHandle, "xPCScSetAutoRestart")) == NULL)
        return -1;
    if ((xPCGetTETLog = (void *)
         GetProcAddress(dllHandle, "xPCGetTETLog")) == NULL)
        return -1;
    if ((xPCGetTimeLog = (void *)
         GetProcAddress(dllHandle, "xPCGetTimeLog")) == NULL)
        return -1;
    if ((xPCGetStateLog = (void *)
         GetProcAddress(dllHandle, "xPCGetStateLog")) == NULL)
        return -1;
    if ((xPCGetOutputLog = (void *)
         GetProcAddress(dllHandle, "xPCGetOutputLog")) == NULL)
        return -1;
    if ((xPCReboot = (void *)
         GetProcAddress(dllHandle, "xPCReboot")) == NULL)
        return -1;
    if ((xPCNumLogWraps = (void *)
         GetProcAddress(dllHandle, "xPCNumLogWraps")) == NULL)
        return -1;
    if ((xPCMaxLogSamples = (void *)
         GetProcAddress(dllHandle, "xPCMaxLogSamples")) == NULL)
        return -1;
    if ((xPCNumLogSamples = (void *)
         GetProcAddress(dllHandle, "xPCNumLogSamples")) == NULL)
        return -1;
    if ((xPCGetLogMode = (void *)
         GetProcAddress(dllHandle, "xPCGetLogMode")) == NULL)
        return -1;
    if ((xPCSetParam = (void *)
         GetProcAddress(dllHandle, "xPCSetParam")) == NULL)
        return -1;
    if ((xPCSetLogMode = (void *)
         GetProcAddress(dllHandle, "xPCSetLogMode")) == NULL)
        return -1;
    if ((xPCGetParam = (void *)
         GetProcAddress(dllHandle, "xPCGetParam")) == NULL)
        return -1;
    if ((xPCGetNumStates = (void *)
         GetProcAddress(dllHandle, "xPCGetNumStates")) == NULL)
        return -1;
    if ((xPCGetNumOutputs = (void *)
         GetProcAddress(dllHandle, "xPCGetNumOutputs")) == NULL)
        return -1;
    if ((xPCIsOverloaded = (void *)
         GetProcAddress(dllHandle, "xPCIsOverloaded")) == NULL)
        return -1;
    if ((xPCIsAppRunning = (void *)
         GetProcAddress(dllHandle, "xPCIsAppRunning")) == NULL)
        return -1;
    if ((xPCStopApp = (void *)
         GetProcAddress(dllHandle, "xPCStopApp")) == NULL)
        return -1;
    if ((xPCStartApp = (void *)
         GetProcAddress(dllHandle, "xPCStartApp")) == NULL)
        return -1;
    if ((xPCUnloadApp = (void *)
         GetProcAddress(dllHandle, "xPCUnloadApp")) == NULL)
        return -1;
    if ((xPCGetAppName = (void *)
         GetProcAddress(dllHandle, "xPCGetAppName")) == NULL)
        return -1;
    if ((xPCGetNumSignals = (void *)
         GetProcAddress(dllHandle, "xPCGetNumSignals")) == NULL)
        return -1;
    if ((xPCGetNumParams = (void *)
         GetProcAddress(dllHandle, "xPCGetNumParams")) == NULL)
        return -1;
    if ((xPCAverageTET = (void *)
         GetProcAddress(dllHandle, "xPCAverageTET")) == NULL)
        return -1;
    if ((xPCGetEcho = (void *)
         GetProcAddress(dllHandle, "xPCGetEcho")) == NULL)
        return -1;
    if ((xPCSetEcho = (void *)
         GetProcAddress(dllHandle, "xPCSetEcho")) == NULL)
        return -1;
    if ((xPCGetSampleTime = (void *)
         GetProcAddress(dllHandle, "xPCGetSampleTime")) == NULL)
        return -1;
    if ((xPCSetSampleTime = (void *)
         GetProcAddress(dllHandle, "xPCSetSampleTime")) == NULL)
        return -1;
    if ((xPCGetStopTime = (void *)
         GetProcAddress(dllHandle, "xPCGetStopTime")) == NULL)
        return -1;
    if ((xPCSetStopTime = (void *)
         GetProcAddress(dllHandle, "xPCSetStopTime")) == NULL)
        return -1;
    if ((xPCGetExecTime = (void *)
         GetProcAddress(dllHandle, "xPCGetExecTime")) == NULL)
        return -1;
    if ((xPCGetLastError = (void *)
         GetProcAddress(dllHandle, "xPCGetLastError")) == NULL)
        return -1;
    if ((xPCSetLastError = (void *)
         GetProcAddress(dllHandle, "xPCSetLastError")) == NULL)
        return -1;
    if ((xPCClosePort = (void *)
         GetProcAddress(dllHandle, "xPCClosePort")) == NULL)
        return -1;
    if ((xPCOpenSerialPort = (void *)
         GetProcAddress(dllHandle, "xPCOpenSerialPort")) == NULL)
        return -1;
    if ((xPCReOpenPort = (void *)
         GetProcAddress(dllHandle, "xPCReOpenPort")) == NULL)
        return -1;
    if ((xPCOpenTcpIpPort = (void *)
         GetProcAddress(dllHandle, "xPCOpenTcpIpPort")) == NULL)
        return -1;
    if ((xPCOpenConnection = (void *)
         GetProcAddress(dllHandle, "xPCOpenConnection")) == NULL)
        return -1;
    if ((xPCCloseConnection = (void *)
         GetProcAddress(dllHandle, "xPCCloseConnection")) == NULL)
        return -1;
    if ((xPCRegisterTarget = (void *)
         GetProcAddress(dllHandle, "xPCRegisterTarget")) == NULL)
        return -1;
    if ((xPCDeRegisterTarget = (void *)
         GetProcAddress(dllHandle, "xPCDeRegisterTarget")) == NULL)
        return -1;
    if ((xPCGetAPIVersion = (void *)
         GetProcAddress(dllHandle, "xPCGetAPIVersion")) == NULL)
        return -1;
    if ((xPCGetTargetVersion = (void *)
         GetProcAddress(dllHandle, "xPCGetTargetVersion")) == NULL)
        return -1;
    if ((xPCTargetPing = (void *)
         GetProcAddress(dllHandle, "xPCTargetPing")) == NULL)
        return -1;
    if ((xPCFSReadFile = (void *)
         GetProcAddress(dllHandle, "xPCFSReadFile")) == NULL)
        return -1;
    if ((xPCFSRead = (void *)
         GetProcAddress(dllHandle, "xPCFSRead")) == NULL)
        return -1;
    if ((xPCFSWriteFile = (void *)
         GetProcAddress(dllHandle, "xPCFSWriteFile")) == NULL)
        return -1;
    if ((xPCFSGetFileSize = (void *)
         GetProcAddress(dllHandle, "xPCFSGetFileSize")) == NULL)
        return -1;
    if ((xPCFSOpenFile = (void *)
         GetProcAddress(dllHandle, "xPCFSOpenFile")) == NULL)
        return -1;
    if ((xPCFSCloseFile = (void *)
         GetProcAddress(dllHandle, "xPCFSCloseFile")) == NULL)
        return -1;
    if ((xPCFSGetPWD = (void *)
         GetProcAddress(dllHandle, "xPCFSGetPWD")) == NULL)
        return -1;
    if ((xPCFTPGet = (void *)
         GetProcAddress(dllHandle, "xPCFTPGet")) == NULL)
        return -1;
    if ((xPCFTPPut = (void *)
         GetProcAddress(dllHandle, "xPCFTPPut")) == NULL)
        return -1;
    if ((xPCFSRemoveFile = (void *)
         GetProcAddress(dllHandle, "xPCFSRemoveFile")) == NULL)
        return -1;
    if ((xPCFSCD = (void *)
         GetProcAddress(dllHandle, "xPCFSCD")) == NULL)
        return -1;
    if ((xPCFSMKDIR = (void *)
         GetProcAddress(dllHandle, "xPCFSMKDIR")) == NULL)
        return -1;
    if ((xPCFSRMDIR = (void *)
         GetProcAddress(dllHandle, "xPCFSRMDIR")) == NULL)
        return -1;
    if ((xPCFSDir = (void *)
         GetProcAddress(dllHandle, "xPCFSDir")) == NULL)
        return -1;
    if ((xPCFSDirSize = (void *)
         GetProcAddress(dllHandle, "xPCFSDirSize")) == NULL)
        return -1;
    if ((xPCFSFileTable = (void *)
         GetProcAddress(dllHandle, "xPCFSFileTable")) == NULL)
        return -1;
    if ((xPCFSGetError = (void *)
         GetProcAddress(dllHandle, "xPCFSGetError")) == NULL)
        return -1;
    if ((xPCSaveParamSet = (void *)
         GetProcAddress(dllHandle, "xPCSaveParamSet")) == NULL)
        return -1;
    if ((xPCLoadParamSet = (void *)
         GetProcAddress(dllHandle, "xPCLoadParamSet")) == NULL)
        return -1;
    if ((xPCReadXML = (void *)
         GetProcAddress(dllHandle, "xPCReadXML")) == NULL)
        return -1;
    if ((xPCFSDiskInfo = (void *)
         GetProcAddress(dllHandle, "xPCFSDiskInfo")) == NULL)
        return -1;
    if ((xPCFSDirItems = (void *)
         GetProcAddress(dllHandle, "xPCFSDirItems")) == NULL)
        return -1;
    if ((xPCFSDirStructSize = (void *)
         GetProcAddress(dllHandle, "xPCFSDirStructSize")) == NULL)
        return -1;
    if ((xPCGetSigLabelWidth = (void *)
         GetProcAddress(dllHandle, "xPCGetSigLabelWidth")) == NULL)
        return -1;
    if ((xPCGetSigIdxfromLabel = (void *)
         GetProcAddress(dllHandle, "xPCGetSigIdxfromLabel")) == NULL)
        return -1;
    if ((xPCGetSignalLabel = (void *)
         GetProcAddress(dllHandle, "xPCGetSignalLabel")) == NULL)
        return -1;
    if ((xPCScGetNumSignals = (void *)
         GetProcAddress(dllHandle, "xPCScGetNumSignals")) == NULL)
        return -1;
    if ((xPCSetHiddenScopeEcho = (void *)
         GetProcAddress(dllHandle, "xPCSetHiddenScopeEcho")) == NULL)
        return -1;
    if ((xPCGetHiddenScopeEcho = (void *)
         GetProcAddress(dllHandle, "xPCGetHiddenScopeEcho")) == NULL)
        return -1;
    if ((xPCGetNumScopes = (void *)
         GetProcAddress(dllHandle, "xPCGetNumScopes")) == NULL)
        return -1;
    if ((xPCGetNumHiddenScopes = (void *)
         GetProcAddress(dllHandle, "xPCGetNumHiddenScopes")) == NULL)
        return -1;
    if ((xPCGetScopeList = (void *)
         GetProcAddress(dllHandle, "xPCGetScopeList")) == NULL)
        return -1;
    if ((xPCGetHiddenList = (void *)
         GetProcAddress(dllHandle, "xPCGetHiddenList")) == NULL)
        return -1;
    if ((xPCScGetSignalList = (void *)
         GetProcAddress(dllHandle, "xPCScGetSignalList")) == NULL)
        return -1;
    if ((xPCGetSimMode = (void *)
         GetProcAddress(dllHandle, "xPCGetSimMode")) == NULL)
        return -1;
    if ((xPCGetPCIInfo = (void *)
         GetProcAddress(dllHandle, "xPCGetPCIInfo")) == NULL)
        return -1;
    if ((xPCGetSessionTime = (void *)
         GetProcAddress(dllHandle, "xPCGetSessionTime")) == NULL)
        return -1;
    if ((xPCGetLogStatus = (void *)
         GetProcAddress(dllHandle, "xPCGetLogStatus")) == NULL)
        return -1;
    if ((xPCFSFileInfo = (void *)
         GetProcAddress(dllHandle, "xPCFSFileInfo")) == NULL)
        return -1;
    if ((xPCSetDefaultStopTime = (void *)
         GetProcAddress(dllHandle, "xPCSetDefaultStopTime")) == NULL)
        return -1;
    if ((xPCGetXMLSize = (void *)
         GetProcAddress(dllHandle, "xPCGetXMLSize")) == NULL)
        return -1;
    if ((xPCIsTargetScope = (void *)
         GetProcAddress(dllHandle, "xPCIsTargetScope")) == NULL)
        return -1;
    if ((xPCSetTargetScopeUpdate = (void *)
         GetProcAddress(dllHandle, "xPCSetTargetScopeUpdate")) == NULL)
        return -1;
    if ((xPCFSReNameFile = (void *)
         GetProcAddress(dllHandle, "xPCFSReNameFile")) == NULL)
        return -1;

    GETFCNPTR(xPCFSScSetFilename);
    GETFCNPTR(xPCFSScGetFilename);
    GETFCNPTR(xPCFSScSetWriteMode);
    GETFCNPTR(xPCFSScGetWriteMode);
    GETFCNPTR(xPCFSScSetWriteSize);
    GETFCNPTR(xPCFSScGetWriteSize);
    GETFCNPTR(xPCFSScSetDynamicMode);
    GETFCNPTR(xPCFSScGetDynamicMode);
    GETFCNPTR(xPCFSScSetMaxWriteFileSize);
    GETFCNPTR(xPCFSScGetMaxWriteFileSize);
    return 0;
}

/*========================================================================*/

void xPCFreeAPI(void) {
    FreeLibrary(dllHandle);

    xPCErrorMsg =                NULL;
    xPCSetLoadTimeOut =          NULL;
    xPCGetLoadTimeOut =          NULL;
    xPCScGetType =               NULL;
    xPCTgScSetYLimits =          NULL;
    xPCTgScSetViewMode =         NULL;
    xPCTgScSetMode =             NULL;
    xPCTgScSetGrid =             NULL;
    xPCTgScGetYLimits =          NULL;
    xPCTgScGetViewMode =         NULL;
    xPCTgScGetMode =             NULL;
    xPCTgScGetGrid =             NULL;
    xPCTgScSetSignalFormat =     NULL;
    xPCTgScGetSignalFormat =     NULL;
    xPCGetSignalName =           NULL;
    xPCGetParamName =            NULL;
    xPCGetParamType =            NULL;
    xPCGetParamIdx =             NULL;
    xPCGetSignalIdx =            NULL;
    xPCGetSignalWidth =          NULL;
    xPCGetParamDims =            NULL;
    xPCGetParamDimsSize =        NULL;
    xPCLoadApp =                 NULL;
    xPCSetScope =                NULL;
    xPCGetScope =                NULL;
    xPCScSetNumPrePostSamples =  NULL;
    xPCScGetNumPrePostSamples =  NULL;
    xPCIsScFinished =            NULL;
    xPCScStop =                  NULL;
    xPCScStart =                 NULL;
    xPCScSoftwareTrigger =       NULL;
    xPCScGetTriggerSlope =       NULL;
    xPCScSetTriggerSlope =       NULL;
    xPCScGetTriggerSignal =      NULL;
    xPCScSetTriggerSignal =      NULL;
    xPCScGetTriggerScope =       NULL;
    xPCScSetTriggerScope =       NULL;
    xPCScGetTriggerScopeSample = NULL;
    xPCScSetTriggerScopeSample = NULL;
    xPCScGetTriggerMode =        NULL;
    xPCScSetTriggerMode =        NULL;
    xPCScGetTriggerLevel =       NULL;
    xPCScSetTriggerLevel =       NULL;
    xPCScGetState =              NULL;
    xPCScGetStartTime =          NULL;
    xPCScGetNumSamples =         NULL;
    xPCScSetNumSamples =         NULL;
    xPCScGetDecimation =         NULL;
    xPCScSetDecimation =         NULL;
    xPCScGetSignals =            NULL;
    xPCScGetAutoRestart =        NULL;
    xPCScSetAutoRestart =        NULL;
    xPCGetScopes =               NULL;
    xPCGetHiddenScopes =         NULL;
    xPCScRemSignal =             NULL;
    xPCScAddSignal =             NULL;
    xPCRemScope =                NULL;
    xPCAddScope =                NULL;
    xPCGetSignal =               NULL;
    xPCGetSignals =              NULL;
    xPCMaximumTET =              NULL;
    xPCMinimumTET =              NULL;
    xPCScGetData =               NULL;
    xPCGetTETLog =               NULL;
    xPCGetTimeLog =              NULL;
    xPCGetStateLog =             NULL;
    xPCGetOutputLog =            NULL;
    xPCReboot =                  NULL;
    xPCNumLogWraps =             NULL;
    xPCMaxLogSamples =           NULL;
    xPCNumLogSamples =           NULL;
    xPCGetLogMode =              NULL;
    xPCSetParam =                NULL;
    xPCSetLogMode =              NULL;
    xPCGetParam =                NULL;
    xPCGetNumStates =            NULL;
    xPCGetNumOutputs =           NULL;
    xPCIsOverloaded =            NULL;
    xPCIsAppRunning =            NULL;
    xPCStopApp =                 NULL;
    xPCStartApp =                NULL;
    xPCUnloadApp =               NULL;
    xPCGetAppName =              NULL;
    xPCGetNumSignals =           NULL;
    xPCGetNumParams =            NULL;
    xPCAverageTET =              NULL;
    xPCGetEcho =                 NULL;
    xPCSetEcho =                 NULL;
    xPCGetSampleTime =           NULL;
    xPCSetSampleTime =           NULL;
    xPCGetStopTime =             NULL;
    xPCSetStopTime =             NULL;
    xPCGetExecTime =             NULL;
    xPCGetLastError =            NULL;
    xPCSetLastError =            NULL;
    xPCClosePort =               NULL;
    xPCOpenSerialPort =          NULL;
    xPCReOpenPort =              NULL;
    xPCOpenTcpIpPort =           NULL;
    xPCOpenConnection =          NULL;
    xPCCloseConnection =         NULL;
    xPCRegisterTarget =          NULL;
    xPCDeRegisterTarget =        NULL;
    xPCGetAPIVersion =           NULL;
    xPCGetTargetVersion =        NULL;
    xPCTargetPing =              NULL;
    xPCFSReadFile =              NULL;
    xPCFSRead     =              NULL;
    xPCFSWriteFile =             NULL;
    xPCFSGetFileSize =           NULL;
    xPCFSOpenFile =              NULL;
    xPCFSCloseFile =             NULL;
    xPCFSGetPWD =                NULL;
    xPCFTPGet =                  NULL;
    xPCFTPPut =                  NULL;
    xPCFSRemoveFile =            NULL;
    xPCFSCD =                    NULL;
    xPCFSMKDIR =                 NULL;
    xPCFSRMDIR =                 NULL;
    xPCFSDir =                   NULL;
    xPCFSDirSize =               NULL;
    xPCFSFileTable =             NULL;
    xPCFSGetError =              NULL;
    xPCSaveParamSet =            NULL;
    xPCLoadParamSet =            NULL;
    xPCFSScSetFilename =         NULL;
    xPCFSScGetFilename =         NULL;
    xPCFSScSetWriteMode =        NULL;
    xPCFSScGetWriteMode =        NULL;
    xPCFSScSetWriteSize =        NULL;
    xPCFSScGetWriteSize =        NULL;
    xPCFSScSetDynamicMode=       NULL;
    xPCFSScGetDynamicMode=       NULL;
    xPCReadXML =                 NULL;
    xPCFSDiskInfo =              NULL;
    xPCFSDirItems =              NULL;
    xPCFSDirStructSize =         NULL;
    xPCGetSigLabelWidth  =       NULL;
    xPCGetSigIdxfromLabel=       NULL;
    xPCGetSignalLabel =          NULL;
    xPCScGetNumSignals=          NULL;
    xPCGetHiddenScopeEcho=       NULL;
    xPCSetHiddenScopeEcho=       NULL;
    xPCGetSimMode=               NULL;
    xPCGetPCIInfo=               NULL;
    xPCGetSessionTime=           NULL;
    xPCGetLogStatus=             NULL;
    xPCFSFileInfo=               NULL;
    xPCSetDefaultStopTime=       NULL;
    xPCGetXMLSize=               NULL;
    xPCIsTargetScope=            NULL;
    xPCSetTargetScopeUpdate=     NULL;
    xPCFSReNameFile=             NULL;
    xPCFSScSetMaxWriteFileSize=  NULL;
    xPCFSScGetMaxWriteFileSize=  NULL;
    return;
}

/* EOF xpcinitfree.c */
