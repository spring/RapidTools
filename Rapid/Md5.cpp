#include "Md5.hpp"

namespace Rapid {

Md5T::Md5T()
{
	apr_md5_init(&mContext);
}

void Md5T::update(void const * Buffer, std::size_t Length)
{
	apr_md5_update(&mContext, Buffer, Length);
}

DigestT Md5T::final()
{
	DigestT Digest;
	apr_md5_final(Digest.Buffer, &mContext);
	return Digest;
}

}
