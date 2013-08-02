#pragma once

#include "Store.hpp"

#include <svn_client.h>

namespace Rapid {

struct LastT
{
	std::uint32_t RevisionNum;
	DigestT Digest;

	static void save(LastT const & Last, StoreT & Store, std::string const & Prefix);
	static LastT load(StoreT & Store, std::string const & Prefix);
};

}
