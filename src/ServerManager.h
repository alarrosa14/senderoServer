#pragma once

#ifndef Server_Manager
#define Server_Manager


//#include "../include/sio_client.h"

#include <functional>
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <string>
#include <stdio.h>
#include <fstream>

//using namespace sio;
using namespace std;



#include "ofMain.h"
#include <string.h>
#include <map>
#include <vector>
#include "MbedComm.h"
#include "Pixel.h"
#include "StreamServerManager.h"
#include "ServerClientProxy.h"
#include "ofxXmlSettings.h"
#include "DTClient.h"
#include "DTFrame.h"
#include "LightController.h"
//#include "ofxLog.h"
#include "GPUManager.h"
#include "DeviceManager.h"
#include "ClientUI.h"
#include "DeviceUI.h"
#include "ofxUI.h"
#include "ofxAssimpModelLoader.h"
#include "ofVboMesh.h"
#include "MidiController.h"
#include "CalibratorUI.h"
#include "Worker.h"


class ServerClientProxy;

class ServerManager : public ofBaseApp, public WorkerDelegate
{
private:
	// sio::client h;
    int pixelQuantity;
    bool compressionEnabled;
    string installationName;
    bool enableDraw;
    bool showFrameRate;
    bool mouseRotEnabled;
    float rotX;
	float rotY;
	float scale;
    GLUquadric* quadric;
    bool doDraw;
    ofVec3f centroid;
    ofVec3f translation;
    
    ofEasyCam cam;
    
	map <int,Pixel*>* pixels;
    vector<Pixel*>* pixelsFast;
    cl_float* pixelArray;//stores colors and it is used for GPU computing
    
	map <int,ServerClientProxy*>* clients;
    vector <ServerClientProxy*>* clientsFast;
    map <int,DTFrame*>* lastFrameBuffer;
    
    map<string,ofxAssimpModelLoader *>* meshes;
	
	float ticks;
	float elapsedTicks;
    float FRTicks;
    float FRElapsedTicks;
    float drawTicks;
    float drawElapsedTicks;
    float drawTicksToReach;
	float frameRateInterval;
    float realFrameRate;
    float averageFrameRate;
    float sumFrameRate;
    float averageQty;
    
    ofTrueTypeFont myfont;
    ofImage logo;
    
    StreamServerManager* SSManager;
    DeviceManager* DVCManager;

    /* Client frames processing and blending */
    Worker* worker;

    //UI collections.
    map<int, ClientUI*>* clientsParams;
    map<int,DeviceUI*>* devicesParams;
    vector<ClientUI*>* clientParamsFast;
    CalibratorUI* calibratorParams;
    
    GPUManager* gpuManager;
    
    ofxUIScrollableCanvas *generalGUI;
    ofxUIScrollableCanvas *devicesGUI;
    ofxUIScrollableCanvas *calibrationGUI;
    
    //for midi output
    MidiController* midiOutController;
    bool midiEnabled;
    
	int loadFromFile();
	void blendFromFrame(DTFrame* newFrame, float blendFactor);
	void sendFrameToStreamingServer(DTFrame* transmitFrame);
	void sendFrameToDevices();
	void setDrawingState(bool newState);
	void decay(int r, int g, int b, int a);
	void loadClientPixels();
    void setGUIGeneral();
    void setGUIDevices();
    void setGUICalibration();
    bool inGUIZone(int x);
    
    int flashR;
    int flashG;
    int flashB;

public:

	void setup();
	void update();
	void draw();
    void exit();

	void keyPressed(int key);
	void mousePressed(int x, int y, int button);
	void mouseReleased();
	void mouseMoved(int x, int y);

	void setClientState(int clientID, bool newState);
	void setMulticastChannelState(bool newState);

	DTFrame* buildFrameToTransmit();
    void guiEvent(ofxUIEventArgs &e);

	float decayFactor;
	bool flash;
    int displayList;
    bool showGUI;
    bool drawAxis;
    
    /* Worker Delegate */
    virtual void workerLoop();
    
};


#endif
