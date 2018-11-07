#pragma once
#include "ofThread.h"
#include "ofTypes.h"
#include <string>

class SystemThread : public ofThread {

public:
	std::string cmd;

	void setup(std::string _cmd) {
		this->cmd = _cmd;
	}

	void threadedFunction() {
		if (isThreadRunning()) {
			system(cmd.c_str());
		}
	};
};
