#pragma once

#include <cstdint>

namespace BuildRapid {
namespace Marshal {

void pack(std::uint32_t Word, unsigned char * Bytes);
void unpack(std::uint32_t & Word, unsigned char const * Bytes);

}
}
