#include "stdafx.h"
#include "Content.h"
#include "Renderer_Drawing.h"


Content::Content()
{
	ofSetFrameRate(30);
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetVerticalSync(true);
	ofSetWindowTitle("Digital Painting");
	m_snapshot = false;
	m_showGui = true;
	m_numPoints = 1024 * 38;
	m_constantShader.load("constantVert.glsl", "constantFrag.glsl", "sphereGeom.glsl");
	m_imageShader.load("imageVert.glsl", "imageFrag.glsl");
	m_compute.load( "compute.glsl");

	m_pause = false;
	m_restart = false;
	m_fboActive = false;


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
	ofSetBackgroundColor(10, 10, 10);


	m_renderer = std::move(std::unique_ptr<Renderer>(new Drawing(this)));
	m_renderer->init();

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

ofxPanel & Content::getGui()
{

	return m_gui;
}

void Content::drawOverlays()
{

	/// SCREEN GRAB
	if (m_snapshot == true) {
		m_screenGrab.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
		string fileName = "screenshots\\snapshot_" + ofGetTimestampString() + ".png";
		m_screenGrab.save(fileName);
		m_snapshot = false;
	}

	if (m_showGui)
	{
		m_cam.begin();
		ofDrawGrid(5000, 5, true, true, true, true);
		m_cam.end();
	}

	///// GUI
	if (m_showGui)
	{

		stringstream ss;
		ss << "FPS: " << ofToString(ofGetFrameRate(), 0) << endl << endl;
		ss << "--CONTROLS--" << endl;
		ss << "'R' TO RESET POINTS" << endl;
		ss << "'F' TO DRAW TO FBO" << endl;
		ss << "' ' TO PAUSE" << endl;
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
			ofSetColor(0, 255, 0);
			ofDrawTriangle(glm::vec2(ofGetWidth() - 150, ofGetHeight() - 200), glm::vec2(ofGetWidth() - 150, ofGetHeight() - 120), glm::vec2(ofGetWidth() - 65, ofGetHeight() - 160));


			ofSetColor(255);
		}
	}
}

void Content::update()
{
	m_imageShader.update();
	m_constantShader.update();
	m_compute.update();
	m_renderer->update();

	if (m_restart)
	{
		initSimPoints();
		m_restart = false;
	}

	if (!m_pause)
	{
		m_compute.getShader().begin();
		m_texture.bindAsImage(0, GL_READ_ONLY);
		m_compute.getShader().setUniform1i("uNumPointsSF", m_numPoints/1024);
		m_compute.getShader().setUniform1f("uWidth", m_image.getWidth());
		m_compute.getShader().setUniform1f("uHeight", m_image.getHeight());
		m_compute.getShader().setUniform1f("uTime", ofGetElapsedTimef());
		m_compute.getShader().dispatchCompute((m_points.size() + 1024 - 1) / 1024, 1, 1);
		m_compute.getShader().end();
		m_pointsBuffer.copyTo(m_pointsBufferOld);
	}

}

void Content::drawScene()
{
	ofSetDepthTest(true);
	m_cam.begin();
	ofTranslate(-m_image.getWidth() / 2, -m_image.getHeight() / 2);
	ofSetColor(255);
	glPointSize(3);
	m_constantShader.getShader().begin();
	m_constantShader.getShader().setUniform1f("uAlpha", 1.f);
	m_pointsVbo.draw(GL_POINTS, 0, m_points.size());
	m_constantShader.getShader().end();
	m_cam.end();
}

void Content::draw()
{
	m_renderer->draw();
	drawOverlays();
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
		m_renderer->reset();
		break;
	case 'f':
		m_renderer->switchRendering();
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
	if (info.files.size() > 0) {
		m_draggedImage.reset(new ofImage());
		m_draggedImage->load(info.files[0]);
		m_compositeImage = true;
	}
}

void Content::gotMessage(ofMessage msg)
{
}

bool Content::isValid()
{
	return true;
}
