#include "Rapid/PoolArchive.hpp"
#include "Rapid/PoolFile.hpp"
#include "Rapid/Store.hpp"
#include "Rapid/Versions.hpp"
#include "Rapid/Zip.hpp"
#include "Rapid/ZipFile.hpp"

#include <iostream>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>

namespace {

using namespace Rapid;

void addZip(
	std::string StorePath,
	std::string ZipPath,
	char const * const * Tags, std::size_t NumTags)
{
	StoreT Store{StorePath};
	Store.init();

	PoolArchiveT Archive{Store};
	ZipT Zip{ZipPath, 0};

	// Add all files
	Zip.iterateFiles([&](ZipFileT const & File)
	{
		PoolFileT PoolFile{Store};
		File.cat([&](char const * Buffer, zip_uint64_t Length)
		{
			PoolFile.write(Buffer, Length);
		});
		auto Entry = PoolFile.close();
		Archive.add(File.getName(), Entry);
	});

	// Save archive and tag it
	VersionsT Versions{Store};
	Versions.load();
	auto Entry = Archive.save();
	for (std::size_t I = 0; I != NumTags; ++I) Versions.add(Tags[I], Entry);
	Versions.save();
}

}

int main(int argc, char const * const * argv, char const * const * env)
{
	apr_app_initialize(&argc, &argv, &env);
	atexit(apr_terminate);
	umask(0002);

	if (argc < 3)
	{
		std::cerr << "Usage: " << argv[0] << " <Store Path> <Zip Path> [Tag1] ...\n";
		return 1;
	}

	try
	{
		addZip(argv[1], argv[2], argv + 3, argc - 3);
	}
	catch (std::exception const & Exception)
	{
		std::cerr << Exception.what() << "\n";
		return 1;
	}
}
