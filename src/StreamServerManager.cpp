/*
 * StreamServerManager.cpp
 *
 *  Created on: Sep 10, 2015
 *      Author: alarrosa14
 */

#include "StreamServerManager.h"

StreamServerManager::StreamServerManager(void) {
	waitForNewFrameMutex.lock();
}

StreamServerManager::~StreamServerManager(void) {}

void StreamServerManager::setEnabled(const bool &isEnabled) {
	this->enabled = isEnabled;
}

void StreamServerManager::setAddress(const string &address) {
	this->address = address;
}

void StreamServerManager::setPort(const int &port){
	this->port = port;
} 

bool StreamServerManager::getEnabled() {
	return this->enabled;
}

string StreamServerManager::getAddress() {
	return this->address;
}

int StreamServerManager::getPort(){
	return this->port;
} 

void StreamServerManager::setupStreamingSender() {
	cout << "Server config: " << this->address << endl << this->port << endl;
	string url = "http://" + this->address + ":" + to_string(this->port);
	cout << "Setting up Streaming Server: " << url << endl;
	this->socketIOClient.connect(url.c_str());
    this->socketIOClient.socket()->emit("connection");
}

void StreamServerManager::addFrameToSendBuffer(DTFrame* newFrame) {
	lock();
	this->sendBuffer.push_back(newFrame);
	unlock();
	waitForNewFrameMutex.unlock();
}

void StreamServerManager::threadedFunction() {
	while(isThreadRunning()) {
		waitForNewFrameMutex.lock();
		lock();
		if(this->sendBuffer.size()>0){
	        vector<DTFrame*>::iterator it = this->sendBuffer.begin();
	        this->sendBuffer.erase(it);
	        unlock();

	        uint8_t* raw_frame = (*it)->getRawFrameData();
			int raw_frame_length = (*it)->getPixelQuantity()*3;

			this->socketIOClient.socket()->emit("sendFrame", std::make_shared<std::string>((char *)raw_frame, raw_frame_length));
	    } else {
	    	unlock();
	    }
	}
}
