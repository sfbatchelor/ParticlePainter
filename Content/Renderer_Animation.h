#pragma once
#include "Renderer.h"
#include "ImageSaverThread.h"
#include "ofxGuiBaseDraws.h"


class Animation : public Renderer 
{

	enum Mode
	{
		VIEWING =0,
		RECORDING
	};
	
public:

	Animation(Content* parent, std::shared_ptr<ofFbo> fbo = nullptr);

	virtual void init();
	virtual void update();
	virtual void draw();

	virtual std::string getName();

protected:

	void resetSequence();
	void renderViewing();
	void renderRecording();
	void updateViewing();
	void updateRecording();


	std::vector<ofImage> m_imageSequence;
	std::shared_ptr<ofFbo> m_animationLayer;
	std::shared_ptr<ofFbo> m_finalLayer;
	std::shared_ptr<ofBufferObject> m_pixelBufferFront;
	std::shared_ptr<ofBufferObject> m_pixelBufferBack;

	ImageSaverThread m_saverThread;

	int m_maxCurrentSequenceIndex;
	int m_currentIndex;
	static const int m_maxIndex = 600; // 20 secs @30fps

	Mode m_mode;

	ofParameter<float> m_fadeAlpha;

	ofxGuiBaseDraws m_sceneView;
	ofxGuiBaseDraws m_animationLayerView;
	ofxGuiBaseDraws m_finalLayerView;

};

