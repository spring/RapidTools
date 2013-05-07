#pragma once

#include <cstdlib>

#include <apr_md5.h>

namespace Rapid {

struct DigestT
{
	unsigned char Buffer[16];
};

class Md5T
{
	private:
	apr_md5_ctx_t mContext;

	public:
	Md5T();
	void update(void const * Buffer, std::size_t Length);
	DigestT final();
};

}
