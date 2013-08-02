#include "Store.hpp"

#include "Hex.hpp"

#include <chrono>
#include <stdexcept>
#include <string>

#include <sys/stat.h>
#include <sys/types.h>

namespace Rapid {

StoreT::StoreT(std::string const & Root)
:
	mRoot{Root}
{}

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
			if (Error != 0) throw std::runtime_error{"Error creating directory: " + Path};
			return;
		}
		else throw std::runtime_error{"Error creating directory: " + Path};
	}

	if (!S_ISDIR(Stats.st_mode)) throw std::runtime_error{"Error creating directory: is a file:" + Path};
}

}

// This only needs to be called if the library user want to do writing to the pool
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
	touchDirectory(mRoot + "/builds");

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

std::string StoreT::getSdpPath(DigestT const & Digest) const
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

std::string StoreT::getPoolPath(DigestT const & Digest) const
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

std::string StoreT::getVersionsPath() const
{
	std::string Path;
	Path += mRoot;
	Path += "/versions.gz";
	return Path;
}

std::string StoreT::getLastPath(std::string const & Prefix) const
{
	std::string Path;
	Path += mRoot;
	Path += "/last/";
	Path += Prefix;
	Path += ".gz";
	return Path;
}

std::string StoreT::getBuildPath(std::string const & Prefix, std::string Version)
{
	// Replace spaces with underscores
	for (auto & Char : Version)
	{
		if (Char == ' ') Char = '_';
	}

	std::string Path;
	Path += mRoot;
	Path += "/builds/";
	Path += Prefix;
	Path += '-';
	Path += Version;
	Path += ".sdz";
	return Path;
}

std::string StoreT::getDigestPath() const {
	std::string Path;
	Path += mRoot;
	Path += "versions.digest";
	return Path;
}

}
