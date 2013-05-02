#pragma once

#include "Md5.hpp"

#include <random>

namespace BuildRapid {

class StoreT
{
	private:
	std::string mRoot;
	std::default_random_engine mEngine;
	std::uniform_int_distribution<unsigned char> mDistribution;

	public:
	StoreT(std::string const & Root);

	void init();
	std::string getTempPath();
	std::string getSdpPath(Md5DigestT const & Digest);
	std::string getPoolPath(Md5DigestT const & Digest);
};

}
