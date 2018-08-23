#include "stdafx.h"
#include "modules\syncTracker\TrackerTimeline.h"
#include "modules\syncTracker\SyncTracker.h"
#include "ofFileUtils.h"

TrackerTimeline::TrackerTimeline(int fps):
	m_trackDirectory("syncedTimeline"),
	m_fps(fps),
	m_connected(false),
	m_playing(false),
	m_time(0.0f)
{
}

TrackerTimeline::~TrackerTimeline()
{
	SyncTracker::saveAndDisconnect();
}

void TrackerTimeline::setTrackDirectory(const std::string& trackDirectory)
{

	m_trackDirectory = trackDirectory;
	if (!m_dirManager.doesDirectoryExist(trackDirectory))
	{
		m_dirManager.createDirectory(trackDirectory);
		if (m_dirManager.doesDirectoryExist(trackDirectory))
			ofLogNotice() << "Created track directory: " << trackDirectory;
		else
			ofLogError() << "Could not create track directory: " << trackDirectory << "even though it needs to be";
	}

}


bool TrackerTimeline::initialize(std::vector<std::string> syncedValues)
{
	m_syncedValues.clear();
	if (isInPlaybackMode())
	{
		ofLogNotice() << "Syncing with tracks in [" << m_trackDirectory.c_str() << "]";
	}
	else
		ofLogNotice() << "Saving Tracks to [" << m_trackDirectory.c_str() << "]";

	SyncTracker::createDevice(m_trackDirectory.c_str());
	connectToTracker();


	for(auto value: syncedValues)
		addValue(value.c_str());
	if (isInPlaybackMode())
		play();
	else
		pause();
	if (m_connected)//todo - default to incrementing time.
		return true;
	return false;
}

void TrackerTimeline::resetSyncedValues(std::vector<std::string> syncedValues)
{
	if(SyncTracker::isDeviceAlive())
		SyncTracker::killDevice();
	m_connected = false;
	if (initialize(syncedValues))
	{
		ofLogNotice() << "Set Sync Values..";
	}
	else
	{
		ofLogError() << "Failed to set synced values: tracker not connected";
	}
}

float TrackerTimeline::getValue(std::string name)
{
	return m_syncedValues[name];
}

float & TrackerTimeline::getValueRef(std::string name)
{
	return m_syncedValues[name];
}

void TrackerTimeline::addValue(std::string name)
{
	SyncTracker::addSyncValue(name.c_str());
	m_syncedValues[name] = SyncTracker::getSyncValue(name.c_str());
}

void TrackerTimeline::setFPS(int fps)
{
	m_fps = fps;
}

int TrackerTimeline::getFPS()
{
	return m_fps;
}

int TrackerTimeline::getFrame()
{
	return m_frame;
}

void TrackerTimeline::setFrame(int frame)
{
	m_frame = glm::max(frame, 0);
	m_time = (float)m_frame / (float)m_fps;
	if (!isPlaying())
	{
		SyncTracker::play();
		SyncTracker::update(m_frame);
		SyncTracker::pause();
	}
	else
		SyncTracker::update(m_frame);
}

void TrackerTimeline::nextFrame(int count)
{
	setFrame(getFrame() + count);
}

void TrackerTimeline::previousFrame(int count)
{
	setFrame(getFrame() - count);
}

float TrackerTimeline::getTime()
{
	return m_time;
}

void TrackerTimeline::setTime(float time)
{
	m_time = glm::max(time, 0.0f);
	m_frame = (int)(m_time * (float)m_fps);
	if (!isPlaying())
	{
		SyncTracker::play();
		SyncTracker::update(m_frame);
		SyncTracker::pause();
	}
	else
		SyncTracker::update(m_frame);
}

float TrackerTimeline::getTimePerFrame()
{
	return 1.0f/(float)m_fps;
}

void TrackerTimeline::play()
{
	if (!m_connected)
	{
		ofLogError() << ("Play failed: not connected to tracker");
		return;
	}
	SyncTracker::play();
	ofLogNotice() << "playing";
}

void TrackerTimeline::pause()
{
	if (!m_connected)
	{
		ofLogError() << "Pause failed: not connected to tracker";
		return;
	}

	SyncTracker::pause();
	ofLogNotice() << "pausing";
}

bool TrackerTimeline::isPlaying()
{
	if (!m_connected)
		return false;
	return SyncTracker::isPlaying();
}

bool TrackerTimeline::isConnected()
{
	return m_connected;
}

void TrackerTimeline::connectToTracker()
{
	if (SyncTracker::connect() == 0 )
	{
		ofLogNotice() << "Connected to sync tracker";
		m_connected = true;
	}
	else if(m_connected)
		m_connected = false;
}

void TrackerTimeline::syncValues()
{
	for (auto& value : m_syncedValues)
	{
		value.second = SyncTracker::getSyncValue(value.first.c_str());
	}
}

void TrackerTimeline::update(float elapsedTime)
{

	if (isPlaying())
	{
		m_time += elapsedTime;
		m_frame = (int)(m_time * (float)m_fps);
		if (m_frame > 6000) // todo - make this a flexibly defined loop. (curently loops every 100 secs at 60fps)
			setFrame(0);
	}
	else
	{
		setFrame(SyncTracker::getFrame());
	}
	if (!SyncTracker::update(m_frame))
		connectToTracker();

	syncValues();
}

bool TrackerTimeline::isInPlaybackMode()
{
#ifndef SYNC_PLAYER
	return false;
#else
	return true;
#endif
}