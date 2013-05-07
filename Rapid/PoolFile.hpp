#pragma once

#include "Gzip.hpp"
#include "Crc32.hpp"
#include "Md5.hpp"
#include "FileEntry.hpp"
#include "Store.hpp"
#include "TempFile.hpp"

#include <string>

namespace Rapid {

class PoolFileT
{
	private:
	StoreT & mStore;
	TempFileT mTempFile;
	Md5T mMd5;
	Crc32T mCrc;
	std::uint32_t mSize;

	public:
	PoolFileT(StoreT & Store);
	void write(void const * Buffer, unsigned Length);
	FileEntryT close();
};

}
