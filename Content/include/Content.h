#pragma once
#include "utils/ShaderWatcher.h"
#include "gui/ofxGui.h"
#include "3d\ParticleSimulation.h"
#include "utils\ProcessLauncher.h"
#include "modules\syncTracker\TrackerTimeline.h"


class Content {

public:

	Content();

	void update();
	void draw();
	void drawScene();
	void drawInteractionArea();
	void exit();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	bool isValid();

	void initSimPoints();

	void resetFbo();

private:


	//ProcessLauncher m_rocketLauncher;
	ParticleSimulation m_particleSim;
	//TrackerTimeline m_timeline;

	ofImage m_screenGrab;
	bool m_snapshot;

	std::shared_ptr<ofFbo> m_fbo;

	ofEasyCam m_cam;
	bool m_showGui;

	std::shared_ptr<ofImage> m_draggedImage;
	bool m_compositeImage;

	ShaderWatcher m_imageShader;
	ShaderWatcher m_constantShader;
	ofImage m_image;
	ofTexture m_texture;

	std::vector<GpuParticle> m_points;
	ofBufferObject m_pointsBuffer;
	ofBufferObject m_pointsBufferOld;
	ofVbo m_pointsVbo;

	int m_numPoints;

	bool m_pause;
	bool m_restart;
	bool m_fboActive;

	ofxPanel m_gui;
	float m_time;



};