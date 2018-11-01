#include "stdafx.h"
#include "3d\ParticleSimulation.h"

ParticleSimulation::ParticleSimulation(std::string computeShader):
	m_playing(true),
	m_computeShader(new ComputeWatcher())
{
	if(!computeShader.empty())
		m_computeShader->load(computeShader);
}

ParticleSimulation::~ParticleSimulation()
{
	m_computeShader->exit();
}

void ParticleSimulation::loadParticles(std::vector<GpuParticle>& particles)
{
	m_particles.empty();
	m_particles = particles;
	m_particlesBufferOld.allocate( m_particles, GL_DYNAMIC_DRAW);
	m_particlesBuffer.allocate( m_particles, GL_DYNAMIC_DRAW);
	m_particlesVbo.setVertexBuffer(m_particlesBuffer, 4, sizeof(GpuParticle));
	m_particlesVbo.setColorBuffer(m_particlesBuffer, sizeof(GpuParticle), sizeof(ofVec4f));
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


void ParticleSimulation::update()
{
	m_computeShader->update();
	if (!isValid())
	{
		ofLogError() << "Attempting to update an invalid simulation";
		return;
	}
	if (m_playing)
	{
		m_computeShader->getShader().begin();
		auto time = ofGetElapsedTimef();
		m_computeShader->getShader().setUniform1f("uTime", time);
		m_computeShader->getShader().setUniform1i("uNumPointsSF", m_particles.size()/1024);
		m_computeShader->getShader().dispatchCompute((m_particles.size() + 1024 - 1) / 1024, 1, 1);
		m_computeShader->getShader().end();
		m_particlesBuffer.copyTo(m_particlesBufferOld);
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
	m_particlesVbo.setVertexBuffer(m_particlesBuffer, 4, sizeof(GpuParticle));
	m_particlesVbo.setColorBuffer(m_particlesBuffer, sizeof(GpuParticle), sizeof(ofVec4f));
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
	if(!m_computeShader || !m_particlesBufferOld.isAllocated() || !m_particlesBuffer.isAllocated())
		return false;
	return true;
}
