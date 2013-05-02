#include "Store.hpp"

#include "Hex.hpp"

#include <chrono>
#include <iostream>
#include <stdexcept>
#include <string>

#include <sys/stat.h>
#include <sys/types.h>

namespace {

void touchDirectory(std::string const & Path)
{
	struct stat Stats;
	auto Error = stat(Path.c_str(), &Stats);

	if (Error == -1)
	{
		if (errno == ENOENT)
		{
			Error = mkdir(Path.c_str(), 0755);
			if (Error != 0) throw std::runtime_error{"Error creating directory"};
			return;
		}
		else throw std::runtime_error{"Error creating directory"};
	}

	if (!S_ISDIR(Stats.st_mode)) throw std::runtime_error{"Error creating directory: is a file"};
}

}

namespace BuildRapid {

StoreT::StoreT(std::string const & Root)
:
	mRoot{Root}
{}

void StoreT::init()
{
	auto Now = std::chrono::high_resolution_clock::now();
	auto Ns = std::chrono::duration_cast<std::chrono::nanoseconds>(Now.time_since_epoch());
	mEngine.seed(Ns.count());

	touchDirectory(mRoot);
	touchDirectory(mRoot + "/temp");
	touchDirectory(mRoot + "/pool");
	touchDirectory(mRoot + "/packages");
	touchDirectory(mRoot + "/last");
	touchDirectory(mRoot + "/deps");
	touchDirectory(mRoot + "/builds");
	touchDirectory(mRoot + "/log");

	for (std::size_t I = 0; I < 16; ++I)
	{
		for (std::size_t J = 0; J < 16; ++J)
		{
			std::string Path;
			Path += mRoot;
			Path += "/pool/";
			Path += Hex::EncodeTable[I];
			Path += Hex::EncodeTable[J];
			touchDirectory(Path);
		}
	}
}

std::string StoreT::getTempPath()
{
	unsigned char Random[8];
	for (std::size_t Index = 0; Index < 8; ++Index) Random[Index] = mDistribution(mEngine);
	char Hexed[16];
	Hex::encode(Hexed, Random, 8);

	std::string Path;
	Path += mRoot;
	Path += "/temp/";
	Path.append(Hexed, Hexed + 16);
	return Path;
}

std::string StoreT::getSdpPath(Md5DigestT const & Digest)
{
	char Hexed[32];
	Hex::encode(Hexed, Digest.Buffer, 16);

	std::string Path;
	Path += mRoot;
	Path += "/packages/";
	Path.append(Hexed, Hexed + 32);
	Path += ".sdp";

	return Path;
}

std::string StoreT::getPoolPath(Md5DigestT const & Digest)
{
	char Hexed[32];
	Hex::encode(Hexed, Digest.Buffer, 16);

	std::string Path;
	Path += mRoot;
	Path += "/pool/";
	Path.append(Hexed, Hexed + 2);
	Path += '/';
	Path.append(Hexed + 2, Hexed + 32);
	Path += ".gz";

	return Path;
}

}
