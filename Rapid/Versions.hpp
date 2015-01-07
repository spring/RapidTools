#pragma once

#include "ArchiveEntry.hpp"
#include "Gzip.hpp"
#include "Store.hpp"

#include <map>
#include <string>


namespace Rapid {

class VersionsT
{
	private:
	StoreT & mStore;
	std::map<std::string, ArchiveEntryT> mEntries;

	public:
	VersionsT(StoreT & Store);
	void clear();
	void load();
	void save();
	void add(std::string const & Tag, ArchiveEntryT const & Entry);
	ArchiveEntryT const & findTag(std::string const & Tag);
};

}
