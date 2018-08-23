#pragma once

class ProcessLauncher : public ofThread
{
public:

#ifdef UNICODE
	ProcessLauncher(std::wstring processName, float sleepMS = 100, std::wstring exeLocation = L"");
#else
	ProcessLauncher(std::string processName, float sleepMS = 100, std::string exeLocation = "");
#endif

	void exit();

protected:

	virtual void threadedFunction();

private:

	float m_sleepMS;
#ifdef UNICODE
	std::wstring m_processName;
	std::wstring m_exeLocation;
#else 
	std::string m_processName;
	std::string m_exePath;
#endif

};
