#include "PoolArchive.hpp"

#include "Marshal.hpp"
#include "Gzip.hpp"
#include "TempFile.hpp"

#include <algorithm>
#include <iostream>
#include <stdexcept>


namespace BuildRapid {

PoolArchiveT::PoolArchiveT(StoreT & Store)
:
	mStore(Store)
{}

PoolArchiveT::PoolArchiveT(StoreT & Store, Md5DigestT const & Digest)
:
	mStore(Store)
{
	GzipT In{Store.getSdpPath(Digest), "rb"};
	unsigned char Length;
	char Name[255];
	unsigned char Checksum[4];
	unsigned char Size[4];

	while (In.readMaybe(&Length, 1))
	{
		std::pair<std::string, PoolEntryT> Pair;
		In.readExpected(Name, Length);
		In.readExpected(Pair.second.Digest.Buffer, 16);
		In.readExpected(Checksum, 4);
		In.readExpected(Size, 4);
		Pair.first = {Name, Length};
		Marshal::unpack(Pair.second.Checksum, Checksum);
		Marshal::unpack(Pair.second.Size, Size);
		mEntries.insert(Pair);
	}
}

void PoolArchiveT::save()
{
	TempFileT TempFile{mStore};
	auto & Out = TempFile.getOut();

	unsigned char Checksum[4];
	unsigned char Size[4];

	for (auto & Pair : mEntries)
	{
		Marshal::pack(Pair.second.Checksum, Checksum);
		Marshal::pack(Pair.second.Size, Size);
		unsigned char Length = Pair.first.size();
		Out.write(&Length, 1);
		Out.write(Pair.first.data(), Length);
		Out.write(Pair.second.Digest.Buffer, 16);
		Out.write(Checksum, 4);
		Out.write(Size, 4);
	}

	TempFile.commit(mStore.getSdpPath(getDigest()));
}

void PoolArchiveT::add(std::string const & Name, PoolEntryT const & Entry)
{
	mEntries.insert({Name, Entry});
}

void PoolArchiveT::remove(std::string const & Name)
{
	auto Iter = mEntries.find(Name);
	auto End = mEntries.end();

	if (Iter == End) throw std::runtime_error{"Attempt to remove non-existent key"};
	mEntries.erase(Iter);
}

void PoolArchiveT::removePrefix(std::string const & Prefix)
{
	auto Iter = mEntries.upper_bound(Prefix);
	auto End = mEntries.end();
	auto PrefixBegin = Prefix.begin();
	auto PrefixEnd = Prefix.end();
	auto PrefixSize = Prefix.size();

	if (Iter == End) return;
	while (true)
	{
		Iter = mEntries.erase(Iter);
		if (Iter == End) break;
		auto NameBegin = Iter->first.begin();
		auto NameSize = Iter->first.size();
		if (NameSize < PrefixSize) break;
		if (!std::equal(PrefixBegin, PrefixEnd, NameBegin)) break;
	}
}

Md5DigestT PoolArchiveT::getDigest()
{
	Md5T Md5;

	for (auto & Pair : mEntries)
	{
		Md5T NameMd5;
		NameMd5.update(Pair.first.data(), Pair.first.size());
		auto NameDigest = NameMd5.final();
		Md5.update(NameDigest.Buffer, 16);
		Md5.update(Pair.second.Digest.Buffer, 16);
	}

	return Md5.final();
}

std::uint32_t PoolArchiveT::getChecksum()
{
	Crc32T Crc32;

	for (auto & Pair : mEntries)
	{
		Crc32T NameCrc32;
		NameCrc32.update(Pair.first.data(), Pair.first.size());
		auto NameChecksum = NameCrc32.final();
		Crc32.update(&NameChecksum, 4);
		Crc32.update(&Pair.second.Checksum, 4);
	}

	return Crc32.final();
}


}
