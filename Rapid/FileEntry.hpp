#pragma once

#include "Crc32.hpp"
#include "Md5.hpp"

#include <cstdint>

namespace Rapid {

struct FileEntryT
{
	std::uint32_t Size;
	DigestT Digest;
	ChecksumT Checksum;
};

}
