#pragma once
#include "ofMain.h"
#include <string.h>

using namespace std;

class DTClient
{
public:
	DTClient(void);
	DTClient(int iId, string iName, int iTCPPort, string iStatusReport, bool iEnabled,float iblendFactor);
	~DTClient(void);
	int getId();
	string getName();
	int getTCPPort();
	string getStatusReport();
	bool isEnabled();
	string toXML();
	float getblendFactor();
private:
	int id;
	string name;
	int TCPPort;
	float blendFactor;
	string statusReport;
	bool enabled;

};
