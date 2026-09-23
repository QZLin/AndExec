#include <process.h>
#include <stdio.h>
#include <wchar.h>
#include <windows.h>

int wWinMain(_In_ [[maybe_unused]] HINSTANCE hInstance, _In_opt_ [[maybe_unused]] HINSTANCE hPrevInstance,
             _In_ LPWSTR lpCmdLine, _In_ [[maybe_unused]] int nShowCmd) {
    int argc = 0;
    const LPWSTR* args = CommandLineToArgvW(GetCommandLine(), &argc);
    LPWSTR commandLine = nullptr;
    LPWSTR workingDir = nullptr;

#ifdef FLAG_ENABLE_WORKDIR
    if(argc < 3)
        exit(2);
    commandLine = args[1];
    workingDir = args[2];
#else
    commandLine = lpCmdLine;
#endif

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
    if(CreateProcessW(nullptr, commandLine, nullptr, nullptr, /**/
                      FALSE, CREATE_NEW_CONSOLE, NULL, workingDir, /**/
                      &si, &pi)) {
#ifdef FLAG_ENABLE_WAIT
        WaitForSingleObject(pi.hProcess, INFINITE);
#endif
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
    else
        exitcode = GetLastError();

    ExitProcess(exitcode);
}
