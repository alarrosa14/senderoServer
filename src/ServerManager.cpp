#include "ServerManager.h"

int ServerManager::loadFromFile()
{
	ofxXmlSettings XML;
    
    float maxX=0;
    float minX=0;
    float maxY=0;
    float minY=0;
    float maxZ=0;
    float minZ=0;
	
	if( XML.loadFile("serverConf.xml") ){

		TiXmlElement* myServer=XML.doc.RootElement();
		if (!myServer){
			return -1;
		}
        
        string teststr = this->installationName = myServer->Attribute("installationName");
        cout << teststr << endl;
        
        TiXmlElement* myDevices=myServer->FirstChildElement();
        if (!myDevices){
            return -1;
        }
        
        TiXmlElement* device=myDevices->FirstChildElement();
		if (!device){
			return -1;
		}
        
        TiXmlElement* myMeshes=myDevices->NextSiblingElement();
        if (!myMeshes){
            return -1;
        }
        
        TiXmlElement* mesh=myMeshes->FirstChildElement();
		if (!mesh){
			return -1;
		}
        
        TiXmlElement* ledTypes=myMeshes->NextSiblingElement();
		if (!ledTypes){
			return -1;
		}
        
        TiXmlElement* ledType=ledTypes->FirstChildElement();
		if (!ledType){
			return -1;
		}
        
		TiXmlElement* myPixels=ledTypes->NextSiblingElement();
		if (!myPixels){
			return -1;
		}
		TiXmlElement* pixel=myPixels->FirstChildElement();
		if (!pixel){
			return -1;
		}
		TiXmlElement* myClientProxys= myPixels->NextSiblingElement();
		if (!myClientProxys){
			return -1;
		}
		TiXmlElement* clientProxy= myClientProxys->FirstChildElement();
		if (!clientProxy){
			return -1;
		}
		
		/*
		TiXmlElement* multicastConfig= myClientProxys->NextSiblingElement();
		if (!multicastConfig){
			return -1;
		}
		*/
		TiXmlElement* streamServerConfig= myClientProxys->NextSiblingElement();
        cout << "El stream server config: " << streamServerConfig << endl;
		if (!streamServerConfig){
			return -1;
		}

		//configuracion de instalacion
		string instName = "installationName";
		if(myServer->Attribute(instName.c_str())){
			this->installationName = myServer->Attribute("installationName");
		}
		else{
			return -1;
		}
		
		string pixelQ = "pixelQuantity";
		if(myServer->Attribute(pixelQ.c_str())){
			this->pixelQuantity = ofToInt(myServer->Attribute("pixelQuantity"));
		}
		else{
			return -1;
		}
        
        this->pixelArray = new cl_float[this->pixelQuantity * 4];
        
        //configuracion de devices
        
        for( device; device; device=device->NextSiblingElement())
		{	//asuming all devices are mBeds
            
            string nameNameDevice= "name";
            string nameDevice="";
            if(device->Attribute(nameNameDevice.c_str())){
                nameDevice=device->Attribute("name");
            }
            else{
                return -1;
            }
            
            string idNameDevice= "id";
            int idDevice=0;
            if(device->Attribute(idNameDevice.c_str())){
                idDevice=ofToInt(device->Attribute("id"));
            }
            else{
                return -1;
            }

            
            string ipNameDevice= "ipAddress";
            string IP="";
            if(device->Attribute(ipNameDevice.c_str())){
                IP=device->Attribute("ipAddress");
            }
            else{
                return -1;
            }
            
            string udpPortDevice = "UDPPort";
            int portDevice=0;
            if(device->Attribute(udpPortDevice.c_str())){ 
                portDevice=ofToInt(device->Attribute("UDPPort"));
            }
            else {
                return -1;
            }
            
            string enabledDevice = "enabled";
            bool deviceEnabled = false;
            if(device->Attribute(enabledDevice.c_str())){ 
                deviceEnabled=ofToInt(device->Attribute("enabled"));
            }
            
            LightController* newDevice = new LightController(idDevice, nameDevice, IP, deviceEnabled, portDevice,this->DVCManager);
            
            this->DVCManager->addLightController(newDevice);
            
            
            DeviceUI* deviceParams = new DeviceUI(idDevice, nameDevice, deviceEnabled);
            this->devicesParams->insert(pair<int,DeviceUI*> (idDevice,deviceParams));
        }
        
        //meshes
        for( mesh; mesh; mesh=mesh->NextSiblingElement())
		{	//asuming all devices are mBeds
            
            string meshNameName= "name";
            string nameMesh="";
            if(mesh->Attribute(meshNameName.c_str())){
                nameMesh=mesh->Attribute("name");
            }
            else{
                return -1;
            }
            
            string pathName= "path";
            string meshPath="";
            if(mesh->Attribute(pathName.c_str())){
                meshPath=mesh->Attribute("path");
            }
            else{
                return -1;
            }
            
            ofxAssimpModelLoader* model= new ofxAssimpModelLoader();
            
            model->loadModel(meshPath);
            
            model->setScaleNormalization(false);
            
            model->setScale(1.0f, 1.0f, 1.0f);
            
            this->meshes->insert(pair<string,ofxAssimpModelLoader*> (nameMesh,model));
        }
        
        //process LEDTypes
        for( ledType; ledType; ledType=ledType->NextSiblingElement())
		{	//asuming all devices are mBeds
            
            string ledTypeIdName= "id";
            int id=0;
            if(ledType->Attribute(ledTypeIdName.c_str())){
                id=ofToInt(ledType->Attribute("id"));
            }
            else{
                return -1;
            }
            
            LEDType* newLedType = new LEDType(id);
            
            TiXmlElement* colorPair=ledType->FirstChildElement();
            if (!colorPair){
                return -1;
            }
            for( colorPair; colorPair; colorPair=colorPair->NextSiblingElement()){
                
                string r0Name= "r0";
                int r0=0;
                if(colorPair->Attribute(r0Name.c_str())){
                    r0=ofToInt(colorPair->Attribute("r0"));
                }
                else{
                    return -1;
                }
                string g0Name= "g0";
                int g0=0;
                if(colorPair->Attribute(g0Name.c_str())){
                    g0=ofToInt(colorPair->Attribute("g0"));
                }
                else{
                    return -1;
                }
                string b0Name= "b0";
                int b0=0;
                if(colorPair->Attribute(b0Name.c_str())){
                    b0=ofToInt(colorPair->Attribute("b0"));
                }
                else{
                    return -1;
                }
                
                string r1Name= "r1";
                int r1=0;
                if(colorPair->Attribute(r1Name.c_str())){
                    r1=ofToInt(colorPair->Attribute("r1"));
                }
                else{
                    return -1;
                }
                string g1Name= "g1";
                int g1=0;
                if(colorPair->Attribute(g1Name.c_str())){
                    g1=ofToInt(colorPair->Attribute("g1"));
                }
                else{
                    return -1;
                }
                string b1Name= "b1";
                int b1=0;
                if(colorPair->Attribute(b1Name.c_str())){
                    b1=ofToInt(colorPair->Attribute("b1"));
                }
                else{
                    return -1;
                }
                
                newLedType->addEquivalentPair(ofColor((float)r0,(float)g0, (float)b0), ofColor((float) r1,(float) g1, (float) b1));
                newLedType->setGamma(2.2f);
                newLedType->useGamma();


            
            }
            //add to device manager
            this->DVCManager->addLedType(newLedType);
        }

        
        int pixelCounter=0;
		//configuracion de pixels
		for( pixel; pixel; pixel=pixel->NextSiblingElement())
		{	
			string idName = "id";
			string rName = "r";
			string gName = "g";
			string bName = "b";
			string aName = "a";

            string deviceName = "device";

			int id=0;
			if (pixel->Attribute(idName.c_str())){
				id=ofToInt(pixel->Attribute("id"));
			}
			else{
				return -1;
			}
			float r = 0;
			if (pixel->Attribute(rName.c_str())){
				r=ofToFloat(pixel->Attribute("r"));
			}
			else{
				r=0;
			}
			float g=0;
			if (pixel->Attribute(gName.c_str())){
				g=ofToFloat(pixel->Attribute("g"));
			}
			else{
				g=0;
			}
			float b=0;
			if (pixel->Attribute(bName.c_str())){
				b=ofToFloat(pixel->Attribute("b"));
			}
			else{
				b=0;
			}
			float a=0;
			if (pixel->Attribute(aName.c_str())){
				a=ofToFloat(pixel->Attribute("a"));
			}
			else{
				a=255.0f;
			}
            
            int pixelDevice=0;
			if (pixel->Attribute(deviceName.c_str())){
				pixelDevice=ofToFloat(pixel->Attribute("device"));
			}
			else{
                return -1;
			}
            
            string ledTypeName = "ledType";
            int ledTypeId=0;
            if(pixel->Attribute(ledTypeName.c_str())){
                ledTypeId=ofToInt(pixel->Attribute("ledType"));
            }
            
            TiXmlElement* render=pixel->FirstChildElement();
            if (!render){
                return -1;
            }
            
            string meshName= "mesh";
            string linkedMesh="";
            if(render->Attribute(meshName.c_str())){
                linkedMesh=render->Attribute("mesh");
            }
            else{
                return -1;
            }
            
            ofVec3f front;
            ofVec3f up;
            ofVec3f positionPixel;
            
            TiXmlElement* frontElement=render->FirstChildElement();
            if (!frontElement){
                return -1;
            }

            TiXmlElement* upElement=frontElement->NextSiblingElement();
            if (!upElement){
                return -1;
            }
            
            TiXmlElement* positionElement=upElement->NextSiblingElement();
            if (!positionElement){
                return -1;
            }
            
            string xName = "x";
			string yName = "y";
			string zName = "z";
            
            
			float xFront=0.0f;
			if (frontElement->Attribute(xName.c_str())){
                string test = frontElement->Attribute("x");
				xFront=ofToFloat(frontElement->Attribute("x"));
                //cout << xFront << endl;
			}
			else{
				xFront=0.0f;
			}
            
			float yFront=0.0f;
			if (frontElement->Attribute(yName.c_str())){
				yFront=ofToFloat(frontElement->Attribute("y"));
			}
			else{
				yFront=0.0f;
			}
            
			float zFront=0.0f;
			if (frontElement->Attribute(zName.c_str())){
				zFront=ofToFloat(frontElement->Attribute("z"));
			}
			else{
				zFront=0.0f;
			}
            
            float xUp=0.0f;
			if (upElement->Attribute(xName.c_str())){
				xUp=ofToFloat(upElement->Attribute("x"));
			}
			else{
				xUp=0.0f;
			}
            
			float yUp=0.0f;
			if (upElement->Attribute(yName.c_str())){
				yUp=ofToFloat(upElement->Attribute("y"));
			}
			else{
				yUp=0.0f;
			}
            
			float zUp=0.0f;
			if (upElement->Attribute(zName.c_str())){
				zUp=ofToFloat(upElement->Attribute("z"));
			}
			else{
				zUp=0.0f;
			}
            
            float xPos=0.0f;
			if (positionElement->Attribute(xName.c_str())){
                string test = positionElement->Attribute("x");
				xPos=ofToFloat(test);
                //cout << xPos << endl;
			}
			else{
				xPos=0.0f;
			}
            
			float yPos=0.0f;
			if (positionElement->Attribute(yName.c_str())){
				yPos=ofToFloat(positionElement->Attribute("y"));
			}
			else{
				yPos=0.0f;
			}
            
			float zPos=0.0f;
			if (positionElement->Attribute(zName.c_str())){
				zPos=ofToFloat(positionElement->Attribute("z"));
			}
			else{
				zPos=0.0f;
			}
            
            //cout<< "-------------- id: " << id << endl;
            
            //cout << "positionPixel x:" << xPos << " y:" << yPos << " z:" << zPos << endl;
            
            
            
            positionPixel.x = xPos; 
            positionPixel.y = yPos; 
            positionPixel.z = zPos; 
            
            //cout << "front x:" << xFront << " y:" << yFront << " z:" << zFront << endl;

            
            front.x = xFront;
            front.y = yFront;
            front.z= zFront;
            
            //cout << "up x:" << xUp << " y:" << yUp << " z:" << zUp << endl;

            
            up.x = xUp;
            up.y = yUp;
            up.z = zUp;
            
			Pixel* newPixel = new Pixel(id,r,g,b,a,positionPixel,ledTypeId);
            
            front.normalize();
            up.normalize();
            
            ofMatrix4x4 rotFront;
            
            ofVec3f sourceFront(1,0,0); //already normalized
            ofVec3f sourceUp(0,0,1); //already normalized
            
            ofQuaternion qFront;
            
            qFront.makeRotate(sourceFront, front);
           
            ofVec3f newSourceUp = rotFront * sourceUp;
            
            //cout << "New Up x:" << newUp.x << " y:" << newUp.y << " z:" << newUp.z << endl;
            
            ofQuaternion qUp;
            
            qUp.makeRotate(newSourceUp, up);
            
            ofQuaternion qTot =  qFront; // * qUp esto anda mal. hay que multiplicar por la rotación up para que todo esté correcto
            
            // we save the rotation in the pixel
            
            newPixel->setRotQ(qTot);
            
            //now we link the mesh
            
            map<string,ofxAssimpModelLoader*>::iterator itModel = this->meshes->find(linkedMesh);
                
            if (itModel!=this->meshes->end()){
                ofxAssimpModelLoader* model = itModel->second;
                newPixel->setModel(model);
            }
            
            newPixel->setModelName(linkedMesh);
            
            newPixel->setFront(front);
            newPixel->setUp(up);
            
			this->pixels->insert(pair<int,Pixel*> (id,newPixel));
            this->pixelsFast->push_back(newPixel);
            
            this->pixelArray[(id*4)] = (cl_float)r;
            this->pixelArray[(id*4)+1] = (cl_float)g;
            this->pixelArray[(id*4)+2] = (cl_float)b;
            this->pixelArray[(id*4)+3] = (cl_float)a;
            
            
            this->DVCManager->addPixelToDevice(newPixel->getId(), pixelDevice);
            
            
            if (pixelCounter==0){
                maxX=positionPixel.x;
                minX=positionPixel.x;
                maxY=positionPixel.y;
                minY=positionPixel.y;
                maxZ=positionPixel.z;
                minZ=positionPixel.z;
            }
            else {
                if (positionPixel.x>maxX){
                    maxX=positionPixel.x;
                }
                
                if (positionPixel.x<minX){
                    minX=positionPixel.x;
                }
                
                if (positionPixel.y>maxY){
                    maxY=positionPixel.y;
                }
                
                if (positionPixel.y<minY){
                    minY=positionPixel.y;
                }
                
                if (positionPixel.z>maxZ){
                    maxZ=positionPixel.z;
                }
                
                if (positionPixel.z<minZ){
                    minZ=positionPixel.z;
                }
            }
            
            pixelCounter++;
		}
        
        this->centroid.x= (maxX+minX)/2;
        this->centroid.y= (maxY+minY)/2;
        this->centroid.z= (maxZ+minZ)/2;
        
		//configuracion de ServerClientProxy
		for( clientProxy; clientProxy; clientProxy=clientProxy->NextSiblingElement())
		{
			string idName = "id";

			int id=0;
			if (clientProxy->Attribute(idName.c_str())){
				id = ofToInt(clientProxy->Attribute("id"));
			}
			else{
				return -1;
			}
			string iName = "name";
			string name="";
			if (clientProxy->Attribute(iName.c_str())){
				name=clientProxy->Attribute("name");
			}
			else{
				return -1;
			}
			
			string enabledName = "enabled";
			bool enabled = false;
			if (clientProxy->Attribute(enabledName.c_str())){
				enabled = ofToInt(clientProxy->Attribute("enabled"));
			}
			else{
				return -1;
			}
            
            string protocolTypeName = "protocolType";
			int protocolType = 0;
			if (clientProxy->Attribute(protocolTypeName.c_str())){
				protocolType = ofToInt(clientProxy->Attribute("protocolType"));
			}
			
			string tcpName = "TCPPort";
			int TCPPort = -1;

			if(clientProxy->Attribute(tcpName.c_str())){
				TCPPort = ofToInt(clientProxy->Attribute("TCPPort"));
			}
			else{
				return -1;
			}

			string udpName = "UDPPort";
			int UDPPort = -1;
			
			if(clientProxy->Attribute(udpName.c_str())){
				UDPPort = ofToInt(clientProxy->Attribute("UDPPort"));
			}
			else{
				return -1;
			}

			float blendFactor = 0.0f;
			string blendName = "blendFactor";
			if(clientProxy->Attribute(blendName.c_str())){
				blendFactor = ofToFloat(clientProxy->Attribute("blendFactor"));
			}
			else{
				return -1;
			}
			
			ServerClientProxy* newClient = new ServerClientProxy(TCPPort,UDPPort,id,name,enabled,blendFactor,protocolType, this->pixelQuantity);
            /*newClient->centroid.x=this->centroid.x;
            newClient->centroid.y=this->centroid.y;
            newClient->centroid.z=this->centroid.z;
            newClient->translation.x=this->translation.x;
            newClient->translation.y=this->translation.y;
            newClient->translation.z=this->translation.z;
             */
			this->clients->insert(pair<int,ServerClientProxy*> (id,newClient));
            this->clientsFast->push_back(newClient);
            
            ClientUI* cliParams = new ClientUI(id, name, enabled, blendFactor, 24); //24 is the default fps
            this->clientsParams->insert(pair<int,ClientUI*> (id,cliParams));
            this->clientParamsFast->push_back(cliParams);
            
            //this->clientsParams->push_back(cliParams);

            
		}		

		//configuracion de MulticastManager
		/*
		string enabledMCName="broadcastEnabled";
		if (multicastConfig->Attribute(enabledMCName.c_str())){
			this->MCManager->setBroadcastState(ofToInt(multicastConfig->Attribute("broadcastEnabled")));
		}
		else{
			return -1;
		}

		string groupAddressName = "groupAddress";
		if (multicastConfig->Attribute(groupAddressName.c_str())){
			this->MCManager->setGroupAddress(multicastConfig->Attribute("groupAddress"));
		}
		else{
			return -1;
		}

		string ipMCName = "ipAddress";
		if (multicastConfig->Attribute(ipMCName.c_str())){
			this->MCManager->setIPAddress(multicastConfig->Attribute("ipAddress"));
		}
		else{
			return -1;
		}
		
		string udpMCName = "UDPPort";
		if(multicastConfig->Attribute(udpMCName.c_str())){
			this->MCManager->setUDPPort(ofToInt(multicastConfig->Attribute("UDPPort")));
		}
		else{
			return -1;
		}
		*/

		string enabledName="enabled";
		if (streamServerConfig->Attribute(enabledName.c_str())){
			this->SSManager->setEnabled(ofToInt(streamServerConfig->Attribute("enabled")));
		}
		else{
			return -1;
		}

		string addressName = "ipAddress";
		if (streamServerConfig->Attribute(addressName.c_str())){
			this->SSManager->setAddress(streamServerConfig->Attribute("ipAddress"));
		}
		else{
			return -1;
		}

		string portName = "port";
		if (streamServerConfig->Attribute(portName.c_str())){
			this->SSManager->setPort(ofToInt(streamServerConfig->Attribute("port")));
		}
		else{
			return -1;
		}

		this->SSManager->setPixelQuantity(this->pixelQuantity);

		string compression = "compression";
		if(streamServerConfig->Attribute(compression.c_str())){
			this->SSManager->setCompressionEnabled(ofToInt(streamServerConfig->Attribute("compression")));
		}
		else{
			return -1;
		}

	}else{
		//error
		return -1;
	}
}

void ServerManager::setup()
{
    this->clients = new map<int,ServerClientProxy*>;
    this->clientsFast = new vector<ServerClientProxy*>;
	// this->MCManager= new MultiCastManager();
    this->SSManager= new StreamServerManager();
    this->DVCManager= new DeviceManager();
    this->worker = new Worker(this, 20);

	this->pixelQuantity=0;
	this->pixels= new map<int,Pixel*>;
    this->pixelsFast=new vector<Pixel*>;
    this->clientsParams=new map<int,ClientUI*>;
    
    this->clientParamsFast=new vector<ClientUI*>;
    this->devicesParams=new map<int,DeviceUI*>;
    this->lastFrameBuffer=new map<int,DTFrame*>;
    this->meshes=new map<string,ofxAssimpModelLoader*>;
	this->ticks=0;
	this->elapsedTicks=0;
	this->decayFactor=0.0f;
	this->enableDraw=true;
    this->showFrameRate=false;
    this->mouseRotEnabled=false;
	this->flash=false;
    this->quadric = gluNewQuadric();
    this->realFrameRate=0;
    this->averageFrameRate=0;
    this->averageQty=200;
    this->sumFrameRate=0;
    this->FRTicks=0;
    this->FRElapsedTicks=0;
    this->flashR=255;
    this->flashG=255;
    this->flashB=255;
    
	this->elapsedTicks=0;
	this->ticks=0;

    double framesPerSecond = 30;
    ofSetFrameRate(framesPerSecond);
	this->frameRateInterval = 1.0f / framesPerSecond * 1000.0f;
    
    this->drawTicks=0;
    this->drawElapsedTicks=0;
    this->drawTicksToReach=1.0f/30.0f * 1000.0f;
    this->doDraw=false;
    
    this->showGUI=true;
    this->drawAxis=true;
    
	this->loadFromFile();
	this->loadClientPixels();

	ticks = 0;
	rotX = 0.0;
	rotY = 0.0;
	scale=1.0;

	//fondo negro
	ofBackground(0, 0, 0);
    //ofDisableArbTex();
	glEnable(GL_DEPTH_TEST);

	ofSetupScreen();
        
    cam.setDistance(600);
    
    //midi output
    
    midiOutController=new MidiController();
    midiOutController->setup();
    this->midiEnabled=false;
	
    //setup General GUI
    
    this->setGUIGeneral();
    
    //setup Devices GUI
    
    this->setGUIDevices();
    
    //setup Calibration GUI
    
    this->setGUICalibration();
    
    //devices
    this->DVCManager->setup();
    this->worker->setup();
	//multicast manager
	//this->MCManager->setupMulticastSender();
    this->SSManager->setupStreamingSender();
    
    //create displaylist for spheres
    this->displayList = glGenLists(1);
    glNewList(this->displayList, GL_COMPILE);
        gluSphere(quadric, 0.5, 4, 4); // param 3: radio, param 4 y 5 determinan level of detail.
    glEndList();
    
    myfont.loadFont("Arial.ttf", 12);
    logo.loadImage("bondi.png");
    
    float width = (float) ofGetWidth();
    float height = (float) ofGetHeight();
    
    translation.x=width/4;
    translation.y=height/4;
    translation.z=0;
    
    gpuManager= new GPUManager(this->pixelQuantity);
    
    //
    //this->MCManager->startThread(true);
    this->SSManager->startThread(true);
    this->DVCManager->startThread(true);
    this->midiOutController->startThread(true);
    vector<ServerClientProxy*>::iterator itClients;
	itClients=this->clientsFast->begin();
	while (itClients != this->clientsFast->end())
	{   
		ServerClientProxy* client=*itClients;
        client->startThread(true);
		itClients++;
	}

    this->worker->startThread();   
}

void ServerManager::workerLoop(){
    
    worker->lock();
    bool _flash = flash;
    worker->unlock();

    if (_flash){

        worker->lock();
        DTFrame* flashFrame = this->buildFrameToTransmit();
        worker->unlock();

        vector<DTPixel*>* pixels = flashFrame->getPixels();
        vector<DTPixel*>::iterator it = pixels->begin();
        while (it != pixels->end())
        {
            DTPixel* newPixel=*it;
            newPixel->setR(flashR);
            newPixel->setG(flashG);
            newPixel->setB(flashB);
            newPixel->setA(255);

            it++;
        }

        worker->lock();
        this->blendFromFrame(flashFrame,1.0f);
        worker->unlock();

        delete flashFrame;

    } else {
    
        vector<ServerClientProxy*>::iterator it;
        it=this->clientsFast->begin();
        vector<cl_float*>* framesToBlend = new vector<cl_float*>;
        vector<bool>* availableFrames = new vector<bool>;
        vector<DTFrame*>* usedDTFrames = new vector<DTFrame*>; // to be deleted when processing ends
        vector<float>* blendFactors = new vector<float>;
        
        int prevTicksCli=ofGetElapsedTimeMillis();
        
        while (it != this->clientsFast->end())
        {
            ServerClientProxy* client=*it;
            int clientID= client->getId();
            DTFrame* newFrame = client->getFrame();
            if (newFrame!=0)
            {
                framesToBlend->push_back(newFrame->getPixelsArray());
                availableFrames->push_back(true);
                blendFactors->push_back(client->getBlendFactor());
                
                map<int,DTFrame*>::iterator lastFrameIT = this->lastFrameBuffer->find(clientID);
                if (lastFrameIT!=this->lastFrameBuffer->end()){
                    //i have a previous frame
                    DTFrame* prevFrame=lastFrameIT->second;
                    
                    //last frame is scheduled to delete
                    usedDTFrames->push_back(prevFrame);
                    
                    //erase last frame from the collection
                    this->lastFrameBuffer->erase(lastFrameIT);
                    
                    //add new frame to last frames collection
                    this->lastFrameBuffer->insert(pair<int,DTFrame*> (clientID,newFrame));
                }
                else{
                    //i dont have a previuous frame
                    //add new frame to last frames collection
                    this->lastFrameBuffer->insert(pair<int,DTFrame*> (clientID,newFrame));
                }
                
            }
            else {
                //if I have a previous frame -> i use it, if not -> I pass Null
                map<int,DTFrame*>::iterator lastFrameIT = this->lastFrameBuffer->find(clientID);
                if (lastFrameIT!=this->lastFrameBuffer->end()){
                    DTFrame* previousDTFrame=lastFrameIT->second;
                    framesToBlend->push_back(previousDTFrame->getPixelsArray());
                    availableFrames->push_back(true);
                    blendFactors->push_back(client->getBlendFactor());
                }
                else{
                    framesToBlend->push_back(0);
                    availableFrames->push_back(false);
                    blendFactors->push_back(0);
                    usedDTFrames->push_back(0);
                }
            }

            it++;
        }

        /////// perform calculations on GPU
        /////// TO_DO change to CPU calculation
        
        this->gpuManager->setup(this->pixelArray, framesToBlend, blendFactors, availableFrames, this->decayFactor);
        this->gpuManager->execute();
        this->pixelArray=this->gpuManager->cleanup(this->pixelArray);

        /////// end calculations on GPU
        
        //delete used DTFrames
        vector<DTFrame*>::iterator itFrames;
        itFrames=usedDTFrames->begin();
        while (itFrames != usedDTFrames->end())
        {
            DTFrame* frame=*itFrames;
            delete frame;
            itFrames++;
        }
        
        //delete used vectors
        delete framesToBlend;
        delete availableFrames;
        delete usedDTFrames;
        delete blendFactors;

        worker->lock();

        for(int p=0; p<this->pixelQuantity; p++){
            Pixel* myPixel= this->pixelsFast->operator[](p);
            float r = this->pixelArray[p*4];
            float g = this->pixelArray[(p*4)+1];
            float b = this->pixelArray[(p*4)+2];
            float a = this->pixelArray[(p*4)+3];
            
            myPixel->r=r;
            myPixel->g=g;
            myPixel->b=b;
            myPixel->a=a;
        }

        worker->unlock();
    }

    if (_flash){
        worker->lock();
        flash=false;
        worker->unlock();   
    }
}

void ServerManager::update()
{
	
	this->ticks = ofGetElapsedTimeMillis();
    this->FRTicks = this->ticks;
    float ticksInterval = this->FRTicks - this->FRElapsedTicks;
    this->realFrameRate = 1000.0f/ticksInterval;
    this->FRElapsedTicks = this->FRTicks;
    
    if (this->averageQty>0){
        this->sumFrameRate += this->realFrameRate;
        this->averageQty-=1;
    }
    else {
        this->averageQty=200;
        this->averageFrameRate = this->sumFrameRate / 200.0f;
        this->sumFrameRate=0;
    }

    worker->lock();
    DTFrame* transmitFrameDevices = this->buildFrameToTransmit();
    DTFrame* transmitFrame = this->buildFrameToTransmit();
    worker->unlock();

    this->DVCManager->addFrameToControllersBuffer(transmitFrameDevices);
    this->sendFrameToStreamingServer(transmitFrame);   
    
    if (midiEnabled){
        DTFrame* transmitFrameMidi = this->buildFrameToTransmit();
        this->midiOutController->addFrameToMidiBuffer(transmitFrameMidi);
    }
}

void ServerManager::draw()
{
    ofBackground(50);
    ofSetColor(255,255,255);
    if(this->showFrameRate){
        myfont.drawString("Frame Rate: " + ofToString(this->averageFrameRate), 10, ofGetHeight() - 40);
    }
    logo.draw(ofGetWidth()-150, ofGetHeight()-100);
    cam.begin();
    if (this->drawAxis){
        //axis
        ofSetColor(255,0,0);
        ofSetLineWidth(3);
        ofLine(0, 0, 0, 100, 0, 0);
        ofSetColor(0,255,0);
        ofLine(0, 0, 0, 0, 100, 0);
        ofSetColor(0,0,255);
        ofLine(0, 0, 0, 0, 0, 100);
        //axis
    }
    if (this->enableDraw){

            glPushMatrix();
            //ofScale(5,5,5);
            float width = (float) ofGetWidth();
            float height = (float) ofGetHeight();

            worker->lock();
            vector<Pixel*>::iterator it = this->pixelsFast->begin();
            while (it!=this->pixelsFast->end())
            {
                Pixel* px = *it;
                px->draw();
                it++;
            }
            worker->unlock();
            glPopMatrix();
    }
    cam.end();
}

void ServerManager::keyPressed(int key){
    if (key == 'h'){
        this->showGUI= !this->showGUI;
        this->generalGUI->toggleVisible();
        this->devicesGUI->toggleVisible();
    }
}


void  ServerManager::mousePressed(int x, int y, int button){
    
}

void  ServerManager::mouseReleased(){
    
}

void  ServerManager::mouseMoved(int x, int y){
    
    if (showGUI){
        if(inGUIZone(x)){
            if(mouseRotEnabled){
                cam.disableMouseInput();
                mouseRotEnabled=false;
            }
        }
        else{
            if(!mouseRotEnabled){
                cam.enableMouseInput();
                mouseRotEnabled=true;
            }
        }
    }
    else{
        if(!mouseRotEnabled){
            cam.enableMouseInput();
            mouseRotEnabled=true;
        }
    }
}

bool ServerManager::inGUIZone(int x){
    bool ret=false;
    
    if (x<510)
        ret=true;
    
    return ret;
}

void ServerManager::setClientState(int clientID, bool newState)
{
	map<int,ServerClientProxy*>::iterator it;
	it=this->clients->find(clientID);
	if (it != this->clients->end()){
		ServerClientProxy* foudedClient=it->second;
		foudedClient->setState(newState);
	}

}

void ServerManager::setMulticastChannelState(bool newState)
{
	//this->MCManager->setBroadcastState(newState);
}

void ServerManager::blendFromFrame(DTFrame* newFrame, float blendFactor)
{
	vector<DTPixel*>* newPixels = newFrame->getPixels();
	vector<DTPixel*>::iterator newPixelsIT = newPixels->begin();
    int counter =0;
	while (newPixelsIT != newPixels->end())
	{
		DTPixel* newPixel=*newPixelsIT;
        Pixel* myPixel = this->pixelsFast->operator[](counter);
        myPixel->blendRGBA(0,0,0,0, this->decayFactor);
        
        float r=newPixel->getR();
        float g=newPixel->getG();
        float b=newPixel->getB();
        float a=newPixel->getA();
        
        myPixel->blendRGBA(r,g,b,a, blendFactor);
        
        this->pixelArray[(counter*4)] = (cl_float)r;
        this->pixelArray[(counter*4)+1] = (cl_float)g;
        this->pixelArray[(counter*4)+2] = (cl_float)b;
        this->pixelArray[(counter*4)+3] = (cl_float)a;
        
		newPixelsIT++;
        counter++;
	}
}

/* void ServerManager::sendFrameToMulticastChannel(DTFrame* transmitFrame)
{
    this->MCManager->addFrameToSendBuffer(transmitFrame);
}*/
void ServerManager::sendFrameToStreamingServer(DTFrame* transmitFrame)
{
    this->SSManager->addFrameToSendBuffer(transmitFrame);
}


DTFrame* ServerManager::buildFrameToTransmit()
{
	vector<Pixel*>::iterator it;
	it=this->pixelsFast->begin();
	vector<DTPixel*>* dtVector = new vector<DTPixel*>();
    cl_float* newPixelArray = new cl_float[this->pixelQuantity * 4];
	while (it != this->pixelsFast->end())
	{
		Pixel* myPx=*it;
		dtVector->push_back(myPx->getDTPixel());
        
        int id=myPx->getId();
        float pR=myPx->getR();
        float pG=myPx->getG();
        float pB=myPx->getB();
        float pA=myPx->getA();

        newPixelArray[(id*4)]=(cl_float)pR;
        newPixelArray[(id*4)+1]=(cl_float)pG;
        newPixelArray[(id*4)+2]=(cl_float)pB;
        newPixelArray[(id*4)+3]=(cl_float)pA;
        
		it++;
	}

	return new DTFrame(this->ticks,dtVector,newPixelArray,this->pixelQuantity,0);
}

void ServerManager::setDrawingState(bool newState)
{
}

void ServerManager::decay(int r, int g, int b, int a)
{
	vector<Pixel*>::iterator it;
	it=this->pixelsFast->begin();
	while (it != this->pixelsFast->end())
	{
		Pixel* px=*it;
		px->blendRGBA(r,g,b,a, this->decayFactor);
		it++;
	}
}

void ServerManager::loadClientPixels()
{
	vector<Pixel*>::iterator it;
	vector<ServerClientProxy*>::iterator itCli;
	it=this->pixelsFast->begin();
	while (it != this->pixelsFast->end())
	{
		Pixel* px=*it;
		itCli=this->clientsFast->begin();
		while (itCli != this->clientsFast->end())
		{
			ServerClientProxy* client=*itCli;
            //we pass 0 as ledTypeId since is dummy information
			Pixel * newPixel = new Pixel(px->getId(),px->getR(),px->getG(),px->getB(),px->getA(),px->getPosition(),0);
            
            newPixel->setModelName(px->getModelName());
            
            newPixel->setFront(px->getFront());
            newPixel->setUp(px->getUp());
            
			client->addPixel(newPixel);
			itCli++;
		}
		it++;
	}
}

void ServerManager::exit(){
    ////this->MCManager->stopThread();
    this->SSManager->stopThread();
    this->DVCManager->stopThread();
    this->worker->stopThread();
    this->midiOutController->stopThread();
    
    vector<ServerClientProxy*>::iterator itClients;
	itClients=this->clientsFast->begin();
	while (itClients != this->clientsFast->end())
	{
		ServerClientProxy* client=*itClients;
		client->stopThread();
		itClients++;
	}
    this->gpuManager->exit();
    //TODO should eliminate all entities
}

void ServerManager::setGUIGeneral(){
    float dim = 16;
	float xInit = OFX_UI_GLOBAL_WIDGET_SPACING; 
    float length = 255-xInit;  
    
    this->generalGUI = new ofxUIScrollableCanvas(0, 0, length+xInit, ofGetHeight());
    this->generalGUI->setStickDistance(1.0f);
    
    generalGUI->addWidgetDown(new ofxUILabel("GENERAL SETTINGS", OFX_UI_FONT_LARGE)); 
    generalGUI->addWidgetDown(new ofxUILabel("Press 'h' to hide the GUI", OFX_UI_FONT_LARGE));
    generalGUI->addSpacer(length-xInit, 2); 
    generalGUI->addSlider("decay factor", 0.0f, 1.0f, this->decayFactor, length-xInit, dim);
    generalGUI->addToggle( "mc_channelEnabled", false, dim, dim);
    generalGUI->addToggle( "draw_enabled", this->enableDraw, dim, dim);
    generalGUI->addToggle( "draw_axis", this->drawAxis, dim, dim);
    generalGUI->addToggle( "show_framerate", this->showFrameRate, dim, dim);
    generalGUI->addButton("flash", false, dim, dim);
    generalGUI->addSlider("flashR", 0.0f, 255.0f, this->flashR, length-xInit, dim);
    generalGUI->addSlider("flashG", 0.0f, 255.0f, this->flashG, length-xInit, dim);
    generalGUI->addSlider("flashB", 0.0f, 255.0f, this->flashB, length-xInit, dim);
    generalGUI->addSpacer(length-xInit, 2);
    generalGUI->addToggle( "midi_enabled", this->midiEnabled, dim, dim);
    generalGUI->addWidgetDown(new ofxUIRotarySlider(dim*4, 0.0f, 255.0f, this->midiOutController->getThresholdON(), "midi_LThreshON")); 
    generalGUI->addWidgetRight(new ofxUIRotarySlider(dim*4, 0.0f, 255.0f, this->midiOutController->getThresholdOFF(),"midi_LThreshOFF")); 
    generalGUI->addWidgetDown(new ofxUIRotarySlider(dim*4, 0.0f, 255.0f, this->midiOutController->getThresholdON(), "midi_GThreshON")); 
    generalGUI->addWidgetRight(new ofxUIRotarySlider(dim*4, 0.0f, 255.0f, this->midiOutController->getThresholdOFF(),"midi_GThreshOFF"));
    generalGUI->addWidgetDown(new ofxUIRotarySlider(dim*4, 0.0f, 20.0f, this->midiOutController->getVelocityMultiplier(), "midi_LVelMult")); 
    generalGUI->addWidgetRight(new ofxUIRotarySlider(dim*4, 0.0f, 20.0f, this->midiOutController->getVelocityMultiplierGlobal(),"midi_GVelMult"));
    generalGUI->addSpacer(length-xInit, 2);
    map<int,ClientUI*>::iterator it;
	it=this->clientsParams->begin();
	while (it != this->clientsParams->end())
	{
		ClientUI* client=it->second;
		string clientTitle = client->getName();
        float value = client->getblendFactor();
        generalGUI->addToggle("c_" + clientTitle +  "_enable", client->enabled, dim, dim);
        generalGUI->addWidgetDown(new ofxUIRotarySlider(dim*4, 0.0f, 1.0f, value, "c_" + clientTitle +  "_blend")); 
        generalGUI->addWidgetRight(new ofxUIRotarySlider(dim*4, 0.0f, 30.0f, 24.0f,"c_" + clientTitle +  "_fps")); 
        generalGUI->addSpacer(length-xInit, 2);
		it++;
	}
    
    ofAddListener(generalGUI->newGUIEvent,this,&ServerManager::guiEvent);
    
}

void ServerManager::setGUIDevices(){
    
    float dim = 16; 
	float xInit = OFX_UI_GLOBAL_WIDGET_SPACING; 
    float length = 255-xInit; 

    this->devicesGUI = new ofxUIScrollableCanvas(length+xInit+2, 0, length+xInit, ofGetHeight());
    this->devicesGUI->setStickDistance(1.0f);
    
    devicesGUI->addWidgetDown(new ofxUILabel("DEVICES SETTINGS", OFX_UI_FONT_LARGE)); 
    devicesGUI->addSpacer(length-xInit, 2); 
    
    map<int,DeviceUI*>::iterator itControllerSettings;
    itControllerSettings=this->devicesParams->begin();
    while (itControllerSettings != this->devicesParams->end())
    {
        DeviceUI* device=itControllerSettings->second;
        string deviceTitle =  device->getName();
        devicesGUI->addToggle("d_" + deviceTitle +  "_enable", device->isEnabled(), dim, dim);
        itControllerSettings++;
    }
    
    ofAddListener(devicesGUI->newGUIEvent,this,&ServerManager::guiEvent);

}

void ServerManager::setGUICalibration(){
    /*
    this->calibratorParams= new CalibratorUI();
    
    float dim = 16; 
	float xInit = OFX_UI_GLOBAL_WIDGET_SPACING; 
    float length = 255-xInit; 
    
    this->calibrationGUI = new ofxUIScrollableCanvas(length*2+xInit+4, 0, length+xInit, ofGetHeight());
    this->calibrationGUI->setStickDistance(1.0f);
    
    calibrationGUI->addWidgetDown(new ofxUILabel("CALIBRATION SETTINGS", OFX_UI_FONT_LARGE)); 
    calibrationGUI->addSpacer(length-xInit, 2); 
    
    vector<int>* radioNames = this->DVCManager->getLEDTypesIDs();
    
    vector<string> names;
    
    vector<int>::iterator itNames=radioNames->begin();
    
    while (itNames != radioNames->end())
    {
        int idExtracted = *itNames;
        string idAsString =  ofToString(idExtracted);
        names.push_back("t_" + idAsString);
        itNames++;
    }
    
    delete radioNames;
    
    calibrationGUI->addRadio("SELECT_LED_TYPE", names, OFX_UI_ORIENTATION_HORIZONTAL, dim, dim); 
    
    calibrationGUI->addSpacer(length-xInit, 2); 
    
    calibrationGUI->addSlider("R_CAL", 0.0f, 255.0f, 0.0f, length-xInit, dim);
    calibrationGUI->addSlider("G_CAL", 0.0f, 255.0f, 0.0f, length-xInit, dim);
    calibrationGUI->addSlider("B_CAL", 0.0f, 255.0f, 0.0f, length-xInit, dim);
    calibrationGUI->addToggle("ADD_PAIR", false, dim, dim);
    
    ofAddListener(calibrationGUI->newGUIEvent,this,&ServerManager::guiEvent);
    */
}

void ServerManager::guiEvent(ofxUIEventArgs &e){
    
    string name = e.widget->getName(); 
	int kind = e.widget->getKind(); 

    if (name=="decay factor"){
        ofxUISlider *slider = (ofxUISlider *) e.widget; 
		float newVal = slider->getScaledValue(); 
        this->decayFactor=newVal;
    }
    else if(name=="flashR"){
        ofxUISlider *slider = (ofxUISlider *) e.widget; 
		float newVal = slider->getScaledValue(); 
        this->flashR=newVal;
    }
    else if(name=="flashG"){
        ofxUISlider *slider = (ofxUISlider *) e.widget; 
		float newVal = slider->getScaledValue(); 
        this->flashG=newVal;
    }
    else if(name=="flashB"){
        ofxUISlider *slider = (ofxUISlider *) e.widget; 
		float newVal = slider->getScaledValue(); 
        this->flashB=newVal;
    }
    else if (name=="mc_channelEnabled"){
        ofxUIToggle *toggle = (ofxUIToggle *) e.widget; 
		bool newVal = toggle->getValue(); 
        //this->MCManager->setBroadcastState(newVal);
    }else if (name=="midi_enabled"){
        ofxUIToggle *toggle = (ofxUIToggle *) e.widget; 
		bool newVal = toggle->getValue(); 
        this->midiEnabled=newVal;
    }
    else if (name=="midi_LThreshON"){
        ofxUIRotarySlider *rotSlider = (ofxUIRotarySlider *) e.widget;
        int newValue = floor(rotSlider->getScaledValue());
        this->midiOutController->setThresholdON(newValue);
    }
    else if (name=="midi_LThreshOFF"){
        ofxUIRotarySlider *rotSlider = (ofxUIRotarySlider *) e.widget;
        int newValue = floor(rotSlider->getScaledValue());
        this->midiOutController->setThresholdOFF(newValue);
    }
    else if (name=="midi_GThreshON"){
        ofxUIRotarySlider *rotSlider = (ofxUIRotarySlider *) e.widget;
        int newValue = floor(rotSlider->getScaledValue());
        this->midiOutController->setThresholdGlobalON(newValue);
    }
    else if (name=="midi_GThreshOFF"){
        ofxUIRotarySlider *rotSlider = (ofxUIRotarySlider *) e.widget;
        int newValue = floor(rotSlider->getScaledValue());
        this->midiOutController->setThresholdGlobalOFF(newValue);
    }
    else if (name=="midi_LVelMult"){
        ofxUIRotarySlider *rotSlider = (ofxUIRotarySlider *) e.widget;
        float newValue = rotSlider->getScaledValue();
        this->midiOutController->setVelocityMultiplier(newValue);
    }
    else if (name=="midi_GVelMult"){
        ofxUIRotarySlider *rotSlider = (ofxUIRotarySlider *) e.widget;
        float newValue = rotSlider->getScaledValue();
        this->midiOutController->setVelocityMultiplierGlobal(newValue);
    }
    else if (name=="draw_enabled"){
        ofxUIToggle *toggle = (ofxUIToggle *) e.widget; 
		bool newVal = toggle->getValue(); 
        this->enableDraw=newVal;
    }
    else if (name=="draw_axis"){
        ofxUIToggle *toggle = (ofxUIToggle *) e.widget; 
		bool newVal = toggle->getValue(); 
        this->drawAxis=newVal;
    }
    else if (name=="show_framerate"){
        ofxUIToggle *toggle = (ofxUIToggle *) e.widget; 
		bool newVal = toggle->getValue(); 
        this->showFrameRate=newVal;
    }
    else if (name=="flash"){
        ofxUIButton *button = (ofxUIButton *) e.widget; 
		bool newVal = button->getValue(); 
        worker->lock();
        this->flash=newVal;
        worker->unlock();
        
    }
    else if(name=="R_CAL"){
        ofxUISlider *slider = (ofxUISlider *) e.widget; 
		float newVal = slider->getScaledValue(); 
        this->calibratorParams->setR((int) newVal);
    }
    else if(name=="G_CAL"){
        ofxUISlider *slider = (ofxUISlider *) e.widget; 
		float newVal = slider->getScaledValue(); 
        this->calibratorParams->setG((int) newVal);
    }
    else if(name=="B_CAL"){
        ofxUISlider *slider = (ofxUISlider *) e.widget; 
		float newVal = slider->getScaledValue(); 
        this->calibratorParams->setB((int) newVal);
    }
    else if(name=="ADD_PAIR"){
        ofxUIToggle* toggle=(ofxUIToggle *) e.widget; 
        toggle->setValue(false);
        int idToCalibrate = this->calibratorParams->getIdLedType();
        int rToCal = this->calibratorParams->getR();
        int gToCal = this->calibratorParams->getG();
        int bToCal = this->calibratorParams->getB();
        
        this->DVCManager->addCalibrationPair(idToCalibrate, this->flashR, this->flashG, this->flashB, rToCal, gToCal, bToCal);
    }
    else{
        vector<string> params = ofSplitString(name, "_");
        if (params[0]=="c"){
            string clientName=params[1];
            map<int,ClientUI*>::iterator it=this->clientsParams->begin();
            bool found=false;
            ClientUI* clientUI=NULL;
            while (!found && it!=this->clientsParams->end()){
                ClientUI* myClientUI = it->second;
                string name=myClientUI->getName();
                if (clientName.compare(name)==0){
                    found=true;
                    clientUI=myClientUI;
                }
                it++;
            }
            int clientId = clientUI->getId();
            map<int,ServerClientProxy*>::iterator itProxy = this->clients->find(clientId);
            if (itProxy!=this->clients->end()){
                ServerClientProxy* clientProxy=itProxy->second;
                
                string operation = params[2];
                
                if (operation=="enable"){
                    ofxUIToggle *toggle = (ofxUIToggle *) e.widget; 
                    bool newVal = toggle->getValue(); 
                    clientProxy->setState(newVal);
                }else if (operation=="blend"){
                    ofxUIRotarySlider *rotSlider = (ofxUIRotarySlider *) e.widget;
                    float newValue = rotSlider->getScaledValue();
                    clientProxy->setBlendFactor(newValue);
                    
                }else if (operation=="fps"){
                    ofxUIRotarySlider *rotSlider = (ofxUIRotarySlider *) e.widget;
                    int newValue = floor(rotSlider->getScaledValue());
                    clientProxy->setNewFPS(newValue);
                }
                
            }
        }else if (params[0]=="d"){
            string deviceName=params[1];
            map<int,DeviceUI*>::iterator it=this->devicesParams->begin();
            bool found=false;
            DeviceUI* deviceUI=NULL;
            while (!found && it!=this->devicesParams->end()){
                DeviceUI* myDeviceUI = it->second;
                string name=myDeviceUI->getName();
                if (deviceName.compare(name)==0){
                    found=true;
                    deviceUI=myDeviceUI;
                }
                it++;
            }
            int deviceID=deviceUI->getId();
            string operation=params[2];
            if (operation=="enable"){
                ofxUIToggle *toggle = (ofxUIToggle *) e.widget; 
                bool newVal = toggle->getValue(); 
                this->DVCManager->setDeviceState(deviceID, newVal);
            }
            
        }else if (params[0]=="t"){
            string typeIdSTR=params[1];
            int typeId = ofToInt(typeIdSTR);
            this->calibratorParams->setId(typeId);
        }
    
    }

}
