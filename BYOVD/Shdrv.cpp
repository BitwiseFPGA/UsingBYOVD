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