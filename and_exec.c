#include <process.h>
#include <windows.h>
#include <wchar.h>
#include <stdbool.h>

LPWSTR subString(LPWSTR str, UINT64 startIndex, UINT64 endIndex) {
    LPWSTR result = malloc((endIndex - startIndex + 1) * sizeof(WCHAR));
    UINT64 i;
    for (i = 0; i < endIndex - startIndex; i++) result[i] = str[startIndex + i];
    result[i] = '\0';
    return result;
}

bool validArgChar(LPWSTR str, WCHAR character, UINT64 index) {
    if (str[index] == character) {
        if (index == 0) return true;
    } else return false;
    if (validArgChar(str, '\\', index - 1))
        return false;
    return true;
}

// andexec :commandline
// andexec :commandline:workdir
// andexec :[flags]:commandline:workdir
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR pCmdLine, int nCmdShow) {
    LPWSTR commandLine = NULL;
    LPWSTR workingDir = NULL;

    bool waitExit = false;

    int argvCount;
    LPWSTR *argv = CommandLineToArgvW(pCmdLine, &argvCount);
    switch (argvCount) {
        case 0:
            exit(2);
        case 1:
            commandLine = pCmdLine;
            break;
        case 2:
            commandLine = argv[0];
            workingDir = argv[1];
            break;
        case 3:
            commandLine = argv[0];
            workingDir = argv[1];
            LPWSTR flags = argv[2];

            UINT64 flagsLen = wcslen(flags);
            for (UINT64 i = 0; i < flagsLen; i++)
                switch (flags[i]) {
                    case 'w':
                        waitExit = TRUE;
                        break;
                }
            break;
        default:
            exit(3);
    }


//    LPWSTR test1 = L"0123456789";
//    LPWSTR r1 = subString(test1, 0, 1);

    /*UINT64 cmdLineLen = wcslen(pCmdLine);
    if (cmdLineLen == 0)
        exit(2);

    const WCHAR SEP = pCmdLine[0];

    int sepCount = 0;
    int sepLocMaxi = 10;
    UINT64 *sepLocs = malloc(sepLocMaxi * sizeof(UINT64));
    for (UINT64 i = 0; i < cmdLineLen; i++) {
        if (pCmdLine[i] != SEP)
            continue;
        if (sepCount < sepLocMaxi)
            sepLocs[sepCount] = i;
        else {
            sepLocMaxi += 10;
            realloc(sepLocs, sepLocMaxi * sizeof(UINT64));
            sepLocs[sepCount] = i;
        }
        sepCount++;
    }*/


    /*switch (sepCount) {
        case 0:
            commandLine = pCmdLine;
            break;
        case 1:
            commandLine = subString(pCmdLine, 1, sepLocs[0]);
        case 2:
            commandLine = subString(pCmdLine, 0, sepLocs[0]);
            workingDir = subString(pCmdLine, sepLocs[1] + 1, cmdLineLen);
            break;
        case 3:
            LPWSTR flags = subString(pCmdLine, 0, sepLocs[0]);
            UINT64 flagsLen = wcslen(flags);
            for (UINT64 i = 0; i < flagsLen; i++) {
                switch (flags[i]) {
                    case FLAGWaitExit:
                        waitExit = TRUE;
                        break;
                }

            }
            commandLine = subString(pCmdLine, sepLocs[0] + 1, sepLocs[1]);
            workingDir = subString(pCmdLine, sepLocs[1] + 1, cmdLineLen);
            break;
        default:
            break;
    }*/

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
//        free(commandLine);
//        free(workingDir);
    } else exitcode = GetLastError();

    ExitProcess(exitcode);
}