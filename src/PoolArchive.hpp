#pragma once

#include "Crc32.hpp"
#include "Md5.hpp"
#include "PoolEntry.hpp"
#include "Store.hpp"

#include <map>
#include <string>
#include <vector>

namespace BuildRapid {

class PoolArchiveT
{
	private:
	StoreT & mStore;
	std::map<std::string, PoolEntryT> mEntries;
	std::string mModName;
	std::vector<std::string> mDepends;

	public:
	PoolArchiveT(StoreT & Store);
	PoolArchiveT(StoreT & Store, Md5DigestT const & Digest);

	void add(std::string const & Name, PoolEntryT const & Entry);
	void remove(std::string const & Name);
	void removePrefix(std::string const & Prefix);
	void save();
	Md5DigestT getDigest();
	std::uint32_t getChecksum();
};



}
