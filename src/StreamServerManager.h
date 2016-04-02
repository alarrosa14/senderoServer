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
	void setAddress(const string &address);
	void setPort(const int &port);
	bool getEnabled();
	string getAddress();
	int getPort(); 

	void setupStreamingSender(int pixelQuantity);
	void addFrameToSendBuffer(DTFrame* newFrame);
	void threadedFunction();

	virtual ~StreamServerManager();

private:
	sio::client socketIOClient;

	bool enabled;
	int pixelQuantity;
	int port;
	string address;
	ofMutex waitForNewFrameMutex;
};
