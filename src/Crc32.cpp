#include "Crc32.hpp"

#include <zlib.h>

namespace BuildRapid {

Crc32T::Crc32T()
{
	mCrc = crc32(0, nullptr, 0);
}

void Crc32T::update(void const * Buffer, std::size_t Length)
{
	mCrc = crc32(mCrc, static_cast<Bytef const *>(Buffer), Length);
}

std::uint32_t Crc32T::final()
{
	return mCrc;
}

}
