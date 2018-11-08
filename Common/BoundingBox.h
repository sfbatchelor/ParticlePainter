#pragma once
#include <glm/common.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>

class BoundingBox2D
{

public:

	enum Corner
	{
		BOTTOMLEFT = 0,
		TOPLEFT,
		BOTTOMRIGHT,
		TOPRIGHT
	};
	enum Side
	{
		BOTTOM = 0,
		LEFT,
		TOP,
		RIGHT
	};

	BoundingBox2D(glm::vec2 pos, glm::vec2 dim) :
		m_position(pos),
		m_dimensions(dim)
	{
	};
	~BoundingBox2D() {};

	void setPosition(glm::vec2 pos)
	{
		m_position = pos;
	};


	void setDimensions(glm::vec2 dim)
	{
		m_dimensions = dim;
	};

	void setWidth(float width)
	{
		m_dimensions.x = width;
	};

	void setHeight(float height)
	{
		m_dimensions.y = height;
	};

	glm::vec2 getCornerPosition(Corner corner)
	{
		switch (corner)
		{
		case BOTTOMLEFT:
			return m_position;
			break;
		case TOPLEFT:
			return glm::vec2(m_position.x, m_position.y + m_dimensions.y);
			break;
		case BOTTOMRIGHT:
			return glm::vec2(m_position.x + m_dimensions.x, m_position.y);
			break;
		case TOPRIGHT:
			return m_position + m_dimensions;
			break;
		}
	};

	float getSidePosition(Side side)
	{
		switch (side)
		{
		case BOTTOM:
			return m_position.y;
			break;
		case LEFT:
			return m_position.x;
			break;
		case TOP:
			return m_position.y + m_dimensions.y;
			break;
		case RIGHT:
			return m_position.x + m_dimensions.x;
			break;
		}
	}

	float getAspectRatio()
	{
		return (float)m_dimensions.x / (float)m_dimensions.y;
	}


	glm::mat4 getMatrix()
	{
		glm::mat4 matrix(1);
		matrix = glm::translate(matrix, glm::vec3(m_position, 0.0f));
		matrix = glm::scale(matrix, glm::vec3(m_dimensions, 1.0f));
		return matrix;
	}


	bool isIntersecting(glm::vec2 point)
	{
		auto maxBounds = m_position + m_dimensions;
		if (point.x <= maxBounds.x &&
			point.x >= m_position.x &&
			point.y <= maxBounds.y &&
			point.y >= m_position.y)
		{
			return true;
		}
		return false;
	}
	bool isIntersecting(BoundingBox2D area)
	{
		return (abs(m_position.x - area.getPosition().x) * 2 <= (m_dimensions.x + area.getDimensions().x)) &&
			(abs(m_position.y - area.getPosition().y) * 2 <= (m_dimensions.y + area.getDimensions().y));
	}

	glm::vec2 getPosition()
	{
		return m_position;
	}

	glm::vec2 getDimensions()
	{
		return m_dimensions;
	}

	float getWidth()
	{
		return m_dimensions.x;
	}

	float getHeight()
	{
		return m_dimensions.y;
	}



protected:

	glm::vec2 m_dimensions;
	glm::vec2 m_position;

};

