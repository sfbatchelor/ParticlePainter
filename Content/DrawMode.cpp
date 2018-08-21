#include "stdafx.h"
#include "DrawMode.h"
#include "Content.h"

DrawMode::DrawMode(Content * parent):
	m_parent(parent)
{
}

Content & DrawMode::getParent()
{
	return *m_parent;
}
