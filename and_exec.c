#include <process.h>
#include <windows.h>
#include <wchar.h>
#include <stdbool.h>
#include <stdio.h>

LPWSTR SelfName() {
	int argc = 0;
	LPWSTR* args = CommandLineToArgvW(GetCommandLine(), &argc);
	LPWSTR fullname = args[0];
	int fullnameLen = wcsnlen(fullname, INT_MAX);
	int nameLen;
	int index = 0;
	for (int i = fullnameLen; i >= 0; i--) {
		if (fullname[i] == '\\') {
			index = i;
			break;
		}
	}
	nameLen = fullnameLen - index;

	PWSTR name = (PWSTR)malloc(sizeof(WCHAR) * (nameLen + 1));
	wcsncpy_s(name, nameLen + 1, fullname + index, nameLen);
	if (name[0] == '\\') {
		name++;
		nameLen--;
	}
	for (int i = 0; i < nameLen; i++)
		name[i] = towlower(name[i]);
	// TODO: remove .exe suffix
	return name;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR pCmdLine, int nCmdShow) {
	LPWSTR commandLine = NULL;
	LPWSTR workingDir = NULL;

	bool hasWorkDir = false;
	bool waitExit = false;

	LPWSTR selfName = SelfName();

	// andexec <commandline>
	// andexec_d <commandline> <workdir>
	// andexec_w <commandline> # WaitExit
	// andexec_dw <commandline> <workdir> # WaitExit

	if (wcscmp(selfName, L"andexec_d.exe") == 0) {
		hasWorkDir = true;
	}
	else if (wcscmp(selfName, L"andexec_w.exe") == 0) {
		commandLine = pCmdLine;
		waitExit = true;
	}
	else if (wcscmp(selfName, L"andexec_wd.exe") == 0) {
		waitExit = true;
		hasWorkDir = true;
	}
	else {
		commandLine = pCmdLine;
	}

	if (hasWorkDir) {
		int argc;
		LPWSTR* argv = CommandLineToArgvW(pCmdLine, &argc);
		if (argc < 2) {
			printf("err:2 args is required");
			exit(2);
		}
		commandLine = argv[0];
		workingDir = argv[1];
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
	)) {
		if (waitExit) WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
	else exitcode = GetLastError();

	ExitProcess(exitcode);
}