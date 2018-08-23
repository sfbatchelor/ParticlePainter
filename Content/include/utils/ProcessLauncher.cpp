#include "stdafx.h"
#include "ProcessLauncher.h"
#include "ProcessUtils.h"


#ifdef UNICODE
ProcessLauncher::ProcessLauncher(std::wstring processName, float sleepMS, std::wstring exeLocation):
#else
ProcessLauncher::ProcessLauncher(std::string processName, float sleepMS, std::string exeLocation)
#endif
	ofThread(),
	m_sleepMS(sleepMS),
	m_processName(processName),
	m_exeLocation(exeLocation)
{
	if (exeLocation.empty())
		m_exeLocation = m_processName;

	startThread();
}


void ProcessLauncher::threadedFunction()
{
	while (isThreadRunning())
	{
		ProcessUtils::startProcessIfNotRunning(m_exeLocation.c_str(), m_processName.c_str());
		sleep(m_sleepMS);
	}
}


void ProcessLauncher::exit()
{
	waitForThread(true, 500);
	if (ProcessUtils::killProcess(m_processName.c_str()))
		ofLogNotice() << "Killed process " << m_processName.c_str();
	
}
