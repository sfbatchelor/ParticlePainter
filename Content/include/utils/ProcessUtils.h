#pragma once
#include <string>

namespace ProcessUtils
{
#include <process.h>
#include <windows.h>
#include <tlhelp32.h>

#ifdef UNICODE

	static bool isProcessRunning(const WCHAR* process)
	{

		HANDLE hProcessSnap;
		PROCESSENTRY32 pe32;

		// Take a snapshot of all processes in the system.
		hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hProcessSnap == INVALID_HANDLE_VALUE)
		{
			// todo - put error here
			return false;
		}

		// Set the size of the structure before using it.
		pe32.dwSize = sizeof(PROCESSENTRY32);

		// Retrieve information about the first process,
		// and exit if unsuccessful
		if (!Process32First(hProcessSnap, &pe32))
		{
			CloseHandle(hProcessSnap);  // clean the snapshot object
			return false;
		}

		// Now walk the snapshot of processes 
		do
		{
			std::wstring str(pe32.szExeFile);

			if (str == process) // put the name of your process you want to kill
			{
				CloseHandle(hProcessSnap);
				return true;
			}
		} while (Process32Next(hProcessSnap, &pe32));

		CloseHandle(hProcessSnap);
		return false;
	}

	static bool killProcess(const WCHAR* process)
	{
		HANDLE hProcessSnap;
		PROCESSENTRY32 pe32;

		// Take a snapshot of all processes in the system.
		hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hProcessSnap == INVALID_HANDLE_VALUE)
		{
			// todo - put error here
			return false;

		}

		// Set the size of the structure before using it.
		pe32.dwSize = sizeof(PROCESSENTRY32);

		// Retrieve information about the first process,
		// and exit if unsuccessful
		if (!Process32First(hProcessSnap, &pe32))
		{
			CloseHandle(hProcessSnap);  // clean the snapshot object
			return false;
		}

		// Now walk the snapshot of processes 
		do
		{
			std::wstring str(pe32.szExeFile);

			if (str == process) // put the name of your process you want to kill
			{
				DWORD dwDesiredAccess = PROCESS_TERMINATE;
				BOOL  bInheritHandle = FALSE;
				HANDLE hProcess = OpenProcess(dwDesiredAccess, bInheritHandle, pe32.th32ProcessID);
				if (hProcess == NULL)
					return true;

				BOOL result = TerminateProcess(hProcess, 1);

				CloseHandle(hProcess);
			}
		} while (Process32Next(hProcessSnap, &pe32));

		CloseHandle(hProcessSnap);
		return false;
	}

	static bool startProcess(const WCHAR* process, WCHAR* args)
	{

		// additional information
		STARTUPINFO si;
		PROCESS_INFORMATION pi;

		// set the size of the structures
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));

		bool success;
		success = CreateProcess(
			process,   // the path
			args,        // Command line
			NULL,           // Process handle not inheritable
			NULL,           // Thread handle not inheritable
			FALSE,          // Set handle inheritance to FALSE
			0,              // No creation flags
			NULL,           // Use parent's environment block
			NULL,           // Use parent's starting directory 
			&si,            // Pointer to STARTUPINFO structure
			&pi             // Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
		);
		if (!success)
		{
			ofLogError() << "Failed to start process: " << process;
			return false;
		}

		// Close process and thread handles. 
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		return true;
	}


	static void startProcessIfNotRunning(const WCHAR* exeLocation, const WCHAR* processName, WCHAR* args  )
	{
		if (!isProcessRunning(processName))
			startProcess(exeLocation, args);
	}
	static void startProcessIfNotRunning(const WCHAR* exeLocation, const WCHAR* processName  )
	{
		if (!isProcessRunning(processName))
		{
			wchar_t args[1];
			startProcess(exeLocation, args);
		}
	}

#else

	static bool isProcessRunning(const CHAR* process)
	{

		HANDLE hProcessSnap;
		PROCESSENTRY32 pe32;

		// Take a snapshot of all processes in the system.
		hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hProcessSnap == INVALID_HANDLE_VALUE)
		{
			// todo - put error here
			return false;
		}

		// Set the size of the structure before using it.
		pe32.dwSize = sizeof(PROCESSENTRY32);

		// Retrieve information about the first process,
		// and exit if unsuccessful
		if (!Process32First(hProcessSnap, &pe32))
		{
			CloseHandle(hProcessSnap);  // clean the snapshot object
			return false;
		}

		// Now walk the snapshot of processes 
		do
		{
			std::string str(pe32.szExeFile);

			if (str == process) // put the name of your process you want to kill
			{
				CloseHandle(hProcessSnap);
				return true;
			}
		} while (Process32Next(hProcessSnap, &pe32));

		CloseHandle(hProcessSnap);
		return false;
	}

	static void killProcess(const CHAR* process)
	{
		HANDLE hProcessSnap;
		PROCESSENTRY32 pe32;

		// Take a snapshot of all processes in the system.
		hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hProcessSnap == INVALID_HANDLE_VALUE)
		{
			// todo - put error here
			return;
		}

		// Set the size of the structure before using it.
		pe32.dwSize = sizeof(PROCESSENTRY32);

		// Retrieve information about the first process,
		// and exit if unsuccessful
		if (!Process32First(hProcessSnap, &pe32))
		{
			CloseHandle(hProcessSnap);  // clean the snapshot object
			return;
		}

		// Now walk the snapshot of processes 
		do
		{
			std::string str(pe32.szExeFile);

			if (str == process) // put the name of your process you want to kill
			{
				DWORD dwDesiredAccess = PROCESS_TERMINATE;
				BOOL  bInheritHandle = FALSE;
				HANDLE hProcess = OpenProcess(dwDesiredAccess, bInheritHandle, pe32.th32ProcessID);
				if (hProcess == NULL)
					return;

				BOOL result = TerminateProcess(hProcess, 1);

				CloseHandle(hProcess);
			}
		} while (Process32Next(hProcessSnap, &pe32));

		CloseHandle(hProcessSnap);
	}

	static bool startProcess(const CHAR* process, CHAR* args)
	{

		// additional information
		STARTUPINFO si;
		PROCESS_INFORMATION pi;

		// set the size of the structures
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));

		bool success;
		success = CreateProcess(
			process,   // the path
			args,        // Command line
			NULL,           // Process handle not inheritable
			NULL,           // Thread handle not inheritable
			FALSE,          // Set handle inheritance to FALSE
			0,              // No creation flags
			NULL,           // Use parent's environment block
			NULL,           // Use parent's starting directory 
			&si,            // Pointer to STARTUPINFO structure
			&pi             // Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
		);
		if (!success)
		{
			return false;
		}

		// Close process and thread handles. 
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		return true;
	}


	static void startProcessIfNotRunning(const CHAR* exeLocation, const CHAR* processName, CHAR* args)
	{
		if (!isProcessRunning(processName))
			startProcess(exeLocation, args);
	}
	static void startProcessIfNotRunning(const WCHAR* exeLocation, const WCHAR* processName  )
	{
		if (!isProcessRunning(processName))
		{
			char args[1];
			startProcess(exeLocation, args);
		}
	}

#endif


}