#pragma once
#include "ofMain.h"
#include "ContentLoader.h"
#include "Filewatcher.h"
#include "SystemThread.h"

struct WindowsContentCode
{
	HMODULE m_dll;
	Content_Setup* m_setup;
	Content_Update* m_update;
	Content_Render* m_render;
	Content_Exit* m_exit;
	Content_Key_Pressed* m_keyPressed;
	Content_Key_Released* m_keyReleased;
	Content_Mouse_Moved* m_mouseMoved;
	Content_Mouse_Dragged* m_mouseDragged;
	Content_Mouse_Pressed* m_mousePressed;
	Content_Mouse_Released* m_mouseReleased;
	Content_Mouse_Entered* m_mouseEntered;
	Content_Mouse_Exited* m_mouseExited;
	Content_Window_Resized* m_windowResized;
	Content_Drag_Event* m_dragEvent;
	Content_Got_Message* m_gotMessage;
	bool m_isValid;
};

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		void exit();


		WindowsContentCode loadContentCode(void );
		void unloadContentCode(WindowsContentCode *contentCode);
		void onDllWasModified();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

private:

	WindowsContentCode m_content;
	FileWatcher m_dllWatcher;
	SystemThread m_processStarter;
	bool m_needsSetup;
	bool m_unload;
		
};
