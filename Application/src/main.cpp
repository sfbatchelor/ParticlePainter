#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){

	ofGLWindowSettings settings;
	settings.setGLVersion(4, 4);
	settings.setSize(3840,2080);
	settings.setPosition(glm::vec2(0,50));
	ofCreateWindow(settings);
	ofRunApp(new ofApp());
	
}
