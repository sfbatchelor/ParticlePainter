#pragma once
#include "FileWatcher.h"
#include "ofRectangle.h"
#include "ofTrueTypeFont.h"
#include "ofFbo.h"

//! Whole screen log display that fades on and off when triggered
//! Files are read line by line and displayed in chronological order.
//! todo - could probs split this into clearer class impls
class LogDisplay
{

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
	void setLogFilenames(std::string filename = "");
	std::string getLogFilename();
	bool isVisible();

	void updateDisplay();
	void resizeEvent();

private:

	void resetBounds();
	void onFileWasModified();
	void updateTextTexture();

	ofFbo m_textTexture;
	float m_alpha;
	static float s_fadeRate;
	std::vector<LogFile> m_logSources;
	std::deque<std::string> m_lines;
	static int s_maxLines;
	static int s_maxWidth;
	ofMutex m_mutex;
	bool m_updateDisplay;
	bool m_isVisible;
	ofRectangle m_bounds;
	std::string m_logFilename;//filename of main .log file. Other files may be added.
	std::string m_logBuildFilename;//filename of main .log file. Other files may be added.

};
