#pragma once

#ifndef Server_Client_Proxy
#define Server_Client_Proxy

#include "ofMain.h"
#include <map>
#include <string.h>
#include <vector>
#include "ofxNetwork.h"
#include "DTClient.h"
#include "DTFrame.h"
//#include "ofxLog.h"
#include <unistd.h>
#include <CL/opencl.h>
#include "Pixel.h"

using namespace std;

#define BUFFLEN 10240
#define MAX_FRAMES_STORED 10

class ServerClientProxy: public ofThread
{
public:
    
	ServerClientProxy(void);
	ServerClientProxy(int iTCPPort, int iUDPPort, int iId, string iName, bool iEnabled, float iblendFactor, int iProtocolType, int pixelQty);
	~ServerClientProxy(void);
	int getTCPPort();
	int getUDPPort();
	int getId();
	string getName();
	float getBlendFactor();
	bool isEnabled();
	void setState(bool newState);
    void setBlendFactor(float newBlend);
    void setNewFPS(int iFps);
	DTClient* getClientStatus();
	void startListening();
	DTFrame* getFrame();
	void configure(string frameData, int clientCode);
	void listenTCP();
	string getMessage();
	void addPixel(Pixel* px);
	void updatePixelsFromDTFrame(DTFrame * frame);
	
    void synchronizeSpeeds();
    void update();
    void threadedFunction();
    
	
private:
    
    
    float blendFactor;
	string name;
	bool enabled;
    bool drawEnabled;
    int newFps;
    int pixelQuantity;
    
    ofVec3f centroid;
    ofVec3f translation;
    
    float rotX;
	float rotY;
	float scale;
    
    DTFrame* getFrameTextProtocol();
    DTFrame* getFrameBinaryProtocol();
    void clearBinaryBuffer(int size);
    void sendErrorMessage(int messageFps,bool isError,int lastReceivedSequence);
	ofxTCPServer TCP;
	int TCPPort;
	ofxUDPManager UDP;
	int UDPPort;
	int id;
    int protocolType; //0 for binary 1 for text XML
	string recivedBuffer;
    char  binaryBuffer[BUFFLEN];
    int intBinaryBuffIndex;
	//map <int,Pixel*> pixels;
    vector<Pixel*> pixelsFast;
    GLUquadric* quadric;
    vector<DTFrame*> framesBuffer;
    DTFrame* parcialFrame;
    bool buildingParcialFrame;
    int fps;
    int sequence;
    bool waitingForErrorAnswer;
    int displayList;
    int errorQty;
    int errorWindow;
    void retrieveFrames();
    DTFrame* buildFrameToTransmit();

    
    void draw(float x, float y, float w, float h); // converter to privated to avoid confusion THIS SHOULD NOT BE CALLED
    

};



#endif
