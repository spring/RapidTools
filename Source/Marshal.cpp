#include "Marshal.hpp"

namespace BuildRapid {
namespace Marshal {

void pack(std::uint32_t Word, unsigned char * Bytes)
{
	Bytes[0] = Word >> 24 & 0xFF;
	Bytes[1] = Word >> 16 & 0xFF;
	Bytes[2] = Word >> 8 & 0xFF;
	Bytes[3] = Word >> 0 & 0xFF;
}

void unpack(std::uint32_t & Word, unsigned char const * Bytes)
{
	Word = 0;
	Word = Bytes[0] << 24 | Word;
	Word = Bytes[1] << 16 | Word;
	Word = Bytes[2] << 8  | Word;
	Word = Bytes[3] << 0  | Word;
}

}
}
