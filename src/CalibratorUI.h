//
//  CalibratorUI.h
//  Lumitron
//
//  Created by Christian Clark on 4/23/13.
//  Copyright (c) 2013 chachi.clark@gmail.com. All rights reserved.
//

#ifndef Lumitron_CalibratorUI_h
#define Lumitron_CalibratorUI_h

#include "ofMain.h"
#include <string.h>

class CalibratorUI
{
public:
	CalibratorUI(void);
	~CalibratorUI(void);
	
    int getIdLedType();
    int getR();
    int getG();
    int getB();
	   
    void setId(int newId);
    void setR(int newR);
    void setG(int newG);
    void setB(int newB);
    
private:
	int id;
	int r;
    int g;
    int b;
};


#endif
