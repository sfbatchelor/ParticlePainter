#include "stdafx.h"
#include "Renderer_Drawing.h"
#include "Content.h"

Drawing::Drawing(Content * parent, std::shared_ptr<ofFbo> fbo):
	Renderer(parent, fbo)
{
}

void Drawing::init()
{
	setRender(false);
	reset();
	getGui().setup("Drawing");
}

void Drawing::update()
{

	if (isReset())
	{
		resetFbo();
		//m_draggedImage.reset(new ofImage());
		//m_compositeImage = false;
		setReset(false);
	}

	if (isRendering() && getFbo())
	{
		getFbo()->begin();
		//if (m_compositeImage)
		//{
		//	m_compositeImage = false;
		//	m_draggedImage->draw(0, 0);
		//	getFbo()->clearDepthBuffer(10000000.);
		//}
		getParent().drawScene();
		getFbo()->end();
	}

	return ;
}

void Drawing::draw()
{
	if (isRendering() && getFbo())
		getFbo()->draw(0, 0);
	else
		getParent().drawScene();
}

std::string Drawing::getName()
{
	return std::string("Drawing");
}

