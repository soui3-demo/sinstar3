#pragma once

struct TASKINFO
{
	DWORD pid;
	WCHAR szName[MAX_PATH];
	WCHAR szPath[1000];
};

enum FC_MSG
{
	FC_REFRESH = 100,
	ACK_REFRESH,
	FC_KILLPID,
	FC_ACTIVATE,
	FC_QUIT,
};