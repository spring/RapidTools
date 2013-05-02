#include "Hex.hpp"
#include "PoolArchive.hpp"
#include "PoolFile.hpp"
#include "Store.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

using namespace BuildRapid;

int main()
{
	try
	{
		Md5DigestT Digest;
		Hex::decode("6fb305594ab58b0cdb56f3f0e9f3cb7a", Digest.Buffer, 16);
		StoreT Store{"/home/chris/storeplay"};
		Store.init();
		PoolArchiveT Archive{Store, Digest};
		PoolFileT File{Store};
		File.write("234234", 7);
		auto Entry = File.close();
		Archive.add("models/blah.obj", Entry);
		Archive.save();
	}
	catch (std::exception & Exception)
	{
		std::cout << "Fatal error: " << Exception.what() << "\n";
	}

	return 0;
}
