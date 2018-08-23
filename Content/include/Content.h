#pragma once
#include "utils/ShaderWatcher.h"
#include "gui/ofxGui.h"


struct Point
{
	ofVec4f m_pos;
	ofFloatColor m_col;
	ofVec4f m_vel;
};

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


	ofImage m_screenGrab;
	bool m_snapshot;

	std::shared_ptr<ofFbo> m_fbo;

	ofEasyCam m_cam;
	bool m_showGui;

	std::shared_ptr<ofImage> m_draggedImage;
	bool m_compositeImage;

	ShaderWatcher m_imageShader;
	ShaderWatcher m_constantShader;
	ComputeWatcher m_compute;
	ofMesh m_mesh;
	ofImage m_image;
	ofTexture m_texture;

	std::vector<Point> m_points;
	ofBufferObject m_pointsBuffer;
	ofBufferObject m_pointsBufferOld;
	ofVbo m_pointsVbo;

	int m_numPoints;

	bool m_pause;
	bool m_restart;
	bool m_fboActive;

	ofxPanel m_gui;

	bool m_animateActive;

};