#pragma once
#include "utils\ShaderWatcher.h"


struct ComputeParticle
{
	ofVec4f m_pos;
	ofFloatColor m_col;
	ofVec4f m_vel;
};

//!Class that encapsulates a compute shader working on a buffer of points
class ParticleSimulation
{

public:

	ParticleSimulation(std::string computeShader = "");
	void loadParticles(std::vector<ComputeParticle>& particles);
	void loadCompute(std::string computeShader);
	void update(float currentTime);
	void draw(int drawMode = GL_POINTS);

	void reset();
	void pause();
	void play();
	void setPlay(bool play);
	bool isValid();

private:

	std::shared_ptr<ComputeWatcher> m_computeShader;

	std::vector<ComputeParticle> m_particles;

	ofVbo m_particlesVbo;
	ofBufferObject m_particlesBuffer;
	ofBufferObject m_particlesBufferOld;
	bool m_playing;
	float m_currentTime;
	float m_prevTime;


};
