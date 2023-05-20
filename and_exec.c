#include <process.h>
#include <windows.h>
#include <wchar.h>
#include <stdbool.h>



LPWSTR subString(LPWSTR str, UINT64 startIndex, UINT64 endIndex) {
	LPWSTR result = malloc((endIndex - startIndex + 1) * sizeof(WCHAR));
	UINT64 i;
	for (i = 0; i <= endIndex - startIndex; i++) result[i] = str[startIndex + i];
	result[i] = '\0';
	return result;
}

bool validArgChar(LPWSTR str, WCHAR character, UINT64 index) {
	if (str[index] == character) {
		if (index == 0) return true;
	}
	else return false;
	if (validArgChar(str, '\\', index - 1))
		return false;
	return true;
}

LPWSTR extractArgString(LPWSTR str, UINT64 startIndex, OUT UINT64* endIndex) {
	UINT64 str_len = wcslen(str);

	WCHAR symbol;
	int trim_start, trim_end;
	if (str[startIndex] == '"' || str[startIndex] == '\'') {
		symbol = str[startIndex];
		trim_start = 1;
		trim_end = 2;
	}
	else {
		symbol = ' ';
		trim_start = 0;
		trim_end = 1;
	}

	UINT64 index = startIndex + trim_start;
	while (true) {
		if (index >= str_len) {
			*endIndex = index;
			index--;
			trim_start = trim_end = 0;
			break;
		}
		if (validArgChar(str, symbol, index)) {
			index--;
			index += trim_end;
			*endIndex = index + 1;
			break;
		}
		index++;
	}
	LPWSTR result = subString(str, startIndex + trim_start, index - trim_end);
	return result;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR pCmdLine, int nCmdShow)
{
	enum ArgType {
		ARG_NONE = 0,
		ARG_WORKDIR
	};
	enum ArgType nextArg = ARG_NONE;
	LPWSTR workingDir = NULL;
	bool waitExit = false;


	//pCmdLine = L"cmd /k ping localhost";
	//pCmdLine = L"\"cmd /k ping localhost\"";
	//pCmdLine = L"cmd /k \"ping localhost\"";
	//pCmdLine = L"-d \"C:\\Program Files\" -w cmd /k ping localhost";
	//pCmdLine = L"-d . -w cmd /k ping localhost";
	//pCmdLine = L"-w cmd /k ping localhost";
	UINT64 cmdLineLen = wcslen(pCmdLine);
	LPWSTR commandLine = NULL;
	for (UINT64 i = 0; i < cmdLineLen; ) {
		UINT64 end = 0;
		PWSTR value = extractArgString(pCmdLine, i, &end);

		if (nextArg != ARG_NONE) {
			switch (nextArg) {
			case ARG_WORKDIR:
				workingDir = value;
				nextArg = ARG_NONE;
				break;
			default:
				free(value);
				nextArg = ARG_NONE;
			}
		}
		else if (value[0] == '-') {
			switch (value[1]) {
			case 'w':
				waitExit = true;
				nextArg = ARG_NONE;
				break;
			case 'd':
				nextArg = ARG_WORKDIR;
				break;
			}
			//free(value);
		}
		// options end
		else if (validArgChar(pCmdLine, '"', i) || validArgChar(pCmdLine, '\'', i)) {
			commandLine = value;
			break;
		}
		else {
			//free(value);
			commandLine = subString(pCmdLine, i, cmdLineLen - 1);
			break;
		}
		i = end;
	}

	STARTUPINFOW si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));
	DWORD exitcode = 0;
	/* create process with new console */
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	/*
	  BOOL CreateProcessW(
	  [in, optional]      LPCWSTR               lpApplicationName,
	  [in, out, optional] LPWSTR                lpCommandLine,

	  [in, optional]      LPSECURITY_ATTRIBUTES lpProcessAttributes,
	  [in, optional]      LPSECURITY_ATTRIBUTES lpThreadAttributes,

	  [in]                BOOL                  bInheritHandles,
	  [in]                DWORD                 dwCreationFlags,

	  [in, optional]      LPVOID                lpEnvironment,
	  [in, optional]      LPCWSTR               lpCurrentDirectory,

	  [in]                LPSTARTUPINFOW        lpStartupInfo,
	  [out]               LPPROCESS_INFORMATION lpProcessInformation
	); */
	if (CreateProcessW(
		NULL, commandLine,
		NULL, NULL,
		FALSE, CREATE_NEW_CONSOLE,
		NULL, workingDir,
		&si, &pi
	))
	{
		if (waitExit) WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		free(commandLine);
		free(workingDir);
	}
	else exitcode = GetLastError();

	ExitProcess(exitcode);
}