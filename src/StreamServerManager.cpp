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

	udpManager.Create();
	cout << this->address.c_str() << this->port << endl;
	if (!udpManager.Connect(this->address.c_str(), this->port));
		cout << endl << endl << "Error connecting socket!" << endl << endl;
	udpManager.SetNonBlocking(true);

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

	const int frameSize = this->pixelQuantity*3;

	size_t compressedBufferMaxSize;
	unsigned char *compressedBuffer = NULL;
	if (compressionEnabled) {
		compressedBufferMaxSize = LZ4F_compressFrameBound(frameSize, NULL);
		compressedBuffer = new unsigned char[compressedBufferMaxSize];
	}

	const int bufferSize = ((compressionEnabled && compressedBufferMaxSize > frameSize) ? compressedBufferMaxSize : frameSize) + sizeof(uint64_t);
	unsigned char *buffer = new unsigned char[bufferSize];

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

			if (compressionEnabled) {
				size_t compressedBufferSize = LZ4F_compressFrame(compressedBuffer, compressedBufferMaxSize, raw_frame, raw_frame_length, NULL);
				convertToArrayOfBytes(compressedBuffer, compressedBufferSize, buffer + sizeof(ms));
				this->udpManager.Send((char*) buffer, sizeof(ms) + compressedBufferSize);
			} else {
				convertToArrayOfBytes(raw_frame, raw_frame_length, buffer + sizeof(ms));
				this->udpManager.Send((char*) buffer, bufferSize);
			}


		} else {
	    	unlock();
	    }
	}
	this->udpManager.Close();
	delete[]buffer;
	delete[]compressedBuffer;
}
