//
//  MidiController.cpp
//  Lumitron
//
//  Created by Christian Clark on 3/30/13.
//  Copyright (c) 2013 chachi.clark@gmail.com. All rights reserved.
//

#include "MidiController.h"

MidiController::MidiController(void){
    
    baseNote=50; //note for cushion sound
    baseNoteChannel=1;
    globalIllumination=0;
    
    thresholdNoteON=120; //threshold to note ON
    thresholdNoteOFF=90; //threshold to note OFF
    
    thresholdNoteGlobalON=120;
    thresholdNoteGlobalOFF=90;
    
    globalIsNoteOn = false;
    
    groupQuantity=9;
    
    velocityMultiplier=15;
    velocityMultiplierGlobal=15;
    
    groups=new vector<MidiGroup*>;
    
}

MidiController::~MidiController(void){
    
}

void MidiController::addFrameToMidiBuffer(DTFrame* newFrame){
    lock();
    
    this->processBuffer.push_back(newFrame);
    
    unlock();
}

void MidiController::threadedFunction(){
    
    while(isThreadRunning()) {
        DTFrame* frame=0;
        lock();
        if(this->processBuffer.size()>0){
            frame=this->processBuffer[0];
            vector<DTFrame*>::iterator it = this->processBuffer.begin();
            this->processBuffer.erase(it);
        }
        unlock();
        if(frame!=0){
            this->processFrame(frame);
            delete frame;
        }
        sleep(25);
    }
    exit();
}


void MidiController::processFrame(DTFrame* newFrame){
    //the frame should be destroyed by this function
    
    //udate every Midigroup
    vector<DTPixel*>* pixels = newFrame->getPixels();
    
    //calculate new values, but we dont save them yet.
    vector<DTPixel*>::iterator it = pixels->begin();
	while (it != pixels->end())
	{	
        DTPixel* px = *it;
		int pixelId= px->getId();
        int r= px->getR();
        int g= px->getG();
        int b= px->getB();
        
        int lightValue = (r + g + b) / 3;
        
        int mappedGroup = pixelId % this->groupQuantity;
        
        MidiGroup* mg = this->groups->operator[](mappedGroup);
        
        int mgValue = mg->getCurrentValue();
        mgValue+=lightValue;
        int mgValueQs = mg->getPixelsProcessed();
        mgValueQs+=1;
        
        mg->setCurrentValue(mgValue);
        mg->setPixelsProcessed(mgValueQs);
        
        it++;
	}
    
    //in each group, we have the old value, and the necessary information to calculate the new one
    //now we iterate each group in order to calculate velocities and final values.
    
    vector<MidiGroup*>::iterator itMG = groups->begin();
    float newMillis = ofGetElapsedTimeMillis();
    float millisDif = newMillis-this->ellapsedMillis;
    int newGlobalIllumination = 0;
    while (itMG != groups->end())
	{
        MidiGroup* mg=*itMG;
        
        int prevValue = mg->getIntensity();
        int newValue = mg->getCurrentValue() / mg->getPixelsProcessed();
        
        
        
        int valueDistance = abs(newValue-prevValue);
        
        float velocity = (valueDistance / millisDif) * velocityMultiplier;
        
        if (velocity>127){
            velocity=127;
        }
        
        if (mg->getIsNoteOn()){
            if (newValue<thresholdNoteOFF){
                mg->setIsNoteOn(false);
                midiOut.sendNoteOff(mg->getChannel(),mg->getNote(), (int) velocity);
                //cout << "off" << endl;
                //dcout << "velocity " << velocity;

            }
        }
        else{
            if (newValue>thresholdNoteON){
                mg->setIsNoteOn(true);
                midiOut.sendNoteOn(mg->getChannel(),mg->getNote(), (int) velocity);
                //cout << "on" << endl;
                //cout << "velocity " << velocity;

            }
        }
        
        mg->setIntensity(newValue);
        mg->setCurrentValue(0);
        mg->setPixelsProcessed(0);
        //accumulate for global illumination
        newGlobalIllumination+=newValue;
        
        itMG++;
    }
    
    int newGlobalValue=newGlobalIllumination/this->groups->size();
    int globalDist = abs(newGlobalValue - this->globalIllumination);
    int globalVelocity = (globalDist/millisDif) * velocityMultiplier;
    
    if (globalVelocity>127){
        globalVelocity=127;
    }

    if (this->globalIsNoteOn){
        if (newGlobalValue<thresholdNoteGlobalOFF){
            this->globalIsNoteOn=false;
            midiOut.sendNoteOff(this->baseNoteChannel,this->baseNote, globalVelocity);
            //cout << "off" << endl;
        }
    }
    else{
        if (newGlobalValue>thresholdNoteGlobalON){
            this->globalIsNoteOn=true;
            midiOut.sendNoteOn(this->baseNoteChannel,this->baseNote, globalVelocity);
            //cout << "on velocity:" << globalVelocity << endl;
        }
    }

    this->globalIllumination=newGlobalValue;
    this->ellapsedMillis=newMillis;
}

void MidiController::setThresholdON(int newThreshold){
    lock();
    this->thresholdNoteON=newThreshold;
    unlock();
}

void MidiController::setThresholdOFF(int newThreshold){
    lock();
    this->thresholdNoteOFF=newThreshold;
    unlock();
}

int MidiController::getThresholdON(){
    int res;
    lock();
    res=this->thresholdNoteON;
    unlock();
    return res;
}

int MidiController::getThresholdOFF(){
    int res;
    lock();
    res=this->thresholdNoteOFF;
    unlock();
    return res;
}

void MidiController::setThresholdGlobalON(int newThreshold){
    lock();
    this->thresholdNoteGlobalON=newThreshold;
    unlock();
}

void MidiController::setThresholdGlobalOFF(int newThreshold){
    lock();
    this->thresholdNoteGlobalOFF=newThreshold;    
    unlock();
}

int MidiController::getThresholdGobalON(){
    int res;
    lock();
    res=this->thresholdNoteGlobalON;
    unlock();
    return res;
}

int MidiController::getThresholdGlobalOFF(){
    int res;
    lock();
    res=this->thresholdNoteGlobalOFF;
    unlock();
    return res;
}

float MidiController::getVelocityMultiplier(){
    float res;
    lock();
    res = this->velocityMultiplier;
    unlock();
    return res;
}

float MidiController::getVelocityMultiplierGlobal(){
    float res;
    lock();
    res=this->velocityMultiplierGlobal;
    unlock();
    return res;
}

void MidiController::setVelocityMultiplier(float newMult){
    lock();
    this->velocityMultiplier=newMult;
    unlock();
}

void MidiController::setVelocityMultiplierGlobal(float newMult){
    lock();
    this->velocityMultiplierGlobal=newMult;
    unlock();
}

void MidiController::setup(){
    // print the available output ports to the console
	midiOut.listPorts(); // via instance
	
	// connect
	midiOut.openPort("IAC Driver Sendero1");	// by name TO DO: load from xml
    
    //load groups
    
    //group1
    MidiGroup* mg1 = new MidiGroup(2, 40, 0);
    MidiGroup* mg2 = new MidiGroup(3, 15, 1);
    MidiGroup* mg3 = new MidiGroup(4, 22, 2);
    MidiGroup* mg4 = new MidiGroup(5, 29, 3);    
    MidiGroup* mg5 = new MidiGroup(6, 33, 4);    
    MidiGroup* mg6 = new MidiGroup(7, 102, 5);
    MidiGroup* mg7 = new MidiGroup(8, 39, 6);    
    MidiGroup* mg8 = new MidiGroup(9, 110, 7);
    MidiGroup* mg9 = new MidiGroup(10, 115, 8);
    
	this->groups->push_back(mg1);
    this->groups->push_back(mg2);
    this->groups->push_back(mg3);
    this->groups->push_back(mg4);
    this->groups->push_back(mg5);
    this->groups->push_back(mg6);
    this->groups->push_back(mg7);
    this->groups->push_back(mg8);
    this->groups->push_back(mg9);
    
    this->groupQuantity = this->groups->size();
    
    this->ellapsedMillis = ofGetElapsedTimeMillis();
	
}

void MidiController::draw(){
    
}

void MidiController::exit(){
    // clean up
	midiOut.closePort();
    
    vector<MidiGroup*>::iterator it = this->groups->begin();
	while (it != this->groups->end())
	{	
		MidiGroup* mg = *it;
		delete mg;
		it++;
	}
	delete this->groups;
}