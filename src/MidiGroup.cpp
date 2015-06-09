//
//  MidiGroup.cpp
//  Lumitron
//
//  Created by Christian Clark on 4/1/13.
//  Copyright (c) 2013 chachi.clark@gmail.com. All rights reserved.
//

#include "MidiGroup.h"



MidiGroup::MidiGroup(void){
    this->channel=1;
    this->note=1;
    this->id=0;
    this->intensity=0;
    this->currentValue=0;
    this->pixelsProcessed=0;

}

MidiGroup::MidiGroup(int iChannel, int iNote, int iId){
    this->channel=iChannel;
    this->note=iNote;
    this->id=iId;
    this->intensity=0;
    this->isNoteON=false;
    this->currentValue=0;
    this->pixelsProcessed=0;
}

MidiGroup::~MidiGroup(void){
    //dso nothing ... yet
}

int MidiGroup::getChannel(){
    return this->channel;
}

int MidiGroup::getNote(){
    return this->note;
}

int MidiGroup::getId(){
    return this->id;
}

void MidiGroup::setChannel(int newChannel){
    this->channel=newChannel;
}

void MidiGroup::setNote(int newNote){
    this->note=newNote;
}

void MidiGroup::setIntensity(int newIntensity){
    this->intensity=newIntensity;
}

void MidiGroup::setIsNoteOn(bool newNoteOn){
    this->isNoteON=newNoteOn;
}

int MidiGroup::getIntensity(){
    return this->intensity;
}

bool MidiGroup::getIsNoteOn(){
    return this->isNoteON;
}

void MidiGroup::setCurrentValue(int newCurrentValue){
    this->currentValue=newCurrentValue;
}

void MidiGroup::setPixelsProcessed(int newPixelsProcessed){
    this->pixelsProcessed=newPixelsProcessed;
}

int MidiGroup::getCurrentValue(){
    return this->currentValue;
}

int MidiGroup::getPixelsProcessed(){
    return this->pixelsProcessed;
}
