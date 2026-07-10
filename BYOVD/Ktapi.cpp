#include "Ktapi.h"
#include "FileUtils.hpp"
#include "VA2PA.h"
#include "Log.hpp"

BOOLEAN
Ktapi::KernelRead(
	PVOID	VirtualAddress,
	PVOID	ReadBuffer,
	SIZE_T	ReadSize)
{
	if (INVALID_HANDLE_VALUE == m_hDevice)
	{
		LOG("[-] Ktapi device handle is invalid. Ensure the driver is loaded and the device is accessible.");
		return FALSE;
	}

	if (!VirtualAddress || !ReadBuffer || !ReadSize || !m_bInitialized)
	{
		LOG("[-] Invalid parameters for KernelRead.");
		return FALSE;
	}

	if (ReadSize <= 0x1000)
	{
		PVOID pPhysicalAddress = this->VirtualToPhysical(VirtualAddress);
		if (!pPhysicalAddress)
		{
			LOG("[-] Failed to translate virtual address to physical address using MemoryMap.");
			return FALSE;
		}

		auto pMappedAddress = MapPhysicalMemory(pPhysicalAddress, ReadSize);
		if (!pMappedAddress)
		{
			LOG("[-] Failed to map physical memory. Error code: %lu" << std::hex << GetLastError() << std::dec);
			return FALSE;
		}

		RtlCopyMemory(ReadBuffer, pMappedAddress, ReadSize);
		UnmapPhysicalMemory(pMappedAddress);
	}
	else
	{
		// ReadSize > 0x1000, need to read page by page
		ULONG NumberOfPages = static_cast<ULONG>((ReadSize + 0xFFF) / 0x1000);
		for (auto i{ 0u }; i < NumberOfPages; ++i)
		{
			PVOID pPhysicalAddress = VirtualToPhysical(reinterpret_cast<PUCHAR>(VirtualAddress) + i * 0x1000);
			if (!pPhysicalAddress)
			{
				LOG("[-] Failed to translate virtual address to physical address using MemoryMap.");
				return FALSE;
			}

			if (i != NumberOfPages - 1)
			{
				auto pMappedAddress = MapPhysicalMemory(pPhysicalAddress, 0x1000);
				if (!pMappedAddress)
				{
					LOG("[-] Failed to map physical memory. Error code: %lu" << std::hex << GetLastError() << std::dec);
					return FALSE;
				}

				RtlCopyMemory(reinterpret_cast<PUCHAR>(ReadBuffer) + i * 0x1000, pMappedAddress, 0x1000);
				UnmapPhysicalMemory(pMappedAddress);
			}
			else
			{
				// Last page, calculate the remaining size
				ULONG RemainingSize = static_cast<ULONG>(ReadSize - i * 0x1000);

				auto pMappedAddress = MapPhysicalMemory(pPhysicalAddress, RemainingSize);
				if (!pMappedAddress)
				{
					LOG("[-] Failed to map physical memory. Error code: %lu" << std::hex << GetLastError() << std::dec);
					return FALSE;
				}

				RtlCopyMemory(reinterpret_cast<PUCHAR>(ReadBuffer) + i * 0x1000, pMappedAddress, RemainingSize);
				UnmapPhysicalMemory(pMappedAddress);
			}
		}


	}

	return TRUE;
}

BOOLEAN
Ktapi::KernelWrite(
	PVOID VirtualAddress,
	PVOID WriteBuffer,
	SIZE_T WriteSize)
{
	if (!VirtualAddress || !WriteBuffer || !WriteSize || !m_bInitialized)
	{
		LOG("[-] Invalid parameters for KernelWrite.");
		return FALSE;
	}

	if (WriteSize <= 0x1000)
	{
		PVOID pPhysicalAddress = this->VirtualToPhysical(VirtualAddress);
		if (!pPhysicalAddress)
		{
			LOG("[-] Failed to translate virtual address to physical address using MemoryMap.");
			return FALSE;
		}

		auto pMappedAddress = MapPhysicalMemory(pPhysicalAddress, WriteSize);
		if (!pMappedAddress)
		{
			LOG("[-] Failed to map physical memory.");
			return FALSE;
		}

		RtlCopyMemory(pMappedAddress, WriteBuffer, WriteSize);
		UnmapPhysicalMemory(pMappedAddress);
	}
	else
	{
		// WriteSize > 0x1000
		ULONG NumberOfPages = static_cast<ULONG>((WriteSize + 0xFFF) / 0x1000);
		for (auto i{ 0u }; i < NumberOfPages; ++i)
		{
			PVOID pPhysicalAddress = VirtualToPhysical(reinterpret_cast<PUCHAR>(VirtualAddress) + i * 0x1000);
			if (!pPhysicalAddress)
			{
				LOG("[-] Failed to translate virtual address to physical address using MemoryMap.");
				return FALSE;
			}
			if (i != NumberOfPages - 1)
			{
				auto pMappedAddress = MapPhysicalMemory(pPhysicalAddress, 0x1000);
				if (!pMappedAddress)
				{
					LOG("[-] Failed to map physical memory.");
					return FALSE;
				}

				RtlCopyMemory(pMappedAddress, reinterpret_cast<PUCHAR>(WriteBuffer) + i * 0x1000, 0x1000);
				UnmapPhysicalMemory(pMappedAddress);
			}
			else
			{
				// Last page, calculate the remaining size
				ULONG RemainingSize = static_cast<ULONG>(WriteSize - i * 0x1000);
				auto pMappedAddress = MapPhysicalMemory(pPhysicalAddress, RemainingSize);
				if (!pMappedAddress)
				{
					LOG("[-] Failed to map physical memory.");
					return FALSE;
				}

				RtlCopyMemory(pMappedAddress, reinterpret_cast<PUCHAR>(WriteBuffer) + i * 0x1000, RemainingSize);
				UnmapPhysicalMemory(pMappedAddress);
			}
		}
	}

	return TRUE;
}


PVOID
Ktapi::MapPhysicalMemory(
	PVOID	PhysicalAddress,
	SIZE_T	Size)
{
	struct
	{
		ULONG InterfaceType;          // Offset 0x00 (4 字节) - INTERFACE_TYPE
		ULONG BusNumber;              // Offset 0x04 (4 字节) - 
		PVOID PhysicalAddress;		  // Offset 0x08 (8 字节) - Map PhysicalAddress
		ULONG AddressSpace;           // Offset 0x10 (4 字节) - 0
		ULONG Length;                 // Offset 0x14 (4 字节) - Map Size
	} Request;

	Request.InterfaceType	= 0;
	Request.BusNumber		= 0;
	Request.AddressSpace	= 0;
	Request.PhysicalAddress = PhysicalAddress;
	Request.Length = static_cast<ULONG>(Size);
	

	PVOID pMappedAddress	= nullptr;
	DWORD dwBytesReturned	= 0;

	if (DeviceIoControl(m_hDevice,
						IOCTL_MAP,
						&Request,
						sizeof(Request),
						&pMappedAddress,
						sizeof(pMappedAddress)))
	{
		return pMappedAddress;
	}
	return nullptr;
}

VOID
Ktapi::UnmapPhysicalMemory(
	PVOID MappedAddress)
{
	if (!MappedAddress)
	{
		return;
	}

	DeviceIoControl(m_hDevice, IOCTL_UNMAP, &MappedAddress, sizeof(MappedAddress), nullptr, 0);
}

PVOID
Ktapi::VirtualToPhysical(PVOID VirtualAddress)
{
	if (!VirtualAddress)
	{
		LOG("[-] Invalid virtual address provided to VirtualToPhysical.");
		return nullptr;
	}

	return Va2Pa(VirtualAddress);
}