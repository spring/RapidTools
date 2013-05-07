#pragma once

#include <cstdint>

namespace Rapid {

using ChecksumT = std::uint32_t;

class Crc32T
{
	private:
	std::uint32_t mCrc;

	public:
	Crc32T();

	void update(void const * Buffer, std::size_t Length);
	ChecksumT final();
};

}
