#pragma once
#include <Filewatcher.h>
#include <vector>

struct ThreadedShaderFile
{
	ThreadedShaderFile() { };
	FileWatcher m_fileWatcher;
	std::filesystem::path m_filePath;
};

class ShaderWatcher
{

public:

	ShaderWatcher();
	~ShaderWatcher();
	bool load(const std::filesystem::path& vertName, const std::filesystem::path& fragName);
	void update();
	void exit();
//	const ofShader& getShader() const;

private:


	//ofShader m_shader;
	//ofShader m_frontShader;
	ThreadedShaderFile m_vertex;
	ThreadedShaderFile m_fragment;
	void onFileWasModified();
	bool m_needsUpdating;
	ofMutex m_mutex;

};

class ComputeWatcher 
{

public:

	ComputeWatcher();
	~ComputeWatcher();
	bool loadCompute(const std::filesystem::path& shaderName);
	void update();
	void exit();
//	const ofShader& getShader() const;

private:
//	ofShader m_shader;
//	ofShader m_frontShader;
	ThreadedShaderFile m_compute;
	void onFileWasModified();
	bool m_needsUpdating;
	ofMutex m_mutex;

};
