#include "MultiCastManager.h"

MultiCastManager::MultiCastManager(void)
{
}

MultiCastManager::~MultiCastManager(void)
{
}
	
MultiCastManager::MultiCastManager(string iGroupAddress, string iIpAddress, int iUDPPort, bool iBroadcastEnabled)
{
	this->BroadcastEnabled=iBroadcastEnabled;
	this->GroupAddress=iGroupAddress;
	this->IPAddress=iIpAddress;
	this->UDPPort=iUDPPort;
    this->sequenceNumber=0;
    this->sendSyncPacket=true;
    this->packetsBetweenSync=60;
    this->packetsSent=0;
    //this->ticksToWait=iTicksToWait;
}
	
int MultiCastManager::getUDPPort()
{
	return this->UDPPort;
}

string MultiCastManager::getGroupAddress()
{
	return this->GroupAddress;
}

bool MultiCastManager::isBroadcastEnabled()
{
	return this->BroadcastEnabled;
}

string MultiCastManager::getIPAddress()
{
	return this->IPAddress;
}

void MultiCastManager::setUDPPort(int iUDPPort)
{
	this->UDPPort=iUDPPort;
}

void MultiCastManager::setGroupAddress(string iGroupAddres)
{
	this->GroupAddress=iGroupAddres;
}

void MultiCastManager::setIPAddress(string iIPAddress)
{
	this->IPAddress=iIPAddress;
}

void MultiCastManager::setBroadcastState(bool iNewStatus)
{
    //using the blocking mutex
    lock();
	this->BroadcastEnabled=iNewStatus;
    unlock();
}

void MultiCastManager::transmitFrame(DTFrame* frame)
{
    string frameToTransmit = frame->toXMLReduced();
    int transmittedData = udpConnection.Send(frameToTransmit.data(), frameToTransmit.length());
    transmittedData=transmittedData;
}

void MultiCastManager::transmitFrameBinary(DTFrame* frame){
    // parto el DTFrame y voy transmitiendo sus partes.
    
    vector<DTPixel*>* newVector=frame->getPixels();
    int pixelQuantity = newVector->size();
    int minId=0;
    int maxId=pixelQuantity-1;
    
    while(pixelQuantity>0){
        celebra_packet_t toTransmit;
        int headerSize =18;
        if (pixelQuantity<=341){
            toTransmit=frame->getBinaryPacketFromFrame(minId, maxId, this->sequenceNumber, 1);
            int sent = udpConnection.SendAll((char *)&toTransmit, headerSize+((maxId-minId+1)*3));
            pixelQuantity=0;
        }
        else{
            
            toTransmit=frame->getBinaryPacketFromFrame(minId, minId+340, this->sequenceNumber, 0);
            int sent = udpConnection.SendAll((char *)&toTransmit, headerSize+(341*3));
            pixelQuantity=pixelQuantity-341;
            minId = minId + 341;
        }
        
        // debo incrementar el sequence number.
        if (this->sequenceNumber==65535){
            this->sequenceNumber=0;
        }
        else{
            this->sequenceNumber++;
        }
        
    }
}

void MultiCastManager::transmitUniqueFrameBinary(DTFrame* frame){
    // The whole DTFrame is transmitted, without slicing.
    // The packet size has been notified to the clients during handshake.
    // First we obtain the packet header, and then the raw pixel information.
    // Sencondly, we attach the header marker sequence (8 bytes)
    // Then we send all through the Multicast Channel.
    
    vector<DTPixel*>* newVector=frame->getPixels();
    int pixelQuantity = newVector->size();
    
    packet_header_t header;
    
    //header
    header = frame->getBinaryPacketHeader(this->sequenceNumber);
    
    //data
    uint8_t * rawData = frame->getRawFrameData();  
    
    //marker
    uint8_t marker[8];
    memcpy(&marker, "_PACKET_", 8);
    
    //we build a new packet, aligned in memory, for better transmission
    int packetSize=(8 + HEADER_SIZE + (pixelQuantity*3));
    uint8_t * packetToSend= new uint8_t[packetSize];
    
    // copy data to aligned memory
    memcpy(packetToSend, &marker, 8);
    memcpy((packetToSend)+8, &header, HEADER_SIZE);
    memcpy((packetToSend)+8+HEADER_SIZE, rawData, (pixelQuantity*3));
    
    int sent = udpConnection.SendAll((char *)packetToSend, packetSize);
       
    // debo incrementar el sequence number.
    if (this->sequenceNumber==65535){
        this->sequenceNumber=0;
    }
    else{
        this->sequenceNumber++;
    }
    
    delete [] rawData; // delete raw data provided by DTFrame.
    delete [] packetToSend;
}

void MultiCastManager::setupMulticastSender()
{
	udpConnection.Create();
    //bool res = udpConnection.SetTTL(66);
    
    //MULTICAST BEHAVIOUR COMMENTED
	/*char * group = (char *) this->GroupAddress.c_str();
	udpConnection.ConnectMcast(group,this->UDPPort);
	udpConnection.SetNonBlocking(true);*/
      
     // UDP BEHAVIOUR
    char * ipSTR = (char *) this->IPAddress.c_str();
	udpConnection.Connect(ipSTR,this->UDPPort);
	udpConnection.SetNonBlocking(true);
}

void MultiCastManager::addFrameToSendBuffer(DTFrame* newFrame){
    lock();
        this->sendBuffer.push_back(newFrame);
    unlock();
}

void MultiCastManager::threadedFunction(){

    while(isThreadRunning()) {
        int elapsed = ofGetElapsedTimeMillis();
        //if (elapsed-this->elapsedTicks>this->ticksToWait){
            //this->elapsedTicks=elapsed;
        DTFrame* frame=0;
        bool doSend=false;
        if(!this->sendSyncPacket){
            lock();
                if(this->sendBuffer.size()>0){
                    doSend=this->BroadcastEnabled;
                    frame=this->sendBuffer[0];
                    vector<DTFrame*>::iterator it = this->sendBuffer.begin();
                    this->sendBuffer.erase(it);
                }
            unlock();
        }
        if(frame!=0){
            if(doSend){
                //this->transmitFrameBinary(frame);
                transmitUniqueFrameBinary(frame);
                this->packetsSent++;
                if(this->packetsSent>=this->packetsBetweenSync){
                    this->sendSyncPacket=true;
                    this->packetsSent=0;
                }
            }
            delete frame;
        }
        if (this->sendSyncPacket){
            //this->transmitSyncFrameBinary();
            this->sendSyncPacket=false;
        }
        //}
        sleep(25);
    }
}

void MultiCastManager::transmitSyncFrameBinary(){
    //uint8_t  data[1100];
    // do nothing for now
}

