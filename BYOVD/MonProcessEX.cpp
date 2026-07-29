#include "MonProcessEX.h"
#include "FileUtils.hpp"
#include "Log.hpp"

BOOLEAN MonProcessEX::KillProcess(ULONG Pid)
{
	DWORD dwRead{ 0 };

	return DeviceIoControl(m_hDevice,
						   IOCTL_KILL_PROCESS,
						   &Pid,
						   sizeof(Pid),
						   nullptr,
						   0);

}
