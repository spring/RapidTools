#pragma once

#include "Gzip.hpp"
#include "Store.hpp"

namespace Rapid {

class TempFileT
{
	private:
	std::string mPath;
	GzipT mOut;

	public:
	TempFileT(StoreT & Store);
	~TempFileT();

	GzipT & getOut();
	void commit(std::string const & Path);
};

}
