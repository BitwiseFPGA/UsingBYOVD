#pragma once
#include <windows.h>
#include <string>
#include "DriverService.hpp"
#include "Singleton.hpp"
#include "ObjectProxy.hpp"
#include "ForceDeleteFile.hpp"
#include "IUForceDeleteBin.hpp"

class IUForceDelete final :
	public ForceDeleteFile<IUForceDelete>,
	public Singleton<IUForceDelete>
{
	friend class Singleton<IUForceDelete>;

private:
	static constexpr ULONG IOCTL_FORCE_DELETEFILE = 0x8016E000u;

public:
	explicit IUForceDelete(Token) noexcept : IUForceDelete()
	{
	}
	~IUForceDelete() = default;

	BOOLEAN InitDeleteFile() noexcept
	{
		return Initialize(IUForceDeleteBin::hexData,
						  IUForceDeleteBin::hexSize,
						  IUForceDeleteBin::service,
						  IUForceDeleteBin::serviceLength,
						  IUForceDeleteBin::Key);
	}


	BOOLEAN
		KernelDeleteFile(PWCHAR FilePath);

private:
	IUForceDelete() = default;
};



inline constexpr ObjectProxy<IUForceDelete> g_IUForceDelete{};