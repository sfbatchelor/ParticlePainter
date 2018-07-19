#pragma once
#include "ofMain.h"


class Content {

public:

	Content(std::shared_ptr<ofAppBaseWindow> mainWindow);

	void setup();
	void update();
	void draw();
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

	std::shared_ptr<ofAppBaseWindow> m_mainWindow;
	std::shared_ptr<ofBaseRenderer> m_renderer;

	vector <ofImage> m_draggedImages;
	vector <ofPixels> m_draggedPixels;

};