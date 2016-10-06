//
//  DeviceUI.cpp
//  Server
//
//  Created by Christian Clark on 2/22/13.
//  Copyright (c) 2013 chachi.clark@gmail.com. All rights reserved.
//

#include "DeviceUI.h"

DeviceUI::DeviceUI(void){
    
}
DeviceUI::DeviceUI(int iId, string iName,bool iEnabled){
    this->id=iId;
    this->enabled=iEnabled;
    this->enabledPrev=iEnabled;
    this->name=iName;
}
DeviceUI::~DeviceUI(void){
    
}

int DeviceUI::getId(){
    return this->id;
}
bool DeviceUI::isEnabled(){
    return this->enabled;
}

void DeviceUI::setEnabled(bool isEnabled){
    this->enabled=isEnabled;
}

void DeviceUI::setEnabledPrev(bool isEnabled){
    this->enabledPrev=isEnabled;
}

string DeviceUI::getName(){
    return this->name;
}