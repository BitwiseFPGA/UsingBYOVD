#pragma once
#include <windows.h>
#include <string>
#include "DriverService.hpp"
#include "Singleton.hpp"
#include "ObjectProxy.hpp"
#include "DriverKiller.hpp"
#include "MonProcessEXBin.hpp"

class MonProcessEX final :
	public DriverKiller<MonProcessEX>,
	public Singleton<MonProcessEX>
{
	friend class Singleton<MonProcessEX>;

private:
	static constexpr ULONG IOCTL_KILL_PROCESS = 0x22400Cu;

public:
	explicit MonProcessEX(Token) noexcept : MonProcessEX()
	{
	}
	~MonProcessEX() = default;

	BOOLEAN InitKiller() noexcept
	{
		return Initialize(MonProcessEXBin::hexData,
						  MonProcessEXBin::hexSize,
						  MonProcessEXBin::service,
						  MonProcessEXBin::serviceLength,
						  MonProcessEXBin::Key);
	}


	BOOLEAN
		KillProcess(ULONG Pid);

private:
	MonProcessEX() = default;
};



inline constexpr ObjectProxy<MonProcessEX> g_MonProcessEX{};

