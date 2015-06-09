//
//  LightController.h
//  Server
//
//  Created by Christian Clark on 1/22/13.
//  Copyright (c) 2013 chachi.clark@gmail.com. All rights reserved.
//

#ifndef Server_LightController_h
#define Server_LightController_h

#include <iostream>
#include <string.h>
#include "ofMain.h"
#include <map>
#include <vector>
#include "MbedComm.h"
#include "Pixel.h"
#include <CL/opencl.h>
#include "DeviceManager.h"
#include "LEDType.h"

class DeviceManager;

class LightController
{
private:
	int id;
	string name;
    MbedComm* mBed;
    int pixelQuantity;
    vector<int>* pixelsIDs;
    DeviceManager* DM;
    
public:
	LightController(void);
	LightController(int iId, string iName, string iIPAdress, bool iEnabled, int iUDPPort, DeviceManager* iDM);
	~LightController(void);
	void update();
    void draw();
    void setup();
    bool sendFrameToLightController();
    void addPixel(int newPixelID);
    void setDeviceEnableState(bool newState);
    void createAndStorePacketToSend(DTFrame* totalFrame);
    void frameCleanUp();
    int getId();
    string getName();
    bool enabledDevice;
    bool prevEnabled;
private:
    DTFrame* buildFrameToTransmit(DTFrame* totalFrame);
    void sendFrameToDevice(DTFrame* newFrame);
    
};


#endif
