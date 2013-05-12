#pragma once

#include <functional>
#include <string>

#include <svn_client.h>
#include <svn_compat.h>
#include <svn_path.h>

namespace Rapid {

class AprPoolT
{
	private:
	apr_pool_t * mPool;

	public:
	AprPoolT();
	~AprPoolT();

	apr_pool_t * get();
};

class SvnT
{
	private:
	AprPoolT mPool;
	svn_client_ctx_t * mContext;

	public:
	typedef std::function<void (svn_client_diff_summarize_t const *)> SummarizeCallbackT;
	typedef std::function<void (char const *, apr_size_t)> CatCallbackT;
	typedef std::function<void (svn_log_entry_t const *)> LogCallbackT;

	SvnT();

	void summarize(
		std::string const & SourcePath, svn_revnum_t SourceRevisionNum,
		std::string const & DestinationPath, svn_revnum_t DestinationRevisionNum,
		SummarizeCallbackT Callback);

	void cat(
		std::string const & Path, svn_revnum_t RevisionNum,
		CatCallbackT Callback);

	void log(
		std::string const & Root,
		std::string const & Path, svn_revnum_t RevisionNum,
		LogCallbackT Callback);

	std::string readFile(std::string const & Path, svn_revnum_t RevisionNum);
};

}
