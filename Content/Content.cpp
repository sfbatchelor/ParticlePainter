#include "Content.h"

Content::Content()
{
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetVerticalSync(true);

	ofSetWindowTitle("Digital Painting");

	m_whiteThresh = 220;
	m_snapshot = false;
	m_showGui = true;

	m_shader.load("vert.glsl", "frag.glsl");


	// load an image from disk
	m_image.load("paint1.png");

	// we're going to load a ton of points into an ofMesh
	m_mesh.setMode(OF_PRIMITIVE_POINTS);

	// loop through the image in the x and y axes
	int skip = 4; // load a subset of the points
	for (int y = 0; y < m_image.getHeight(); y += skip) {
		for (int x = 0; x < m_image.getWidth(); x += skip) {
			ofColor cur = m_image.getColor(x, y);
			if (cur.a > 0) {
				// the alpha value encodes depth, let's remap it to a good depth range
				float z = ofMap(cur.getBrightness(), 0, 255, -300, 300);
				cur.a = 255;
				m_mesh.addColor(cur);
				ofVec3f pos(x, y, z);
				m_mesh.addVertex(pos);
			}
		}
	}


	m_plane.set(ofGetWidth(), ofGetHeight(), 10, 10);
	m_plane.mapTexCoords(0, 0, ofGetWidth(), ofGetHeight());
	m_cam.setVFlip(true); //flip for upside down image
	ofEnableDepthTest();
	glEnable(GL_POINT_SMOOTH); // use circular points instead of square point
	glPointSize(6);
//	ofSetBackgroundColor(5, 5, 5);
	ofBackgroundGradient(ofColor::gray, ofColor::black, OF_GRADIENT_CIRCULAR);


}

void Content::update()
{
	m_shader.update();
}

void Content::draw()
{


	///// WORLD
	{
		m_cam.begin();
		//m_image.getTexture().setTextureWrap(GL_REPEAT, GL_REPEAT);
		//m_image.getTexture().bind();
		//m_shader.getShader().begin();
		//m_shader.getShader().setUniform1f("uTime", ofGetElapsedTimef());
		//ofPushMatrix();
		//m_plane.draw();
		//ofPopMatrix();
		//m_image.getTexture().unbind();
		//m_shader.getShader().end();

		ofScale(2, -2, 2); // flip the y axis and zoom in a bit
		ofTranslate(-m_image.getWidth() / 2, -m_image.getHeight() / 2);
		glEnable(GL_POINT_SMOOTH); // use circular points instead of square point
		m_mesh.draw();


		/// SCREEN GRAB
		if (m_snapshot == true) {
			m_screenGrab.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
			string fileName = "snapshot_" + ofGetTimestampString() + ".png";
			m_screenGrab.save(fileName);
			m_screenGrabFilename = "saved " + fileName;
			m_snapshot = false;
		}

		if (m_showGui)
			ofDrawGrid(5000, 5, true, true, true, true);

		m_cam.end();
	}


	///// GUI
	if (m_showGui)
	{

		stringstream ss;
		ss << "FPS: " << ofToString(ofGetFrameRate(), 0) << endl << endl;
		//ss << "MODE: " << (m_cam.getOrtho() ? "ORTHO" : "PERSPECTIVE") << endl;
		//ss << "MOUSE INPUT ENABLED: " << (m_cam.getMouseInputEnabled() ? "TRUE" : "FALSE") << endl;
		//ss << "INERTIA ENABLED: " << (m_cam.getInertiaEnabled() ? "TRUE" : "FALSE") << endl;
		//ss << "ROTATION RELATIVE Y AXIS: " << (m_cam.getRelativeYAxis() ? "TRUE" : "FALSE") << endl;
		//if (m_cam.getOrtho()) {
		//	ss << "    Notice that in ortho mode zoom will be centered at the mouse position." << endl;
		//}
		const std::string string = ss.str();
		ofDrawBitmapStringHighlight(string, glm::vec2(20, 100));
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
	switch (key) {
	case 'x':
		m_snapshot = true;
		break;
	case 'h':
		m_showGui = !m_showGui;
		break;
	}
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
	//if (info.files.size() > 0) {

	//	m_draggedImages.assign(info.files.size(), ofImage());
	//	for (unsigned int k = 0; k < info.files.size(); k++) {
	//		m_draggedImages[k].load(info.files[k]);
	//	}
	//	m_plane.mapTexCoordsFromTexture(m_draggedImages[0].getTexture());
	//}
}

void Content::gotMessage(ofMessage msg)
{
}

bool Content::isValid()
{
	return true;
}
