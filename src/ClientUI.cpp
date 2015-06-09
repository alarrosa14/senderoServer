//
//  ClientUI.cpp
//  Server
//
//  Created by Christian Clark on 2/22/13.
//  Copyright (c) 2013 chachi.clark@gmail.com. All rights reserved.
//

#include "ClientUI.h"

ClientUI::ClientUI(void){
    
}

ClientUI::ClientUI(int iId, string iName, bool iEnabled,float iblendFactor, int iFps){
    this->id=iId;
    this->name=iName;
    this->blendFactor=iblendFactor;
    this->enabled=iEnabled;
    
    this->enabledPrev=iEnabled;
    this->blendFactorPrev=iblendFactor;
    
    this->fps=iFps;
    this->fpsPrev=iFps;
}

ClientUI::~ClientUI(void){
    
}

int ClientUI::getId(){
    return this->id;
}

string ClientUI::getName(){
    return this->name;
}

bool ClientUI::isEnabled(){
    return this->enabled;
}

float ClientUI::getblendFactor(){
    return this->blendFactor;
}

int ClientUI::getFps(){
    return this->fps;
}

void ClientUI::setName(string newName){
    this->name=newName;
}

void ClientUI::setEnabled(bool isEnabled){
    this->enabled=isEnabled;
}

void ClientUI::setEnabledPrev(bool isEnabled){
    this->enabledPrev=isEnabled;
}

void ClientUI::setBlendFactor(float newBlendFactor){
    this->blendFactor=newBlendFactor;
}

void ClientUI::setBlendFactorPrev(float newBlendFactor){
    this->blendFactorPrev=newBlendFactor;
}

void ClientUI::setFps(int newFps){
    this->fps=newFps;
}

void ClientUI::setFpsPrev(int newFps){
    this->fpsPrev=newFps;
}