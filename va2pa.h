#pragma once
#include "SuperFetch.h"

inline PVOID Va2Pa(PVOID VirtualAddress);

inline PVOID Va2Pa(PVOID VirtualAddress)
{
	static spf::memory_map g_cached_map;
	static bool g_initialized = false;

	if (!g_initialized)
	{
		auto res = spf::memory_map::current();
		if (res.has_value())
		{
			g_cached_map = std::move(res.value());
			g_initialized = true;
		}
		else
		{
			return nullptr;
		}
	}

	std::uint64_t pa = g_cached_map.translate(VirtualAddress);
	if (pa != 0)
	{
		return reinterpret_cast<PVOID>(pa);
	}

	auto res = spf::memory_map::current();
	if (res.has_value())
	{
		g_cached_map = std::move(res.value());

		pa = g_cached_map.translate(VirtualAddress);
		return reinterpret_cast<PVOID>(pa);
	}

	return nullptr;
}
