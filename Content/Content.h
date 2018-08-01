#pragma once
#include "ofMain.h"
#include "ShaderWatcher.h"

struct Ray
{

	ofVec4f m_id;
	ofFloatColor m_col;
	ofVec4f m_origin;
	ofVec3f m_dir;
};

struct Point
{
	ofVec3f m_pos;
	ofFloatColor m_col;
};

class Content {

public:

	Content();

	void update();
	void draw();
	void drawInteractionArea();
	void drawRayDirs();
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

	void setRays();
	void readRays();

private:


	//vector <ofImage> m_draggedImages;
	//vector <ofRectangle> m_rectangles;
	//vector <ofColor> m_colours;

	int m_whiteThresh;

	ofImage m_screenGrab;
	string m_screenGrabFilename;
	bool m_snapshot;

	ofEasyCam m_cam;
	bool m_showGui;

	ShaderWatcher m_shader;
	ofPlanePrimitive m_plane;
	ofMesh m_mesh;
	ofImage m_image;

	ofColor m_color;

	std::vector<Ray> m_rays;
	std::vector<Ray> m_newRays;
	ofBufferObject m_rayBuffer;
	ComputeWatcher m_compute;
	ofVbo m_raysVbo;

	std::vector<Point> m_points;
	ofBufferObject m_pointsBuffer;
	ofVbo m_pointsVbo;

	ofImage m_outputImage;
	ofTexture m_outputTexture;

};