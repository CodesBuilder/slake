#include "exec.h"

#ifdef _WIN32
#include <Windows.h>
#endif

int hkExec(const char *cmdline)
{
#ifdef _WIN32
	STARTUPINFOA si;
	PROCESS_INFORMATION pi;
	if (!CreateProcessA(
			NULL,
			(char *)cmdline,
			NULL,
			NULL,
			TRUE,
			NULL,
			NULL,
			NULL,
			&si,
			&pi))
		return -1;
	WaitForSingleObject(pi.hThread, INFINITE);
	WaitForSingleObject(pi.hProcess, INFINITE);
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);

	DWORD exitcode;
	GetExitCodeProcess(pi.hProcess, &exitcode);

	return *((int *)&exitcode);
#endif
}
