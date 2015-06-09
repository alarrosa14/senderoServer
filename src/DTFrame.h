#pragma once
#include <vector>
#include "DTPixel.h"
#include <string.h>
#include "ofxXmlSettings.h"
#include <CL/opencl.h>

#define LOW_BYTE 0x00FF
#define HIGH_BYTE 0xFF00

// convert from shorts to bytes and back again
#define short_get_high_byte(x) ((HIGH_BYTE & x) >> 8)
#define short_get_low_byte(x)  (LOW_BYTE & x)

#define bytes_to_short(h,l) ( ((h << 8) & 0xff00) | (l & 0x00FF) ) ;

#define HEADER_SIZE 12

using namespace std;

//paquete binario de celebra
//el maximo de pixels por paquete es de 341
struct celebra_packet_s {
    uint8_t  id[8]; //8 bytes
    uint8_t  ver; // 9 bytes
    uint8_t  sequenceHi; //10bytes
    uint8_t  sequence; //11 bytes
    uint8_t  minIdHi; // 12 bytes
    uint8_t  minId; // 13 bytes
    uint8_t  maxIdHi; // 14bytes
    uint8_t  maxId; // 15 bytes
    uint8_t  lengthHi; //16 bytes
    uint8_t  length; //17 bytes
    uint8_t finishFrame; //18bytes
    uint8_t  data[1024];
}  __attribute__((packed));

typedef struct celebra_packet_s celebra_packet_t;

struct packet_header_s {
    uint8_t  crc; //1 bytes
    uint8_t  id[8]; //9 bytes
    uint8_t  ver; // 10 bytes
    uint8_t  sequenceHi; //11 bytes
    uint8_t  sequence; //12 bytes
}  __attribute__((packed));

typedef struct packet_header_s packet_header_t;

class DTFrame
{
public:
	DTFrame(int iPixelQuantity);
	DTFrame(string sourceXML);
	DTFrame(long iTimeStamp, vector<DTPixel*>* iVectorPixels, float* iPixelArray, int iPixelQuantity , unsigned short iSequence);
	~DTFrame(void);
	long getTimestamp();
	string getClientName();
	int getClientID();
    int getSequence();
	vector<DTPixel*>* getPixels();
    cl_float* getPixelsArray();
	string toXML();
    string toXMLReduced();
    int getPixelQuantity();
    void addPixel(DTPixel* newPixel);
    void setSequence(int newSequence);
    celebra_packet_t getBinaryPacketFromFrame(unsigned short minId, unsigned short maxId, unsigned short sequence,unsigned short finishFrame);
    packet_header_t getBinaryPacketHeader(unsigned short sequence);
    uint8_t * getRawFrameData();
private:
	long timestamp;
	int clientID;
    unsigned short sequence;
	string clientName;
	vector <DTPixel*>* pixels;
    cl_float* pixelArray;
    int pixelQuantity;
	
};
