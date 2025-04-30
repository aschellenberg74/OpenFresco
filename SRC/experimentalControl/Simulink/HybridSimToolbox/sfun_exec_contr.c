/* ===================================================================
**
** Copyright (C) 2005 The MathWorks, Inc. All rights reserved.
**
** Written By Roger Aarenstrup 2005-01-03
**
** For questions, suggestions or bug reports contact:
**
**   roger.aarenstrup@mathworks.com
**
** File: sfun_timer.c
**
** Abstract:
**     This s-function allows the model to be run in Real-Time or a factor 
**     of Real-Time. The block makes Simulink sleep on every time step 
**     until the real world time and the simulation time is equal. 
**     Because Simulink sleeps it will not consume any processor time while 
**     waiting. 
**    
** Parameters: 
**     Real-Time Factor  - How may times faster (or slower if less than 
**                         one) than Real-Time the simulation should 
**                         execute. 
**     Overrun Behaviour - What should happen in case of an overrun 
**     Sample Time       - The blocks sample time 
**
** Inputs:
**     System time
**
** Outputs: 
**     The number of overruns that have occured
**
** Change log:
**
**     2005-01-03 Created
**     2006-07-11 Updated
**   
**
** ===============================================
*/
#define S_FUNCTION_LEVEL 2
#define S_FUNCTION_NAME  sfun_exec_contr

#include "simstruc.h"
#include <time.h>
#include <windows.h>

/* S-Function parameter defines
 */
#define PARAM_RT_FACTOR(S) ssGetSFcnParam(S, 0)
#define PARAM_OVERRUN_ACTION(S) ssGetSFcnParam(S, 1)
#define PARAM_CLASS_PRIORITY(S) ssGetSFcnParam(S, 2)
#define PARAM_THREAD_PRIORITY(S) ssGetSFcnParam(S, 3)
#define PARAM_SAMPLE_TIME(S) ssGetSFcnParam(S, 4)
#define PARAM_RT_SIM(S) ssGetSFcnParam(S, 5)

#define OVERRUN_ACTION_CONTINUE 1
#define OVERRUN_ACTION_ERROR 2

static char err_msg[256];

/* Function: mdlInitializeSizes ===============================================
 * Abstract:
 *    The sizes information is used by Simulink to determine the S-function
 *    block's characteristics (number of inputs, outputs, states, etc.).
 */
static void 
mdlInitializeSizes(SimStruct *S)
{
   ssSetNumSFcnParams(S, 6);  
   if (ssGetNumSFcnParams(S) != ssGetSFcnParamsCount(S)) { return; }

   ssSetNumContStates(S, 0);
   ssSetNumDiscStates(S, 0);

   if (!ssSetNumInputPorts(S, 0)) { return; }

   if (!ssSetNumOutputPorts(S, 1)) { return; }
   ssSetOutputPortWidth(S, 0, 3);

   ssSetNumSampleTimes(S, 1);

   ssSetNumRWork(S, 6);
   ssSetNumIWork(S, 1);
   ssSetNumPWork(S, 0);
   ssSetNumModes(S, 0);
   ssSetNumNonsampledZCs(S, 0);

   ssSetOptions(S, 0);
}

/* Function: mdlInitializeSampleTimes =========================================
 * Abstract:
 *    This function is used to specify the sample time(s) for your
 *    S-function. You must register the same number of sample times as
 *    specified in ssSetNumSampleTimes.
 *    
 *    The sample time is set through a parameter.
 */
static void 
mdlInitializeSampleTimes(SimStruct *S)
{
   ssSetSampleTime(S, 0, mxGetPr(PARAM_SAMPLE_TIME(S))[0]);
   ssSetOffsetTime(S, 0, 0.0);
   return;
}

#define MDL_PROCESS_PARAMETERS   
#if defined(MDL_PROCESS_PARAMETERS)
 /* Function: mdlProcessParameters ===========================================
   * Abstract:
   *    This function is called when parameters are changed
   *    
   *    Here the value of the overrun action parameter is put into 
   *    a work vector.
   */
static void
mdlProcessParameters(SimStruct *S)
{
    ssSetRWorkValue(S, 2, mxGetPr(PARAM_OVERRUN_ACTION(S))[0]);
    ssSetRWorkValue(S, 3, mxGetPr(PARAM_RT_FACTOR(S))[0]);
    ssSetIWorkValue(S, 0, (int)mxGetPr(PARAM_RT_SIM(S))[0]);
}
#endif /* MDL_PROCESS_PARAMETERS */

#define MDL_START  /* Change to #undef to remove function */
#if defined(MDL_START) 
 /* Function: mdlStart =======================================================
   * Abstract:
   *    This function is called once at start of model execution. If you
   *    have states that should be initialized once, this is the place
   *    to do it.
   */
static void 
mdlStart(SimStruct *S)
{
    static char msg[64];
    HANDLE hProc;
    HANDLE hThread;
    BOOL rv;
    unsigned short pri_lvl;

    /* Get the current time and set as the time for the start
     * of the simulation.
     */
    ssSetRWorkValue(S, 0, (real_T) clock());
 
    /* Set number of overruns counter to 0
     */
    ssSetRWorkValue(S, 1, 0.0);

    mdlProcessParameters(S);

    /* Set the process priority class
     */
    hProc = GetCurrentProcess();

    pri_lvl = (unsigned short)mxGetPr(PARAM_CLASS_PRIORITY(S))[0];

    switch (pri_lvl)
    {

    case 1:
        rv = SetPriorityClass(hProc, IDLE_PRIORITY_CLASS);
	break;
    case 2:
        rv = SetPriorityClass(hProc, BELOW_NORMAL_PRIORITY_CLASS);
	break;
    case 3:
        rv = SetPriorityClass(hProc, NORMAL_PRIORITY_CLASS);
	break;
    case 4:
        rv = SetPriorityClass(hProc, ABOVE_NORMAL_PRIORITY_CLASS);
	break;
    case 5:
        rv = SetPriorityClass(hProc, HIGH_PRIORITY_CLASS);
	break;
    case 6:
        rv = SetPriorityClass(hProc, REALTIME_PRIORITY_CLASS);
	break;
    default:
        sprintf(msg,"Error due illegal process priority class set.");
        ssSetErrorStatus(S, msg);
        return;
        break;
    }

    if (!rv)
    {
        sprintf(msg,"Error due to couldn't set process priority class.");
        ssSetErrorStatus(S, msg);
        return;
    }

    /* Set thread priority
     */
    hThread = GetCurrentThread();

    pri_lvl = (unsigned short)mxGetPr(PARAM_THREAD_PRIORITY(S))[0];

    switch (pri_lvl)
    {

    case 1:
        rv = SetThreadPriority(hThread, THREAD_PRIORITY_IDLE); 
	break;
    case 2:
        rv = SetThreadPriority(hThread, THREAD_PRIORITY_LOWEST); 
	break;
    case 3:
        rv = SetThreadPriority(hThread, THREAD_PRIORITY_BELOW_NORMAL); 
	break;
    case 4:
        rv = SetThreadPriority(hThread, THREAD_PRIORITY_NORMAL); 
	break;
    case 5:
        rv = SetThreadPriority(hThread, THREAD_PRIORITY_ABOVE_NORMAL); 
	break;
    case 6:
        rv = SetThreadPriority(hThread, THREAD_PRIORITY_HIGHEST); 
	break;
    case 7:
        rv = SetThreadPriority(hThread, THREAD_PRIORITY_TIME_CRITICAL); 
	break;
    default:
        sprintf(msg,"Error due illegal thread priority set.");
        ssSetErrorStatus(S, msg);
        return;
        break;
    }

    if (!rv)
    {
        sprintf(msg,"Error due to couldn't set thread priority.");
        ssSetErrorStatus(S, msg);
        return;
    }

    /* Set clock time at start
     */
    ssSetRWorkValue(S, 4, (real_T) clock());

    return;
}
#endif /*  MDL_START */

/* Function: mdlOutputs =======================================================
 * Abstract:
 *    In this function, you compute the outputs of your S-function
 *    block. Generally outputs are placed in the output vector, ssGetY(S).
 */
static void 
mdlOutputs(SimStruct *S, int_T tid)
{
    real_T *out1 = ssGetOutputPortRealSignal(S, 0);
    real_T sim_time;
    real_T rw_time;
    real_T start_time;
    real_T curr_time;
    real_T rt_factor;
    real_T step_time;

    if (ssGetIWorkValue(S, 0))
    {
        /* Get the real-time factor
         */
        rt_factor = ssGetRWorkValue(S, 3);

        /* Get the current simulation time in ms
         */
        sim_time = ssGetT(S) * rt_factor * CLOCKS_PER_SEC;

        /* Get the time when the simulation started in ms
         */
        start_time = ssGetRWorkValue(S, 0);

        /* Get the current time in ms
         */
        curr_time = (real_T) clock();

        /* Get the time for one step TET
         */
        step_time = curr_time - ssGetRWorkValue(S, 4);

        /* Get the true (real world) time (in ms) from when the 
         * simulation started
         */
        rw_time = (curr_time - start_time);

        if (rw_time < sim_time)
        {
            /* The real world time is less than the simulation time.
             * Sleep until the real world has cought up with the 
	     * simulation time.
	     */
            Sleep((DWORD)(sim_time - rw_time));
        }
        else
        {
            /* Not good the real world time has passed the simulation time,
	     * that means that we have an overrun.
	     * If overrun ignored is selected, then just increase the 
	     * overrun counter and continue. If error on overrun is selected
	     * raise an error (stop execution).
   	     *
             * Since the first simulation step always will be zero (or start time)
	     * this is not really an overrun.
	     *
             */
            if (sim_time != (ssGetTStart(S) * CLOCKS_PER_SEC))
	    {
                if (ssGetRWorkValue(S, 2) == OVERRUN_ACTION_CONTINUE)
	        {
                    real_T overrun_cnt;

                    /* Since the first simulation step always will be zero
	             * this is not really an overrun.
	             * FIXME change 0.0 to simulation start time.
	             */
	            overrun_cnt =  ssGetRWorkValue(S, 1) + 1;
                    ssSetRWorkValue(S, 1, overrun_cnt);
	        }
	        else
                {
                    static char msg[256];
                    sprintf(msg, "Error due to overrun at time %f", 
                            sim_time * CLOCKS_PER_SEC);
                    ssSetErrorStatus(S, msg);
                    return;
	        }
            }
        }

        out1[0] = ssGetRWorkValue(S, 1);
        out1[1] = ((real_T) clock() - ssGetRWorkValue(S, 4)) / 
                  (CLOCKS_PER_SEC * rt_factor);
        out1[2] = step_time/(CLOCKS_PER_SEC * rt_factor);

        ssSetRWorkValue(S, 4, (real_T) clock());
    }
    else
    { 
        out1[0] = 0.0;
        out1[1] = 0.0;
        out1[2] = 0.0;
    }
    return;
}

/* Function: mdlTerminate =====================================================
 * Abstract:
 *    In this function, you should perform any actions that are necessary
 *    at the termination of a simulation.  For example, if memory was
 *    allocated in mdlStart, this is the place to free it.
 */
static void mdlTerminate(SimStruct *S)
{
    static char msg[64];
    HANDLE hProc;
    HANDLE hThread;
    BOOL rv;

    /* Set the process priority class
     */
    hProc = GetCurrentProcess();
    rv = SetPriorityClass(hProc, NORMAL_PRIORITY_CLASS);
    if (!rv)
    {
        sprintf(msg,"Error due to couldn't set process priority class.");
        ssSetErrorStatus(S, msg);
        return;
    }

    /* Set thread priority
     */
    hThread = GetCurrentThread();
    rv = SetThreadPriority(hThread, THREAD_PRIORITY_NORMAL); 
    if (!rv)
    {
        sprintf(msg,"Error due to couldn't set thread priority priority.");
        ssSetErrorStatus(S, msg);
        return;
    }

    return;
}

/*
 *  Required S-function trailer:
 *  ----------------------------
 */
#ifdef  MATLAB_MEX_FILE    /* Is this file being compiled as a MEX-file? */
#include "simulink.c"      /* MEX-file interface mechanism */
#else
#include "cg_sfun.h"       /* Code generation registration function */
#endif
