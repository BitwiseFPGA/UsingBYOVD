#pragma once
#include <windows.h>
#include <string>
#include "DriverProvider.hpp"
#include "DriverService.hpp"
#include "Singleton.hpp"
#include "KtapiBin.hpp"

class Ktapi final :
	public DriverProvider<Ktapi>,
	public Singleton<Ktapi>
{
	friend class Singleton<Ktapi>;

private:
	static constexpr ULONG IOCTL_MAP	= 0x82007000u;
	static constexpr ULONG IOCTL_UNMAP	= 0x82007100u;

public:
	explicit Ktapi(Token) noexcept : Ktapi()
	{
	}
	~Ktapi() = default;

	BOOLEAN InitDriver() noexcept
	{
		return Initialize(KtapiBin::hexData,
						  KtapiBin::hexSize,
						  KtapiBin::service,
						  KtapiBin::serviceLength,
						  KtapiBin::Key);
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
	Ktapi() = default;

private:
	PVOID
		MapPhysicalMemory(
			PVOID	PhysicalAddress,
			SIZE_T	Size);

	VOID
		UnmapPhysicalMemory(PVOID MappedAddress);

	PVOID
		VirtualToPhysical(PVOID VirtualAddress);
};

inline constexpr ObjectProxy<Ktapi> g_Ktapi{};