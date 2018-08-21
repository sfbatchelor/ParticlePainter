#include "stdafx.h"
#include "DrawMode_Drawing.h"
#include "Content.h"

Drawing::Drawing(Content * parent):
	DrawMode(parent)
{
}

void Drawing::init()
{
}

void Drawing::update()
{


	return ;
}

void Drawing::draw()
{



}

std::string Drawing::getName()
{
	return std::string("Drawing");
}
