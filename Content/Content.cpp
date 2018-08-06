#include "Content.h"

Content::Content()
{
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetVerticalSync(true);
	ofSetWindowTitle("Digital Painting");
	m_snapshot = false;
	m_showGui = true;
	m_numPoints = 1024 * 10;
	m_constantShader.load("constantVert.glsl", "constantFrag.glsl");
	m_imageShader.load("imageVert.glsl", "imageFrag.glsl");
	m_compute.load( "compute.glsl");

	m_pause = false;
	m_restart = false;


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
			point.m_col = ofFloatColor(cur.r, cur.g, cur.b, cur.a);
			//point.m_col = ofFloatColor(.6, 0.7, 0, 1.0);
			point.m_pos = ofVec4f(pos.x, pos.y, pos.z, 1.);
			point.m_vel = ofVec4f(0);
			m_points.push_back(point);
		}
	}
	m_pointsBuffer.allocate(m_points, GL_DYNAMIC_DRAW);
	m_pointsBufferOld.allocate(m_points, GL_DYNAMIC_DRAW);
	m_pointsVbo.setVertexBuffer(m_pointsBuffer,4,sizeof(Point));
	m_pointsVbo.setColorBuffer(m_pointsBuffer,sizeof(Point),sizeof(ofVec4f));
	m_pointsVbo.enableColors();
	m_pointsVbo.disableNormals();
	m_pointsVbo.disableIndices();
	m_pointsVbo.disableTexCoords();
	m_pointsBuffer.bindBase(GL_SHADER_STORAGE_BUFFER, 0);
	m_pointsBufferOld.bindBase(GL_SHADER_STORAGE_BUFFER, 1);
	m_texture.loadData(m_image.getPixels());

	// SETUP RAY BUFFER ON GPU
	m_cam.setVFlip(true); //flip for upside down image
	m_cam.setFarClip(100000000.);

	ofEnableDepthTest();
	glPointSize(6);
	//ofSetBackgroundColor(10, 10, 10);
	ofSetBackgroundColor(50, 50, 50);


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

void Content::initSimPoints()
{
	m_pointsBuffer.unbindBase(GL_SHADER_STORAGE_BUFFER, 0);
	m_pointsBufferOld.allocate(m_points, GL_DYNAMIC_DRAW);
	m_pointsBuffer.allocate(m_points, GL_DYNAMIC_DRAW);
	m_pointsVbo.setVertexBuffer(m_pointsBuffer,4,sizeof(Point));
	m_pointsVbo.setColorBuffer(m_pointsBuffer,sizeof(Point),sizeof(ofVec4f));
	m_pointsBuffer.bindBase(GL_SHADER_STORAGE_BUFFER, 0);
	m_pointsBufferOld.bindBase(GL_SHADER_STORAGE_BUFFER, 1);
}

void Content::update()
{
	m_imageShader.update();
	m_constantShader.update();
	m_compute.update();


	if (m_restart)
	{
		initSimPoints();
		m_restart = false;
	}

	if (!m_pause)
	{
		m_compute.getShader().begin();		m_texture.bindAsImage(0, GL_READ_ONLY);		m_compute.getShader().setUniform1i("uNumPoints", m_numPoints);		m_compute.getShader().setUniform1i("uWidth", m_image.getWidth());		m_compute.getShader().setUniform1i("uHeight", m_image.getHeight());		m_compute.getShader().dispatchCompute((m_points.size() + 1024 - 1) / 1024, 1, 1);		m_compute.getShader().end();
		m_pointsBuffer.copyTo(m_pointsBufferOld);
	}
}

void Content::draw()
{
	///// WORLD
	{
		m_cam.begin();		ofScale(2, -2, 2); // flip the y axis and zoom in a bit
		ofTranslate(-m_image.getWidth() / 2, -m_image.getHeight() / 2);		//ofPointSmooth();		//m_mesh.draw();
		ofPointSmooth();		ofSetColor(255);
		glPointSize(6);		m_texture.draw(10000, 0, -1000, m_image.getWidth(), m_image.getHeight());			m_constantShader.getShader().begin();		m_constantShader.getShader().setUniform1f("uAlpha", 1.f);		m_pointsVbo.draw(GL_POINTS, 0, m_points.size());		m_constantShader.getShader().end();		m_cam.end();		/// SCREEN GRAB
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
		ss << "CONTROLS" << endl << endl;
		ss << "R TO RESET POINTS" <<endl << endl;
		ss << "SPACE TO PAUSE" <<endl << endl;
		const std::string string = ss.str();
		ofDrawBitmapStringHighlight(string, glm::vec2(20, 100));
		drawInteractionArea();

		if (m_pause) // draw a pause symbol
		{

			ofSetColor(255);
			ofDrawRectangle(glm::vec2(ofGetWidth() - 100, ofGetHeight() - 200), 35, 80);
			ofDrawRectangle(glm::vec2(ofGetWidth() - 150, ofGetHeight() - 200), 35, 80);
		}
		else
		{
			ofSetColor(0,255, 0);
			ofDrawTriangle(glm::vec2(ofGetWidth() - 150, ofGetHeight() - 200), glm::vec2(ofGetWidth() - 150, ofGetHeight() - 120),  glm::vec2(ofGetWidth() - 65, ofGetHeight() - 160));


			ofSetColor(255);
		}
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
	m_imageShader.exit();
	m_constantShader.exit();
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
		m_pause = !m_pause;
		break;
	case 'r':
		m_restart = true;
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
