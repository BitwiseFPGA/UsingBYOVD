#pragma once
#include <windows.h>
#include <string>
#include "DriverProvider.hpp"
#include "DriverService.hpp"
#include "Singleton.hpp"
#include "ObjectProxy.hpp"
#include "HP_WKS_SWTOOLS_DRIVER_Bin.hpp"

class HP_WKS_SWTOOLS_DRIVER final :
	public DriverProvider<HP_WKS_SWTOOLS_DRIVER>,
	public Singleton<HP_WKS_SWTOOLS_DRIVER>
{
	friend class Singleton<HP_WKS_SWTOOLS_DRIVER>;
private:
	/*static constexpr ULONG IOCTL_READ_PHYSICAL  = 0x9C40610Cu;
	static constexpr ULONG IOCTL_WRITE_PHYSICAL = 0x9C40A110u;*/
	static constexpr ULONG IOCTL_MAP_PHYSICAL   = 0x9C406104u;
	static constexpr ULONG IOCTL_UNMAP_PHYSICAL = 0x9C40A108u;

public:
	explicit HP_WKS_SWTOOLS_DRIVER(Token) noexcept : HP_WKS_SWTOOLS_DRIVER()
	{
	}
	~HP_WKS_SWTOOLS_DRIVER() = default;

	BOOLEAN InitDriver() noexcept
	{
		return Initialize(HP_WKS_SWTOOLS_DRIVER_Bin::hexData,
						  HP_WKS_SWTOOLS_DRIVER_Bin::hexSize,
						  HP_WKS_SWTOOLS_DRIVER_Bin::service,
						  HP_WKS_SWTOOLS_DRIVER_Bin::serviceSize,
						  HP_WKS_SWTOOLS_DRIVER_Bin::Key);
	}

	BOOLEAN
		KernelRead(PVOID	VirtualAddress,
				   PVOID	ReadBuffer,
				   SIZE_T	ReadSize);
	BOOLEAN
		KernelWrite(PVOID	VirtualAddress,
					PVOID	WriteBuffer,
					SIZE_T	WriteSize);

private:
	HP_WKS_SWTOOLS_DRIVER() = default;

private:
	BOOLEAN
		ReadPhysicalMemory(
			PVOID	PhysicalAddress,
			SIZE_T	Size,
			PVOID	ReadBuffer);

	BOOLEAN
		WritePhysicalMemory(
			PVOID	PhysicalAddress,
			SIZE_T	Size,
			PVOID	WriteBuffe);

	PVOID
		VirtualToPhysical(PVOID VirtualAddress);

};

inline constexpr ObjectProxy<HP_WKS_SWTOOLS_DRIVER> g_HP_WKS_SWTOOLS_DRIVER{};