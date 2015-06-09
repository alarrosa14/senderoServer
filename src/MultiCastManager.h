#pragma once
#include "ofMain.h"
#include <string.h>
#include "DTFrame.h"
#include <vector>
#include "ofxNetwork.h"

using namespace std;

class MultiCastManager: public ofThread
{
public:
    
	MultiCastManager(void);
	MultiCastManager(string iGroupAddress, string iIpAddress, int iUDPPort, bool iBroadcastEnabled);
	~MultiCastManager(void);
	int getUDPPort();
	string getGroupAddress();
	bool isBroadcastEnabled();
	string getIPAddress();
	void setUDPPort(int iUDPPort);
	void setGroupAddress(string iGroupAddres);
	void setIPAddress(string iIPAddress);
	void setBroadcastState(bool iNewStatus);
	void transmitFrame(DTFrame* frame); //deprecated
    void transmitFrameBinary(DTFrame* frame);
    void transmitUniqueFrameBinary(DTFrame* frame);
	void setupMulticastSender();
    void addFrameToSendBuffer(DTFrame* newFrame);
    void threadedFunction();
    void transmitSyncFrameBinary();
    
private:
    
	int UDPPort;
	string GroupAddress;
	string IPAddress;
	ofxUDPManager udpConnection;
    unsigned short sequenceNumber;
    
    //for syncing packet
    bool sendSyncPacket;
    int packetsBetweenSync;
    int packetsSent;

public:
    
	bool BroadcastEnabled;
    vector<DTFrame*> sendBuffer;


};
