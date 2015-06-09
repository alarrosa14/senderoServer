//
//  DeviceUI.h
//  Server
//
//  Created by Christian Clark on 2/22/13.
//  Copyright (c) 2013 chachi.clark@gmail.com. All rights reserved.
//

#ifndef Server_DeviceUI_h
#define Server_DeviceUI_h

#include "ofMain.h"
#include <string.h>

class DeviceUI
{
public:
	DeviceUI(void);
	DeviceUI(int iId, string iName,bool iEnabled);
	~DeviceUI(void);
	
    int getId();
	bool isEnabled();
    string getName();
    
    void setEnabled(bool isEnabled);
    void setEnabledPrev(bool isEnabled);
    
    //public because of TweakBar
    bool enabled;
    bool enabledPrev;
    
private:
	int id;
	string name;
};

#endif
