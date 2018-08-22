/*
 * ImageSaverThread.h
 *
 *  Created on: Oct 14, 2014
 *      Author: arturo
 */
#pragma once

class SaveToFileThread : public ofThread {
public:
	SaveToFileThread();
	~SaveToFileThread();

	void save(ofPixels& pixels, std::string name);
	void threadedFunction();
	bool isReady();

private:

	ofThreadChannel<ofPixels> channelPixels;
	ofThreadChannel<std::string> channelName;
	ofThreadChannel<bool> channelReady;

};

class ImageSaverThread: public ofThread{
public:
	ImageSaverThread();
	~ImageSaverThread();

	void save(unsigned char * pixels);
	void waitReady();
	void threadedFunction();

private:
	ofPixels pixels;
	ofThreadChannel<unsigned char *> channel;
	ofThreadChannel<bool> channelReady;
	int frame;
	std::vector<std::shared_ptr<SaveToFileThread>> saverThreads;
};
