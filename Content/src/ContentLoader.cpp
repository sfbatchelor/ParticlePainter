#include "stdafx.h"
#include "ContentLoader.h"
#include "Content.h"

std::unique_ptr<Content> s_content = nullptr;
extern "C" CONTENT_SETUP(ContentSetup)
{

	s_content = std::move(std::unique_ptr<Content>(new Content()));
	return;
}

extern "C" CONTENT_UPDATE(ContentUpdate)
{
	if (s_content && s_content->isValid())
		s_content->update();
	return;
}

extern "C" CONTENT_RENDER(ContentRender)
{
	if (s_content && s_content->isValid())
		s_content->draw();
	return;
}

extern "C" CONTENT_EXIT(ContentExit)
{
	if (s_content && s_content->isValid())
	{
		s_content->exit();
		s_content.reset();
	}
	return;
}
extern "C" CONTENT_KEY_PRESSED(ContentKeyPressed)
{
	if (s_content && s_content->isValid())
		s_content->keyPressed(key);
	return;
}
extern "C" CONTENT_KEY_RELEASED(ContentKeyReleased)
{
	if (s_content && s_content->isValid())
		s_content->keyReleased(key);
	return;
}
extern "C" CONTENT_MOUSE_MOVED(ContentMouseMoved)
{
	if (s_content && s_content->isValid())
		s_content->mouseMoved(x, y);
	return;
}
extern "C" CONTENT_MOUSE_DRAGGED(ContentMouseDragged)
{
	if (s_content && s_content->isValid())
		s_content->mouseDragged(x, y, button);
	return;
}
extern "C" CONTENT_MOUSE_PRESSED(ContentMousePressed)
{
	if (s_content && s_content->isValid())
		s_content->mousePressed(x, y, button);
	return;
}
extern "C" CONTENT_MOUSE_RELEASED(ContentMouseReleased)
{
	if (s_content && s_content->isValid())
		s_content->mouseReleased(x, y, button);
	return;
}
extern "C" CONTENT_MOUSE_ENTERED(ContentMouseEntered)
{
	if (s_content && s_content->isValid())
		s_content->mouseEntered(x, y);
	return;
}
extern "C" CONTENT_MOUSE_EXITED(ContentMouseExited)
{
	if (s_content && s_content->isValid())
		s_content->mouseExited(x, y);
	return;
}
extern "C" CONTENT_WINDOW_RESIZED(ContentWindowResized)
{
	if (s_content && s_content->isValid())
		s_content->windowResized(w, h);
	return;
}
extern "C" CONTENT_DRAG_EVENT(ContentDragEvent)
{
	if (s_content && s_content->isValid())
		s_content->dragEvent(dragInfo);
	return;
}
extern "C" CONTENT_GOT_MESSAGE(ContentGotMessage)
{
	if (s_content && s_content->isValid())
		s_content->gotMessage(msg);
	return;
}