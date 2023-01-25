#include <process.h>
#include <windows.h>
#include <wchar.h>

void WinMainCRTStartup(void)
{
	STARTUPINFO startupInfo;
	PROCESS_INFORMATION processInfo;
	DWORD exitcode = 0;

	LPWSTR pCmd = GetCommandLine();
	UINT64 pCmdLen = wcslen(pCmd);
	LPWSTR workingDir = NULL;
	int waitExit = 0;

	enum ArgType {
		ARG_NOT = 0,
		ARG_WORKDIR,
		ARG_FLAG,
		ARG_SELF
	};
	enum ArgType nextArg = ARG_SELF;

	LPWSTR cmdl = malloc((pCmdLen + 1) * sizeof(WCHAR));
	UINT64 p_cmdl = 0;

	int self_head_hit = 0;
	int non_space_hit = 0;
	for (UINT64 i = 0; i < pCmdLen; i++) {
		WCHAR c = pCmd[i];

		if (nextArg == ARG_SELF) {
			if (c == '"')
				self_head_hit++;
			if (self_head_hit == 2)
				nextArg = ARG_NOT;
		}
		else if (c == '-' && (i == 0 || pCmd[i - 1] == ' ') && (i == 0 || pCmd[i - 1] != '\\')) {
			switch (pCmd[i + 1]) {
			case 'w':
				waitExit = TRUE;
				nextArg = ARG_NOT;
				i++;
				break;
			case 'd':
				nextArg = ARG_WORKDIR;
				i++;
				break;
			}
		}
		else if (nextArg != ARG_NOT && c == ' ' && (i == 0 || pCmd[i - 1] != '\\')) {
			UINT64 index_start = i + 1;
			UINT64 len = pCmdLen - index_start;
			WCHAR* arg_unit = malloc((len + 1) * sizeof(WCHAR));

			UINT64 j;
			for (j = 0; j < len; j++) {
				arg_unit[j] = pCmd[index_start + j];
				if (pCmd[index_start + j + 1] == ' ') {
					arg_unit = realloc(arg_unit, (j + 2) * sizeof(WCHAR));
					break;
				}
			}
			arg_unit[j + 1] = '\0';
			i += j + 1;
			switch (nextArg) {
			case ARG_WORKDIR:
				workingDir = arg_unit;
				break;
			default:
				free(arg_unit);
			}
			nextArg = ARG_NOT;
		}
		else if (nextArg == ARG_NOT) {
			if (nextArg != ARG_SELF && non_space_hit == 0 && c != ' ') non_space_hit++;
			if (non_space_hit > 0) {
				cmdl[p_cmdl] = c;
				p_cmdl++;
				//cmdl[p_cmdl] = '\0';
			}
		}
	}
	cmdl[p_cmdl] = '\0';
	//cmdl = realloc(cmdl, p_cmdl * sizeof(WCHAR));

	//MessageBox(0, workingDir, L"test", 0);
	/*if (pCmd[0] == '"') stopchar = '"';
	do {
		pCmd++;
	} while ((pCmd[0] != stopchar) && (pCmd[0] != 0));
	if (pCmd[0] != 0)
	{
		do {
			pCmd++;
		} while ((pCmd[0] != 0) && ((pCmd[0] == ' ') || (pCmd[0] == 't')));
	};
	if (pCmd[0] == 0)
	{
		MessageBox(0,
			L"About:\n\nhidec hides console window of started program & waits (opt.) for its termination\n\n"
			L"Usage:\n\n\thidec [/w] <filename>\n\nWhere:\n\n"
			L"/w\twait for program termination\nfilename\texecutable file name",

			L"Error: Incorrect usage"
			, 0);
		ExitProcess(0);
	};

	if ((pCmd[0] == '/') && (((pCmd[1]) | 0x20) == 'w') && (pCmd[2] == ' '))
	{
		bWait = 1;
		pCmd += 3;
	};
	while ((pCmd[0] != 0) && ((pCmd[0] == ' ') || (pCmd[0] == 't'))) pCmd++;*/

	/* create process with new console */
	unsigned char* ps = (unsigned char*)&startupInfo;
	for (unsigned int i = 0; i < sizeof(startupInfo); i++) ps[i] = 0x00;

	startupInfo.cb = sizeof(startupInfo);
	startupInfo.dwFlags = STARTF_USESHOWWINDOW;
	startupInfo.wShowWindow = SW_HIDE;
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
	);
	*/

	//LPWSTR test = L"node node-server.js 6880";
	if (CreateProcess(
		NULL, cmdl,
		NULL, NULL,
		FALSE, CREATE_NEW_CONSOLE,
		NULL, workingDir,
		&startupInfo, &processInfo
	))
	{
		if (waitExit) WaitForSingleObject(processInfo.hProcess, INFINITE);
		CloseHandle(processInfo.hProcess);
		CloseHandle(processInfo.hThread);
	}
	else exitcode = GetLastError();

	/* terminate this */
	ExitProcess(exitcode);
}