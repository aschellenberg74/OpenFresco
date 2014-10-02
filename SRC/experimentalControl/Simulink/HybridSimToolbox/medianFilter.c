/******************************************************************************/
/* medianFilter.c:  median filter                 MTS, Brad Thoen, 15-Feb-13  */
/******************************************************************************/
/*
Implements a median filter for removal of impulsive noise up to "medSize"
samples long.
*/
/******************************************************************************/
/* Simulink preamble */

#define S_FUNCTION_NAME  medianFilter
#define S_FUNCTION_LEVEL 2

/* optional S-function methods called in this module */
/* (must be defined or Simulink will not call these functions) */
#define MDL_INITIALIZE_CONDITIONS

/******************************************************************************/
/* imports */

#include "simstruc.h"

/******************************************************************************/
/* definitions */

#define clip(x, min, max)		(((x) > (max)) ? (max) : 		\
								(((x) < (min)) ? (min) : (x)))
#define heapAlloc(x)			malloc(x)
#define heapFree(x)				free(x)

#ifndef RT
	#define TRUE	1
	#define FALSE	0
#endif
#define	NIL			((void *)0)

/******************************************************************************/
/* typedefs */

typedef real_T		 fltvar;	/* float variable */
typedef int_T		 intvar;	/* integer variable */
typedef uint_T		 uintvar;	/* unsigned int variable */

typedef struct sampVars
{	struct sampVars	*next;		/* next sample allocation */
	struct sampVars	*larger;	/* next larger sample */
	struct sampVars	*smaller;	/* next smaller sample */
	struct sampVars	*newer;		/* next newer sample */
	fltvar			value;		/* sample value */
} samples;						/* median filter samples */

typedef struct stateVars
{	uintvar			medSize;	/* median size (= (size - 1) / 2) */
    uintvar			size;		/* size (= 2 * medSize + 1) */
	samples			*next;		/* head of allocation list */
	samples			*largest;	/* head of amplitude-ordered list */
	samples			*oldest;	/* head of time-ordered list */
	samples			*newest;	/* back of time-ordered list */
} states;						/* median filter states */

typedef struct IVARS
{   fltvar			inp;		/* input entry */
    fltvar			out;		/* output result */
	states			state;		/* median filter state */
} ivars;						/* instance variables */

/******************************************************************************/
/* variables */

static samples *samplePool = NIL;	/* sample pool */

/******************************************************************************/
/* median filter procedures */

/* Notes:
	1)	This sorting algorithm is an order n algorithm.
	2)	The state samples are assumed to be ranked in amplitude order as well
		as time-order from a previous procedure call.  Making use of prior
		sorting operations is what makes this algorithm efficient.
	3)	The filter state consists of a linked-list of time samples. State
		samples are linked in three ways:
		i)	 In order of allocation, using the "next" pointer.
		ii)	 In descending amplitude-order using the "smaller" pointer.
		iii) In older to newer time-order, using the "newer" pointer.
	4)	When not on a state list, sample data structures are single-linked
		together into a pool using the "next" pointer.
*/

static void destroyMedian(pool, state)
	samples	*pool;
	states	*state;
{	samples	*sample;

	sample = state->next;
	while (sample)
	{	state->next	 = sample->next;
		sample->next = pool;
		pool		 = sample;
		sample		 = state->next;
	}
	state->next		= NIL;
	state->largest	= NIL;
	state->oldest	= NIL;
	state->newest	= NIL;
}

static void createMedian(pool, state, medSize)
	samples	*pool;
	states	*state;
	uintvar	medSize;
{	samples	*sample;
	intvar	i;

    state->medSize	= medSize;
	state->size		= 2 * medSize + 1;
	state->next		= NIL;
	for (i=state->size; i>0; i--)
	{	if (pool)
		{	sample	= pool;
			pool	= pool->next;
		}
		else
			sample = heapAlloc(sizeof(samples));
		sample->next = state->next;
		state->next	 = sample;
	}
}

static void initMedian(inp, state)
	fltvar	inp;
	states	*state;
{	samples	*sample;

	sample			= state->next;
	state->largest	= sample;
	state->oldest	= sample;
	state->newest	= NIL;
	while (sample)
	{	sample->larger	= state->newest;
		sample->smaller	= sample->next;
		sample->newer	= sample->next;
		sample->value	= inp;
		state->newest	= sample;
		sample			= sample->next;
	}
}

static fltvar doMedian(inp, state)
	fltvar	inp;
	states	*state;
{	samples	*sample;
	samples	*newSample;
	intvar	i;

	/* remove oldest sample from time-ordered list; add as newest sample */
	/* (Note:  Order of the following statements is crucial; do not alter.) */
	sample			= state->oldest;
	state->newest->newer = sample;
	state->oldest	= state->oldest->newer;
	state->newest	= sample;
	sample->newer	= NIL;
	sample->value	= inp;

	/* remove oldest sample from amplitude-ordered list */
	if (sample->larger)
		sample->larger->smaller = sample->smaller;
	else
		state->largest = sample->smaller;
	if (sample->smaller)
		sample->smaller->larger = sample->larger;

	/* place new sample in amplitude-ordered list */
	newSample = state->newest;
	sample	  = state->largest;
	if (!sample)
	{	
		/* no other samples; new sample is automatically the largest */
		newSample->larger	= NIL;
		newSample->smaller	= NIL;
		state->largest		= newSample;
	}
	else
	{
		/* traverse state list to find where to place new sample */
		while (sample)
		{	if (inp > sample->value)
			{	
				/* place new sample before sample */	
				if (sample == state->largest)
				{	newSample->larger	= NIL;
					newSample->smaller	= sample;
					sample->larger		= newSample;
					state->largest		= newSample;
				}
				else
				{	newSample->larger	= sample->larger;
					newSample->smaller	= sample;
					sample->larger->smaller = newSample;
					sample->larger		= newSample;
				}
				sample = NIL;
			}
			else if (!sample->smaller)
			{
				/* new sample is the smallest; place after sample */
				sample->smaller		= newSample;
				newSample->larger	= sample;
				newSample->smaller	= NIL;

				sample = NIL;
			}
			else
				sample = sample->smaller;
		}
	}

	/* traverse state list halfway to find median */
	sample = state->largest;
	for (i=state->medSize; i>0; i--)
		sample = sample->smaller;
	return sample->value;
}

/******************************************************************************/
/* S-function methods */

static void mdlInitializeSizes(SimStruct *S)
{	void	mdlCheckParameters();

	/* check input parameters */
	mdlCheckParameters(S);
	if (ssGetErrorStatus(S))
		return;

	/* states */
	ssSetNumContStates(S, 0);
	ssSetNumDiscStates(S, 0);

	/* inputs */
	if (!ssSetNumInputPorts(S, 1))
		return;
	ssSetInputPortWidth(S, 0, DYNAMICALLY_SIZED);
	ssSetInputPortDirectFeedThrough(S, 0, TRUE);

	/* outputs */
	if (!ssSetNumOutputPorts(S, 1))
		return;
	ssSetOutputPortWidth(S, 0, DYNAMICALLY_SIZED);

	/* other */
	ssSetNumSampleTimes(S, 1);
	ssSetNumRWork(S, 0);
	ssSetNumIWork(S, 0);
	ssSetNumPWork(S, 0);
	ssSetNumModes(S, 0);
	ssSetNumNonsampledZCs(S, 0);
	ssSetOptions(S, SS_OPTION_EXCEPTION_FREE_CODE);
}

static void initializeSelf(SimStruct *S, uintvar nchan)
{	ivars	**sself;
	ivars	*self;
	uintvar	medSize, i;

	/* create instance vars */
	sself = (ivars **)heapAlloc(nchan*sizeof(ivars *));
	for (i=0; i<nchan; i++)
	{	self = (ivars *)heapAlloc(sizeof(ivars));
		if (!self)
		{	ssSetErrorStatus(S, "(mdlStart) Could not allocate ivars.");
			return;
		}
		sself[i] = self;
	}
	ssSetUserData(S, sself);

	/* set the filter median size (actual size will be 2 * medSize + 1) */
	medSize	= (uintvar)*mxGetPr(ssGetSFcnParam(S, 0));
	for (i=0; i<nchan; i++)
	{	self = sself[i];
		createMedian(samplePool, &self->state, medSize);
	}
}

#if defined(MATLAB_MEX_FILE)
#define MDL_SET_INPUT_PORT_WIDTH
static void mdlSetInputPortWidth(SimStruct *S, intvar port, intvar portWidth)
{
	ssSetInputPortWidth(S, port, portWidth);
	initializeSelf(S, portWidth);
}
#define MDL_SET_OUTPUT_PORT_WIDTH
static void mdlSetOutputPortWidth(SimStruct *S, intvar port, intvar portWidth)
{
	ssSetOutputPortWidth(S, port, portWidth);
}
#endif

static void mdlCheckParameters(SimStruct *S)
{	const mxArray	*param;
	uintvar			nparams;

	/* check number of S-function input parameters */
	nparams = ssGetSFcnParamsCount(S);
	if (nparams != 1)
	{	ssSetErrorStatus(S,
			"(mdlCheckParameters) Incorrect # of S-function parameters.");
		return;
	}
	ssSetNumSFcnParams(S, nparams);

	/* check parameter array */
	param = ssGetSFcnParam(S, 0);
	if (mxIsEmpty(param))
	{	ssSetErrorStatus(S,
			"(mdlCheckParameters) Empty parameter array.");
		return;
	}
    nparams = mxGetNumberOfElements(param);
	if (nparams != 1)
	{	ssSetErrorStatus(S,
			"(mdlCheckParameters) Incorrect # of parameters.");
		return;
	}
}

static void mdlInitializeSampleTimes(SimStruct *S)
{
	ssSetSampleTime(S, 0, INHERITED_SAMPLE_TIME);
	ssSetOffsetTime(S, 0, 0.0);
}

static void mdlInitializeConditions(SimStruct *S)
{	ivars	**sself;
	ivars	*self;
	uintvar	nchan;
	uintvar	i;

	sself = (ivars **)ssGetUserData(S);
	nchan = ssGetInputPortWidth(S, 0);
	for (i=0; i<nchan; i++)
	{	self = sself[i];
		self->inp = *(ssGetInputPortRealSignalPtrs(S, 0)[0]);
		initMedian(self->inp, &self->state);
	}
}

static void mdlOutputs(SimStruct *S, intvar tid)
{	ivars				**sself;
	ivars				*self;
	InputRealPtrsType   inpPtr;
	fltvar              *outPtr;
	uintvar				nchan;
	uintvar				i;
	UNUSED_ARG(tid);

	sself  = (ivars **)ssGetUserData(S);
	inpPtr = ssGetInputPortRealSignalPtrs(S, 0);
	outPtr = ssGetOutputPortRealSignal(S, 0);
	nchan  = ssGetInputPortWidth(S, 0);
	for (i=0; i<nchan; i++)
	{	self = sself[i];
		self->inp = *inpPtr[i];
		self->out = doMedian(self->inp, &self->state);
		outPtr[i] = self->out;
	}
}

static void mdlTerminate(SimStruct *S)
{	ivars	**sself;
	ivars	*self;
	uintvar	nchan;
	uintvar	i;
	samples	*sample;

	sself  = (ivars **)ssGetUserData(S);
	nchan = ssGetInputPortWidth(S, 0);
	for (i=0; i<nchan; i++)
	{	self = sself[i];
		if (self)
		{	destroyMedian(samplePool, &self->state);
			while (samplePool)
			{	sample		= samplePool;
				samplePool	= sample->next;
				heapFree(sample);
			}
			heapFree(self);
		}
	}
	heapFree(sself);
}

/******************************************************************************/
/* Simulink postamble */

#ifdef MATLAB_MEX_FILE			/* Is this file being compiled as a MEX-file? */
	#include "simulink.c"		/* MEX-file interface mechanism */
#else
	#include "cg_sfun.h"		/* Code generation registration function */
#endif

/******************************************************************************/
