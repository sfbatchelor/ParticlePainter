#pragma once
#include "FileWatcher.h"
#include "ofRectangle.h"

//! Whole screen log display that fades on and off when triggered
//! Files are read line by line and displayed in chronological order.
class LogDisplay
{
	enum FadeState {
		ENTERING,
		EXITING
	};

	struct LogFile {
		std::shared_ptr<FileWatcher> m_watcher;
		int m_lineCount;
	};

public:

	LogDisplay();
	~LogDisplay();

	void draw();
	void update();

	void addStreamSource(std::string filepath);

	void setVisible(bool isVisible);
	void setVisibleImmediately(bool isVisible);
	void setLogFilename(std::string filename = "");
	std::string getLogFilename();
	bool isVisible();

	void resetBounds();
	void updateDisplay();

private:

	void onFileWasModified();

	float m_alpha;
	FadeState m_fadeState;
	static float m_fadeRate;

	std::vector<LogFile> m_logSources;
	std::vector<std::string> m_lines;
	ofMutex m_mutex;
	bool m_updateDisplay;
	ofRectangle m_bounds;
	std::string m_logFilename;//filename of main .log file. Other files may be added.
};
