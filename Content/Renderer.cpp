#include "stdafx.h"
#include "Renderer.h"
#include "Content.h"

Renderer::Renderer(Content* parent, std::shared_ptr<ofFbo> fbo):
	m_parent(parent),
	m_fbo(fbo),
	m_renderActive(false),
	m_reset(false),
	m_gui(ofxPanel())
{
}

void Renderer::drawGui()
{
	m_gui.draw();
}

Content & Renderer::getParent()
{
	return *m_parent;
}

ofxPanel & Renderer::getGui()
{
	return m_gui;
}

std::shared_ptr<ofFbo> Renderer::getFbo()
{
	return m_fbo;
}

void Renderer::reset()
{
	m_reset = true;
}

void Renderer::setRender(bool render)
{
	m_renderActive = render;
}

void Renderer::switchRendering()
{
	m_renderActive = !m_renderActive;
}

bool Renderer::isReset()
{
	return m_reset;
}

bool Renderer::isRendering()
{
	return m_renderActive;
}

void Renderer::setReset(bool reset)
{
	m_reset = reset;
}

void Renderer::resetFbo()
{

	m_fbo.reset(new ofFbo());
	m_fbo->allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);
	m_fbo->begin();
	ofClear(0, 0, 0, 255);
	m_fbo->end();
}
