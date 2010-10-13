/******************************************************************************/
/* actuator.c:  actuator Simulink S-function				  BKT  30-Jun-03  */
/******************************************************************************/
/*
Implements a forward hydraulic servovalve/actuator model for simulation
purposes.

Inputs:
	1:  valve command
	2:  displacement
	3:  velocity
	4:  supply pressure
	5:  return pressure

Output:
	1:  force

Auxiliary diagnostic output vector:
	1:	valve opening
	2:	C1 pressure
	3:	C2 pressure
	4:	delta pressure
	5:	force
	6:	velocity
	7:	C1 flow
	8:	C2 flow
	9:	total flow
	10:	kinematic flow
	11:	compressibility flow
	12:	leakage flow

Parameter vector:
	1:  oil bulk modulus (force/displ^2)
	2:  servovalve delay (sec)
	3:  servovalve overlap
	4:  rated pressure (force/displ^2)
	5:	rated flow at rated pressure (displ^3/sec)
	6:	nominal flow at rated pressure (displ^3/sec)
	7:	actuator piston area 1 (displ^2)
	8:	actuator piston area 2 (displ^2)
	9:	actuator usable stroke 1 (displ)
	10:	actuator usable stroke 2 (displ)
	11:	actuator end cushion length 1 (displ)
	12:	actuator end cushion length 2 (displ)
	13: actuator parasitic damping factor
	14: cross-chamber leakage factor (flow/press)
	15: ratio port openings according to area
		(0.0: don't ratio, 1.0: do ratio)
	16: sample period (sec)

	Conversion factors used for converting
	signals from external (user) to internal
	(canonical) units:

	17: displacement
	18: velocity
	19: force
	20: pressure
	21: flow

Notes:
1)	All simulation parameters must units-wise be in canonical form.
2)	Simulation outputs are converted to user units according to conversion
	factors entered via method "setMax".
3)	Real hydraulic systems have more damping than that predicted by this
	model due to various parasitic damping effects.  These effects cannot
	be individually modeled, so an overall damping parameter is provided to
	achieve realistic damping.  Given a desired damping ratio, compute this
	damping parameter using this formula:
		damping parameter = damping ratio * oil column radian frequency * mass
	where
		oil column radian frequency
			= sqrt(2 * area * bulkModulus
				   / ((actuator stroke + end cushion length) * mass)).		
4)	The actuator is modeled as a four port device.  For the case of a non-equal
	area actuator, each port must have a different scale factor in order to
	minimize distortion.  These scale factors are optionally included in this
	simulation, but in a real actuator they must be implemented mechanically
	(in the case of spool-type servovalves) or electronically (in the case of
	poppet-type servovalves).
5)	Cylinder designations C1 and C2 are such that with positive valve drive,
	oil is ported into C1 and out of C2, resulting in positive displacement.
6)	For single-ended actuators, set either of the actuator piston areas to zero.
*/
/******************************************************************************/
/* Simulink preamble */

#define S_FUNCTION_NAME  actuator
#define S_FUNCTION_LEVEL 2

/* optional S-function methods called in this module */
/* (must be defined or Simulink will not call these functions) */
#define MDL_CHECK_PARAMETERS
#define MDL_PROCESS_PARAMETERS
#define MDL_START 
#define MDL_INITIALIZE_CONDITIONS
#define MDL_UPDATE

/******************************************************************************/
/* imports */

#include <math.h>
#include "tmwtypes.h"
#include "simstruc.h"

extern void *malloc();
extern void free();

/******************************************************************************/
/* definitions */

#define	absx(x)					(((x) > 0) ? (x) : -(x))
#define clip(x, min, max)		(((x) > (max)) ? (max) : 		\
								(((x) < (min)) ? (min) : (x)))
#define max(x, y)				(((x) > (y)) ? (x) : (y))
#define outRange(x, min, max)	(((x) > (max)) || ((x) < (min)))
#define	sgn(x)					(((x) >= 0.0) ? 1.0 : -1.0)

#define NIL						((void*)0L)
#define TRUE					1
#define FALSE					0

enum							/* chamber ids */
{	C1,							/* chamber 1 */
	C2							/* chamber 2 */
};

enum							/* port ids */
{	S1,							/* chamber 1 supply port */
	R1,							/* chamber 1 return port */
	S2,							/* chamber 2 supply port */
	R2							/* chamber 2 return port */
};

enum							/* dimension ids */
{	DISPL,
	VELOC,
	FORCE,
	PRESS,
	FLOW,

	NDIMS						/* (number of dimensions) */
};

enum							/* input terminal ids */
{	VALVE_INP,					/* servovalve command */
	DISPL_INP,					/* displacement */
	VELOC_INP,					/* velocity */

	NINPS						/* (number of inputs) */
};

enum							/* auxiliary input terminal ids */
{	SPLPRESS_XINP,				/* supply pressure */
	RTNPRESS_XINP,				/* return pressure */

	NXINPS						/* (number of auxiliary inputs) */
};

enum							/* auxiliary diagnostic output terminal ids */
{	VALVEOPEN_XOUT,				/* valve opening (volts) */
	C1PRESS_XOUT,				/* C1 pressure */
	C2PRESS_XOUT,				/* C2 pressure */
	DELTAPRESS_XOUT,			/* delta pressure */
	FORCE_XOUT,					/* force */
	VELOC_XOUT,					/* velocity */	
	C1FLOW_XOUT,				/* C1 flow */
	C2FLOW_XOUT,				/* C2 flow */
	TOTALFLOW_XOUT,				/* total flow */
	KINFLOW_XOUT,				/* kinematic flow */
	COMPFLOW_XOUT,				/* compressibility flow */
	LEAKFLOW_XOUT,				/* leakage flow */
	NXOUTS						/* (number of outputs) */
};

enum							/* simulation parameter ids */
{	/* oil parameters */
	BULKMODULUS,				/* oil bulk modulus (force/displ^2) */

	/* servovalve parameters */
	VALVEDELAY,					/* servovalve delay (sec) */
	OVERLAP,					/* servovalve overlap */
	RATEDPRESS,					/* rated pressure (force/displ^2) */
	RATEDFLOW,					/* rated flow at rated press (displ^3/sec) */
	NOMINFLOW,					/* nominal flow at rated press (displ^3/sec) */

	/* actuator parameters */
	AREA1,						/* actuator piston area 1 (displ^2) */
	AREA2,						/* actuator piston area 2 (displ^2) */
	STROKE1,					/* actuator usable stroke 1 (displ) */
	STROKE2,					/* actuator usable stroke 2 (displ) */
	ENDLENGTH1,					/* actuator end cushion length 1 (displ) */
	ENDLENGTH2,					/* actuator end cushion length 2 (displ) */
	DAMPING,					/* actuator parasitic damping factor */
								/* (see Note 3 above) */
	LEAKAGE,					/* cross-chamber leakage factor (flow/press) */
								/* (displ^3/sec / force/displ^2) */
	RATIOPORTS,					/* ratio port openings according to area */

	/* controller parameters */
	SAMPLEPERIOD,				/* sample period */

	/* user to canonical units conversion factors */
	DISPLCANON,
	VELOCCANON,
	FORCECANON,
	PRESSCANON,
	FLOWCANON,

	NPARAMS						/* (number of parameters) */
};

/******************************************************************************/
/* typedefs */

typedef struct biqVars
{	struct biqVars	*next;			/* next biquad */
    real_T			*inp;			/* biquad input */
    real_T			out;			/* biquad output */
    real_T			coef[5];		/* biquad coefs (a1, a2, b0, b1, b2) */
    real_T			state[2];		/* biquad state (sk-1, sk-2) */
} biquads;							/* biquadratic filter element */

typedef struct
{   real_T			inp[NINPS];		/* input entries */
	real_T			out;			/* servovalve command output result */
	real_T			xinp[NXINPS];	/* auxiliary input entries */
    real_T			xout[NXOUTS];	/* auxiliary diagnostic output results */
	real_T			rate;			/* sampling frequency (Hz) */
	real_T			period;			/* sampling period (sec) */
	real_T			canon[NDIMS];	/* user-to-canonical conversion factors */
	real_T			units[NDIMS];	/* canonical-to-user conversion factors */
	real_T			param[NPARAMS];	/* simulation parameters */
	uint_T			spoolDelay;		/* s/v spool delay (samples) */
	uint_T			spoolInsert;	/* s/v spool delay state insertion index */
	real_T			*spoolState;	/* s/v spool delay state */

	/* precomputed parameters (for efficiency) */
	real_T			invSatOpen;		/* inverse valve saturation opening */
	real_T			flowGain[4];	/* flow gains */
	real_T			fixedDispl[2];	/* fixed displacement */
	boolean_T		active[2];		/* active cylinders */

	/* states */
	real_T			oilDispl[2];	/* oil spring displacements */
} ivars;							/* instance variables */

/******************************************************************************/
/* variables */

/******************************************************************************/
/* auxiliary functions */

static int_T irnd(x)
	real_T	x;
{	int_T	ix;

	ix = (int)x;
	if (x > 0.0)
	{	if ((x - (real_T)ix) >= 0.5)
			ix++;
	}
	else
	{	if (((real_T)ix - x) >= 0.5)
			ix--;
	}		
	return ix;
}

static real_T sqr(x)
	real_T x;
{
	return x * x;
}

/******************************************************************************/
/* delay line procedure */

static real_T dly
(	uint_T	size,	 	/* delay line size n (>= 0) */
	uint_T	*insert,	/* state insertion index i (0 <= i <= n-1) */
	real_T	inp,	 	/* input value */
	real_T	*state		/* state array of length n */
)
{   real_T	out;
	int_T	i;

	if (size)
	{	i = *insert + 1;
		if (i >= size)
			i = 0;
		*insert = i;
		out = state[i];
		state[i] = inp;
	}
	else
		out = inp;
	return out;
}

/******************************************************************************/
/* forward model */

static void doit(self)
	ivars	*self;
{	real_T	displ[2], veloc[2], force, splPress, rtnPress, dpress[2], maxPress;
	real_T	valveOpen, alpha;
	real_T	open[4], drop[4], flow[4];
	real_T	portFlow[2], kinFlow[2], compFlow[2], leakFlow[2];
	real_T	oilDispl[2], oilVeloc[2], koil[2];
	real_T	press[2];

	/* read inputs and scale to canonical units */
	displ[C1]  = self->inp[DISPL_INP]	   * self->canon[DISPL];
	displ[C2]  = -displ[C1];
	veloc[C1]  = self->inp[VELOC_INP]	   * self->canon[VELOC];
	veloc[C2]  = -veloc[C1];
	splPress   = self->xinp[SPLPRESS_XINP] * self->canon[PRESS];
	rtnPress   = self->xinp[RTNPRESS_XINP] * self->canon[PRESS];
	press[C1]  = self->xout[C1PRESS_XOUT]  * self->canon[PRESS];
	press[C2]  = self->xout[C2PRESS_XOUT]  * self->canon[PRESS];
	dpress[C1] = press[C1] - press[C2];
	dpress[C2] = -dpress[C1];
	maxPress   = 2.0 * splPress;

	/* servovalve dynamics */
	valveOpen = dly(self->spoolDelay, &self->spoolInsert,
					self->inp[VALVE_INP], self->spoolState);
	alpha	  = 0.1 * valveOpen;

	/* port openings */
	open[S1] =  alpha - self->param[OVERLAP];
	open[S1] =  clip(open[S1], 0.0, 1.0);
	open[R2] =  open[S1];
	open[S2] = -(alpha + self->param[OVERLAP]);
	open[S2] =  clip(open[S2], 0.0, 1.0);
	open[R1] =  open[S2];

	/* cylinder 1 dynamics */
	if (self->active[C1])
	{
		/* close off ports if in end cushion */
		if (displ[C1] >= self->param[STROKE1])
		{	open[S1] = 0.0;
			open[R1] = 0.0;
		}

		/* port flows */
		drop[S1]	 = splPress - press[C1];
		flow[S1]	 = open[S1] * self->flowGain[S1]
					 * sgn(drop[S1]) * sqrt(absx(drop[S1]))
					 / sqrt(1.0 + sqr(open[S1] * self->invSatOpen));
		drop[R1]	 = rtnPress - press[C1];
		flow[R1]	 = open[R1] * self->flowGain[R1]
					 * sgn(drop[R1]) * sqrt(absx(drop[R1]))
					 / sqrt(1.0 + sqr(open[R1] * self->invSatOpen));
		portFlow[C1] = flow[S1] + flow[R1];

		/* chamber pressure */
		koil[C1]	 = self->param[AREA1] * self->param[BULKMODULUS]
					 / (self->fixedDispl[C1] + displ[C1]);
		kinFlow[C1]	 = self->param[AREA1] * veloc[C1];
		leakFlow[C1] = self->param[LEAKAGE] * dpress[C1];
		compFlow[C1] = portFlow[C1] - kinFlow[C1] - leakFlow[C1];
		oilVeloc[C1] = compFlow[C1] / self->param[AREA1];
		oilDispl[C1] = self->oilDispl[C1] + oilVeloc[C1] * self->period;
		press[C1]	 = (  koil[C1] * oilDispl[C1]
					 + self->param[DAMPING] * oilVeloc[C1])
					 / self->param[AREA1];
		if (press[C1] > maxPress)
		{	press[C1]	 = maxPress;
			oilDispl[C1] = press[C1] * self->param[AREA1] / koil[C1];
		}
		else if (press[C1] < 0.0)
		{	press[C1]	 = 0.0;
			oilDispl[C1] = 0.0;
		}
	}
	else
	{	portFlow[C1] = 0.0;
		kinFlow[C1]	 = 0.0;
		leakFlow[C1] = 0.0;
		compFlow[C1] = 0.0;
		press[C1]	 = 0.0;
		oilDispl[C1] = 0.0;
	}

	/* cylinder 2 dynamics */
	if (self->active[C2])
	{
		/* close off ports if in end cushion */
		if (displ[C1] <= -self->param[STROKE1])
		{	open[S1] = 0.0;
			open[R1] = 0.0;
		}

		/* port flows */
		drop[S2]	 = splPress - press[C2];
		flow[S2]	 = open[S2] * self->flowGain[S2]
					 * sgn(drop[S2]) * sqrt(absx(drop[S2]))
					 / sqrt(1.0 + sqr(open[S2] * self->invSatOpen));
		drop[R2]	 = rtnPress - press[C2];
		flow[R2]	 = open[R2] * self->flowGain[R2]
					 * sgn(drop[R2]) * sqrt(absx(drop[R2]))
					 / sqrt(1.0 + sqr(open[R2] * self->invSatOpen));
		portFlow[C2] = flow[S2] + flow[R2];

		/* chamber pressure */
		koil[C2]	 = self->param[AREA2] * self->param[BULKMODULUS]
					 / (self->fixedDispl[C2] + displ[C2]);
		kinFlow[C2]	 = self->param[AREA2] * veloc[C2];
		leakFlow[C2] = self->param[LEAKAGE] * dpress[C2];
		compFlow[C2] = portFlow[C2] - kinFlow[C2] - leakFlow[C2];
		oilVeloc[C2] = compFlow[C2] / self->param[AREA2];
		oilDispl[C2] = self->oilDispl[C2] + oilVeloc[C2] * self->period;
		press[C2]	 = (  koil[C2] * oilDispl[C2]
					 + self->param[DAMPING] * oilVeloc[C2])
					 / self->param[AREA2];
		if (press[C2] > maxPress)
		{	press[C2]	 = maxPress;
			oilDispl[C2] = press[C2] * self->param[AREA2] / koil[C2];
		}
		else if (press[C2] < 0.0)
		{	press[C2]	 = 0.0;
			oilDispl[C2] = 0.0;
		}
	}
	else
	{	portFlow[C2] = 0.0;
		kinFlow[C2]	 = 0.0;
		leakFlow[C2] = 0.0;
		compFlow[C2] = 0.0;
		press[C2]	 = 0.0;
		oilDispl[C2] = 0.0;
	}

	/* available force output */
	force = press[C1] * self->param[AREA1] - press[C2] * self->param[AREA2];

	/* scale to desired units and output results */
	self->out = force * self->units[FORCE];

	/* scale to desired units and output auxiliary results */
	self->xout[VALVEOPEN_XOUT]	= valveOpen;
	self->xout[C1PRESS_XOUT]	= press[C1]				  * self->units[PRESS];
	self->xout[C2PRESS_XOUT]	= press[C2]				  * self->units[PRESS];
	self->xout[DELTAPRESS_XOUT]	= (press[C1] - press[C2]) * self->units[PRESS];
	self->xout[FORCE_XOUT]		= force					  * self->units[FORCE];
	self->xout[VELOC_XOUT]		= veloc[C1]				  * self->units[VELOC];
	self->xout[C1FLOW_XOUT]		= portFlow[C1]			  * self->units[FLOW];
	self->xout[C2FLOW_XOUT]		= portFlow[C2]			  * self->units[FLOW];
	self->xout[TOTALFLOW_XOUT]	= 0.5*(portFlow[C1]-portFlow[C2])
														  * self->units[FLOW];
	self->xout[KINFLOW_XOUT]	= 0.5*(kinFlow[C1]-kinFlow[C2])
														  * self->units[FLOW];
	self->xout[COMPFLOW_XOUT]	= 0.5*(compFlow[C1]-compFlow[C2])
														  * self->units[FLOW];
	self->xout[LEAKFLOW_XOUT]	= 0.5*(leakFlow[C1]-leakFlow[C2])
														  * self->units[FLOW];

	/* update states */
	self->oilDispl[C1] = oilDispl[C1];
	self->oilDispl[C2] = oilDispl[C2];
}

/******************************************************************************/
/* S-function methods */

static void mdlInitializeSizes(SimStruct *S)
{	void	mdlCheckParameters();
	int_T	i;

	/* parameters */
	ssSetNumSFcnParams(S, 1);
	if (ssGetNumSFcnParams(S) != ssGetSFcnParamsCount(S))
		return;	/* parameter mismatch will be reported by Simulink */
	mdlCheckParameters(S);
	if (ssGetErrorStatus(S))
		return;
	ssSetSFcnParamTunable(S, 0, TRUE);

	/* states */
	ssSetNumContStates(S, 0);
	ssSetNumDiscStates(S, 0);

	/* inputs */
	if (!ssSetNumInputPorts(S, NINPS+NXINPS))
		return;
	for (i=0; i<(NINPS+NXINPS); i++)
	{	ssSetInputPortWidth(S, i, 1);
		ssSetInputPortDirectFeedThrough(S, i, FALSE);
	}

	/* outputs */
	if (!ssSetNumOutputPorts(S, 2))
		return;
	ssSetOutputPortWidth(S, 0, 1);		/* valve cmd output */
	ssSetOutputPortWidth(S, 1, NXOUTS);	/* auxiliary diagnostic outputs */

	/* other */
	ssSetNumSampleTimes(S, 1);
	ssSetNumRWork(S, 0);
	ssSetNumIWork(S, 0);
	ssSetNumPWork(S, 0);
	ssSetNumModes(S, 0);
	ssSetNumNonsampledZCs(S, 0);
	ssSetOptions(S, (SS_OPTION_EXCEPTION_FREE_CODE |
					 SS_OPTION_CALL_TERMINATE_ON_EXIT));
}

static void mdlInitializeSampleTimes(SimStruct *S)
{	real_T	*params;

	params = mxGetPr(ssGetSFcnParam(S, 0));
	ssSetSampleTime(S, 0, INHERITED_SAMPLE_TIME);
	ssSetOffsetTime(S, 0, 0.0);
}

static void mdlCheckParameters(SimStruct *S)
{	int_T	nparams;

	if(mxIsEmpty(ssGetSFcnParam(S, 0)))
	{	ssSetErrorStatus(S, "(mdlCheckParameters) Empty parameter array.");
		return;
	}
    nparams = mxGetNumberOfElements(ssGetSFcnParam(S, 0));
	if (nparams != NPARAMS)
	{	ssSetErrorStatus(S, "(mdlCheckParameters) Incorrect # of parameters.");
		return;
	}
}

static void mdlProcessParameters(SimStruct *S)
{	ivars	*self;
	real_T	*params;
	real_T	linearity;
	int_T	i;

	self = (ivars *)ssGetUserData(S);

	/* input parameter array */
	params = mxGetPr(ssGetSFcnParam(S, 0));
	for (i=0; i<NPARAMS; i++)
		self->param[i] = params[i];

	/* compute internal parameters */
	self->period		= self->param[SAMPLEPERIOD];
	self->rate			= 1.0 / self->period;
	self->spoolDelay	= irnd(absx(self->param[VALVEDELAY])*self->rate);
	linearity			= self->param[RATEDFLOW] / self->param[NOMINFLOW];
	self->invSatOpen	= sqrt(1.0 - sqr(linearity)) / linearity;
	self->flowGain[S1]	= (  self->param[RATEDFLOW]
						   / sqrt(0.5 * self->param[RATEDPRESS]))
						/ linearity;
	self->flowGain[R1]	= self->flowGain[S1];
	self->flowGain[S2]	= self->flowGain[S1];
	self->flowGain[R2]	= self->flowGain[S1];
	self->active[C1]	= (self->param[AREA1] != 0.0);
	self->active[C2]	= (self->param[AREA2] != 0.0);
	if (   (self->param[RATIOPORTS] != 0.0)
		&& (self->active[C1] && self->active[C2]))
	{	self->flowGain[R1] *= sqrt(  self->param[AREA1]
								   / self->param[AREA2]);
		self->flowGain[S2] *= sqrt(  self->param[AREA2]
								   / self->param[AREA1]);
		self->flowGain[R2] *=	  (  self->param[AREA2]
								   / self->param[AREA1]);
	}
	self->fixedDispl[C1]= self->param[STROKE1] + self->param[ENDLENGTH1];
	self->fixedDispl[C2]= self->param[STROKE2] + self->param[ENDLENGTH2];
	self->canon[DISPL]	= self->param[DISPLCANON];
	self->canon[VELOC]	= self->param[VELOCCANON];
	self->canon[FORCE]	= self->param[FORCECANON];
	self->canon[PRESS]	= self->param[PRESSCANON];
	self->canon[FLOW]	= self->param[FLOWCANON];
	for (i=0; i<NDIMS; i++)
		self->units[i] = 1.0 / self->canon[i];
}

static void mdlStart(SimStruct *S)
{	ivars			*self;

	/* create instance vars */
	self = (ivars *)malloc(sizeof(ivars));
	if (!self)
	{	ssSetErrorStatus(S, "(mdlStart) Could not allocate ivars.");
		return;
	}
	ssSetUserData(S, self);

	/* check and process parameters */
	mdlCheckParameters(S);
	mdlProcessParameters(S);

	/* allocate delay line */
	self->spoolState = (real_T *)malloc(self->spoolDelay*sizeof(real_T));
	if (!self->spoolState)
	{	ssSetErrorStatus(S, "(mdlStart) Could not allocate delay line.");
		return;
	}
}

static void mdlInitializeConditions(SimStruct *S)
{	ivars				*self;
	InputRealPtrsType	u;
	real_T				splPress, rtnPress, strutPress[2], koil[2], oilDispl[2];
	int_T				i;

	self = (ivars *)ssGetUserData(S);

	for (i=0; i<NINPS; i++)
	{	u = ssGetInputPortRealSignalPtrs(S, i);
		self->inp[i] = *u[0];
	}
	for (i=0; i<NXINPS; i++)
	{	u = ssGetInputPortRealSignalPtrs(S, i+NINPS);
		self->xinp[i] = *u[0];
	}
	self->out = 0.0;
	for (i=0; i<NXOUTS; i++)
		self->xout[i] = 0.0;
	splPress		= self->xinp[SPLPRESS_XINP] * self->canon[PRESS];
	rtnPress		= self->xinp[RTNPRESS_XINP] * self->canon[PRESS];
	if (self->active[C1])
	{	strutPress[C1]	= (self->param[AREA2] * (splPress + rtnPress)
						/ (self->param[AREA1] + self->param[AREA2]));
		koil[C1]		= self->param[AREA1] * self->param[BULKMODULUS]
						/ self->fixedDispl[C1];
		oilDispl[C1]	= strutPress[C1] * self->param[AREA1] / koil[C1];
	}
	else
	{	strutPress[C1]	= 0.0;
		oilDispl[C1]	= 0.0;
	}
	if (self->active[C2])
	{	strutPress[C2]	= (self->param[AREA1] * (splPress + rtnPress)
						/ (self->param[AREA1] + self->param[AREA2]));
		koil[C2]		= self->param[AREA2] * self->param[BULKMODULUS]
						/ self->fixedDispl[C2];
		oilDispl[C2]	= strutPress[C2] * self->param[AREA2] / koil[C2];
	}
	else
	{	strutPress[C2]	= 0.0;
		oilDispl[C2]	= 0.0;
	}
	self->spoolInsert	= 0;
	for (i=0; i<self->spoolDelay; i++)
		self->spoolState[i] = 0.0;
	self->xout[C1PRESS_XOUT] = strutPress[C1] * self->units[PRESS];
	self->xout[C2PRESS_XOUT] = strutPress[C2] * self->units[PRESS];
	self->oilDispl[C1]		 = oilDispl[C1];
	self->oilDispl[C2]		 = oilDispl[C2];
}

static void mdlOutputs(SimStruct *S, int_T tid)
{	ivars	*self;
	real_T	*y;
	int_T	i;

	self = (ivars *)ssGetUserData(S);

	y	 = ssGetOutputPortRealSignal(S, 0);
	y[0] = self->out;
	y	 = ssGetOutputPortRealSignal(S, 1);
	for (i=0; i<NXOUTS; i++)
		y[i] = self->xout[i];
}

static void mdlUpdate(SimStruct *S, int_T tid)
{	ivars				*self;
	InputRealPtrsType	u;
	int_T				i;

	self = (ivars *)ssGetUserData(S);

	for (i=0; i<NINPS; i++)
	{	u = ssGetInputPortRealSignalPtrs(S, i);
		self->inp[i] = *u[0];
	}
	for (i=0; i<NXINPS; i++)
	{	u = ssGetInputPortRealSignalPtrs(S, i+NINPS);
		self->xinp[i] = *u[0];
	}
	doit(self);
}

static void mdlTerminate(SimStruct *S)
{	ivars	*self;

	self = (ivars *)ssGetUserData(S);
	if (self)
	{	if (self->spoolState)
			free(self->spoolState);
        free(self);
	}
}

/******************************************************************************/
/* Simulink postamble */

#ifdef MATLAB_MEX_FILE			/* Is this file being compiled as a MEX-file? */
	#include "simulink.c"		/* MEX-file interface mechanism */
#else
	#include "cg_sfun.h"		/* Code generation registration function */
#endif

/******************************************************************************/
