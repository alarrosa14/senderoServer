#pragma once
#include "ofMain.h"
#include <string.h>
#include "DTFrame.h"
#include "ofxNetwork.h"
#include "stdint.h"

#define LOW_BYTE 0x00FF
#define HIGH_BYTE 0xFF00

// convert from shorts to bytes and back again
#define short_get_high_byte(x) ((HIGH_BYTE & x) >> 8)
#define short_get_low_byte(x)  (LOW_BYTE & x)

#define bytes_to_short(h,l) ( ((h << 8) & 0xff00) | (l & 0x00FF) ) ;

using namespace std;

//#pragma pack(1) TODO: REVISAR por qué estaba esto
struct artnet_dmx_s {
  uint8_t  id[8];
  uint8_t opCodeHi;
  uint8_t opCode;
  uint8_t  verH;
  uint8_t  ver;
  uint8_t  sequence;
  uint8_t  physical;
  uint8_t  universeHi;
  uint8_t  universe;
  uint8_t  lengthHi;
  uint8_t  length;
  uint8_t  data[512];
} __attribute__((packed)); ;

typedef struct artnet_dmx_s artnet_dmx_t;


class MbedComm
{
public:
	MbedComm(void);
	MbedComm(string iIpAddress, int iUDPPort,bool iEnabled);
	~MbedComm(void);
	void setIPAddress(string iIpAddress);
	void setUDPPort(int iUDPPort);
	void setTransmitState(bool newState);
	bool isEnabled();
	void transmitFrame(DTFrame* frameToTransmit);
	void setupSender();
    void storeFrame(DTFrame* newFrame);
    void clenUpBufferedFrame();
    bool sendFrameFragment();
private:
	string ipAddress;
	int UDPPort;
	ofxUDPManager udpConnection;
	int universeSize;
    int dataSize;
	int sequence;
    DTFrame* bufferedFrame;
    int fragmentsSent;
    unsigned short universoFragment;
public:
	bool enabled;

};
