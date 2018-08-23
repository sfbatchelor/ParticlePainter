#include "stdafx.h"
#include "ParticleSimulation.h"

ParticleSimulation::ParticleSimulation(std::string computeShader):
	m_playing(true),
	m_computeShader(new ComputeWatcher())
{
	if(!computeShader.empty())
		m_computeShader->load(computeShader);
}

void ParticleSimulation::loadParticles(std::vector<ComputeParticle>& particles)
{
	m_particles.empty();
	m_particles = particles;
	m_particlesBufferOld.allocate( m_particles, GL_DYNAMIC_DRAW);
	m_particlesBuffer.allocate( m_particles, GL_DYNAMIC_DRAW);
	m_particlesVbo.setVertexBuffer(m_particlesBuffer, 4, sizeof(ComputeParticle));
	m_particlesVbo.setColorBuffer(m_particlesBuffer, sizeof(ComputeParticle), sizeof(ofVec4f));
	m_particlesVbo.enableColors();
	m_particlesVbo.disableNormals();
	m_particlesVbo.disableIndices();
	m_particlesVbo.disableTexCoords();
	m_particlesBuffer.bindBase(GL_SHADER_STORAGE_BUFFER, 0);
	m_particlesBufferOld.bindBase(GL_SHADER_STORAGE_BUFFER, 1);
}

void ParticleSimulation::loadCompute(std::string computeShader)
{
	m_computeShader->load(computeShader);
}


void ParticleSimulation::update(float currentTime)
{

	m_currentTime = currentTime;
	m_computeShader->update();
	if (!isValid())
	{
		ofLogError() << "Attempting to update an invalid simulation";
		return;
	}
	if (m_playing)
	{
		m_computeShader->getShader().begin();
		m_computeShader->getShader().setUniform1f("uTime", m_currentTime);
		m_computeShader->getShader().setUniform1f("uPrevTime", m_prevTime);
		m_computeShader->getShader().setUniform1i("uNumPointsSF", m_particles.size()/1024);
		m_computeShader->getShader().dispatchCompute((m_particles.size() + 1024 - 1) / 1024, 1, 1);
		m_computeShader->getShader().end();
		m_particlesBuffer.copyTo(m_particlesBufferOld);
		m_prevTime = currentTime;
	}
	else
	{
		m_prevTime = currentTime;
		m_currentTime= currentTime;
	}
}

void ParticleSimulation::draw(int drawMode)
{
	if (!isValid())
	{
		ofLogError() << "Attempting to render an invalid simulation";
		return;
	}
	m_particlesVbo.draw(drawMode, 0, m_particles.size());
}

void ParticleSimulation::reset()
{
	m_particlesBuffer.unbindBase(GL_SHADER_STORAGE_BUFFER, 0);
	m_particlesBufferOld.allocate( m_particles, GL_DYNAMIC_DRAW);
	m_particlesBuffer.allocate( m_particles, GL_DYNAMIC_DRAW);
	m_particlesVbo.setVertexBuffer(m_particlesBuffer, 4, sizeof(ComputeParticle));
	m_particlesVbo.setColorBuffer(m_particlesBuffer, sizeof(ComputeParticle), sizeof(ofVec4f));
	m_particlesBuffer.bindBase(GL_SHADER_STORAGE_BUFFER, 0);
	m_particlesBufferOld.bindBase(GL_SHADER_STORAGE_BUFFER, 1);
}

void ParticleSimulation::pause()
{
	m_playing = false;
}

void ParticleSimulation::play()
{
	m_playing = true;
}

void ParticleSimulation::setPlay(bool play)
{
	m_playing = play;
}

bool ParticleSimulation::isValid()
{
	if(!m_computeShader)
		return false;
	return true;
}
