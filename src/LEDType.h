//
//  LEDType.h
//  Lumitron
//
//  Created by Christian Clark on 4/22/13.
//  Copyright (c) 2013 chachi.clark@gmail.com. All rights reserved.
//

#ifndef Lumitron_LEDType_h
#define Lumitron_LEDType_h

#include "LinearColorCalibrator.h"
#include "ofMain.h"

class LEDType
{
private:
	int id;
    LinearColorCalibrator* calibrator;
public:
	LEDType(int iId);
	~LEDType(void);

    ofColor getCalibratedColor(ofColor color);
    void addEquivalentPair (ofColor first, ofColor second);
    
    void setGamma(float gamma);
    float getGamma();
    void useGamma();
    void dontUseGamma();
    
    void reset();
    
    int getId();
    
};


#endif
