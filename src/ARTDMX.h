#ifndef ARTDMX_H

#define ARTDMX_H



#include "ofxUDPManager.h"


struct artnet_dmx_s {
  uint8_t  id[8];
  uint16_t opCode;
  uint8_t  verH;
  uint8_t  ver;
  uint8_t  sequence;
  uint8_t  physical;
  uint16_t  universe;
  uint8_t  lengthHi;
  uint8_t  length;
  uint8_t  data[150];
}  __attribute__((packed));

typedef struct artnet_dmx_s artnet_dmx_t;



class ARTDMX

{



protected:

private:



    ofxUDPManager udp;

    int universeSize;

    int dataSize;



    artnet_dmx_t packet;

    unsigned char * message;



public:



    ARTDMX(string address, int port, uint16_t size)

    {
        udp.Create();

        udp.Connect(address.c_str(), port);

        udp.SetNonBlocking(true);


         // now build packet
          memcpy(&packet.id, "Art-Net", 8);
          p.opCode =  htols("0x5000");
          p.verH = 0;
          p.ver = 14;
          p.sequence = port->seq;
          p.physical = port_id;
          p.universe = htols(port->port_addr);

          // set length
          p.lengthHi = short_get_high_byte(length);
          p.length = short_get_low_byte(length);
          memcpy(&p.data, data, length);





        universeSize = size;

        dataSize = universeSize + 18;







    }

    virtual ~ARTDMX() {}



    void sendDMXBuffer()

    {

        memcpy(message, &packet, 8);

        int sent = udp.Send((const char *)message, 8);

        cout << "Sent Bytes: " << sent << endl;

    }

    //Sets a byte in the message

     void setMessageByte(int index, int value)

    {

        message[index] = value;

    }

    //Sets value to channel

    void setDMXChannelBuffer(int channel, int value)

    {

        message[channel + 18] = value;

    }

    //Writes value to the channel, if the value is different from the last written.

    void setDMXChannel(int channel, int value)

    {

        if (message[channel + 18] != value)

        {

            message[channel + 18] = value;

            sendDMXBuffer();

        }

    }

    void onExit()

    {

        for (int i = 0; i < universeSize; i++)

        {

            setDMXChannelBuffer(i, 0);

        }

        sendDMXBuffer();

    }

    int hex2int(string s)

    {

        stringstream ss(s);

        int i;

        ss >> hex >> i;

        return i;

    }

};



#endif // ARTDMX_H


