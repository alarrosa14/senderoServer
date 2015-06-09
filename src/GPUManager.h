//
//  GPUManager.h
//  Server
//
//  Created by Christian Clark on 2/12/13.
//  Copyright (c) 2013 chachi.clark@gmail.com. All rights reserved.
//

#ifndef Server_GPUManager_h
#define Server_GPUManager_h

#include <stdio.h>
#include <vector>
//#include "MSAOpenCL.h"




using namespace std;



class GPUManager
{
private:
	int pixelQuantityGPU;
	float* outBuffGPU;
	float* dummyBuff;
	float decayGPU;
    
public:
	GPUManager(int iPixelQuantity);
	~GPUManager(void);
    void setup(float* iAppFrame, vector<float*>* framesToBlend, vector<float>* blendFactors, vector<bool>* availableFrames, float decayFactor);
    void execute();
    float* cleanup(float* returnedFrame);
    void exit();
    bool initialCall;
    
};




#endif
