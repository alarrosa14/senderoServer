#include "ServerClientProxy.h"

ServerClientProxy::ServerClientProxy(void)
{
}

ServerClientProxy::ServerClientProxy(int iTCPPort, int iUDPPort, int iId, string iName, bool iEnabled, float iblendFactor, int iProtocolType, int pixelQty)
{
	this->TCPPort=iTCPPort;
	this->UDPPort=iUDPPort;
	this->id=iId;
	this->name=iName;
	this->enabled=iEnabled;
	this->blendFactor=iblendFactor;
    this->quadric = gluNewQuadric();
    this->protocolType=iProtocolType;
    this->parcialFrame = 0;
    this->buildingParcialFrame=false;
    this->intBinaryBuffIndex=0; // guarda la cantidad de elementos en el array
    this->fps = 24;
    this->newFps = 24;
    this->sequence=65535;
    this->waitingForErrorAnswer=false;
    this->errorQty=0;
    this->errorWindow=10000;
    this->drawEnabled=true;
    this->pixelQuantity = pixelQty;
    
    //create displaylist for spheres
    this->displayList = glGenLists(1);
    glNewList(this->displayList, GL_COMPILE);
    gluSphere(quadric, 0.5, 3, 3); // esta es la posta
    glEndList(); 
}

ServerClientProxy::~ServerClientProxy(void)
{
	cout << "ASLKDASKDJASDK ADJASK D" << endl;
    this->TCP.close();
    this->UDP.Close();
	vector<Pixel*>::iterator it = this->pixelsFast.begin();
	while (it != this->pixelsFast.end())
	{	
		Pixel* px = *it;
		delete px;
		it++;
	}
}



int ServerClientProxy::getTCPPort()
{
	return this->TCPPort;
}


int ServerClientProxy::getUDPPort()
{
	return this->UDPPort;
}



int ServerClientProxy::getId()
{
	return this->id;
}

string ServerClientProxy::getName()
{
	return this->name;
}

float ServerClientProxy::getBlendFactor()
{
    lock();
    float retVal=this->blendFactor;
    unlock();
	return retVal;

}

bool ServerClientProxy::isEnabled()
{
    bool retVal=false;
    lock();
    retVal=this->enabled;
    unlock();
	return retVal;
}

void ServerClientProxy::setState(bool newState)
{
    lock();
	this->enabled=newState;
    unlock();
}

void ServerClientProxy::setBlendFactor(float newBlend){
    lock();
    this->blendFactor=newBlend;
    unlock();
}

void ServerClientProxy::setNewFPS(int iFps){
    lock();
    this->newFps=iFps;
    unlock();
}

DTClient* ServerClientProxy::getClientStatus()
{
    lock();
    bool iEnabled = this->enabled;
    float iBlend = this->blendFactor;
    unlock();
	return new DTClient(this->id,this->name,this->TCPPort,"",iEnabled, iBlend);
}


void ServerClientProxy::startListening()
{
	this->TCP.setup(this->TCPPort);
	UDP.Create();
	UDP.Bind(this->UDPPort);
	UDP.SetNonBlocking(true);

}

void ServerClientProxy::listenTCP(){
	string message;
    int clientCode=0;
    int numCli = this->TCP.getNumClients();

	if (this->TCP.getNumClients()>0){
		//solo se toma el primer cliente conectado.
		
		for (int i=0; i<TCP.getNumClients(); i++){
			if (TCP.isClientConnected(i)){
				message = TCP.receive(i);
                clientCode=i;
                //TCP.TCPServer.
				//break;
			}
            //else{
                //TCP.disconnectClient(i);
                //TCP.close();
                //TCP.setup(this->TCPPort);
            //}
                
		}
		
		if(message.length() > 0){

			ofxXmlSettings XML;
			XML.loadFromBuffer(message);

            TiXmlElement* myMessage=XML.doc.RootElement();
            if (myMessage){
                if (myMessage->ValueStr() == "ConfigurationRequest" ){
                	ofLogNotice("Configuration request received");
                    DTFrame* confFrame = this->buildFrameToTransmit();
                    if (confFrame!=0) {
                        string confMessage = confFrame->toXML();
                        configure("<ServerInfo clientID='" + ofToString(this->id) + "' UDPPort='" + ofToString(this->UDPPort) + "'>"+ confMessage + "</ServerInfo>", clientCode);
                        delete confFrame;
                    }
                }
                else{
                    if (myMessage->ValueStr() == "ErrorACK" ){
                        if (this->waitingForErrorAnswer){
                            string sequenceName = "sequence";
                            if (myMessage->Attribute(sequenceName.c_str())){
                                int messageSequence = ofToInt(myMessage->Attribute("sequence"));
                                messageSequence-=1;
                                if (messageSequence<0){
                                    messageSequence=65535;
                                }
                                if ((messageSequence)==this->sequence){
                                    this->waitingForErrorAnswer=false;
                                }
                                
                            }
                        }
                    }
                }
            }
		}
	}
}


DTFrame* ServerClientProxy::getFrame(){
    
	DTFrame* receivedFrame = 0;
    lock();
        if ((this->framesBuffer.size()>0)&&this->enabled){
            receivedFrame=framesBuffer[0];
            vector<DTFrame*>::iterator it = this->framesBuffer.begin();
            this->framesBuffer.erase(it);
        }
    unlock();
    

    return receivedFrame;
}

void ServerClientProxy::retrieveFrames(){
    //proces UDP input and stores it in framesBuffer
    DTFrame* receivedFrame = NULL;
	if (!this->waitingForErrorAnswer){
        do{
            if (this->protocolType==1){
                //text
                receivedFrame = getFrameTextProtocol();
            }
            else{
                receivedFrame = getFrameBinaryProtocol();
            }
            if (receivedFrame!=NULL){
                DTFrame* buffFrameToSpare=NULL;
                lock();
                if (this->framesBuffer.size()>MAX_FRAMES_STORED){
                    //should eliminate the first frame in the vector and release the memory.
                    //this is because we dont wat the application to be storing too many frames.
                    buffFrameToSpare=framesBuffer[0];
                    vector<DTFrame*>::iterator it = this->framesBuffer.begin();
                    this->framesBuffer.erase(it);
                    delete buffFrameToSpare;
                }
                this->framesBuffer.push_back(receivedFrame);
                unlock();
            }
        }while (receivedFrame!=NULL);
    }
    else {
        //si no lleg:o la respuesta, limpio el buffer UDP
        char buff[BUFFLEN];
        int receivedBytes = UDP.Receive(buff,BUFFLEN);
        while (receivedBytes>0){
            receivedBytes = UDP.Receive(buff,BUFFLEN);
        }
    }
    
}


DTFrame* ServerClientProxy::getFrameBinaryProtocol(){
    char buff[BUFFLEN];
	int receivedBytes = UDP.Receive(buff,BUFFLEN);
    //copio la lectura en el buffer principal
    if (receivedBytes>0){
        if((this->intBinaryBuffIndex + receivedBytes)<BUFFLEN){
            memcpy(&(this->binaryBuffer)+this->intBinaryBuffIndex,&buff,receivedBytes);
            //aumento el index del buffer
            this->intBinaryBuffIndex += receivedBytes;
        }
    }
    //Cuando hay OVERFLOW, se pierden paquetes--> se da el mismo tratamiento que frente a un error--> se resincroniza mandando mensaje de error
    
    // intento parsear el primer paquete que figura en el buffer principal
    if (this->intBinaryBuffIndex>18){
        //tengo informacion en el buffer
        //el cabezal del paquete tiene 18 bytes
        //obtengo el largo del contenido de data
        
        u_int8_t hi = this->binaryBuffer[15];
        u_int8_t lo = this->binaryBuffer[16];
        
        int size = (hi << 8) + lo;
        bool error=false;
        if(this->intBinaryBuffIndex >= (18+size)){
            //tengo todo el paquete
            
            
            u_int8_t seqIdHi = this->binaryBuffer[9];
            u_int8_t seqIdLo = this->binaryBuffer[10];
            unsigned short sequence= (seqIdHi << 8) + seqIdLo;
            
            u_int8_t minIdHi = this->binaryBuffer[11];
            u_int8_t minIdLo = this->binaryBuffer[12];
            unsigned short minId = (minIdHi << 8) + minIdLo;
            
            u_int8_t maxIdHi = this->binaryBuffer[13];
            u_int8_t maxIdLo = this->binaryBuffer[14];

            unsigned short maxId= (maxIdHi << 8) + maxIdLo;

            unsigned short finished = this->binaryBuffer[17];
            
            int nextPacketSequence = this->sequence + 1;
            if (nextPacketSequence>65535){
                nextPacketSequence=0;
            }
            
            if (nextPacketSequence==sequence){
                if (this->parcialFrame!=0){
                    //continuo construyendo el frame
                    int cont = 0;
                    for (int q=0; q<size; q+=3){
                        u_int8_t cR= this->binaryBuffer[18 + q];
                        u_int8_t cG= this->binaryBuffer[18 + q + 1];
                        u_int8_t cB= this->binaryBuffer[18 + q + 2];
                        float R = (float) cR;
                        float G = (float) cG;
                        float B = (float) cB;
                        ofVec3f dummyFront(0,0,0);
                        ofVec3f dummyUp(0,0,0);
                        //we pass 0 as ledTypeId since is dummy information
                        DTPixel* newPixel = new DTPixel(minId+cont, R, G, B, 255, 0, 0, 0, dummyFront, dummyUp, "dummy",0);
                        this->parcialFrame->addPixel(newPixel);
                        cont++;
                    }

                }
                else{ 
                    vector<DTPixel*>* newPixels = new vector<DTPixel*>;
                    cl_float* pixelArray = new cl_float[this->pixelQuantity * 4];
                    int cont = 0;
                    for (int q=0; q<size; q+=3){
                        u_int8_t cR= this->binaryBuffer[18 + q];
                        u_int8_t cG= this->binaryBuffer[18 + q + 1];
                        u_int8_t cB= this->binaryBuffer[18 + q + 2];
                        float R = (float) cR;
                        float G = (float) cG;
                        float B = (float) cB;
                        ofVec3f dummyFront(0,0,0);
                        ofVec3f dummyUp(0,0,0);
                        //we pass 0 as ledTypeId since is dummy information
                        DTPixel* newPixel = new DTPixel(minId+cont, R, G, B, 255, 0, 0, 0, dummyFront, dummyUp, "dummy",0);
                        newPixels->push_back(newPixel);
                        
                        pixelArray[((minId+cont)*4)]=(cl_float)R;
                        pixelArray[((minId+cont)*4)+1]=(cl_float)G;
                        pixelArray[((minId+cont)*4)+2]=(cl_float)B;
                        pixelArray[((minId+cont)*4)+3]=255.0f;

                        
                        cont++;
                    }
                    this->parcialFrame= new DTFrame(0, newPixels, pixelArray, this->pixelQuantity, sequence);
                }
                this->sequence=nextPacketSequence;
            
            }
            else{
                //error purgo el frame
                error=true;
                this->errorQty+=1;
                if (this->parcialFrame!=0){
                    delete this->parcialFrame;
                }
                this->parcialFrame=0;
                //continuo en el proximo frame a leer desde el nuevo sequence. pueden generarse frames no completos y mezclados
                //debo purgar todo lo recibido, y enviar mensage de esera y retransmision
                this->sendErrorMessage(this->fps,true,this->sequence);
                while (receivedBytes>0){ //limpio el buffer de UDP
                    receivedBytes = UDP.Receive(buff,BUFFLEN);
                }
                //limpio el buffer recibido
                for(int i=0; i<BUFFLEN;i++){
                    this->binaryBuffer[i]=0;
                    this->intBinaryBuffIndex=0;
                }
            }
            if(!error){
                //elimino la informacion procesada del buffer
                
                clearBinaryBuffer(size);
                
                //verifico si el paquete tiene marca de fin de frame
                if(this->parcialFrame!=0){
                    if (finished!=0){
                        //paquete terminado, retorno el dataFrame
                        DTFrame* returningFrame = this->parcialFrame;
                        this->parcialFrame=0;
                        this->updatePixelsFromDTFrame(returningFrame);
                        this->errorWindow-=1;
                        if(this->errorWindow<0){
                            this->errorQty=0;
                            this->errorWindow=10000;
                        }
                        return returningFrame;
                    }
                }
            }
        }
    }
    return 0;
}

void ServerClientProxy::update(){
    float ifps=0.0f;
    lock();
    ifps=this->newFps;
    unlock();
    if (this->fps!=ifps){
        sendErrorMessage(ifps,false,this->sequence);
        this->fps=ifps;
    }
    
    retrieveFrames();
}

void ServerClientProxy::sendErrorMessage(int messageFps,bool isError,int lastReceivedSequence){
    string message= "<?xml version=\"1.0\" ?><ServerMessage fps='" + ofToString(messageFps) + "' messageError='" + ofToString(isError) + "' lastSequenceNumber='" + ofToString(lastReceivedSequence) + "'></ServerMessage>";
        if (this->TCP.getNumClients()>0){
            //solo se toma el primer cliente.
            for (int i=0; i<TCP.getNumClients(); i++){
                if (TCP.isClientConnected(i)){
                    bool result = TCP.send(i, message);
                    if(isError){
                        if (result){
                            this->waitingForErrorAnswer=true;
                        }
                    }
                }
            }
        }    
}

void ServerClientProxy::synchronizeSpeeds(){
    if (this->errorQty>=10){
        this->fps-=1;
        lock();
        this->newFps=this->fps;
        unlock();
        this->errorQty=0;
        this->errorWindow=10000;
        if (this->fps<10){
            this->fps=10;
        }
        sendErrorMessage(this->fps,false,this->sequence);
    }
}

void ServerClientProxy::clearBinaryBuffer(int size){
    int totalSize = 18 + size; //header + data
    int bufferSwapIndex = 0;
    for (int i = totalSize - 1; i<(BUFFLEN-1); i++){
        this->binaryBuffer[bufferSwapIndex] = this->binaryBuffer[i];
        bufferSwapIndex++;
    }
    this->intBinaryBuffIndex = this->intBinaryBuffIndex - totalSize;
}



DTFrame* ServerClientProxy::getFrameTextProtocol(){
    DTFrame* receivedFrame = 0;
	
	char buff[BUFFLEN];
	int receivedBytes = UDP.Receive(buff,BUFFLEN);
    
	if (receivedBytes>0){
		for (int i=0; i<receivedBytes; i++){
			this->recivedBuffer += buff[i];
		}
	}
	
	string message = getMessage();
	if(message.compare("")!=0){
		receivedFrame = new DTFrame(message);
		this->updatePixelsFromDTFrame(receivedFrame);
		if (receivedFrame->getClientID() == -1){
			delete receivedFrame;
			receivedFrame = 0;
		}
	}
    
	return receivedFrame;
}

void ServerClientProxy::updatePixelsFromDTFrame(DTFrame* frame){
	vector<DTPixel*>* newPixels = frame->getPixels();
	vector<DTPixel*>::iterator it = newPixels->begin();
    int counter=0;
	while (it != newPixels->end())
	{
		DTPixel* newPixel= *it;
        //pixels must be ordered <
        Pixel* localPixel = this->pixelsFast[counter];
		localPixel->blendRGBA(newPixel->getR(),newPixel->getG(),newPixel->getB(),newPixel->getA(),1);
		it++;
        counter++;
	}
}


string ServerClientProxy::getMessage(){
	size_t found;
	string terminator = "[/UDP]";
	found=this->recivedBuffer.find(terminator);
	
	if(found!=string::npos){
		string message = this->recivedBuffer.substr(0,found);
		string auxbuff = this->recivedBuffer.substr(found+6,this->recivedBuffer.length());
		this->recivedBuffer = auxbuff;
		return message;
	}
	return "";
	
}

void ServerClientProxy::configure(string frameData, int clientCode)
{       
        
    int amountOfSends = frameData.length()/8000;
    
    if (this->TCP.getNumClients()>0){
        //solo se toma el primer cliente.
        for (int i=0; i<TCP.getNumClients(); i++){
            if (TCP.isClientConnected(clientCode)){
                bool result = TCP.send(clientCode, frameData);
                for (int sends= 0; sends<amountOfSends; sends++){
                    usleep(3);// so that the message fragments can be sent
                    result = TCP.send(clientCode, "");
                }
            }
        }
    }
}

void ServerClientProxy::addPixel(Pixel* px)
{
	int id = px->getId();
	//this->pixels.insert(pair<int,Pixel*> (id,px));
    this->pixelsFast.push_back(px);
}

void ServerClientProxy::threadedFunction(){
    startListening();
    while(isThreadRunning()) {
        this->update();
        this->listenTCP();
        this->synchronizeSpeeds();
        sleep(25);
    }
	TCP.close();
}

DTFrame* ServerClientProxy::buildFrameToTransmit(){
    vector<Pixel*>::iterator it;
	it=this->pixelsFast.begin();
    
	vector<DTPixel*>* dtVector = new vector<DTPixel*>();
    
    int size=this->pixelsFast.size();
    
    cl_float* newPixelArray = new cl_float[size * 4];
    int cont=0;
	while (it != this->pixelsFast.end())
	{
        Pixel* px = *it;

        int pId = px->getId();
        float pR = px->getR();
        float pG = px->getG();
        float pB = px->getB();
        float pA = px->getA();
        
        DTPixel * pixelForNewFrame= px->getDTPixel();
		
        dtVector->push_back(pixelForNewFrame);
        
        newPixelArray[(cont*4)] = (cl_float)pR;
        newPixelArray[(cont*4)+1] = (cl_float)pG;
        newPixelArray[(cont*4)+2] = (cl_float)pB;
        newPixelArray[(cont*4)+3] = (cl_float)pA;
        cont++;
		it++;
	}
	return new DTFrame(0, dtVector, newPixelArray, this->pixelsFast.size(), 0);
}


void ServerClientProxy::draw(float x, float y, float w, float h){ //not used anymore since Clients do not render on the server UI
	if(drawEnabled){
        glViewport(x, ofGetHeight()-y-h, w-1, h-1);  
              
        // all the matrix setup is copied from the ofGraphics.cpp::void ofSetupScreen() method.  
        float halfFov, theTan, screenFov, aspect;  
        screenFov       = 60.0f;  
        float eyeX      = (float)w / 2.0;  
        float eyeY      = (float)h / 2.0;  
        halfFov         = PI * screenFov / 360.0;  
        theTan          = tanf(halfFov);  
        float dist      = eyeY / theTan;  
        float nearDist  = dist / 10.0;  // near / far clip plane  
        float farDist   = dist * 10.0;  
        aspect          = (float)w/(float)h;  
          
        glMatrixMode(GL_PROJECTION);  
        glLoadIdentity();  
        gluPerspective(screenFov, aspect, nearDist, farDist);  
          
        glMatrixMode(GL_MODELVIEW);  
        glLoadIdentity();  
        gluLookAt(eyeX, eyeY, dist, eyeX, eyeY, 0.0, 0.0, 1.0, 0.0);  
          
        glClear(GL_DEPTH_BUFFER_BIT);  
                  
        glScalef(1, -1, 1);           // invert Y axis so increasing Y goes down.  
        glTranslatef(10, -y-h+10, 0);       // shift origin up to upper-left corner.  

        
        // draw the pixels  
        //ofTranslate(translation.x, translation.y, translation.z);
        ofTranslate(centroid.x, centroid.y, centroid.z);
        ofRotateX(this->rotX);
        ofRotateZ(this->rotY);
        ofTranslate(-centroid.x, -centroid.y, -centroid.z);
        glScalef(scale, scale, scale);
        
        vector<Pixel*>::iterator it = this->pixelsFast.begin();
        while (it!=this->pixelsFast.end())
        {
            Pixel* px = *it;
            ofVec3f position = px->getPosition();
            glPushMatrix();
            glTranslatef(position.x, position.y, position.z);
            ofSetColor(px->getR(),px->getG(), px->getB());
            glCallList(this->displayList);
            glPopMatrix();
            it++;
        } 
          
        // reset viewport back to main screen  
        glFlush(); 
    }
}
