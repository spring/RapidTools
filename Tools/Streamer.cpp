#include "Rapid/BitArray.hpp"
#include "Rapid/Hex.hpp"
#include "Rapid/Marshal.hpp"
#include "Rapid/PoolArchive.hpp"
#include "Rapid/Store.hpp"

#include <fstream>
#include <iostream>
#include <string>
#include <stdexcept>
#include <cstdio>

#include <sys/types.h>
#include <sys/stat.h>
#include <zlib.h>

namespace {

using namespace Rapid;

struct StreamEntryT
{
	FileEntryT File;
	std::size_t Size;
};

void stream(std::string StorePath, std::string Hexed)
{
	// Read bit array
	auto File = gzdopen(fileno(stdin), "rb");
	BitArrayT Bits;
	char Buffer[4096];

	while (true)
	{
		auto Bytes = gzread(File, Buffer, 4096);
		if (Bytes < 0) throw std::runtime_error{"Error reading bit array"};
		if (Bytes == 0) break;
		Bits.append(Buffer, Bytes);
	}

	gzclose(File);

	// Load archive
	if (Hexed.size() != 32) throw std::runtime_error{"Hex must be 32 bytes"};
	DigestT Digest;
	Hex::decode(Hexed.data(), Digest.Buffer, 16);
	StoreT Store{StorePath};
	PoolArchiveT Archive{Store};
	Archive.load(Digest);

	// Accumulate marked files
	std::vector<StreamEntryT> Entries;
	std::size_t TotalSize = 0;

	Archive.iterate(Bits, [&](FileEntryT const & Entry)
	{
		auto Path = Store.getPoolPath(Entry.Digest);
		struct stat Stats;
		auto Error = stat(Path.c_str(), &Stats);
		if (Error == -1) throw std::runtime_error{"Error reading pool file"};

		Entries.push_back({Entry, static_cast<std::uint32_t>(Stats.st_size)});
		TotalSize += Stats.st_size;
		TotalSize += 4;
	});

	// Repond to request
	std::cout << "Content-Type: application/octet-stream\n";
	std::cout << "Content-Transfer-Encoding: binary\n";
	std::cout << "Content-Length: " << TotalSize << '\n';
	std::cout << '\n';

	for (auto & Entry: Entries)
	{
		auto Path = Store.getPoolPath(Entry.File.Digest);

		std::ifstream In{Path};
		std::uint8_t Bytes[4];
		Marshal::packLittle(Entry.Size, Bytes);
		std::cout.write(reinterpret_cast<char *>(Bytes), 4);

		while (true)
		{
			In.read(Buffer, 4096);
			std::cout.write(Buffer, In.gcount());
			if (!In) break;
		}
	}
}

}

int main(int argc, char const * const * argv, char const * const * env)
{
	apr_app_initialize(&argc, &argv, &env);
	atexit(apr_terminate);
	umask(0002);

	auto DocumentRoot = getenv("DOCUMENT_ROOT");
	auto QueryString = getenv("QUERY_STRING");

	if (
		argc != 1 ||
		DocumentRoot == nullptr ||
		QueryString == nullptr)
	{
		std::cerr << "Usage: " << argv[0] << "\n";
		std::cerr << "Expects environment variables DOCUMENT_ROOT and QUERY_STRING\n";
		std::cerr << "Expects a gziped bitarray as stdin\n";
		return 1;
	}

	try
	{
		stream(DocumentRoot, QueryString);
	}
	catch (std::exception const & Exception)
	{
		std::cerr << Exception.what() << "\n";
		return 1;
	}
}
