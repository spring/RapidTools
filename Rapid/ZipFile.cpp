#include "ZipFile.hpp"

namespace Rapid {

ZipFileT::ZipFileT(const std::string& Name, zip_file * File)
:
	mName{std::move(Name)},
	mFile{File}
{}

ZipFileT::~ZipFileT()
{
	zip_fclose(mFile);
}

std::string const & ZipFileT::getName() const
{
	return mName;
}

}
