#pragma once

#include <cstdint>

namespace BuildRapid {

class Crc32T
{
	private:
	std::uint32_t mCrc;

	public:
	Crc32T();

	void update(void const * Buffer, std::size_t Length);
	std::uint32_t final();
};

}
