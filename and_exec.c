#include <process.h>
#include <windows.h>
#include <wchar.h>
#include <stdbool.h>

bool validChar(LPWSTR str, WCHAR character, UINT64 index) {
	if (str[index] == character) {
		if (index == 0) return true;
	}
	else return false;
	if (validChar(str, '\\', index - 1))
		return false;
	return true;
}

LPWSTR subString(LPWSTR str, UINT64 startIndex, UINT64 endIndex) {
	LPWSTR result = malloc((endIndex - startIndex + 1) * sizeof(WCHAR));
	UINT64 i;
	for (i = 0; i <= endIndex - startIndex; i++) result[i] = str[startIndex + i];
	result[i] = '\0';
	return result;
}

LPWSTR extractString(LPWSTR str, UINT64 startIndex, OUT UINT64* endIndex) {
	UINT64 str_len = wcslen(str);

	LPWSTR result = NULL;
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
		//WCHAR c = str[index];
		if (validChar(str, symbol, index)) {
			index--;
			index += trim_end;
			*endIndex = index + 1;
			break;
		}
		index++;
	}
	result = subString(str, startIndex + trim_start, index - trim_end);
	/*result = malloc(((index - trim_end) - (startIndex + trim_start) + 2) * sizeof(WCHAR));
	UINT64 p_result;
	for (p_result = 0; p_result <= (index - trim_end) - (startIndex + trim_start); p_result++) {
		result[p_result] = str[(startIndex + trim_start) + p_result];
	}
	result[p_result] = '\0';*/
	return result;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	enum ArgType {
		ARG_NONE = 0,
		ARG_WORKDIR
	};
	enum ArgType nextArg = ARG_NONE;
	LPWSTR workingDir = NULL;
	bool waitExit = false;


	//pCmdLine = __TEXT("cmd /k ping localhost");
	//pCmdLine = __TEXT("\"cmd /k ping localhost\"");
	//pCmdLine = __TEXT("cmd /k \"ping localhost\"");
	//pCmdLine = __TEXT("-d \"C:\\Program Files\" -w cmd /k ping localhost");
	//pCmdLine = __TEXT("-d . -w cmd /k ping localhost");
	//pCmdLine = __TEXT("-w cmd /k ping localhost");
	UINT cmdLineLen = wcslen(pCmdLine);
	LPWSTR commandLine = NULL;
	for (UINT i = 0; i < cmdLineLen; ) {
		UINT end = 0;
		PWSTR value = extractString(pCmdLine, i, &end);

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
		}
		// options end
		else if (validChar(pCmdLine, '"', i)) {
			commandLine = value;
			break;
		}
		else {
			commandLine = subString(pCmdLine, i, cmdLineLen - 1);
			break;
		}
		i = end;
	}

	STARTUPINFO si;
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
	if (CreateProcess(
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
	}
	else exitcode = GetLastError();

	ExitProcess(exitcode);
}