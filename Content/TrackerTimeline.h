#pragma once

//! Encapsulates the functionality of a keyframed timeline with data driven by a sync-tracker, 
//! either cached or connected via TCP depending on the SYNC_PLAYER  preprocessor being defined
class TrackerTimeline
{

public:

	TrackerTimeline(int fps = 60);
	~TrackerTimeline();
	void setTrackDirectory(const std::string& trackDirectory = "/tracks");
	bool initialize(std::vector<std::string> syncedValues);
	void resetSyncedValues(std::vector<std::string> syncedValues);
	float getValue(std::string name);
	float& getValueRef(std::string name);
	void addValue(std::string name);

	void setFPS(int fps);
	int getFPS();
	int getFrame();
	void setFrame(int frame);
	void nextFrame(int count = 1);
	void previousFrame(int count = 1);
	float getTime();
	void setTime(float time);
	float getTimePerFrame();

	void play();
	void pause();
	bool isPlaying();
	bool isConnected();
	bool isInPlaybackMode();
	void update(float elapsedTime);

protected:

	bool m_connected;
	bool m_playing;
	int m_frame;
	float m_time;
	int m_fps;
	ofDirectory m_dirManager;
	std::string m_trackDirectory;
	std::map<std::string, float> m_syncedValues;

	void connectToTracker();
	void syncValues();
	//todo - threaded sync tracker 

};
