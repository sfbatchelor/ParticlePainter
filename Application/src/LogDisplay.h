#pragma once
#include "FileWatcher.h"
#include <BoundingBox.h>

//! Whole screen log display that fades on and off when triggered
//! Files are read line by line and displayed in chronological order.
class LogDisplay
{
	enum FadeState {
		ENTERING,
		EXITING
	};

public:

	LogDisplay();
	~LogDisplay();

	void draw();
	void update();

	void addStreamSource(std::string filepath);

	void setVisible(bool isVisible);
	void setVisibleImmediately(bool isVisible);
	bool isVisible();

	void resetBounds();

private:

	void onFileWasModified();

	float m_alpha;
	FadeState m_fadeState;
	static float m_fadeRate;

	std::vector<std::shared_ptr<FileWatcher>> m_logSources;
	std::vector<std::string> m_lines;
	BoundingBox2D m_bounds;
};
