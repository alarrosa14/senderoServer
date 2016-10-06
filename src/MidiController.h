//
//  MidiController.h
//  Lumitron
//
//  Created by Christian Clark on 3/30/13.
//  Copyright (c) 2013 chachi.clark@gmail.com. All rights reserved.
//

#ifndef Lumitron_MidiController_h
#define Lumitron_MidiController_h

#include "ofMain.h"
#include "ofxMidi.h"
#include "DTFrame.h"
#include "DTPixel.h"
#include "MidiGroup.h"
#include <vector>

class MidiController: public ofThread
{
private:
    
    ofxMidiOut midiOut;
    
    vector<MidiGroup*>* groups;
    
    vector<DTFrame*> processBuffer;
       
    int baseNote; //note for 'cushion' sound
    int baseNoteChannel;
    
    int thresholdNoteON; //threshold to note ON
    int thresholdNoteOFF; //threshold to note OFF
    
    int thresholdNoteGlobalON; //threshold to note ON
    int thresholdNoteGlobalOFF; //threshold to note OFF
    
    int groupQuantity;
    
    float ellapsedMillis;
    
    int globalIllumination;
    
    bool globalIsNoteOn;
    
    float velocityMultiplier;
    float velocityMultiplierGlobal;
    
    void threadedFunction();
    
public:
    
	MidiController(void);
	~MidiController(void);
    
    void addFrameToMidiBuffer(DTFrame* newFrame);

    void setThresholdON(int newThreshold);
    void setThresholdOFF(int newThreshold);
    
    int getThresholdON();
    int getThresholdOFF();
    
    void setThresholdGlobalON(int newThreshold);
    void setThresholdGlobalOFF(int newThreshold);
    
    int getThresholdGobalON();
    int getThresholdGlobalOFF();
    
    float getVelocityMultiplier();
    float getVelocityMultiplierGlobal();
    
    void setVelocityMultiplier(float newMult);
    void setVelocityMultiplierGlobal(float newMult);
    

    void setup();
    void processFrame(DTFrame* newFrame);
    void draw();
    void exit();

};


#endif
