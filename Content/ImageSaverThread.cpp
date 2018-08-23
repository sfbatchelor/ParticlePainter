/*
 * ImageSaverThread.cpp
 *
 *  Created on: Oct 14, 2014
 *      Author: arturo
 */

#include "stdafx.h"
#include "ImageSaverThread.h"

ImageSaverThread::ImageSaverThread(){
	startThread();
	frame = 0;
	for (int i = 0; i < 100; i++)//otherwise make a new one
	{
		saverThreads.push_back(std::shared_ptr<SaveToFileThread>(new SaveToFileThread()));
	}

}

ImageSaverThread::~ImageSaverThread() {
	channel.close();
	channelReady.close();
	waitForThread(true);
}

void ImageSaverThread::save(unsigned char * pixels){
	// send the pixels to save to the thread
	channel.send(pixels);
}

void ImageSaverThread::waitReady(){
	// wait till the thread is done saving the
	// previous frame
	bool ready;
	channelReady.receive(ready);
}

void ImageSaverThread::threadedFunction(){
	// wait to receive some pixels,
	// save them as jpeg and then tell the main
	// thread that we are done
	// if the channel closes go out of the thread
	unsigned char * p;
	while(channel.receive(p)){
		pixels.setFromPixels(p, ofGetWidth(), ofGetHeight(), OF_PIXELS_RGB);

		// allocate a saver thread
		std::shared_ptr<SaveToFileThread> saverThread;
		while (!saverThread)
		{
			for (auto saver : saverThreads)// get one if it's free
			{
				if (saver->isReady())
				{
					saverThread = saver;
					break;
				}
			}
		}

		saverThread->save(pixels, "sequences\\" + ofToString(frame) + ".jpg");
		channelReady.send(true);
		frame++;
	}
}

SaveToFileThread::SaveToFileThread()
{
	startThread();
	channelReady.send(true);
}

SaveToFileThread::~SaveToFileThread()
{
	channelPixels.close();
	channelName.close();
	channelReady.close();
	waitForThread(true);
}

void SaveToFileThread::save(ofPixels & pixels, std::string name)
{
	channelPixels.send(pixels);
	channelName.send(name);
}

void SaveToFileThread::threadedFunction()
{
	ofPixels pixels;
	std::string name;
	while (channelPixels.receive(pixels))
	{
		channelName.receive(name);
		ofSaveImage(pixels, name);
		channelReady.send(true);
	}

}

bool SaveToFileThread::isReady()
{
	// returns if it's not doing anything without blocking
	bool ready;
	return channelReady.tryReceive(ready);
}
