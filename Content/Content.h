#pragma once
#include "ofMain.h"
#include "ShaderWatcher.h"
#include "ofxGui.h"


class Content {

public:

	Content(std::shared_ptr<ofMainLoop> mainLoop);

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
	void dragEvent(ofDragInfo info);
	void gotMessage(ofMessage msg);

	bool isValid();

private:

	std::shared_ptr<ofMainLoop> s_loop;

	vector <ofImage> m_draggedImages;
	vector <ofRectangle> m_rectangles;
	vector <ofColor> m_colours;


	int m_whiteThresh;


	ofImage m_screenGrab;
	string m_screenGrabFilename;
	bool m_snapshot;

	ofEasyCam m_cam;
	bool m_helpText;

	ofxPanel m_gui;
	ofParameter<string> m_currentImageLabel;
	ofParameter<string> m_screenSize;
	ofParameter<string> m_currentShaderLabel;
	bool m_hideGUI;
	vector<ofFile> m_files;

	ShaderWatcher m_shader;
	ofPlanePrimitive m_plane;

};