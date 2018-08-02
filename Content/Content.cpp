#include "Content.h"

Content::Content()
{
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetVerticalSync(true);
	ofSetWindowTitle("Digital Painting");
	m_snapshot = false;
	m_showGui = true;
	m_numPoints = 1000;
	m_shader.load("vert.glsl", "frag.glsl");
	m_compute.load( "compute.glsl");

	// GENERATE POINTS FROM IMAGE
	// load an image from disk
	m_image.load("paint1.png");
	m_texture.allocate(m_image.getPixels());
	m_mesh.setMode(OF_PRIMITIVE_POINTS);
	// loop through the image in the x and y axes
	for(int i = 0; i<m_numPoints; i++)
	{
		int x = ofRandom(float(m_image.getWidth()));
		int y = ofRandom(float(m_image.getHeight()));

		ofColor cur = m_image.getColor(x, y);
		if (cur.a > 0) {
			// the alpha value encodes depth, let's remap it to a good depth range
			float z = ofMap(cur.getBrightness(), 0, 255, -300, 300);
			cur.a = 255;
			m_mesh.addColor(cur);
			ofVec3f pos(x, y, z);
			m_mesh.addVertex(pos);

			Point point{};
			point.m_col = ofVec4f(cur.r, cur.g, cur.b, cur.a);
			point.m_pos = ofVec3f(pos.x, pos.y, pos.z);
			point.m_vel = ofVec3f(0);
			point.m_accel = ofVec3f(0);
			m_points.push_back(point);
		}
	}
	m_pointsBuffer.allocate(m_points, GL_DYNAMIC_DRAW);
	m_pointsVbo.setVertexBuffer(m_pointsBuffer,3,sizeof(Point));
	m_pointsVbo.setColorBuffer(m_pointsBuffer,sizeof(Point),sizeof(ofVec3f));

	// SETUP RAY BUFFER ON GPU
	m_cam.setVFlip(true); //flip for upside down image
	m_cam.setFarClip(100000000.);

	ofEnableDepthTest();
	glPointSize(6);
	ofSetBackgroundColor(50, 50, 50);

	m_pointsBuffer.bindBase(GL_SHADER_STORAGE_BUFFER, 0);
	m_texture.loadData(m_image.getPixels());
}

void Content::readComputeOutput()
{
	/*m_outputTexture.copyTo(m_computeReadBuffer);
	unsigned char* p = m_computeReadBuffer.map<unsigned char>(GL_READ_ONLY);
	m_computeReadPixels.setFromPixels(p, m_outputTexture.getWidth(), m_outputTexture.getHeight(), 4);
	m_computeReadBuffer.unmap();
	m_computePixelCache.clear();
	m_computePixelCache.resize(0);
	for (int i = 0; i < m_computeReadPixels.size(); i+= 4)
	{
			m_computePixelCache.push_back(glm::vec4(
				m_computeReadPixels[i], 
				m_computeReadPixels[i + 1], 
				m_computeReadPixels[i + 2], 
				m_computeReadPixels[i + 3]));
	}*/
}

void Content::update()
{
	m_shader.update();
	m_compute.update();


	m_compute.getShader().begin();	m_texture.bindAsImage(0, GL_READ_ONLY);	m_compute.getShader().setUniform1i("u_numPoints", m_numPoints);	m_compute.getShader().dispatchCompute(100, 100, 1);	m_compute.getShader().end();
}

void Content::draw()
{
	///// WORLD
	{
		//m_cam.begin();		//ofScale(2, -2, 2); // flip the y axis and zoom in a bit
		//ofTranslate(-m_image.getWidth() / 2, -m_image.getHeight() / 2);		//ofPointSmooth();		//m_mesh.draw();
		//m_cam.end();		ofPointSmooth();		glPointSize(16);		ofSetColor(255);
		m_pointsVbo.draw(GL_POINTS, 0, m_points.size());		glPointSize(6);		/// SCREEN GRAB
		if (m_snapshot == true) {
			m_screenGrab.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
			string fileName = "snapshot_" + ofGetTimestampString() + ".png";
			m_screenGrab.save(fileName);
			m_screenGrabFilename = "saved " + fileName;
			m_snapshot = false;
		}

		if (m_showGui)
		{
			m_cam.begin();
			ofDrawGrid(5000, 5, true, true, true, true);
			m_cam.end();
		}

	}

	///// GUI
	if (m_showGui)
	{

		stringstream ss;
		ss << "FPS: " << ofToString(ofGetFrameRate(), 0) << endl << endl;
		ss << "CONTROLS" << endl;
		ss << "SPACE TO RESET RAYS" <<endl << endl;
		const std::string string = ss.str();
		ofDrawBitmapStringHighlight(string, glm::vec2(20, 100));
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
	m_compute.exit();
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
	case ' ':
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
