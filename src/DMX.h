#ifndef DMX_H

#define DMX_H



#define DMX_PRO_MESSAGE_START 0x7E;

#define DMX_PRO_MESSAGE_END 0xE7;

#define DMX_PRO_SEND_PACKET 0x06;



class DMX

{





protected:

private:



    bool serialOk;

    ofSerial serial;

    //num channels

    int universeSize;

    int dataSize;

    unsigned char * message;



    // Format of the dmx message:

    // message[0] = DMX_PRO_MESSAGE_START;

    // message[1] = DMX_PRO_SEND_PACKET;

    // message[2] = byte(dataSize & 255);

    // message[3] = byte((dataSize >> 8) & 255);

    //     message[4] = 0;

    // message[4 + 1] = value in channel 0

    // message[4 + 2] = value in channel 1

    // ...

    // message[4 + universeSize] = value in channel universeSize - 1

    // message[4 + universeSize + 1] = DMX_PRO_MESSAGE_END

    // where dataSize = universeSize + 1;



public:

    DMX(string port, int rate, int size)

    {



        serialOk = serial.setup(port, rate);



        universeSize = size;

        dataSize = universeSize + 6;

        int tempSize = universeSize + 1;



        message = new unsigned char[dataSize];

        message[0] = DMX_PRO_MESSAGE_START;

        message[1] = DMX_PRO_SEND_PACKET;

        message[2] = tempSize & 255;

        message[3] = (tempSize >> 8) & 255;

        message[4] = 0;

        for (int i = 1; i <= universeSize; i++)

        {

            message[4 + i] = 0;

        }

        message[universeSize + 5] = DMX_PRO_MESSAGE_END;

    }



    virtual ~DMX() {}



    // Writes value to the channel, if the value is different from the last written.

    void setDMXChannel(int channel, int value)

    {

        if (message[channel + 5] != value)

        {

            message[channel + 5] = value;

            sendDMXBuffer();

        }

    }



    void setDMXChannelBuffer(int channel, int value)

    {

        if (message[channel + 5] != value)

        {

            message[channel + 5] = value;

        }

    }



    void sendDMXBuffer()

    {



        serial.writeBytes(message, dataSize);

    }



    void onExit() {

        for (int i = 0; i < universeSize; i++) {

            setDMXChannelBuffer(i, 0);

        }

        serial.writeBytes(message, dataSize);

    }



};



#endif // DMX_H


