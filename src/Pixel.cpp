#include "Pixel.h"

Pixel::Pixel(void)
{
}

Pixel::Pixel(int iId, float iR, float iG, float iB, float iA, ofVec3f iPosition,int iLedTypeId)
{
	this->id=iId;
	this->r=iR;
	this->g=iG;
	this->b=iB;
	this->a=iA;
	this->position=iPosition;
    this->modelName="";
    this->ledTypeId=iLedTypeId;
}

Pixel::~Pixel(void)
{
    this->model->clear();
}

float Pixel::getId()
{
	return this->id;
}

float Pixel::getR()
{
	return this->r;
}

float Pixel::getG()
{
	return this->g;
}

float Pixel::getB()
{
	return this->b;
}

float Pixel::getA()
{
	return this->a;
}

int Pixel::getLedTypeId(){
    return this->ledTypeId;
}

ofQuaternion Pixel::getRotQ()
{
	return this->rot;
}

void Pixel::setRotQ(ofQuaternion newQ){
    this->rot=newQ;
}

void Pixel::setModel(ofxAssimpModelLoader* newModel){
    this->model=newModel;
}

ofVec3f Pixel::getPosition()
{
	return this->position;
}

ofVec3f Pixel::getFront(){
    return this->front;
}
ofVec3f Pixel::getUp(){
    return this->up;
}

void Pixel::setFront(ofVec3f newFront){
    this->front=newFront;
}

void Pixel::setUp(ofVec3f newUp){
    this->up=newUp;
}

string Pixel::getModelName(){
    return this->modelName;
}

void Pixel::setModelName(string newName){
    this->modelName=newName;
}

void Pixel::blendRGBA(float iR, float iG, float iB, float iA, float blendFactor)
{
	/// TODO: hay que hacer mas tipos de blending... maybe
	
	this->r = (this->r * (1- blendFactor)) + (iR * blendFactor);
	if (this->r >255)
		this->r=255;

	this->g = (this->g * (1- blendFactor)) + (iG * blendFactor);
	if (this->g >255)
		this->g=255;
	
	this->b = (this->b * (1- blendFactor)) + (iB * blendFactor);
	if (this->b >255)
		this->b=255;
	// a no se utiliza, se sustituyo por blendfactor
	this->a =0;
}

DTPixel* Pixel::getDTPixel()
{
	return new DTPixel(this->id,this->r, this->g, this->b, this->a, this->position.x,this->position.y, this->position.z, this->front,this->up, this->modelName, this->ledTypeId);
}

void Pixel::draw(){
    ofVec3f position = this->getPosition();
    ofPushMatrix();
   
    ofTranslate(ofPoint(position.x, position.y, position.z));
    
    ofVec3f qaxis; float qangle;  
    rot.getRotate(qangle, qaxis);  
    ofRotate(qangle, qaxis.x, qaxis.y, qaxis.z);  
    
    this->model->disableColors();
    
    ofSetColor(this->getR(),this->getG(), this->getB());
    
    this->model->drawFaces();
    
    ofPopMatrix();
}
