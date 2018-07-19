#include "Content.h"

Content::Content(std::shared_ptr<ofAppBaseWindow> mainWindow)
{
	if (!mainWindow)
		return;

	m_mainWindow = mainWindow;
	m_renderer = m_mainWindow->renderer();
	m_mainWindow->setWindowTitle("Ex Anim");
}

void Content::setup()
{
}

void Content::update()
{
}

void Content::draw()
{

	m_renderer->setBackgroundColor(300);
	for (auto image : m_draggedImages)
	{
		m_renderer->draw(image, 0, 0, 0, image.getWidth(), image.getHeight(), 0, 0, image.getWidth(), image.getHeight());
	}
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

void Content::dragEvent(ofDragInfo info)
{
	if (info.files.size() > 0) {

		m_draggedImages.assign(info.files.size(), ofImage());
		for (unsigned int k = 0; k < info.files.size(); k++) {
			m_draggedImages[k].load(info.files[k]);
		}
	}
	m_draggedPixels.clear();
	for (auto image : m_draggedImages)
	{
		m_draggedPixels.push_back(image.getPixels());
	}
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
