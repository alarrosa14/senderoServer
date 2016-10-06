#include "Worker.h"

Worker::Worker(WorkerDelegate* delegate, int msInterval) {
	this->delegate = delegate;
	this->msInterval = msInterval;
}

Worker::~Worker(void) {}

void Worker::setup() {}

void Worker::threadedFunction() {
	while(isThreadRunning()) {
		delegate->workerLoop();
		sleep(msInterval);
	}
}
