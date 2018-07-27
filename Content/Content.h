#pragma once
#include "ofMain.h"
#include "ShaderWatcher.h"


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

};