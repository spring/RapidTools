#pragma once

#include "qtmd5.hpp"

namespace BuildRapid {

struct Md5DigestT
{
	unsigned char Buffer[16];
};

class Md5T
{
	private:
	MD5Context mContext;

	public:
	Md5T();
	void update(void const * Buffer, std::size_t Length);
	Md5DigestT final();
};

}
