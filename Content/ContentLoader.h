#pragma once

#ifdef CONTENT_EXPORTS  
#define CONTENT_API __declspec(dllexport)   
#else  
#define CONTENT_API __declspec(dllimport)   
#endif  

#include "Content.h"

//--------------------------------------------------------------
//setup
#define CONTENT_SETUP(name) void name(std::shared_ptr<ofMainLoop> & mainLoop, std::shared_ptr<ofAppBaseWindow> window)
typedef CONTENT_SETUP(Content_Setup);
inline CONTENT_SETUP(ContentSetupStub)
{
}
extern "C" CONTENT_API CONTENT_SETUP(ContentSetup);
//--------------------------------------------------------------
//update
#define CONTENT_UPDATE(name) void name()
typedef CONTENT_UPDATE(Content_Update);
inline CONTENT_UPDATE(ContentUpdateStub)
{
}
extern "C" CONTENT_API CONTENT_UPDATE(ContentUpdate);
//--------------------------------------------------------------
//render
#define CONTENT_RENDER(name) void name(	shared_ptr<ofBaseRenderer> renderer)
typedef CONTENT_RENDER(Content_Render);
inline CONTENT_RENDER(ContentRenderStub)
{
}
extern "C" CONTENT_API CONTENT_RENDER(ContentRender);
//--------------------------------------------------------------
//exit
#define CONTENT_EXIT(name) void name()
typedef CONTENT_EXIT(Content_Exit);
inline CONTENT_EXIT(ContentExitStub)
{
}
extern "C" CONTENT_API CONTENT_EXIT(ContentExit);
//--------------------------------------------------------------
//key-events
#define CONTENT_KEY_PRESSED(name) void name(int key)
typedef CONTENT_KEY_PRESSED(Content_Key_Pressed);
inline CONTENT_KEY_PRESSED(ContentKeyPressedStub)
{
}
extern "C" CONTENT_API CONTENT_KEY_PRESSED(ContentKeyPressed);
#define CONTENT_KEY_RELEASED(name) void name(int key)
typedef CONTENT_KEY_RELEASED(Content_Key_Released);
inline CONTENT_KEY_RELEASED(ContentKeyReleasedStub)
{
}
extern "C" CONTENT_API CONTENT_KEY_RELEASED(ContentKeyReleased);
//--------------------------------------------------------------
//mouse-events
#define CONTENT_MOUSE_MOVED(name) void name(int x, int y)
typedef CONTENT_MOUSE_MOVED(Content_Mouse_Moved);
inline CONTENT_MOUSE_MOVED(ContentMouseMovedStub)
{
}
extern "C" CONTENT_API CONTENT_MOUSE_MOVED(ContentMouseMoved);
#define CONTENT_MOUSE_DRAGGED(name) void name(int x, int y, int button)
typedef CONTENT_MOUSE_DRAGGED(Content_Mouse_Dragged);
inline CONTENT_MOUSE_DRAGGED(ContentMouseDraggedStub)
{
}
extern "C" CONTENT_API CONTENT_MOUSE_DRAGGED(ContentMouseDragged);
#define CONTENT_MOUSE_PRESSED(name) void name(int x, int y, int button)
typedef CONTENT_MOUSE_PRESSED(Content_Mouse_Pressed);
inline CONTENT_MOUSE_PRESSED(ContentMousePressedStub)
{
}
extern "C" CONTENT_API CONTENT_MOUSE_PRESSED(ContentMousePressed);
#define CONTENT_MOUSE_RELEASED(name) void name(int x, int y, int button)
typedef CONTENT_MOUSE_RELEASED(Content_Mouse_Released);
inline CONTENT_MOUSE_RELEASED(ContentMouseReleasedStub)
{
}
extern "C" CONTENT_API CONTENT_MOUSE_RELEASED(ContentMouseReleased);
#define CONTENT_MOUSE_ENTERED(name) void name(int x, int y)
typedef CONTENT_MOUSE_ENTERED(Content_Mouse_Entered);
inline CONTENT_MOUSE_ENTERED(ContentMouseEnteredStub)
{
}
extern "C" CONTENT_API CONTENT_MOUSE_ENTERED(ContentMouseEntered);
#define CONTENT_MOUSE_EXITED(name) void name(int x, int y)
typedef CONTENT_MOUSE_EXITED(Content_Mouse_Exited);
inline CONTENT_MOUSE_EXITED(ContentMouseExitedStub)
{
}
extern "C" CONTENT_API CONTENT_MOUSE_EXITED(ContentMouseExited);
//--------------------------------------------------------------
//window resize
#define CONTENT_WINDOW_RESIZED(name) void name(int w, int h)
typedef CONTENT_WINDOW_RESIZED(Content_Window_Resized);
inline CONTENT_WINDOW_RESIZED(ContentWindowResizedStub)
{
}
extern "C" CONTENT_API CONTENT_WINDOW_RESIZED(ContentWindowResized);
//--------------------------------------------------------------
//window drag
#define CONTENT_DRAG_EVENT(name) void name(ofDragInfo dragInfo)
typedef CONTENT_DRAG_EVENT(Content_Drag_Event);
inline CONTENT_DRAG_EVENT(ContentDragEventStub)
{
}
extern "C" CONTENT_API CONTENT_DRAG_EVENT(ContentDragEvent);
//--------------------------------------------------------------
//got message
#define CONTENT_GOT_MESSAGE(name) void name(ofMessage msg)
typedef CONTENT_GOT_MESSAGE(Content_Got_Message);
inline CONTENT_GOT_MESSAGE(ContentGotMessageStub)
{
}
extern "C" CONTENT_API CONTENT_GOT_MESSAGE(ContentGotMessage);
