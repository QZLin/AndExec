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

PWSTR extractString(PWSTR str, UINT64 startIndex, OUT UINT64* endIndex) {
	UINT64 str_len = wcslen(str);

	LPWSTR result = NULL;
	WCHAR symbol;
	int sub_start;
	int sub_end = 0;
	UINT64 index = startIndex;
	if (str[startIndex] == '"') {
		symbol = '"';
		index++;
		sub_start = 1;
	}
	else if (str[startIndex] == '\'') {
		symbol = '\'';
		index++;
		sub_start = 1;
	}
	else {
		symbol = ' ';
		sub_start = 0;
	}

	while (true) {
		if (index >= str_len) break;
		WCHAR c = str[index];
		if (c == symbol && validChar(str, symbol, index)) {
			sub_end = 1;
			break;
		}
		index++;
	}
	*endIndex = index + sub_end;
	UINT64 result_size = ((index - sub_end) - (startIndex + sub_start) + 1) * sizeof(WCHAR);
	result = malloc(result_size + sizeof(WCHAR));

	UINT64 p_result = 0;
	for (UINT64 i = startIndex + sub_start; i < index - sub_end + 1; i++) {
		result[p_result] = str[i];
		p_result++;
	}
	result[p_result] = '\0';
	return result;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{

	LPWSTR fullCmd = GetCommandLine();
	UINT64 fullCmdLen = wcslen(fullCmd);
	LPWSTR workingDir = NULL;
	bool waitExit = 0;

	enum ArgType {
		ARG_NOT = 0,
		ARG_WORKDIR,
		ARG_FLAG,
		ARG_SELF
	};
	enum ArgType nextArg = ARG_SELF;

	LPWSTR realCmd = malloc((fullCmdLen + 1) * sizeof(WCHAR));
	UINT p_realCmd = 0;

	UINT64 cmdlSize = (fullCmdLen + 1) * sizeof(WCHAR);
	LPWSTR cmdl = malloc(cmdlSize);
	UINT64 p_cmdl = 0;

	// Remove args part of self
	int self_head_hit = 0;
	int non_space_hit = 0;
	for (UINT64 i = 0; i < fullCmdLen; i++) {
		WCHAR c = fullCmd[i];

		//if (nextArg == ARG_SELF) {
		if (c == '"')
			self_head_hit++;
		if (self_head_hit == 2) {
			//nextArg = ARG_NOT;
			//wcscpy_s(realCmd,(pCmdLen+1)* sizeof(wchar_t),pCmd);
			p_realCmd = fullCmdLen - i - 1;
			wmemcpy_s(realCmd, (fullCmdLen + 1) * sizeof(WCHAR), fullCmd + i + 1, p_realCmd);
			if (realCmd[0] == ' ') {
				realCmd++;
				p_realCmd -= 1;
			}
			realCmd[p_realCmd] = '\0';
			break;
		}
	}
#define L_IS(index,value) ((int)index-1>0 && realCmd[index-1]==value)
#define N_IS(index,value) ((int)index+1<p_realCmd && realCmd[index+1]==value)
	bool quotation_hit = false;
	nextArg = ARG_NOT;
	for (UINT i = 0; i < p_realCmd; i++) {
		WCHAR c = realCmd[i];
		if (c == '"')
			quotation_hit = true;
		if (!quotation_hit && c == '-' && !L_IS(i, '\\') && i + 1 < p_realCmd) {
			switch (realCmd[i + 1]) {
			case 'w':
				waitExit = true;
				nextArg = ARG_NOT;
				i++;
				break;
			case 'd':
				nextArg = ARG_WORKDIR;
				i++;
				break;
			}
		}
		else if (nextArg != ARG_NOT && c == ' ' && !L_IS(i, '\\')) {
			UINT64 index_start = i + 1;
			UINT64 len = p_realCmd - index_start;
			PWSTR arg_unit = malloc((len + 1) * sizeof(WCHAR));

			UINT64 j;
			for (j = 0; j < len; j++) {
				arg_unit[j] = realCmd[index_start + j];
				if (realCmd[index_start + j + 1] == ' ') {
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
		if (nextArg == ARG_NOT) {
			if (non_space_hit == 0 && c != ' ') non_space_hit++;
			if (non_space_hit > 0) {
				cmdl[p_cmdl] = c;
				p_cmdl++;
				//cmdl[p_cmdl] = '\0';
			}
		}
	}
	if (cmdl == NULL) { exit(2); }
	cmdl[p_cmdl] = '\0';

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
	);
	*/

	if (CreateProcess(
		NULL, cmdl,
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