//
//  MidiGroup.h
//  Lumitron
//
//  Created by Christian Clark on 4/1/13.
//  Copyright (c) 2013 chachi.clark@gmail.com. All rights reserved.
//

#ifndef Lumitron_MidiGroup_h
#define Lumitron_MidiGroup_h

#include "ofMain.h"
#include "ofxMidi.h"
#include "DTFrame.h"
#include <vector>

class MidiGroup
{
    
    int channel;
	int note;
    int id;
    int intensity;
    bool isNoteON;
    int currentValue;
    int pixelsProcessed;

public:
    
	MidiGroup(void);
    MidiGroup(int iChannel, int iNote, int iId);
	~MidiGroup(void);
    
    int getIntensity();
    bool getIsNoteOn();
    int getChannel();
    int getNote();
    int getId();
    void setChannel(int newChannel);
    void setNote(int newNote);
    void setIntensity(int newIntensity);
    void setIsNoteOn(bool newNoteOn);
    void setCurrentValue(int newCurrentValue);
    void setPixelsProcessed(int newPixelsProcessed);
    
    int getCurrentValue();
    int getPixelsProcessed();
    
};


#endif
