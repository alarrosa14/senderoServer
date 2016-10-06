//
//  DeviceManager.cpp
//  Server
//
//  Created by Christian Clark on 2/19/13.
//  Copyright (c) 2013 chachi.clark@gmail.com. All rights reserved.
//

#include "DeviceManager.h"


DeviceManager::DeviceManager(){
    waitForNewFrameMutex.lock();
}

DeviceManager::~DeviceManager(void){
    
}

void DeviceManager::addLightController(LightController* newDevice){
    this->devices.push_back(newDevice);
}

void DeviceManager::addFrameToControllersBuffer(DTFrame* newFrame){
    lock();
    
    this->sendBuffer.push_back(newFrame);
    
    unlock();

    waitForNewFrameMutex.unlock();
}

void DeviceManager::threadedFunction(){
    
    while(isThreadRunning()) {
        
        waitForNewFrameMutex.lock();
        DTFrame* frame=0;
        lock();
        if(this->sendBuffer.size()>0){
            frame=this->sendBuffer[0];
            vector<DTFrame*>::iterator it = this->sendBuffer.begin();
            this->sendBuffer.erase(it);
        }
        unlock();
        if(frame!=0){
            //cout << "send" << endl; 
            this->sendFrameToDevices(frame);
            delete frame;
        }
    }
    
}

void DeviceManager::sendFrameToDevices(DTFrame* frame){
    
    // this methid should deliver the packets to the light controllers.
    // for large frames (>170 pixels), the light controllers shouls get every packet with a 1ms delay (to give them time to process it.
    // this is why we need to send the first part of the frame to all devices, then wait for 1ms, and then send the second part.
    // keep in mind that not all devices have the same amount of pixels attached to them.
    
    vector<LightController*>::iterator it = this->devices.begin();
    
    //ask every ligh controller device to create his packet for sending
    while(it!=this->devices.end()){
        //cout << "creo paquete" << endl;
        LightController* device=*it;
        device->createAndStorePacketToSend(frame);
        it++;
    }
    
    //then, while there is any light controller that needs to send data, we tell them to send.
    // after each iteration we wait 1ms to send the next packet.
    
    bool needToKeepSending=false;
    long elapsedTimeToSend = ofGetElapsedTimeMillis();
    bool firstIteration= true;
    do{
        needToKeepSending=false;
        long elapsed = ofGetElapsedTimeMillis();
        if (((elapsed-elapsedTimeToSend)>1) || firstIteration){
            firstIteration=false;
            //cout << "entro if" << endl;
            it = this->devices.begin();
            do{
                LightController* device=*it;
                bool hasRemainingPacket = device->sendFrameToLightController();
                //cout << "BRUTE" << endl;
                //cout << hasRemainingPacket << endl;
                needToKeepSending=(needToKeepSending||hasRemainingPacket);
                // BRUTE FORCE
                //needToKeepSending=false;
                // BRUTE FORCE
                it++;
            }while(it!=this->devices.end());
            elapsedTimeToSend = ofGetElapsedTimeMillis();
        }
        else{
            //cout << "slepping" << endl ;
            usleep(1000);
        }
        //cout << "Needtokeepsending" << needToKeepSending << endl;
    }while(needToKeepSending);

    //cout << " no need to send anymore " << endl;
    
    //frame cleanup
    it = this->devices.begin();
    while(it!=this->devices.end()){
        LightController* device=*it;
        //cout << "calling cleanup" << endl;
        device->frameCleanUp();
        it++;
    }

        
}


void DeviceManager::addPixelToDevice(int pixelID, int deviceID){
    
    vector<LightController*>::iterator it = this->devices.begin();
    bool done=false;
    while((it!=this->devices.end()) || (!done)){
        LightController* device=*it;
        if (device->getId()==deviceID){
            device->addPixel(pixelID);
            done=true;
        }
        it++;
    }

}


void DeviceManager::setup(){
    vector<LightController*>::iterator it = this->devices.begin();
    while(it!=this->devices.end()){
        LightController* device=*it;
        device->setup();
        it++;
    }
}

void DeviceManager::setDeviceState(int deviceId, bool state){
    lock();
    vector<LightController*>::iterator it = this->devices.begin();
    bool done=false;
    while((it!=this->devices.end()) || (!done)){
        LightController* device=*it;
        if (device->getId()==deviceId){
            device->setDeviceEnableState(state);
            done=true;
        }
        it++;
    }
    unlock();
}

void DeviceManager::addLedType(LEDType* newLedType){
    this->ledTypes.insert(pair<int,LEDType*> (newLedType->getId(),newLedType));
}

LEDType* DeviceManager::getCalibratorById(int ledTypeid){
    map<int,LEDType*>::iterator it=this->ledTypes.find(ledTypeid);
    if (it!=this->ledTypes.end()){
        return it->second;
    }
    return NULL;
}

vector<int>* DeviceManager::getLEDTypesIDs(){
    vector<int>* resVector= new vector<int>;
    map<int,LEDType*>::iterator it=this->ledTypes.begin();
    
    while(it!=this->ledTypes.end()){
        resVector->push_back(it->second->getId());
        it++;
    }
    
    return resVector;
    
}

void DeviceManager::addCalibrationPair(int idToCalibrate, int r0, int g0, int b0, int r1, int g1, int b1){
    lock();
    
    map<int,LEDType*>::iterator it=this->ledTypes.find(idToCalibrate);
    if(it!=this->ledTypes.end()){
        
        LEDType* ledToCal = it->second;
        
        ledToCal->addEquivalentPair(ofColor((float)r0,(float)g0,(float)b0), ofColor((float)r1,(float)g1,(float)b1));
        
    }
    
    
    unlock();
}




