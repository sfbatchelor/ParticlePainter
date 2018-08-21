#pragma once
#include "DrawMode.h"


class Drawing : public DrawMode 
{
	
public:

	Drawing(Content* parent);

	virtual void init();
	virtual void update();
	virtual void draw();

	virtual std::string getName();

};

