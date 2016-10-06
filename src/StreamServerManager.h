/*
 * StreamServerManager.h
 *
 *  Created on: Sep 10, 2015
 *      Author: alarrosa14
 */

#pragma once

#include "../include/sio_client.h"

#include "ofMain.h"
#include <string.h>
#include "DTFrame.h"
#include <vector>
#include "ofxNetwork.h"

using namespace sio;
using namespace std;

class StreamServerManager : public ofThread {
public:
    vector<DTFrame*> sendBuffer;


	StreamServerManager();

	void setEnabled(const bool &isEnabled);
	void setCompressionEnabled(const bool &compression);
	void setAddress(const string &address);
	void setPixelQuantity(const int &pixelQty);
	void setPort(const int &port);

	bool getEnabled();
	bool getCompressionEnabled();
	string getAddress();
	int getPixelQuantity();
	int getPort(); 

	void setupStreamingSender();
	void addFrameToSendBuffer(DTFrame* newFrame);
	void threadedFunction();

	virtual ~StreamServerManager();

private:
	ofxUDPManager udpManager;

	bool enabled;
	bool compressionEnabled;
	int pixelQuantity;
	int port;
	string address;
	ofMutex waitForNewFrameMutex;
};
