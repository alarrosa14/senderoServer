/*
 * StreamServerManager.cpp
 *
 *  Created on: Sep 10, 2015
 *      Author: alarrosa14
 */
#include <chrono>

#include "lz4.h"
#include "lz4frame.h"

#include "StreamServerManager.h"

StreamServerManager::StreamServerManager(void) {
	waitForNewFrameMutex.lock();
}

StreamServerManager::~StreamServerManager(void) {}

void StreamServerManager::setEnabled(const bool &isEnabled) {
	this->enabled = isEnabled;
}

void StreamServerManager::setCompressionEnabled(const bool &compression) {
	this->compressionEnabled = compression;
}

void StreamServerManager::setAddress(const string &address) {
	this->address = address;
}

void StreamServerManager::setPixelQuantity(const int &pixelQty){
	this->pixelQuantity = pixelQty;
}

void StreamServerManager::setPort(const int &port){
	this->port = port;
} 

bool StreamServerManager::getEnabled() {
	return this->enabled;
}

bool StreamServerManager::getCompressionEnabled() {
	return this->compressionEnabled;
}

string StreamServerManager::getAddress() {
	return this->address;
}

int StreamServerManager::getPixelQuantity() {
	return this->pixelQuantity;
}

int StreamServerManager::getPort(){
	return this->port;
} 

void StreamServerManager::setupStreamingSender() {
	cout << "Server config: " << this->address << endl << this->port << endl;
	cout << "Compression enabled? " << (this->compressionEnabled ? "True" : "False") << endl;
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

	const int bufferSize = this->pixelQuantity*3 + sizeof(uint64_t);
	unsigned char *buffer = new unsigned char[bufferSize];

	size_t compressedBufferMaxSize;
	unsigned char *compressedBuffer;
	if (compressionEnabled) {
		size_t compressedBufferMaxSize = LZ4F_compressFrameBound(bufferSize, NULL);
		unsigned char *compressedBuffer = new unsigned char[compressedBufferMaxSize];
	}

	while(isThreadRunning()) {
		waitForNewFrameMutex.lock();
		lock();
		if(this->sendBuffer.size()>0){
	        vector<DTFrame*>::iterator it = this->sendBuffer.begin();
	        this->sendBuffer.erase(it);
	        unlock();

	        uint8_t* raw_frame = (*it)->getRawFrameData();
			int raw_frame_length = (*it)->getPixelQuantity()*3;

			assert(raw_frame_length == this->pixelQuantity*3);

			using namespace std::chrono;
			uint64_t ms = htonll(duration_cast< milliseconds >(
			    system_clock::now().time_since_epoch()
			).count());

			convertToArrayOfBytes(&ms, sizeof(ms), buffer);
			convertToArrayOfBytes(raw_frame, raw_frame_length, buffer + sizeof(ms));

			if (compressionEnabled) {
				size_t compressedBufferSize = LZ4F_compressFrame(compressedBuffer, compressedBufferMaxSize, buffer, bufferSize, NULL);
				this->socketIOClient.socket()->emit("sendFrame", std::make_shared<std::string>((char *)compressedBuffer, compressedBufferSize));
			} else {
				this->socketIOClient.socket()->emit("sendFrame", std::make_shared<std::string>((char *)buffer, bufferSize));
			}


		} else {
	    	unlock();
	    }
	}
	delete[]buffer;
	delete[]compressedBuffer;
}
