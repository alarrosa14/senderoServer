//
//  DeviceManager.h
//  Server
//
//  Created by Christian Clark on 2/19/13.
//  Copyright (c) 2013 chachi.clark@gmail.com. All rights reserved.
//

#ifndef Server_DeviceManager_h
#define Server_DeviceManager_h

#include "ofMain.h"
#include <vector>
#include <map>
#include "LightController.h"
#include "DTFrame.h"
#include "LEDType.h"

class LightController;

class DeviceManager: public ofThread
{
public:
	DeviceManager();
	~DeviceManager(void);
    void addLightController(LightController* newDevice);
    void addFrameToControllersBuffer(DTFrame* newFrame);
    void threadedFunction();
    void addPixelToDevice(int pixelID, int deviceID);
    void setup();
    void setDeviceState(int deviceId, bool state);
    void addLedType(LEDType* newLedType);
    LEDType* getCalibratorById(int ledTypeid);
    vector<int>* getLEDTypesIDs();
    void addCalibrationPair(int idToCalibrate, int r0, int g0, int b0, int r1, int g1, int b1);
    
private:
    
    vector<LightController*> devices;
    vector<DTFrame*> sendBuffer;
    map<int,LEDType*> ledTypes;
    void sendFrameToDevices(DTFrame* frame);
    ofMutex waitForNewFrameMutex;
    
    
};
#endif
