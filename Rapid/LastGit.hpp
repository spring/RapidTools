#pragma once

#include "Optional.hpp"
#include "Store.hpp"

#include <array>

#include <svn_client.h>

namespace Rapid {

struct LastGitT
{
	std::array<std::uint8_t, 20> Hex;
	DigestT Digest;

	static void save(LastGitT const & Last, StoreT & Store, std::string const & Prefix);
	static OptionalT<LastGitT> load(StoreT & Store, std::string const & Prefix);
};

}
