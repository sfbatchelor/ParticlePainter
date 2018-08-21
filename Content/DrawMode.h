#pragma once

class Content;

class DrawMode 
{
	
public:

	DrawMode(Content* parent);

	virtual void init()=0;
	virtual void update()=0;
	virtual void draw()=0;

	virtual std::string getName() = 0;
	Content & getParent();


private:

	Content* m_parent;

};
