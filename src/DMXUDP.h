#ifndef DMXUDP_H

#define DMXUDP_H



#define DMX_PRO_MESSAGE_START 0x7E;

#define DMX_PRO_MESSAGE_END 0xE7;

#define DMX_PRO_SEND_PACKET 0x06;

#define RECONNECT_TIME 400



#include "ofxUDPManager.h"



class DMXUDP

{



protected:

private:

    ofxUDPManager udp;

    int universeSize;

    int dataSize;

    uint8_t * message;



public:

    DMXUDP(string address, int port, int size)

    {

        udp.Create();

        udp.Connect(address.c_str(), port);

        udp.SetNonBlocking(true);



        universeSize = size;

        dataSize = universeSize + 18;



        //Madrix sample header: 41 72 74 2d 4e 65 74 00 00 50 00 0e 00 00 00 00 00 93



        message = new uint8_t[dataSize];

        message[0] = 'A';

        message[1] = 'r';

        message[2] = 't';

        message[3] = '-';

        message[4] = 'N';

        message[5] = 'e';

        message[6] = 't';

        message[7] = 0x00;

        //OPCODE

        message[8] = 0x00; //OpCode Low Byte as per spec

        message[9] = 0x50; //OpCode High Byte as per spec

        //VERSION

        message[10] = 0x00; //ver High

        message[11] = 0x0e; //ver Low

        //SEQUENCE (FRAME)

        message[12] = 0x00; //current frame number

        //PHYSICAL PORT

        message[13] = 0x00; //not sure what this should be set to???

        //UNIVERSE

        message[14] = 0x00; //universe low byte first (my universe is 1???

        message[15] = 0x00; //universe high byte

        //DMX LENGTH

        message[16] = 0x00; //HI Byte

        message[17] = universeSize; //LOW Byte (150)

        //DMX DATA

        for (int i = 0; i < universeSize; i++)

        {

            message[18 + i] = 0x00;

        }

        //print the header

        for (int i = 0; i < 18; i++) {

            cout << i << " : ";

            cout << hex << (int)message[i] << endl;

        }

    }

    virtual ~DMXUDP() {}



    void sendDMXBuffer()

    {

        int sent = udp.Send((const char *)message, dataSize);

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

    //Clear all channels on exit

    void onExit()

    {

        for (int i = 0; i < universeSize; i++)

        {

            setDMXChannelBuffer(i, 0);

        }

        sendDMXBuffer();

    }

};



#endif // DMXUDP_H


