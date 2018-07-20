#include "Content.h"
#include <GLFW\glfw3.h>


Content::Content(std::shared_ptr<ofMainLoop> mainLoop)
{
	s_loop = mainLoop;
	ofSetMainLoop(mainLoop);
	ofSetCurrentRenderer(mainLoop->getCurrentWindow()->renderer());
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetVerticalSync(true);

	ofSetWindowTitle("Ex Anim");

	m_whiteThresh = 220;
	m_snapshot = false;

	//m_shader.load("vert.glsl", "frag.glsl");

	m_hideGUI = false;

	m_plane.set(ofGetWidth(), ofGetHeight(), 10, 10);
	m_plane.mapTexCoords(0, 0, ofGetWidth(), ofGetHeight());
	m_files = ofDirectory("").getFiles();
	ofSetBackgroundColor(0.2, 0.2, 0.2);

	m_cam.setVFlip(true); //flip for upside down image
	m_draggedImages = {};

}

void Content::update()
{
}

void Content::draw()
{

	ofSetBackgroundColor(100);
	for (auto image : m_draggedImages)
	{
		ofSetColor(255, 255, 255);
		image.draw(0, 0);
	}

	/// WORLD
	{
		m_cam.begin();
		for (auto image : m_draggedImages)
		{
			image.getTexture().setTextureWrap(GL_REPEAT, GL_REPEAT);
			image.getTexture().bind();
		}
		m_shader.getShader().begin();
		m_shader.getShader().setUniform1f("uTime", ofGetElapsedTimef());
		ofPushMatrix();
		m_plane.draw();
		ofPopMatrix();
		m_shader.getShader().end();

		for (auto image : m_draggedImages)
			image.getTexture().unbind();

		/// SCREEN GRAB
		if (m_snapshot == true) {
			m_screenGrab.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
			string fileName = "snapshot_" + ofGetTimestampString() + ".png";
			m_screenGrab.save(fileName);
			m_screenGrabFilename = "saved " + fileName;
			m_snapshot = false;
		}

		if (!m_helpText)
			ofDrawGrid(5000, 5, true, true, true, true);
		m_cam.end();
	}


	/// GUI
	if (!m_helpText)
	{

		stringstream ss;
		ss << "FPS: " << ofToString(ofGetFrameRate(), 0) << endl << endl;
		ss << "MODE: " << (m_cam.getOrtho() ? "ORTHO" : "PERSPECTIVE") << endl;
		ss << "MOUSE INPUT ENABLED: " << (m_cam.getMouseInputEnabled() ? "TRUE" : "FALSE") << endl;
		ss << "INERTIA ENABLED: " << (m_cam.getInertiaEnabled() ? "TRUE" : "FALSE") << endl;
		ss << "ROTATION RELATIVE Y AXIS: " << (m_cam.getRelativeYAxis() ? "TRUE" : "FALSE") << endl;
		if (m_cam.getOrtho()) {
			ss << "    Notice that in ortho mode zoom will be centered at the mouse position." << endl;
		}
		ofDrawBitmapString(ss.str().c_str(), 20, 100);
		// also interaction area
		drawInteractionArea();
	}


}

void Content::drawInteractionArea()
{
	ofRectangle vp = ofGetCurrentViewport();
	float r = std::min<float>(vp.width, vp.height) * 0.5f;
	float x = vp.width * 0.5f;
	float y = vp.height * 0.5f;

	ofPushStyle();
	ofSetLineWidth(3);
	ofSetColor(255, 255, 0);
	ofNoFill();
	glDepthMask(false);
	ofSetCircleResolution(64);

	ofDrawCircle(x, y, r);
	glDepthMask(true);
	ofPopStyle();
}


void Content::exit()
{
	m_shader.exit();
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
		m_plane.mapTexCoordsFromTexture(m_draggedImages[0].getTexture());
	}
}

void Content::gotMessage(ofMessage msg)
{
}

bool Content::isValid()
{
	if (s_loop)
		return true;
	return false;
}
