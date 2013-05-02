#pragma once

#include "Md5.hpp"

#include <cstdint>

namespace BuildRapid {

struct PoolEntryT
{
	std::uint32_t Size;
	Md5DigestT Digest;
	std::uint32_t Checksum;
};

}
