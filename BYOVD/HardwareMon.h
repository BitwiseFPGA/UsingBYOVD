#pragma once
#include <windows.h>
#include <string>
#include "DriverProvider.hpp"
#include "DriverService.hpp"
#include "Singleton.hpp"
#include "ObjectProxy.hpp"
#include "HardwareMonBin.hpp"

class HardwareMon final :
	public DriverProvider<HardwareMon>,
	public Singleton<HardwareMon>
{
	friend class Singleton<HardwareMon>;
private:
	static constexpr ULONG IOCTL_READ_PHYSICAL  = 0x9C40E184u;
	static constexpr ULONG IOCTL_WRITE_PHYSICAL = 0x9C40E188u;

public:
	explicit HardwareMon(Token) noexcept : HardwareMon()
	{
	}
	~HardwareMon() = default;

	BOOLEAN InitDriver() noexcept
	{
		return Initialize(HardwareMonBin::hexData,
						  HardwareMonBin::hexSize,
						  HardwareMonBin::service,
						  HardwareMonBin::serviceSize,
						  HardwareMonBin::Key);
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
	HardwareMon() = default;

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

inline constexpr ObjectProxy<HardwareMon> g_HardwareMon{};