#pragma once

#include <cstdint>
#include <string>

#include <zlib.h>

namespace BuildRapid {

class GzipT
{
	private:
	gzFile mFile;

	public:
	GzipT();
	GzipT(std::string const & Path, char const * Mode);
	~GzipT();

	void open(std::string const & Path, char const * Mode);
	void close();
	bool readMaybe(void * Buffer, unsigned Length);
	void readExpected(void * Buffer, unsigned Length);
	void write(void const * Buffer, unsigned Length);
};

}
