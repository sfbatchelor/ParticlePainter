#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){

	ofGLWindowSettings settings;
	settings.setGLVersion(4, 4);
	settings.setSize(1560, 2130);
	ofCreateWindow(settings);
	ofRunApp(new ofApp());
	
}
