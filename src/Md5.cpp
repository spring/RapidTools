#include "Md5.hpp"

namespace BuildRapid {

Md5T::Md5T()
{
	MD5Init(&mContext);
}

void Md5T::update(void const * Buffer, std::size_t Length)
{
	MD5Update(&mContext, static_cast<md5byte const *>(Buffer), Length);
}

Md5DigestT Md5T::final()
{
	Md5DigestT Digest;
	MD5Final(&mContext, Digest.Buffer);
	return Digest;
}

}
