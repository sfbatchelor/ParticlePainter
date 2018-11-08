#include "LogDisplay.h"
#include "ofMain.h"

float LogDisplay::s_fadeRate = .01;
int LogDisplay::s_maxLines= 100;
int LogDisplay::s_maxWidth= 1200;

LogDisplay::LogDisplay():
	m_alpha(1.f),
	m_updateDisplay(false)
{
	resetBounds();
	setLogFilenames();
	addStreamSource(m_logBuildFilename);
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
	if (!m_textTexture.isAllocated() ||  m_alpha < .0)
		return;

	ofSetDepthTest(true);
	ofPushMatrix();
	ofPushStyle();
	ofTranslate(m_bounds.getPosition());
	ofSetColor(255, 255 * m_alpha);
	m_textTexture.draw(0, 0);
	ofPopStyle();
	ofPopMatrix();
}

void LogDisplay::update()
{
	if (!m_isVisible && m_alpha > .0f)
		m_alpha -= s_fadeRate;
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
	ofScopedLock(m_mutex);
	m_isVisible = isVisible;
	if(m_isVisible)
		m_alpha = 1.0;
}

void LogDisplay::setLogFilenames(std::string filename)
{
	if (filename.empty())
	{
		m_logFilename = "\\logs\\";
		m_logFilename += ofToString(ofGetYear()) + "_";
		m_logFilename += (ofToString(ofGetMonth()).length() > 1 ? ofToString(ofGetMonth()) : "0" + ofToString(ofGetMonth())) + "_";
		m_logFilename += (ofToString(ofGetDay()).length() > 1 ? ofToString(ofGetDay()) : "0" + ofToString(ofGetDay()));
		m_logBuildFilename = m_logFilename;
		m_logFilename += +".log";
		m_logBuildFilename += ".build";
	}
	else
	{
		m_logFilename = filename ;
		m_logFilename += ".log";
		m_logBuildFilename = filename;
		m_logBuildFilename += ".build";
	}
}

std::string LogDisplay::getLogFilename()
{
	return m_logFilename;
}

bool LogDisplay::isVisible()
{
	return m_isVisible;
}

void LogDisplay::resetBounds()
{
	int maxLineLength = 0;
	for (int i = 0; i < (int)m_lines.size(); i++) {
		// tabs are not rendered
		const string & line(m_lines[i]);
		int currentLineLength = 0;
		for (int j = 0; j < (int)line.size(); j++) {
			if (line[j] == '\t') {
				currentLineLength += 8 - (currentLineLength % 8);
			}
			else {
				currentLineLength++;
			}
		}
		maxLineLength = MAX(maxLineLength, currentLineLength);
	}
	int padding = 7;
	int fontSize = 8;
	float leading = 15./8.;
	int height = m_lines.size() * fontSize * leading - 1;
	int width = maxLineLength * fontSize;

	if (width > s_maxWidth)
		width = s_maxWidth;

	if (width == 0)
		width = 1;
	if (height == 0)
		height = 1;

	m_bounds.setWidth(width);
	m_bounds.setHeight(height);
	m_bounds.setPosition(ofGetWidth() - m_bounds.getWidth() - 50, 50);
	m_textTexture.allocate(m_bounds.getWidth(), m_bounds.getHeight(), GL_RGBA);
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
		while (m_lines.size() > s_maxLines)
		{
			m_lines.pop_front();
		}
		updateTextTexture();
		m_updateDisplay = false;
		m_alpha = 1.0f;
	}
	ofLogToFile(m_logFilename, true);
	m_mutex.unlock();
}

void LogDisplay::resizeEvent()
{
	m_mutex.lock();
	resetBounds();
	updateTextTexture();
	m_mutex.unlock();
}

void LogDisplay::onFileWasModified()
{
	m_mutex.lock();
	m_updateDisplay = true;
	m_mutex.unlock();
}

void LogDisplay::updateTextTexture()
{
	resetBounds();
	m_textTexture.begin();
	ofClear(255, 255, 255, 80.);
	ofPushStyle();
	ofPushMatrix();
	auto pos = glm::vec2(0);
	for (auto line : m_lines)
	{
		ofSetColor(255,255.);
		ofDrawBitmapString(line, pos.x, pos.y);
		pos.y += 15;
	}
	ofPopStyle();
	ofPopMatrix();
	m_textTexture.end();
}
