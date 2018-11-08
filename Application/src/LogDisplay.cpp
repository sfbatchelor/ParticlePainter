#include "LogDisplay.h"
#include "ofMain.h"

float LogDisplay::m_fadeRate = .01;

LogDisplay::LogDisplay():
	m_alpha(1.f),
	m_updateDisplay(false)
{
	resetBounds();
	setLogFilename();
	addStreamSource(m_logFilename);
}

LogDisplay::~LogDisplay()
{
	for (auto logFile : m_logSources)
	{
		logFile.m_watcher->waitForThread(true, 500);
	}
}

void LogDisplay::draw()
{

	m_mutex.lock();
	ofPushStyle();
	ofSetColor(255,255.);
	ofDrawRectangle(m_bounds);
	ofPopStyle();
	m_mutex.unlock();
}

void LogDisplay::update()
{

	if (m_alpha > .0f && m_fadeState == FadeState::EXITING)
	{
		m_alpha -= m_fadeRate;
	}
	else if(m_alpha < 1.f && m_fadeState == FadeState::ENTERING)
	{
		m_alpha += m_fadeRate;
	}

	updateDisplay();
}


	

void LogDisplay::addStreamSource(std::string filepath)
{
	std::shared_ptr<FileWatcher> fileWatcher = std::shared_ptr<FileWatcher>(new FileWatcher());
	fileWatcher->lock();
	fileWatcher->setFile(boost::filesystem::path(filepath.c_str()), 100);
	fileWatcher->registerCallback(std::function<void()>([this]() {onFileWasModified(); }));
	fileWatcher->unlock();
	fileWatcher->startThread();
	LogFile logFile;
	logFile.m_lineCount = 0;
	logFile.m_watcher = fileWatcher;
	m_logSources.push_back(logFile);

}

void LogDisplay::setVisible(bool isVisible)
{
	if (isVisible)
	{
		m_fadeState = FadeState::ENTERING;
	}
	else
		m_fadeState = FadeState::EXITING;
}

void LogDisplay::setVisibleImmediately(bool isVisible)
{
	if (isVisible)
		m_alpha= 1.;
	else
		m_alpha= 0.;
}

void LogDisplay::setLogFilename(std::string filename)
{
	if (filename.empty())
	{
		m_logFilename = "\\logs\\";
		m_logFilename += ofToString(ofGetYear()) + "_";
		m_logFilename += (ofToString(ofGetMonth()).length() > 1 ? ofToString(ofGetMonth()) : "0" + ofToString(ofGetMonth())) + "_";
		m_logFilename += (ofToString(ofGetDay()).length() > 1 ? ofToString(ofGetDay()) : "0" + ofToString(ofGetDay()));
		m_logFilename += +".log";
	}
	else
		m_logFilename = filename;
}

std::string LogDisplay::getLogFilename()
{
	return m_logFilename;
}

bool LogDisplay::isVisible()
{
	if (m_alpha > 0.0)
		return true;
	return false;
}

void LogDisplay::resetBounds()
{
	m_bounds.setWidth(ofGetWidth() - (ofGetWidth()*.1f));
	m_bounds.setHeight(ofGetHeight() - (ofGetHeight()*.1f));
	m_bounds.setPosition(ofGetWidth()*.05, ofGetHeight()*.05);
}

void LogDisplay::updateDisplay()
{
	// want to update the display in main thread to unlock the log file from OF and then relock it via file open/close
	m_mutex.lock();
	ofLogToDebugView();
	if (m_updateDisplay)
	{
		for (auto& logFile : m_logSources)
		{

			logFile.m_watcher->lock();
			auto lines = ofSplitString(ofBufferFromFile(logFile.m_watcher->getAbsolutePath()).getText(), "\n");
			lines.pop_back();// to get rid of the extra empty \n at the end
			if (logFile.m_lineCount == 0 || lines.size() != logFile.m_lineCount)
			{
				int lineCount = 0;
				for (auto line : lines) {
					if (lineCount >= logFile.m_lineCount)
						m_lines.push_back(line);
					lineCount++;
				}
				logFile.m_lineCount = lineCount;
			}
			logFile.m_watcher->unlock();
		}
		m_updateDisplay = false;
	}
	ofLogToFile(m_logFilename, true);
	m_mutex.unlock();
}

void LogDisplay::onFileWasModified()
{
	m_mutex.lock();
	m_updateDisplay = true;
	m_mutex.unlock();
}
