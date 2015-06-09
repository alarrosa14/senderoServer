//
//  LEDType.cpp
//  Lumitron
//
//  Created by Christian Clark on 4/22/13.
//  Copyright (c) 2013 chachi.clark@gmail.com. All rights reserved.
//

#include "LEDType.h"

LEDType::LEDType(int iId){
    this->id=iId;
    this->calibrator=new LinearColorCalibrator();
}

LEDType::~LEDType(void){
    delete this->calibrator;
}

ofColor LEDType::getCalibratedColor(ofColor color){
    return this->calibrator->getCalibratedColor(color);
}

void LEDType::addEquivalentPair (ofColor first, ofColor second){
    this->calibrator->addEquivalentPair(first, second);
}

void LEDType::setGamma(float gamma){
    this->calibrator->setGamma(gamma);
}

float LEDType::getGamma(){
    return this->calibrator->getGamma();
}

void LEDType::useGamma(){
    this->calibrator->useGamma();
}

void LEDType::dontUseGamma(){
    this->calibrator->dontUseGamma();
}

void LEDType::reset(){
    this->calibrator->reset();
}

int LEDType::getId(){
    return this->id;
}