#include "stdafx.h"
#include "Renderer_Animation.h"
#include "Content.h"

Animation::Animation(Content* parent, std::shared_ptr<ofFbo> fbo):
	Renderer(parent, fbo),
	m_mode(VIEWING)
{
}

void Animation::init()
{

	m_animationLayer.reset(new ofFbo());
	m_finalLayer.reset(new ofFbo());
	resetSequence();
	std::vector<ofImage> m_imageSequence;
	m_sceneView = ofxGuiBaseDraws(getFbo().get());
	m_animationLayerView = ofxGuiBaseDraws( m_animationLayer.get());
	m_finalLayerView = ofxGuiBaseDraws(m_finalLayer.get());
	getGui().setup("Animation");
	getGui().add(m_fadeAlpha.set("Fade Amount", 25, 0, 255));
	ofParameterGroup scene;
	scene.setName("Scene");
	getGui().add(&m_sceneView);
	getGui().add(&m_animationLayerView);
	getGui().add(&m_finalLayerView);
}

void Animation::update()
{
	if (isReset())
	{
		switch (m_mode)
		{
		case RECORDING:
			m_mode = VIEWING;
			break;
		case VIEWING:
			resetSequence();
			setRender(false);
			break;
		}
	}

	if (isRendering())
	{
		if (m_maxCurrentSequenceIndex == 0)
			m_mode = RECORDING;

		switch (m_mode) {
		case RECORDING:
			updateRecording();
			break;
		case VIEWING:
			updateViewing();
			break;
		}
	}

}

void Animation::draw()
{
	if (isRendering())
	{
		switch (m_mode) {
		case RECORDING:
			renderRecording();
			break;
		case VIEWING:
			renderViewing();
			break;
		}
	}
	else
	{
	  // otherwise render normally
	  getFbo()->draw(0, 0);
	  getParent().drawScene();
	}
}

void Animation::renderViewing()
{
	if (m_maxCurrentSequenceIndex != 0)
	{
		// then we play the sequence
		if (m_currentIndex >= m_maxCurrentSequenceIndex)
			m_currentIndex = 0;
		ofSetColor(255);
		m_imageSequence[m_currentIndex].draw(0, 0);
		m_currentIndex++;
	}
	else
	{
		// otherwise render normally
		getFbo()->draw(0, 0);
		getParent().drawScene();
	}

	m_finalLayer->draw(100, 100);
}

void Animation::renderRecording()
{
	m_finalLayer->draw(0, 0);
}

void Animation::updateViewing()
{
}

void Animation::updateRecording()
{
	if (m_maxCurrentSequenceIndex >= m_maxIndex) //stop recording if hit max images
	{
		m_mode = VIEWING;
		m_currentIndex = 0;
		return;
	}

	// draw a faded version of the scene
	m_animationLayer->begin();
	ofClear(0);
	//ofSetDepthTest(false);
	//ofFill();
	//ofSetColor(0, 0, 0, m_fadeAlpha.get());
	//ofDrawRectangle(0, 0, m_animationLayer->getWidth(), m_animationLayer->getHeight());
	//ofSetColor(255);
	//ofSetDepthTest(true);
	getParent().drawScene();
	m_animationLayer->end();

	// composite scene with animation layer
	m_finalLayer->begin();
	ofClear(0);
	ofEnableAlphaBlending();
	getFbo()->draw(0,0);
	m_animationLayer->draw(0, 0);
	m_finalLayer->end();

	//send to thread to save
	if (m_maxCurrentSequenceIndex != 0)
	{
		//wait for saver thread
		m_saverThread.waitReady();
		m_pixelBufferBack->unmap();
	}

	m_finalLayer->getTexture().copyTo(*m_pixelBufferBack);

	// bind and map the buffer as PIXEL_UNPACK so it can be
	// accessed from a different thread  from the cpu
	// and send the memory address to the saver thread
	m_pixelBufferFront->bind(GL_PIXEL_UNPACK_BUFFER);
	unsigned char * p = m_pixelBufferFront->map<unsigned char>(GL_READ_ONLY);
	m_saverThread.save(p);

	// swap the front and back buffer so we are always
	// copying the texture to one buffer and reading
	// back from another to avoid stalls
	swap(*m_pixelBufferBack, *m_pixelBufferFront);
	m_maxCurrentSequenceIndex++;
}


std::string Animation::getName()
{
	return std::string("Animation");
}

void Animation::resetSequence()
{
	m_maxCurrentSequenceIndex = 0;
	m_currentIndex = 0;


	//m_animationLayer.reset(new ofFbo());
	//m_finalLayer.reset(new ofFbo());
	m_pixelBufferBack.reset(new ofBufferObject());
	m_pixelBufferFront.reset(new ofBufferObject());
	m_animationLayer->allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);
	m_finalLayer->allocate(ofGetWidth(), ofGetHeight(), GL_RGB);
	m_pixelBufferBack->allocate(ofGetWidth()*ofGetHeight() * 3, GL_DYNAMIC_READ);
	m_pixelBufferFront->allocate(ofGetWidth()*ofGetHeight() * 3, GL_DYNAMIC_READ);
}
