#pragma once
#include "ofThread.h"
#include "ofTypes.h"
#include "ofFileUtils.h"
#include "ofUtils.h"
#include <string>

class FileWatcher : public ofThread{
    void threadedFunction(){
		while (isThreadRunning()) {

			bool fileWasModified = false;
			{
				ofScopedLock lock(*mutex);
				m_file.open(m_path);
				if (m_file.canRead())
				{
					auto currentWriteTime = std::filesystem::last_write_time(m_file);
					if (currentWriteTime > m_lastWriteTime)
					{
						m_lastWriteTime = currentWriteTime;
						fileWasModified = true;
					}
				}
				m_file.close();
			}

			if (fileWasModified)
			{
				m_changedCallback();
			}
			ofSleepMillis(m_sleepMS);
		}
    }

    std::filesystem::path m_path;
	time_t m_lastWriteTime;
	ofFile m_file;
	int m_sleepMS;
	std::function<void()> m_changedCallback;

public:

	FileWatcher() {};
    FileWatcher(std::filesystem::path& filepath, int sleepMS){
        this->m_path = filepath;
        this->m_sleepMS = sleepMS;
		this->m_file = ofFile(filepath);
		if (this->m_file.canRead())
		{
			this->m_lastWriteTime = std::filesystem::last_write_time(m_file);
		}
		m_file.close();
    }
	
	void setFile(std::filesystem::path& filepath, int sleepMS) {
        this->m_path = filepath;
        this->m_sleepMS = sleepMS;
		this->m_file = ofFile(filepath);
		if (this->m_file.canRead())
		{
			this->m_lastWriteTime = std::filesystem::last_write_time(m_file);
		}
		m_file.close();
 
	}
	void registerCallback(std::function<void()> callback)
	{
		m_changedCallback = callback;
	}
};
