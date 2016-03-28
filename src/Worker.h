#ifndef Server_Worker_h
#define Server_Worker_h

#include "ofMain.h"

class WorkerDelegate {
public:
    virtual void workerLoop() = 0;
};

class Worker: public ofThread {
public:
	Worker(WorkerDelegate* delegate, int msInterval = 20);
	~Worker(void);
    void threadedFunction();
    void setup();

private:    
	WorkerDelegate* delegate;
	int msInterval;
};

#endif
