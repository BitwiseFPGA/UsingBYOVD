#include "Shdrv.h"

BOOLEAN Shdrv::KillProcess(ULONG Pid)
{
	struct
	{
		ULONG ReqCode = 1;
		ULONG Pid{};
		WCHAR ProcessName[260]{};
	}Req;

	Req.Pid = Pid;

	return DeviceIoControl(m_hDevice,
						   IOCTL_KILL_PROCESS,
						   &Req,
						   sizeof(Req),
						   nullptr,
						   0);
}

BOOLEAN Shdrv::KillProcess(const std::string& ProcessName)
{
	struct
	{
		ULONG ReqCode = 2;
		ULONG Pid{};
		WCHAR ProcessName[260]{};
	}Req;


	memcpy(Req.ProcessName, std::wstring(ProcessName.begin(), ProcessName.end()).c_str(), ProcessName.size() * sizeof(WCHAR));

	return DeviceIoControl(m_hDevice,
						   IOCTL_KILL_PROCESS,
						   &Req,
						   sizeof(Req),
						   nullptr,
						   0);
}

BOOLEAN Shdrv::KillProcess(const std::wstring& ProcessPath)
{
	struct
	{
		ULONG ReqCode = 3;
		ULONG Pid{};
		WCHAR ProcessPath[260]{};
	}Req;

	memcpy(Req.ProcessPath, ProcessPath.c_str(), ProcessPath.size() * sizeof(WCHAR));

	return DeviceIoControl(m_hDevice,
						   IOCTL_KILL_PROCESS,
						   &Req,
						   sizeof(Req),
						   nullptr,
						   0);
}
