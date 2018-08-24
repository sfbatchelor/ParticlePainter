#pragma once
#include "utils\ShaderWatcher.h"
#include "Particle.h"

//!Class that encapsulates a compute shader working on a buffer of points
class ParticleSimulation
{

public:

	ParticleSimulation(std::string computeShader = "");
	~ParticleSimulation();

	void loadParticles(std::vector<GpuParticle>& particles);
	void loadCompute(std::string computeShader);
	void update();
	void draw(int drawMode = GL_POINTS);

	void reset();
	void pause();
	void play();
	void setPlay(bool play);
	bool isValid();

private:

	std::shared_ptr<ComputeWatcher> m_computeShader;

	std::vector<GpuParticle> m_particles;

	ofVbo m_particlesVbo;
	ofBufferObject m_particlesBuffer;
	ofBufferObject m_particlesBufferOld;
	bool m_playing;


};
