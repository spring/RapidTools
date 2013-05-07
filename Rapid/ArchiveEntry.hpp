#pragma once

#include "Md5.hpp"

#include <string>
#include <vector>

namespace Rapid {

struct ArchiveEntryT
{
	DigestT Digest;
	std::vector<std::string> Depends;
	std::string Name;
};

}
