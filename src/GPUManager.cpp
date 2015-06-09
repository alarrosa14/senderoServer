//
//  GPUManager.cpp
//  Server
//
//  Created by Christian Clark on 2/12/13.
//  Copyright (c) 2013 chachi.clark@gmail.com. All rights reserved.
//

#include "GPUManager.h"


GPUManager::GPUManager(int iPixelQuantity){
    pixelQuantityGPU=iPixelQuantity;
    
    dummyBuff = new float[iPixelQuantity*4];
    // outBuffGPU = new float[iPixelQuantity*4]; 
    
    //cargar dummybuff con 0s
    for(int i=0;  i<iPixelQuantity*4; i++){
        dummyBuff[i] = 0.0f;
    }
    this->initialCall=true;
    
}

void GPUManager::setup(float* iAppFrame, vector<float*>* framesToBlend, vector<float>* blendFactors, vector<bool>* availableFrames, float decayFactor){
    //preparing output buffer
    decayGPU=decayFactor;
    
	outBuffGPU = new float[pixelQuantityGPU*4]; 
    
	//initialize outBuffer en el appframe con decay;
	for (int i=0;i<pixelQuantityGPU;i++){
		outBuffGPU[(4*i)] =(iAppFrame[(4*i)]*(1- decayGPU)) + (0 * decayGPU); //R
		outBuffGPU[(4*i)+1] = (iAppFrame[(4*i)+1]*(1- decayGPU)) + (0 * decayGPU); //G
		outBuffGPU[(4*i)+2] = (iAppFrame[(4*i)+2]*(1- decayGPU)) + (0 * decayGPU); //B
		outBuffGPU[(4*i)+3] = 1.0f; //A
        
		if(outBuffGPU[(4*i)]>255)
			outBuffGPU[(4*i)]=255;
		if(outBuffGPU[(4*i)+1]>255)
			outBuffGPU[(4*i)+1]=255;
		if(outBuffGPU[(4*i)+2]>255)
			outBuffGPU[(4*i)+2]=255;
		if(outBuffGPU[(4*i)+3]>255)
			outBuffGPU[(4*i)+3]=255;
	}
	
	for(int v=0;v<framesToBlend->size();v++){
		
		float* currentFrame = framesToBlend->operator[](v);
		float blendFact = blendFactors->operator[](v);
		bool available =  availableFrames->operator[](v);
        
		if(available){
			for (int i=0;i<pixelQuantityGPU;i++){
				//if ((currentFrame[(4*i)]!=0) || (currentFrame[(4*i)+1]!=0) || (currentFrame[(4*i)+2]!=0)){ //this is so zero values wont diminish ligh on painted pixels TODO---> erase or migrate to alpha blending
					outBuffGPU[(4*i)] =(outBuffGPU[(4*i)]*(1- blendFact)) + (currentFrame[(4*i)] * blendFact); //R
					outBuffGPU[(4*i)+1] = (outBuffGPU[(4*i)+1]*(1- blendFact)) + (currentFrame[(4*i)+1] * blendFact); //G
					outBuffGPU[(4*i)+2] = (outBuffGPU[(4*i)+2]*(1- blendFact)) + (currentFrame[(4*i)+2] * blendFact); //B
					outBuffGPU[(4*i)+3] = 1.0f; //A
				//}
				if(outBuffGPU[(4*i)]>255)
					outBuffGPU[(4*i)]=255;
				if(outBuffGPU[(4*i)+1]>255)
					outBuffGPU[(4*i)+1]=255;
				if(outBuffGPU[(4*i)+2]>255)
					outBuffGPU[(4*i)+2]=255;
				if(outBuffGPU[(4*i)+3]>255)
					outBuffGPU[(4*i)+3]=255;
			}
		}
        
	}
    
}

void GPUManager::execute(){
	// all execution gets done in setup :S
}


float* GPUManager::cleanup(float* returnedFrame){
    
    
    //memcpy(outBuffGPU, ptr, sizeof(float)*(pixelQuantityGPU*4));
    
    // And the same goes for system memory, as usual.
    if (returnedFrame!=NULL){
        delete [] returnedFrame;
        returnedFrame=NULL;
    }
    
    returnedFrame= outBuffGPU;
    // outBuffGPU = new float[pixelQuantityGPU*4]; 
    
    return returnedFrame;
    
    
}

void GPUManager::exit(){
    
    delete [] dummyBuff;
    delete [] outBuffGPU;
}


GPUManager::~GPUManager(void){
    
    //free(this->outBuff);
}