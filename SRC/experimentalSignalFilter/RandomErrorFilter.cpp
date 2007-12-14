/* ****************************************************************** **
**    OpenFRESCO - Open Framework                                     **
**                 for Experimental Setup and Control                 **
**                                                                    **
**                                                                    **
** Copyright (c) 2006, Yoshikazu Takahashi, Kyoto University          **
** All rights reserved.                                               **
**                                                                    **
** Licensed under the modified BSD License (the "License");           **
** you may not use this file except in compliance with the License.   **
** You may obtain a copy of the License in main directory.            **
** Unless required by applicable law or agreed to in writing,         **
** software distributed under the License is distributed on an        **
** "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,       **
** either express or implied. See the License for the specific        **
** language governing permissions and limitations under the License.  **
**                                                                    **
** Developed by:                                                      **
**   Yoshikazu Takahashi (yos@catfish.dpri.kyoto-u.ac.jp)             **
**   Andreas Schellenberg (andreas.schellenberg@gmx.net)              **
**   Gregory L. Fenves (fenves@berkeley.edu)                          **
**                                                                    **
** ****************************************************************** */

// $Revision$
// $Date$
// $URL: $

// Written: Yoshi (yos@catfish.dpri.kyoto-u.ac.jp)
// Created: 09/06
// Revision: A
//
// Description: This file contains the implementation of
// RandomErrorFilter.

#include "RandomErrorFilter.h"

#include <stdlib.h>
#include <math.h>
#include <time.h>


RandomErrorFilter::RandomErrorFilter(int tag, double ave, double std)
    : ErrorFilter(tag), data(0.0),
    mean(ave), sigma(std), idseed(0),
    Pi(atan(1.0)*4.0), sw(0), inext(0), inextp(0),
    MBIG(1000000000), MSEED(161803398), MZ(0), FAC(1.0/MBIG), ma(0),
    amp(0.0), theta(0.0)
{
    idseed = ((unsigned)time(NULL)) % MBIG;
    ma = new int [56];
    if(ma == 0) {
        opserr << "FATAL - RandomErrorFilter::RandomErrorFilter - failed to create ma\n";
    }
    idseed = ((unsigned)time(NULL)) % MBIG;
    init_rand3();
    srand(idseed);
    for(int i=0; i<100; i++)
        filtering(0.0);
}


RandomErrorFilter::RandomErrorFilter(const RandomErrorFilter& uef)
    : ErrorFilter(uef)
{
    data = uef.data;
    mean = uef.mean;
    sigma = uef.sigma;
    Pi = uef.Pi;
    sw = uef.sw;
    inext = uef.inext;
    inextp = uef.inextp;
    MBIG = uef.MBIG;
    MSEED = uef.MSEED;
    MZ = uef.MZ;
    FAC = uef.FAC;
    ma = new int[56];
    
    idseed = ((unsigned)time(NULL)) % MBIG;
    init_rand3();
    srand(idseed);
    for(int i=0; i<100; i++)
        filtering(0.0);
}


RandomErrorFilter::~RandomErrorFilter()
{
    if(ma != 0)
        delete [] ma;
}


double RandomErrorFilter::filtering(double d)
{
    data = d;
    data += gen_rand();
    //  opserr << "U : " << d << ", err = " << d-data << endln;
    return data;
}


void RandomErrorFilter::update()
{
    // does nothing
}


SignalFilter* RandomErrorFilter::getCopy()
{
    return new RandomErrorFilter(*this);
}


void RandomErrorFilter::Print(OPS_Stream &s, int flag =0)
{
    s << "Filter: " << this->getTag(); 
    s << " type: RandomErrorFilter\n";
    s << "\tmean = " << mean << ", sigma = " << sigma << endln;
}


void RandomErrorFilter::init_rand3()
{
    // Numerical Recipes in C++, 2nd Ed., pp.287
    int i, ii, k, mj, mk;
    
    mj = labs(MSEED-labs(idseed));      // Initialize ma[55] using the seed
    mj %= MBIG;                         // idseed and the large number MSEED.
    ma[55] = mj;
    mk = 1;
    for(i=1; i<=54; i++) {              // Now initialize the rest of the table,
        ii=(21*i)%55;                     // in a slightly random order,
        ma[ii] = mk;                      // with numbers that are not especially 
        mk = mj-mk;                       // random.
        if(mk < int(MZ)) mk += MBIG;
        mj = ma[ii];
    }
    for(k=0; k<4; k++)                  // We randomize them by "warpping up
        for(i=1; i<=55; i++) {            // the generator".
            ma[i] -= ma[1+(i+30)%55];
            if(ma[i] < int(MZ)) ma[i] += MBIG;
        }
        
        inext = 0;                  // Prepare indices for our first generated 
        inextp = 31;                // number. The contant 31 is special; see Knuth.
}


double RandomErrorFilter::rand3()
{
    // Numerical Recipes in C++, 2nd Ed., pp.287
    int mj;
    
    if(++inext == 56) inext=1;      // Increment inext and inextp, wrapping
    if(++inextp == 56) inextp = 1;  // around 56 to 1
    mj = ma[inext]-ma[inextp];      // Generate a new random number subtractively
    if(mj < int(MZ)) mj += MBIG;    // Be sure that it is in range
    ma[inext] = mj;
    
    return mj*FAC;       // Output the derived uniform deviate between 0.0 .. 1.0
}


double RandomErrorFilter::rand1()
{
    // simple but not recommended random generator
    return (1.0 / (RAND_MAX + 1.0)) * rand();
}


double RandomErrorFilter::gen_rand()
{
    double nd;
    
    amp  = sqrt(-2 * log(rand3()));
    theta  = 2 * Pi * rand3();
    if(sw == 0) {
        sw = 1;
        nd = amp*cos(theta);
    } else {
        sw = 0;
        nd = amp*sin(theta);
    }
    return mean + sigma*nd;
}

