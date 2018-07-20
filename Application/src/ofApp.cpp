#include "ofApp.h"
#include "Windows.h"

//--------------------------------------------------------------
void ofApp::setup(){

	m_content = loadContentCode();
	m_content.m_setup(ofGetMainLoop(), ofGetCurrentWindow());
	m_dllWatcher.lock();

#ifdef _DEBUG
	m_dllWatcher.setFile(boost::filesystem::path("..\\Content_debug.dll"), 100);
#else 
	m_dllWatcher.setFile(boost::filesystem::path("..\\Content.dll"), 100);
#endif

	m_dllWatcher.registerCallback(std::function<void()>([this]() {onDllWasModified(); }));
	m_dllWatcher.unlock();
	m_dllWatcher.startThread();
}

//--------------------------------------------------------------
void ofApp::update(){
	m_dllWatcher.lock();
	m_content.m_update();
	m_dllWatcher.unlock();
}

//--------------------------------------------------------------
void ofApp::draw(){
	m_dllWatcher.lock();
	m_content.m_render(ofGetCurrentWindow()->renderer());
	m_dllWatcher.unlock();
}

void ofApp::exit()
{
	m_dllWatcher.waitForThread(true, 100);
	m_content.m_exit();
}

WindowsContentCode ofApp::loadContentCode(void)
{
	WindowsContentCode result = {};

#ifdef _DEBUG
	CopyFile(L"Content_debug.dll", L"Content_temp.dll", FALSE);
#else
	CopyFile(L"Content.dll", L"Content_temp.dll", FALSE);
#endif

	result.m_dll = LoadLibraryA("Content_temp.dll");
	if (result.m_dll)
	{
		result.m_setup= (Content_Setup*)
			GetProcAddress(result.m_dll, "ContentSetup");
		result.m_update= (Content_Update*)
			GetProcAddress(result.m_dll, "ContentUpdate");
		result.m_render= (Content_Render*)
			GetProcAddress(result.m_dll, "ContentRender");
		result.m_exit= (Content_Exit*)
			GetProcAddress(result.m_dll, "ContentExit");
		result.m_keyPressed= (Content_Key_Pressed*)
			GetProcAddress(result.m_dll, "ContentKeyPressed");
		result.m_keyReleased= (Content_Key_Released*)
			GetProcAddress(result.m_dll, "ContentKeyReleased");
		result.m_mouseMoved= (Content_Mouse_Moved*)
			GetProcAddress(result.m_dll, "ContentMouseMoved");
		result.m_mouseDragged= (Content_Mouse_Dragged*)
			GetProcAddress(result.m_dll, "ContentMouseDragged");
		result.m_mousePressed= (Content_Mouse_Pressed*)
			GetProcAddress(result.m_dll, "ContentMousePressed");
		result.m_mouseReleased= (Content_Mouse_Released*)
			GetProcAddress(result.m_dll, "ContentMouseReleased");
		result.m_mouseEntered= (Content_Mouse_Entered*)
			GetProcAddress(result.m_dll, "ContentMouseEntered");
		result.m_mouseExited = (Content_Mouse_Exited*)
			GetProcAddress(result.m_dll, "ContentMouseExited");
		result.m_windowResized = (Content_Window_Resized*)
			GetProcAddress(result.m_dll, "ContentWindowResized");
		result.m_dragEvent = (Content_Drag_Event*)
			GetProcAddress(result.m_dll, "ContentDragEvent");
		result.m_gotMessage = (Content_Got_Message*)
			GetProcAddress(result.m_dll, "ContentGotMessage");

		result.m_isValid = (result.m_setup && result.m_update && result.m_render && result.m_exit
						&& result.m_keyReleased && result.m_keyPressed && result.m_mouseMoved
						&& result.m_mouseDragged && result.m_mousePressed && result.m_mouseReleased
						&& result.m_mouseEntered && result.m_mouseExited && result.m_windowResized
						&& result.m_dragEvent && result.m_gotMessage);
	}
	if (!result.m_isValid)
	{
		result.m_setup			= ContentSetupStub;
		result.m_update			= ContentUpdateStub;
		result.m_render			= ContentRenderStub;
		result.m_exit			= ContentExitStub;
		result.m_keyPressed		= ContentKeyPressedStub;
		result.m_keyReleased	= ContentKeyReleasedStub;
		result.m_mouseMoved		= ContentMouseMovedStub;
		result.m_mouseDragged   = ContentMouseDraggedStub;
		result.m_mousePressed   = ContentMousePressedStub;
		result.m_mouseReleased	= ContentMouseReleasedStub;
		result.m_mouseEntered	= ContentMouseEnteredStub;
		result.m_mouseExited	= ContentMouseExitedStub;
		result.m_windowResized	= ContentWindowResizedStub;
		result.m_dragEvent		= ContentDragEventStub;
		result.m_gotMessage		= ContentGotMessageStub;
	}

	return result;
}

void ofApp::unloadContentCode(WindowsContentCode *contentCode)
{
	if (contentCode->m_dll)
		FreeLibrary(contentCode->m_dll);

	contentCode->m_isValid = false;
	contentCode->m_setup = ContentSetupStub;
	contentCode->m_update = ContentUpdateStub;
	contentCode->m_render = ContentRenderStub;
	contentCode->m_exit = ContentExitStub;
	contentCode->m_keyPressed = ContentKeyPressedStub;
	contentCode->m_keyReleased = ContentKeyReleasedStub;
	contentCode->m_mouseMoved = ContentMouseMovedStub;
	contentCode->m_mouseDragged = ContentMouseDraggedStub;
	contentCode->m_mousePressed = ContentMousePressedStub;
	contentCode->m_mouseReleased = ContentMouseReleasedStub;
	contentCode->m_mouseEntered = ContentMouseEnteredStub;
	contentCode->m_mouseExited = ContentMouseExitedStub;
	contentCode->m_windowResized = ContentWindowResizedStub;
	contentCode->m_dragEvent = ContentDragEventStub;
	contentCode->m_gotMessage = ContentGotMessageStub;
}

void ofApp::onDllWasModified()
{
	m_dllWatcher.lock();
	m_content.m_exit();
	unloadContentCode(&m_content);
	m_content = loadContentCode();
	m_dllWatcher.unlock();

	m_content.m_setup(ofGetMainLoop(), ofGetCurrentWindow());

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	m_dllWatcher.lock();
	m_content.m_keyPressed(key);
	m_dllWatcher.unlock();
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	m_dllWatcher.lock();
	m_content.m_keyReleased(key);
	m_dllWatcher.unlock();
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
	m_dllWatcher.lock();
	m_content.m_mouseMoved(x, y);
	m_dllWatcher.unlock();
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
	m_dllWatcher.lock();
	m_content.m_mouseDragged(x, y, button);
	m_dllWatcher.unlock();

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	m_dllWatcher.lock();
	m_content.m_mousePressed(x, y, button);
	m_dllWatcher.unlock();

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	m_dllWatcher.lock();
	m_content.m_mouseReleased(x, y, button);
	m_dllWatcher.unlock();
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){
	m_dllWatcher.lock();
	m_content.m_mouseEntered(x, y);
	m_dllWatcher.unlock();
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){
	m_dllWatcher.lock();
	m_content.m_mouseExited(x, y);
	m_dllWatcher.unlock();

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
	m_dllWatcher.lock();
	m_content.m_windowResized(w, h);
	m_dllWatcher.unlock();
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
	m_dllWatcher.lock();
	m_content.m_gotMessage(msg);
	m_dllWatcher.unlock();
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 
	m_dllWatcher.lock();
	m_content.m_dragEvent(dragInfo);
	m_dllWatcher.unlock();
}
