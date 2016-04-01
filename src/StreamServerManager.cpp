/*
 * StreamServerManager.cpp
 *
 *  Created on: Sep 10, 2015
 *      Author: alarrosa14
 */
#include <chrono>

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

void convertToArrayOfBytes(void* data, int length, unsigned char *buffer) {
	char* ptr = (char*)data;
	for (int i = 0; i < length; i++)
		buffer[i] = *ptr++;
};

uint64_t htonll(uint64_t n) {
#if __BYTE_ORDER == __BIG_ENDIAN
    return n;
#else
    return (((uint64_t)htonl(n)) << 32) + htonl(n >> 32);
#endif
}

uint64_t ntohll(uint64_t n) {
	return htonll(n);
}

void StreamServerManager::threadedFunction() {
	unsigned char *buffer;
	while(isThreadRunning()) {
		waitForNewFrameMutex.lock();
		lock();
		if(this->sendBuffer.size()>0){
	        vector<DTFrame*>::iterator it = this->sendBuffer.begin();
	        this->sendBuffer.erase(it);
	        unlock();

	        uint8_t* raw_frame = (*it)->getRawFrameData();
			int raw_frame_length = (*it)->getPixelQuantity()*3;

			buffer = new unsigned char[raw_frame_length + sizeof(uint64_t)];

			using namespace std::chrono;
			uint64_t ms = htonll(duration_cast< milliseconds >(
			    system_clock::now().time_since_epoch()
			).count());

			convertToArrayOfBytes(&ms, sizeof(ms), buffer);
			convertToArrayOfBytes(raw_frame, raw_frame_length, buffer + sizeof(ms));

			this->socketIOClient.socket()->emit("sendFrame", std::make_shared<std::string>((char *)buffer, sizeof(ms) + raw_frame_length));
			delete[]buffer;
		} else {
	    	unlock();
	    }
	}
}
