#pragma once
#include "ofMain.h"
#include "ShaderWatcher.h"

struct Point
{
	ofVec3f m_pos;
	ofVec4f m_col;
	ofVec3f m_vel;
	ofVec3f m_accel;
};

class Content {

public:

	Content();

	void update();
	void draw();
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

	void readComputeOutput();

private:

	ofImage m_screenGrab;
	string m_screenGrabFilename;
	bool m_snapshot;

	ofEasyCam m_cam;
	bool m_showGui;

	ShaderWatcher m_shader;
	ComputeWatcher m_compute;
	ofMesh m_mesh;
	ofImage m_image;
	ofTexture m_texture;

	std::vector<Point> m_points;
	ofBufferObject m_pointsBuffer;
	ofVbo m_pointsVbo;

	int m_numPoints;

};