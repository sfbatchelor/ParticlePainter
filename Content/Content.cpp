#include "Content.h"

Content::Content(std::shared_ptr<ofAppBaseWindow> mainWindow)
{
	if (!mainWindow)
		return;

	m_mainWindow = mainWindow;
	m_renderer = m_mainWindow->renderer();
	m_mainWindow->setWindowTitle("Graphics Window");
}

void Content::setup()
{
}

void Content::update()
{
}

void Content::draw()
{
	int a, b;
	a = 2;
	b = 3;
	a *= b;

	m_renderer->setBackgroundColor(300);

	float gc = sin(glm::length(ofGetElapsedTimef() * 4));
	m_renderer->setColor(255, gc * 130, 0);
	float radius = 50 + 10 * sin(100);
	m_renderer->setFillMode(OF_FILLED);

	m_renderer->drawCircle(400 + cos(ofGetElapsedTimef()) * 110, 600 + (cos(ofGetElapsedTimef()*.9)) * 80, 0, radius);

	gc = sin(glm::fract(ofGetElapsedTimef() * 5));
	m_renderer->setColor(155, 100, gc * 230);
	glm::vec3 bPos = glm::vec3(900 +  sin(ofGetElapsedTimef()) * 200, 400+ glm::length(cos(ofGetElapsedTimef()*.3)) *580., 0.);
	m_renderer->drawBox(bPos, 50 + 200 * glm::length(sin(ofGetElapsedTimef())));

}

void Content::exit()
{
}

void Content::keyPressed(int key)
{
}

void Content::keyReleased(int key)
{
}

void Content::mouseMoved(int x, int y)
{
}

void Content::mouseDragged(int x, int y, int button)
{
}

void Content::mousePressed(int x, int y, int button)
{
}

void Content::mouseReleased(int x, int y, int button)
{
}

void Content::mouseEntered(int x, int y)
{
}

void Content::mouseExited(int x, int y)
{
}

void Content::windowResized(int w, int h)
{
}

void Content::dragEvent(ofDragInfo dragInfo)
{
}

void Content::gotMessage(ofMessage msg)
{
}

bool Content::isValid()
{
	if (m_mainWindow)
		return true;
	return false;
}
