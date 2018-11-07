#include "LogDisplay.h"
#include "ofMain.h"

float LogDisplay::m_fadeRate = .01;

LogDisplay::LogDisplay():
	m_alpha(1.f),
	m_bounds(glm::vec2(), glm::vec2())
{
	resetBounds();
}

LogDisplay::~LogDisplay()
{
}

void LogDisplay::draw()
{
	ofPushStyle();
	ofSetColor(.8,1.);
	ofDrawRectangle(glm::vec2(0,100), m_bounds.getWidth(), m_bounds.getHeight());
	ofPopStyle();
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
	
}

void LogDisplay::addStreamSource(std::string filepath)
{
	std::shared_ptr<FileWatcher> fileWatcher = std::shared_ptr<FileWatcher>(new FileWatcher());
	fileWatcher->lock();
	fileWatcher->setFile(boost::filesystem::path(filepath.c_str()), 100);
	fileWatcher->registerCallback(std::function<void()>([this]() {onFileWasModified(); }));
	fileWatcher->unlock();
	fileWatcher->startThread();
	m_logSources.push_back(fileWatcher);

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

bool LogDisplay::isVisible()
{
	if (m_alpha > 0.0)
		return true;
	return false;
}

void LogDisplay::resetBounds()
{
	m_bounds.setWidth(ofGetWidth() - (ofGetWidth()*.2f));
	m_bounds.setHeight(ofGetHeight() - (ofGetHeight()*.2f));
	m_bounds.setPosition(glm::vec2(ofGetWidth()*.1, -100.0f));
}

void LogDisplay::onFileWasModified()
{

}
