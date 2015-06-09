//
//  ClientUI.h
//  Server
//
//  Created by Christian Clark on 2/22/13.
//  Copyright (c) 2013 chachi.clark@gmail.com. All rights reserved.
//

#ifndef Server_ClientUI_h
#define Server_ClientUI_h

#include "ofMain.h"
#include <string.h>

class ClientUI
{
public:
    //mut be public in order to TweakBar to maintain them.
    bool enabled;
    bool enabledPrev;
	float blendFactor;
    float blendFactorPrev;
    int fps;
    int fpsPrev;
    
	ClientUI(void);
	ClientUI(int iId, string iName, bool iEnabled,float iblendFactor, int iFps);
	~ClientUI(void);
	
    int getId();
	string getName();
	bool isEnabled();
	float getblendFactor();
    int getFps();
    
    
    void setName(string newName);
    void setEnabled(bool isEnabled);
    void setBlendFactor(float newBlendFactor);
    void setEnabledPrev(bool isEnabled);
    void setBlendFactorPrev(float newBlendFactor);
    void setFps(int newFps);
    void setFpsPrev(int fpsPrev);
    

    
private:
	int id;
	string name;

};

#endif
