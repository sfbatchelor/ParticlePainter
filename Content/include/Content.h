#pragma once
#include "utils/ShaderWatcher.h"
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


	std::vector<GpuParticle> m_points;
	ParticleSimulation m_particleSim;

	ofImage m_screenGrab;
	bool m_snapshot;

	std::shared_ptr<ofFbo> m_fbo;

	ofEasyCam m_cam;
	bool m_showGui;

	std::shared_ptr<ofImage> m_draggedImage;
	bool m_compositeImage;

	ShaderWatcher m_imageShader;
	ShaderWatcher m_constantShader;
	ofImage m_paintImage;
	ofTexture m_paintTexture;
	ofImage m_canvasImage;
	ofTexture m_canvasTexture;


	int m_numPoints;

	bool m_pause;
	bool m_restart;
	bool m_fboActive;

	float m_time;



};