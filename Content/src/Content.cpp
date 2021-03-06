#include "stdafx.h"
#include "Content.h"
#include "3d\Particle.h"

Content::Content()
{
	ofSetFrameRate(30);
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetVerticalSync(true);
	ofSetWindowTitle("Particle Painter");
	m_snapshot = false;
	m_showGui = true;
	m_numPoints = 1024 * 38;
	m_constantShader.load("constantVert.glsl", "constantFrag.glsl", "sphereGeom.glsl");
	m_imageShader.load("imageVert.glsl", "imageFrag.glsl");

	m_particleSim.loadCompute("compute.glsl");

	m_pause = false;
	m_restart = false;
	m_fboActive = false;


	// GENERATE POINTS FROM IMAGE
	// load an image from disk
	if (!m_paintImage.load("paint1.png"))
	{
		ofLogError() << "Failed to find particle image spawner";
	}
	else
	{
		m_paintTexture.allocate(m_paintImage.getPixels());
		m_points = GpuParticleFactory::fromImage(m_paintImage, m_numPoints, -300, 300);
		m_particleSim.loadParticles(m_points);
		m_paintTexture.loadData(m_paintImage.getPixels());
	}

	if (!m_canvasImage.load("canvas.jpg"))
	{
		ofLogError() << "Failed to find canvas image";
	}
	else
	{
		m_canvasTexture.allocate(m_canvasImage.getPixels());
		m_canvasTexture.loadData(m_canvasImage.getPixels());
	}

	resetFbo();
	// SETUP RAY BUFFER ON GPU
	m_cam.setVFlip(true); //flip for upside down image
	m_cam.setFarClip(100000000.);

	ofEnableDepthTest();
	ofSetBackgroundColor(10, 10, 10);

}

void Content::initSimPoints()
{


}

void Content::update()
{


	m_imageShader.update();
	m_constantShader.update();
	m_paintTexture.bindAsImage(0, GL_READ_ONLY);
	m_particleSim.update();

	if (m_restart)
	{
		m_particleSim.reset();
		resetFbo();
		m_draggedImage.reset(new ofImage());
		m_compositeImage = false;
		m_restart = false;
	}


	if (m_fboActive && m_fbo)
	{
		m_fbo->begin();
		if (m_compositeImage)
		{
			m_compositeImage = false;
			m_draggedImage->draw(0, 0);
			m_fbo->clearDepthBuffer(10000000.);
		}
		drawScene();
		m_fbo->end();
	}
}

void Content::drawScene()
{
	m_cam.begin();
	ofEnableAlphaBlending();
	ofTranslate(-m_paintImage.getWidth() / 2, -m_paintImage.getHeight() / 2); //move to origin
	ofSetColor(255);
	glPointSize(3);
	m_constantShader.getShader().begin();
	m_constantShader.getShader().setUniform1f("uAlpha", 1.f);
	m_particleSim.draw(GL_POINTS);
	m_constantShader.getShader().end();
	m_cam.end();

}

void Content::draw()
{
	///// WORLD
	{

		ofSetDepthTest(true);
		ofEnableAlphaBlending();
		if (m_fboActive && m_fbo)
			m_fbo->draw(0, 0);
		else
			drawScene();

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

	}

	///// GUI
	if (m_showGui)
	{

		stringstream ss;
		ss << "FPS: " << ofToString(ofGetFrameRate(), 0) << endl << endl;
		ss << "--CONTROLS--" << endl ;
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
			ofSetColor(0,255, 0);
			ofDrawTriangle(glm::vec2(ofGetWidth() - 150, ofGetHeight() - 200), glm::vec2(ofGetWidth() - 150, ofGetHeight() - 120),  glm::vec2(ofGetWidth() - 65, ofGetHeight() - 160));


			ofSetColor(255);
		}
	}


}


void Content::resetFbo()
{

	m_fbo.reset(new ofFbo());
	m_fbo->allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);
	m_fbo->begin();
	ofClear(0, 0, 0, 255);
	ofDisableDepthTest();
	m_canvasTexture.draw(0, 0);
	m_fbo->end();

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
		m_particleSim.setPlay(m_pause);
		m_pause = !m_pause;
		break;
	case 'r':
		m_restart = true;
		break;
	case 'f':
		m_fboActive = !m_fboActive;
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
