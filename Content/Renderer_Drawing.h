#pragma once
#include "Renderer.h"


class Drawing : public Renderer 
{
	
public:

	Drawing(Content* parent);

	virtual void init();
	virtual void update();
	virtual void draw();

	virtual std::string getName();

};

