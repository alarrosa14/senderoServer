//
//  CalibratorUI.cpp
//  Lumitron
//
//  Created by Christian Clark on 4/23/13.
//  Copyright (c) 2013 chachi.clark@gmail.com. All rights reserved.
//

#include "CalibratorUI.h"


CalibratorUI::CalibratorUI(void){
    this->id=0;
    this->r=0;
    this->g=0;
    this->b=0;
}

CalibratorUI::~CalibratorUI(void){
    
}

int CalibratorUI::getIdLedType(){
    return id;
}
int CalibratorUI::getR(){
    return r;
}
int CalibratorUI::getG(){
    return g;
}
int CalibratorUI::getB(){
    return b;
}

void CalibratorUI::setId(int newId){
    this->id=newId;
}
void CalibratorUI::setR(int newR){
    this->r=newR;
}
void CalibratorUI::setG(int newG){
    this->g=newG;
}
void CalibratorUI::setB(int newB){
    this->b=newB;
}