//
//  LinearColorCalibrator.h
//  LEDColorCalibrator
//
//  Created by Tomas Laurenzo on 4/20/13.
//
//

#ifndef __LEDColorCalibrator__LinearColorCalibrator__
#define __LEDColorCalibrator__LinearColorCalibrator__

#include <iostream>
#include "ofMain.h"

extern "C" void dgesv_( int* n, int* nrhs, double* a, int* lda, int* ipiv,
    double *x, int *incx, int *info);

// info on Accelerate:
//      http://www.cs.rochester.edu/~bh/cs400/using_lapack.html
//      http://www.netlib.org/lapack/double/dgesv.f

class LinearColorCalibrator {
    
public:
    LinearColorCalibrator();
    
    ofColor getCalibratedColor(ofColor color);
    void addEquivalentPair (ofColor first, ofColor second);
    
    void setGamma(float gamma);
    float getGamma();
    void useGamma();
    void dontUseGamma();
    
    void reset();
    
private:
    bool isCalculated;
    
    bool gammaCorrecting;
    float gamma;
    
    bool calculateCalibration();

    vector<pair <ofColor, ofColor> > pairs;
    float T[3][3]; // the transformation matrix from monitor to LED

};

#endif /* defined(__LEDColorCalibrator__LinearColorCalibrator__) */
