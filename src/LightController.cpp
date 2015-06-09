//
//  LightController.cpp
//  Server
//
//  Created by Christian Clark on 1/22/13.
//  Copyright (c) 2013 chachi.clark@gmail.com. All rights reserved.
//


#include "LightController.h"

LightController::LightController(void){
    
}

LightController::LightController(int iId, string iName, string iIPAdress, bool iEnabled, int iUDPPort, DeviceManager* iDM){
    this->id=iId;
    this->name=iName;
    this->pixelQuantity=0;
    //this->pixels = new map<int, Pixel*>;
    this->pixelsIDs = new vector<int>;
    this->mBed=new MbedComm(iIPAdress, iUDPPort, iEnabled);
    this->enabledDevice=iEnabled;
    this->prevEnabled=iEnabled;
    this->DM=iDM;
    
}

LightController::~LightController(void){
    delete this->mBed;
    delete this->pixelsIDs;
}

void LightController::update(){
    // do nothing...
    if(enabledDevice!=prevEnabled){
        this->mBed->setTransmitState(enabledDevice);
        prevEnabled=enabledDevice;
    }
}

void LightController::draw(){
    // do nothing...
}

void LightController::setup(){
    // do nothing...
    this->mBed->setupSender();
}

bool LightController::sendFrameToLightController(){
    bool hasRemainingFragment=false;
    if(this->mBed->enabled){
        hasRemainingFragment = this->mBed->sendFrameFragment();
    }
    return hasRemainingFragment;
}

void LightController::createAndStorePacketToSend(DTFrame* totalFrame){
    if(this->mBed->enabled){
        DTFrame* newFrame = this->buildFrameToTransmit(totalFrame);
        this->mBed->storeFrame(newFrame);
    }
}

void LightController::frameCleanUp(){
    this->mBed->clenUpBufferedFrame();
}

void LightController::addPixel(int newPixelID){
    this->pixelsIDs->push_back(newPixelID);
    this->pixelQuantity+=1;
}

DTFrame* LightController::buildFrameToTransmit(DTFrame* totalFrame){
    //debo extraer los pixles que pertenecen a este device del total frame
    //en totalPixels, los pixels estan ordenados por id de <
    //además, debo aplicar la transformación de color utilizando el LEDType* que viene en el map que me pasan.
    
    vector<DTPixel*>* totalPixels = totalFrame->getPixels();
    
    vector<int>::iterator it;
	it=this->pixelsIDs->begin();
	vector<DTPixel*>* dtVector = new vector<DTPixel*>();
    
    int size=this->pixelsIDs->size();
    
    cl_float* newPixelArray = new cl_float[size * 4];
    int cont=0;
    
    //cout << " Cant de IDs de MBEd " << this->pixelsIDs->size() << endl;
    
	while (it != this->pixelsIDs->end())
	{
		int myPxId=*it;
        DTPixel* px = totalPixels->operator[](myPxId);
        //lo unico que se retira de la memoria del server es el ID de los 
        int pId = myPxId;
        
        float pR = px->getR();
        float pG = px->getG();
        float pB = px->getB();
        float pA = px->getA();
        ofVec3f dummyFront(0,0,0);
        ofVec3f dummyUp(0,0,0);
        int ledTypeId= px->getLedTypeId();
        
        //get the ledCalibrator
        
        LEDType* ledCalib = this->DM->getCalibratorById(ledTypeId);
        ofColor calibratedColor=ledCalib->getCalibratedColor(ofColor(pR,pG,pB));
        
        DTPixel * pixelForNewFrame= new DTPixel(pId, (int)calibratedColor.r, (int)calibratedColor.g, (int)calibratedColor.b, pA, px->getPosX(), px->getPosY(), px->getPosZ(),dummyFront,dummyUp,"dummy",ledTypeId);
		
        dtVector->push_back(pixelForNewFrame);
        
        newPixelArray[(cont*4)] = (cl_float)pR;
        newPixelArray[(cont*4)+1] = (cl_float)pG;
        newPixelArray[(cont*4)+2] = (cl_float)pB;
        newPixelArray[(cont*4)+3] = (cl_float)pA;
        cont++;
		it++;
	}
	return new DTFrame(0, dtVector, newPixelArray, this->pixelsIDs->size(), 0);
}

void LightController::sendFrameToDevice(DTFrame* newFrame){
    this->mBed->transmitFrame(newFrame);
}

void LightController::setDeviceEnableState(bool newState){
    this->mBed->enabled = newState;
}

int LightController::getId(){
    return this->id;
}

string LightController::getName(){
    return this->name;
}

