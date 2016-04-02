#include "MbedComm.h"
#include <math.h>


MbedComm::MbedComm(void)
{
}

MbedComm::MbedComm(string iIpAddress, int iUDPPort,bool iEnabled)
{
	this->ipAddress=iIpAddress;
	this->UDPPort=iUDPPort;
	this->enabled=iEnabled;
	this->sequence = 1;
    this->bufferedFrame=NULL;
    this->fragmentsSent=0;
    this->universoFragment=0;
}

MbedComm::~MbedComm(void)
{
}

void MbedComm::setIPAddress(string iIpAddress)
{
	this->ipAddress=iIpAddress;
}

void MbedComm::setUDPPort(int iUDPPort)
{
	this->UDPPort=iUDPPort;
}

void MbedComm::setTransmitState(bool newState)
{
	this->enabled=newState;
}

bool MbedComm::isEnabled()
{
	return this->enabled;
}


int hex2int(string s)
{

    stringstream ss(s);

    int i;

    ss >> hex >> i;

    return i;

}

//this function sends the whole frame and puts the thread to sleep while waiting to send the next fragment.
//it SHOULD not be used, unless working just with one device.
void MbedComm::transmitFrame(DTFrame* frameToTransmit)
{
	vector<DTPixel*>* pixels = frameToTransmit->getPixels();
	vector <DTPixel*>::size_type i = pixels->size();
	int cantPixelsInt = i;
	int cantP = cantPixelsInt / 170;
	int resto = cantPixelsInt % 170;
	
	//float cantPixels = (float) i;
	//float cantP = cantPixels/170.0f;
	////
	//int cantPackets = (int)(cantP + 0.5f);
	int cantPackets = cantP + (resto>0);
	
	short universo = 0;
	for (int j=1; j<=cantPackets; j++)
	{
		if (j>1)
			usleep(1000);
		artnet_dmx_t packet;
		// now build packet
		memcpy(&packet.id, "Art-Net", 8);
		packet.opCodeHi = 0x00;
		packet.opCode =  0x50;
		packet.verH = 0;
		packet.ver = 14;
		packet.sequence = this->sequence;
		packet.physical = 0;
		packet.universeHi = short_get_low_byte(universo);
		packet.universe= short_get_high_byte(universo);
		// set length
		short length = 0;
		if (j<cantPackets){
			length = 170; // se transmiten 170 pixels
		}
		else{
			length =  cantPixelsInt - ((cantPackets - 1) * 170);
		}
		length= length * 3; // son 3 bytes por luz
		packet.lengthHi = short_get_high_byte(length);
		packet.length = short_get_low_byte(length);
		
		//busco posicion en el vector de pixeles a partir de la cual voy a mandar pixeles

		int pos = (j * 170) - 170;

		vector<DTPixel*>::iterator it = pixels->begin();

		for (int cp = 0; cp<(length-2); cp+=3)
		{

			packet.data[cp] = it[pos]->getR();
			packet.data[cp+1] = it[pos]->getG();
			packet.data[cp+2] = it[pos]->getB();
			
			pos++;

		}
		
		universo ++;

		int sent = udpConnection.Send((const char*)&packet, length+18);

	}
	
	this->sequence++;
}

bool MbedComm::sendFrameFragment(){
    if (this->bufferedFrame!=NULL){
        
        vector<DTPixel*>* pixels = bufferedFrame->getPixels();
        
        //DTPixel* ppp = pixels->operator[](0);
        
        //cout << ppp->getR() << endl;
        
        vector <DTPixel*>::size_type i = pixels->size();
        int cantPixelsInt = i;
        
        int cantP = cantPixelsInt / 170;
        int resto = cantPixelsInt % 170;
        
        int cantPackets = cantP;
        
        if (resto!=0){
            cantPackets++;
        }
        
        //catPackets is the total amount of fragments to be sent
        // in this->fragmentsSent we store the amount of fragments actualli sent at the time being.
        
        //short universo = 0;
        //universo should start at 0

        artnet_dmx_t packet;
        // now build packet
        memcpy(&packet.id, "Art-Net", 8);
        packet.opCodeHi = 0x00;
        packet.opCode =  0x50;
        packet.verH = 0;
        packet.ver = 14;
        packet.sequence = this->sequence;
        packet.physical = 0;
        packet.universeHi = short_get_low_byte(this->universoFragment); // TODO: Why is this inverted... and works?
        packet.universe= short_get_high_byte(this->universoFragment);
        // set length
        short length = 0;
        if ((this->fragmentsSent+1)<cantPackets){
            length = 170; // se transmiten 170 pixels
        }
        else{
            length =  cantPixelsInt - ((cantPackets - 1) * 170); ///////////////////////////////// TODO: revisar
        }
        length= length * 3; // son 3 bytes por luz
        packet.lengthHi = short_get_high_byte(length);
        packet.length = short_get_low_byte(length);
        
        //busco posicion en el vector de pixeles a partir de la cual voy a mandar pixeles
        
        int pos = (this->fragmentsSent * 170);
        
        vector<DTPixel*>::iterator it = pixels->begin();
        
        for (int cp = 0; cp<(length-2); cp+=3)
        {
            // original order!! FIXXX uncomment for OCTOBARS

            //int rNew= it[pos]->getR();
            //int gNew= it[pos]->getG();
            //int bNew= it[pos]->getB();
            
            int rNew= pixels->operator[](pos)->getR();
            int gNew= pixels->operator[](pos)->getG();
            int bNew= pixels->operator[](pos)->getB();
            
            if(rNew>255)
                rNew=255;
            if(gNew>255)
                gNew=255;
            if(bNew>255)
                bNew=255;
            
            //for Bondibar
            packet.data[cp] = rNew;
            packet.data[cp+1] = gNew;
            packet.data[cp+2] = bNew;
             
            /*
            // for octobar
            packet.data[cp] = rNew;
            packet.data[cp+1] = gNew;
            packet.data[cp+2] = bNew;
            */
            pos++;
            
        }
        
        this->universoFragment++;
        
        int sent = udpConnection.Send((const char*)&packet, length+18);
        //cout << "r" << endl;
        this->sequence++;
        
        this->fragmentsSent++;
        
        if (this->fragmentsSent==cantPackets){
            //cout << "all sent" << endl;
            //we've sent the whole frame
            this->fragmentsSent=0;
            this->universoFragment=0;
            return false;
        }
        else{
            //we still have fragments to process
            return true;
        }
    }
    return false;
}

void MbedComm::storeFrame(DTFrame* newFrame){
    this->bufferedFrame=newFrame;
}

void MbedComm::clenUpBufferedFrame(){
    if(bufferedFrame!=NULL){
        delete this->bufferedFrame;
        this->bufferedFrame=NULL;
    }
}

void MbedComm::setupSender()
{
	udpConnection.Create();
	const char * ip = this->ipAddress.c_str(); 
	udpConnection.Connect(ip,this->UDPPort);
	udpConnection.SetNonBlocking(true);
}
