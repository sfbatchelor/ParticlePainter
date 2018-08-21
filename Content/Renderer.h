#pragma once

class Content;

class Renderer 
{
	
public:

	Renderer(Content* parent, std::shared_ptr<ofFbo> fbo = nullptr);

	virtual void init()=0;
	virtual void update()=0;
	virtual void draw()=0;

	virtual std::string getName() = 0;
	Content & getParent();
	std::shared_ptr<ofFbo> getFbo();
	void resetFbo();
	void reset();
	void setRender(bool render = true);
	void switchRendering();
	bool isReset();
	bool isRendering();

protected:

	void setReset(bool reset);

private:

	Content* m_parent;
	std::shared_ptr<ofFbo> m_fbo;
	bool m_reset;
	bool m_renderActive;

};
