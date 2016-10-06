#include "DTClient.h"

DTClient::DTClient(void)
{
}

DTClient::DTClient(int iId, string iName, int iTCPPort, string iStatusReport, bool iEnabled, float iblendFactor){
	this->id=iId;
	this->name=iName;
	this->TCPPort=iTCPPort;
	this->statusReport=iStatusReport;
	this->enabled=iEnabled;
	this->blendFactor=iblendFactor;
}

DTClient::~DTClient(void){
}

int DTClient::getId(){
	return this->id;
}
string DTClient::getName(){
	return this->name;
}
int DTClient::getTCPPort(){
	return this->TCPPort;
}
float DTClient::getblendFactor(){
	return this->blendFactor;
}
string DTClient::getStatusReport(){
	return this->statusReport;
}
bool DTClient::isEnabled(){
	return this->enabled;
}
string DTClient::toXML(){
	return "";
}