#include "Svn.hpp"

#include <stdexcept>

namespace Rapid {

AprPoolT::AprPoolT()
{
	apr_pool_create(&mPool, nullptr);
}

AprPoolT::~AprPoolT()
{
	apr_pool_destroy(mPool);
}

apr_pool_t * AprPoolT::get()
{
	return mPool;
}

namespace {

template<typename T>
T * allocPool(apr_pool_t * Pool)
{
	return static_cast<T *>(apr_pcalloc(Pool, sizeof(T)));
}

}

SvnT::SvnT()
{
	svn_auth_baton_t * AuthBaton;
	apr_hash_t * Config;

	svn_client_create_context(&mContext, mPool.get());
	svn_config_get_config(&Config, nullptr, mPool.get());
	mContext->config = Config;

	auto Provider = allocPool<svn_auth_provider_object_t>(mPool.get());
	auto Providers = apr_array_make(mPool.get(), 1, sizeof(svn_auth_provider_object_t));
	svn_auth_get_username_provider(&Provider, mPool.get());
	APR_ARRAY_PUSH(Providers, svn_auth_provider_object_t *) = Provider;

	svn_auth_open(&AuthBaton, Providers, mPool.get());
	mContext->auth_baton = AuthBaton;
}

namespace {

svn_error_t * callSummarize(
	svn_client_diff_summarize_t const * Diff,
	void * Baton,
	apr_pool_t *)
{
	auto & Callback = *static_cast<SvnT::SummarizeCallbackT *>(Baton);
	Callback(Diff);
	return nullptr;
}

}

void SvnT::summarize(
	std::string const & SourcePath, svn_revnum_t SourceRevisionNum,
	std::string const & DestinationPath, svn_revnum_t DestinationRevisionNum,
	SummarizeCallbackT Callback)
{
	AprPoolT Pool;

	auto SourceRevision = allocPool<svn_opt_revision_t>(Pool.get());
	auto DestRevision = allocPool<svn_opt_revision_t>(Pool.get());
	SourceRevision->kind = svn_opt_revision_number;
	DestRevision->kind = svn_opt_revision_number;
	SourceRevision->value.number = SourceRevisionNum;
	DestRevision->value.number = DestinationRevisionNum;

	auto Error = svn_client_diff_summarize2(
		SourcePath.c_str(),
		SourceRevision,
		DestinationPath.c_str(),
		DestRevision,
		svn_depth_infinity,
		true,
		nullptr,
		&callSummarize,
		&Callback,
		mContext,
		Pool.get());

	if (Error != nullptr) throw std::runtime_error{Error->message};
}

namespace {

svn_error_t * callCat(void * Baton, const char * Data, apr_size_t * Length)
{
	auto & Callback = *static_cast<SvnT::CatCallbackT *>(Baton);
	Callback(Data, *Length);
	return nullptr;
}

}

void SvnT::cat(
	std::string const & Path, svn_revnum_t RevisionNum,
	CatCallbackT Callback)
{
	AprPoolT Pool;

	svn_opt_revision_t * Revision;
	svn_opt_revision_t * Peg;
	svn_stream_t * Stream;
	svn_error_t * Error;

	Revision = allocPool<svn_opt_revision_t>(Pool.get());
	Peg = allocPool<svn_opt_revision_t>(Pool.get());
	Revision->kind = svn_opt_revision_number;
	Revision->value.number = RevisionNum;
	Peg->kind = svn_opt_revision_unspecified;

	Stream = svn_stream_create(&Callback, Pool.get());
	svn_stream_set_baton(Stream, &Callback);
	svn_stream_set_write(Stream, &callCat);

	Error = svn_client_cat2(
		Stream,
		Path.c_str(),
		Revision,
		Revision,
		mContext,
		Pool.get());

	if (Error != nullptr) throw std::runtime_error{Error->message};
}


namespace {

svn_error_t * callLog(void * Baton, svn_log_entry_t * Entry, apr_pool_t *)
{
	auto & Callback = *static_cast<SvnT::LogCallbackT *>(Baton);
	Callback(Entry);
	return nullptr;
}

}

void SvnT::log(
	std::string const & Root,
	std::string const & Path, svn_revnum_t RevisionNum,
	LogCallbackT Callback)
{
	AprPoolT Pool;

	auto Revision = allocPool<svn_opt_revision_t>(Pool.get());
	auto Peg = allocPool<svn_opt_revision_t>(Pool.get());
	Revision->kind = svn_opt_revision_number;
	Revision->value.number = RevisionNum;

	auto RevisionRanges = apr_array_make(Pool.get(), 2, sizeof(svn_opt_revision_t *));
	APR_ARRAY_PUSH(RevisionRanges, svn_opt_revision_t *) = Revision;
	APR_ARRAY_PUSH(RevisionRanges, svn_opt_revision_t *) = Revision;

	Peg->kind = svn_opt_revision_unspecified;

	auto Targets = apr_array_make(Pool.get(), 2, sizeof(char const *));
	APR_ARRAY_PUSH(Targets, char const *) = Root.c_str();
	APR_ARRAY_PUSH(Targets, char const *) = Path.c_str();

	auto Error = svn_client_log5(
		Targets,
		Revision,
		RevisionRanges,
		1,
		false,
		false,
		false,
		nullptr,
		&callLog,
		&Callback,
		mContext,
		Pool.get());

	if (Error != nullptr) throw std::runtime_error{Error->message};
}

std::string SvnT::readFile(std::string const & Path, svn_revnum_t RevisionNum)
{
	std::string Buffer;
	cat(Path, RevisionNum, [&](char const * Data, std::size_t Length)
	{
		Buffer.append(Data, Data + Length);
	});
	return Buffer;
}

}
