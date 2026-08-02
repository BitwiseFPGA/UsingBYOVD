#pragma once
#include <windows.h>
#include <any>

// RX Driver
#include "CorMem.h"
#include "PGRHostControl.h"
#include "BiosToolCommonDriver.h"
#include "WinMsrDev.h"
#include "MyPortIODev.h"
#include "HardwareMon.h"
#include "Ktapi.h"
#include "HP_WKS_SWTOOLS_DRIVER.h"


// Killer
#include "BootRepair.h"
#include "ProcessCtr.h"
#include "GGProtect64.h"
#include "Ardrv.h"
#include "HWAudioX64.h"
#include "MonProcessEX.h"

namespace KillerSelector
{
	enum class KillerType
	{
		BootRepair,
		ProcessCtr,
		GGProtect64,
		Ardrv,
		HWAudioX64,
		MonProcessEX
	};
	static std::any Killers[] = {
		std::any(std::addressof(BootRepair::Instance())),
		std::any(std::addressof(ProcessCtr::Instance())),
		std::any(std::addressof(GGProtect64::Instance())),
		std::any(std::addressof(Ardrv::Instance())),
		std::any(std::addressof(HWAudioX64::Instance())),
		std::any(std::addressof(MonProcessEX::Instance()))
	};
	template <KillerType _Type>
	struct GetKillerImpl
	{
	};

	template <>
	struct GetKillerImpl<KillerType::BootRepair>
	{
		using Type = std::add_pointer_t<BootRepair>;
	};

	template <>
	struct GetKillerImpl<KillerType::ProcessCtr>
	{
		using Type = std::add_pointer_t<ProcessCtr>;
	};

	template <>
	struct GetKillerImpl<KillerType::GGProtect64>
	{
		using Type = std::add_pointer_t<GGProtect64>;
	};
	
	template <>
	struct GetKillerImpl<KillerType::Ardrv>
	{
		using Type = std::add_pointer_t<Ardrv>;
	};

	template <>
	struct GetKillerImpl<KillerType::HWAudioX64>
	{
		using Type = std::add_pointer_t<HWAudioX64>;
	};

	template <>
	struct GetKillerImpl<KillerType::MonProcessEX>
	{
		using Type = std::add_pointer_t<MonProcessEX>;
	};

	template <KillerType _Type>
	auto GetKiller()
	{
		return std::any_cast<GetKillerImpl<_Type>::Type>(Killers[static_cast<int>(_Type)]);
	}
}

using KillerType = KillerSelector::KillerType;
using KillerSelector::GetKiller;

#define		_KILL_PROVIDER KillerType::MonProcessEX
#define		CurrentKiller() GetKiller<_KILL_PROVIDER>()


namespace DriverWorker
{
	enum class ProviderType
	{
		CorMem,
		PGRHostControl,
		BiosToolCommonDriver,
		WinMsrDev,
		MyPortIODev,
		HardwareMon,
		Ktapi,
		HP_WKS_SWTOOLS_DRIVER
	};

	static std::any Providers[] = {
		std::any(std::addressof(CorMem::Instance())),
		std::any(std::addressof(PGRHostControl::Instance())),
		std::any(std::addressof(BiosToolCommonDriver::Instance())),
		std::any(std::addressof(WinMsrDev::Instance())),
		std::any(std::addressof(MyPortIODev::Instance())),
		std::any(std::addressof(HardwareMon::Instance())),
		std::any(std::addressof(Ktapi::Instance())),
		std::any(std::addressof(HP_WKS_SWTOOLS_DRIVER::Instance()))
	};

	template <ProviderType _Type>
	struct GetProviderImpl {};

	template <>
	struct GetProviderImpl<ProviderType::CorMem>
	{
		using Type = std::add_pointer_t<CorMem>;
	};

	template <>
	struct GetProviderImpl<ProviderType::PGRHostControl>
	{
		using Type = std::add_pointer_t<PGRHostControl>;
	};

	template <>
	struct GetProviderImpl<ProviderType::BiosToolCommonDriver>
	{
		using Type = std::add_pointer_t<BiosToolCommonDriver>;
	};

	template <>
	struct GetProviderImpl<ProviderType::WinMsrDev>
	{
		using Type = std::add_pointer_t<WinMsrDev>;
	};

	template <>
	struct GetProviderImpl<ProviderType::MyPortIODev>
	{
		using Type = std::add_pointer_t<MyPortIODev>;
	};

	template <>
	struct GetProviderImpl<ProviderType::HardwareMon>
	{
		using Type = std::add_pointer_t<HardwareMon>;
	};

	template <>
	struct GetProviderImpl<ProviderType::Ktapi>
	{
		using Type = std::add_pointer_t<Ktapi>;
	};

	template <>
	struct GetProviderImpl<ProviderType::HP_WKS_SWTOOLS_DRIVER>
	{
		using Type = std::add_pointer_t<HP_WKS_SWTOOLS_DRIVER>;
	};

	template <ProviderType _Type>
	auto GetProvider()
	{
		return std::any_cast<GetProviderImpl<_Type>::Type>(Providers[static_cast<int>(_Type)]);
	}
}

using ProviderType = DriverWorker::ProviderType;

using DriverWorker::GetProvider;

// Change this to switch to different provider
// DONT'T using MyPortIODev to mapping driver, it too slowly
// BiosToolCommonDriver so fast
#define _USE_PROVIDER ProviderType::HP_WKS_SWTOOLS_DRIVER

#define CurrentProvider() GetProvider<_USE_PROVIDER>()