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
	m_compute.setupShaderFromFile(GL_COMPUTE_SHADER, "compute.glsl");
	m_compute.linkProgram();


	// GENERATE POINTS FROM IMAGE
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


	// SETUP RAY BUFFER ON GPU
	setRays();
	m_vbo.setVertexBuffer(m_rayBuffer, 4, sizeof(Ray)); // the id
	m_vbo.setColorBuffer(m_rayBuffer, sizeof(Ray), sizeof(ofVec4f) );
	m_rayBuffer.bindBase(GL_SHADER_STORAGE_BUFFER, 0);

	m_plane.set(ofGetWidth(), ofGetHeight(), 10, 10);
	m_plane.mapTexCoords(0, 0, ofGetWidth(), ofGetHeight());
	m_cam.setVFlip(true); //flip for upside down image
	ofEnableDepthTest();
	glPointSize(6);
	ofSetBackgroundColor(5, 5, 5);

}

void Content::setRays()
{


	int numRays = ofGetWidth()*ofGetHeight();
	m_rays.resize(numRays);
	int i = 0;
	for (int x = 1; x <= ofGetWidth(); x++)
	{
		for (int y = 1; y <= ofGetHeight(); y++)
		{
			glm::vec4 origin;
		origin.x = m_cam.getPosition().x;
		origin.y = m_cam.getPosition().y;
		origin.z = m_cam.getPosition().z;
		origin.w = 1.;
			float px = (2. * ((x*.5) / ofGetWidth()) - 1.) * glm::tan(m_cam.getFov() / 2.*glm::pi<float>() / 180.) * m_cam.getAspectRatio();
			float py = (1. - 2. * ((y*.5) / ofGetHeight()) - 1.) * glm::tan(m_cam.getFov() / 2.*glm::pi<float>() / 180.);
			glm::vec3 dirf = glm::vec3(origin.x, origin.y, origin.z) - glm::vec3(px, py, -1.);
			ofVec3f dir = glm::normalize(dirf);
			//m_rays[i].m_dir = ofVec4f(dir.x, dir.y, dir.z, 1.0);
			m_rays[i].m_col = ofFloatColor(dir.x, dir.y, dir.z);
			m_rays[i].m_id = ofVec4f(x, y, 0, 1);
			i++;
		}
	}

	m_rayBuffer.allocate(m_rays, GL_DYNAMIC_DRAW);

}
void Content::readRays()
{

	struct Ray* rays = NULL;
	try {
		rays = (Ray*)m_rayBuffer.map(GL_MAP_READ_BIT| GL_MAP_WRITE_BIT);
	}
	catch (std::runtime_error e) {
		ofLogError() << e.what();
	}
	if (glGetError() != GL_NO_ERROR || rays == NULL) {
		ofLogError() << "Could not map the shader storage buffer for particles";
	}

	m_newRays.resize(m_rays.size());
	for (int i = 0; i < m_newRays.size(); i++)
	{
		Ray r = rays[i];
		m_newRays.push_back(r);
	}

	m_rayBuffer.unmap();

}

void Content::update()
{
	m_shader.update();

	m_compute.begin();
	m_compute.dispatchCompute(m_rays.size(), 1, 1);
	m_compute.end();
	//readRays();
}

void Content::draw()
{

	ofEnableBlendMode(OF_BLENDMODE_ALPHA);


	///// WORLD
	{
		m_cam.begin();



		/// SCREEN GRAB
		if (m_snapshot == true) {
			m_screenGrab.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
			string fileName = "snapshot_" + ofGetTimestampString() + ".png";
			m_screenGrab.save(fileName);
			m_screenGrabFilename = "saved " + fileName;
			m_snapshot = false;
		}

		ofSetColor(255, 255);
		glPointSize(10);
		m_vbo.draw(GL_POINTS, 0, m_rays.size());


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
