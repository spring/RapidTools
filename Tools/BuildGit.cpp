#include "Rapid/Hex.hpp"
#include "Rapid/LastGit.hpp"
#include "Rapid/PoolArchive.hpp"
#include "Rapid/PoolFile.hpp"
#include "Rapid/ScopeGuard.hpp"
#include "Rapid/Store.hpp"
#include "Rapid/Versions.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>

#include <git2.h>
#include <sys/types.h>
#include <sys/stat.h>

namespace {

using namespace Rapid;

void check_lg2(int error, const std::string& message, const std::string& extra = "")
{
	const git_error *lg2err;
	const char *lg2msg = "", *lg2spacer = "";

	if (!error)
		return;

	if ((lg2err = giterr_last()) != NULL && lg2err->message != NULL) {
		lg2msg = lg2err->message;
		lg2spacer = " - ";
	}

	if (!extra.empty())
		fprintf(stderr, "%s '%s' [%d]%s%s\n", message.c_str(), extra.c_str(), error, lg2spacer, lg2msg);
	else
		fprintf(stderr, "%s [%d]%s%s\n", message.c_str(), error, lg2spacer, lg2msg);
	throw std::runtime_error("git error");
}

/**
	returns the first commit hash of a repo
*/
std::string get_root(git_repository * repo)
{
	git_oid oid;
	git_revwalk *walk;
	git_commit *wcommit;

	git_revwalk_new(&walk, repo);
	git_revwalk_sorting(walk, GIT_SORT_TOPOLOGICAL | GIT_SORT_REVERSE);
	git_revwalk_push_head(walk);
	while ((git_revwalk_next(&oid, walk)) == 0) {
		check_lg2(git_commit_lookup(&wcommit, repo, &oid), "git_commit_lookup");

/*		auto cmsg  = git_commit_message(wcommit);
		auto cauth = git_commit_author(wcommit);
		printf("%s (%s)\n", cmsg, cauth->email);
*/
		git_revwalk_free(walk);
		char out[41];
		out[40] = '\0';
		git_oid_fmt(out, git_commit_id(wcommit));
		std::string res(out);
		git_commit_free(wcommit);
		return res;
	}
	return "";
}

// Replace all instances of $VERSION in a string by calling a functor with substrings
template<typename FunctorT>
void replaceVersion(char const * Buffer, std::size_t BufferSize, std::string const & Replacement, FunctorT Functor)
{
	auto Pos = Buffer;
	auto Last = Buffer + BufferSize;
	std::string VersionString{"$VERSION"};

	while (true)
	{
		auto NewPos = std::search(Pos, Last, VersionString.begin(), VersionString.end());
		if (NewPos == Last) break;
		auto Size = NewPos - Pos;
		if (Size == 0) continue;
		Functor(Pos, Size);
		Functor(Replacement.data(), Replacement.size());
		Pos = NewPos + VersionString.size();
	}

	auto Size = Last - Pos;
	if (Size == 0) return;
	Functor(Pos, Size);
}

struct CommitInfoT
{
	std::string Branch;
	std::string Version;
	bool MakeZip;
};

CommitInfoT extractVersion(std::string const & Log, std::string const & RevisionString)
{
	std::string const StableString{"STABLE"};
	std::string const VersionString{"VERSION{"};

	if (
		Log.size() >= StableString.size() &&
		std::equal(StableString.begin(), StableString.end(), Log.begin()))
	{
		return {"stable", "stable-" + RevisionString, true};
	}
	else if (
		Log.size() >= VersionString.size() &&
		std::equal(VersionString.begin(), VersionString.end(), Log.begin()))
	{
		auto First = Log.begin() + VersionString.size();
		auto Last = Log.end();
		auto EndPos = std::find(First, Last, '}');
		if (EndPos != Last) return {"stable", {First, EndPos}, true};
	}

	return {"test", "test-" + RevisionString, false};
}

void convertTreeishToTree(git_tree** Tree, git_repository* Repo, const std::string& Treeish)
{
	git_object * Object;
	check_lg2(git_revparse_single(&Object, Repo, Treeish.c_str()), "git_revparse_single", Treeish);
	auto && ObjectGuard = makeScopeGuard([&] { git_object_free(Object); });
	check_lg2(git_object_peel(reinterpret_cast<git_object * *>(Tree), Object, GIT_OBJ_TREE), "git_object_peel");
}

void buildGit(
	const std::string& GitPath,
	const std::string& ModRoot,
	const std::string& Modinfo,
	const std::string& StorePath,
	const std::string& GitHash,
	const std::string& Prefix)
{
	check_lg2(git_threads_init(), "git_threads_init()");
	auto && ThreadsGuard = makeScopeGuard([&] { git_threads_shutdown(); });

	git_repository * Repo;
	check_lg2(git_repository_open_ext(&Repo, GitPath.c_str(), 0, nullptr), "git_repository_open_ext");
	auto && RepoGuard = makeScopeGuard([&] { git_repository_free(Repo); });

	git_oid DestOid;
	check_lg2(git_oid_fromstr(&DestOid, GitHash.c_str()), "git_oid_fromstr");

	git_commit * Commit;
	check_lg2(git_commit_lookup(&Commit, Repo, &DestOid), "git_commit_lookup");
	auto && CommitGuard = makeScopeGuard([&] { git_commit_free(Commit); });
	auto CommitInfo = extractVersion(git_commit_message_raw(Commit), GitHash);

	// Initialize the store
	StoreT Store{StorePath};
	Store.init();

	// Load the last proccessed revision
	auto Option = LastGitT::load(Store, Prefix);

	PoolArchiveT Archive{Store};
	git_tree * SourceTree;
	if (!Option)
	{
		std::cout << "Unable to perform incremental an update\n";
		const std::string root = get_root(Repo) + ":" + ModRoot;
		convertTreeishToTree(&SourceTree, Repo, root);
	}
	else
	{
		auto & Last = *Option;
		Archive.load(Last.Digest);
		std::string SourceTreeish;
		SourceTreeish.resize(40);
		Hex::encode(&SourceTreeish[0], Last.Hex.data(), 20);

		std::cout <<
			"Performing incremental an update from " <<
			SourceTreeish.data() <<
			" to " <<
			GitHash <<
			"\n";

		SourceTreeish += ':' + ModRoot;
		convertTreeishToTree(&SourceTree, Repo, SourceTreeish.data());
	}
	auto && SourceGuard = makeScopeGuard([&] { git_tree_free(SourceTree); });

	git_tree * DestTree;
	const std::string DestTreeish = GitHash + ":" + ModRoot;

	convertTreeishToTree(&DestTree, Repo, DestTreeish.c_str());
	auto && DestGuard = makeScopeGuard([&] { git_tree_free(DestTree); });

	git_diff * Diff;
	git_diff_options Options;
	git_diff_options_init(&Options, GIT_DIFF_OPTIONS_VERSION);
	check_lg2(git_diff_tree_to_tree(&Diff, Repo, SourceTree, DestTree, &Options), "git_diff_tree_to_tree");
	auto && DiffGuard = makeScopeGuard([&] { git_diff_free(Diff); });

	// Helper function used for adds/modifications
	auto add = [&](git_diff_delta const * Delta)
	{
		PoolFileT File{Store};
		git_blob * Blob;
		check_lg2(git_blob_lookup(&Blob, Repo, &Delta->new_file.oid), "git_blob_lookup");
		auto && BlobGuard = makeScopeGuard([&] { git_blob_free(Blob); });
		auto Size = git_blob_rawsize(Blob);
		auto Pointer = static_cast<char const *>(git_blob_rawcontent(Blob));
		File.write(Pointer, Size);
		auto FileEntry = File.close();
		Archive.add(Delta->new_file.path, FileEntry);
	};

	for (std::size_t I = 0, E = git_diff_num_deltas(Diff); I != E; ++I)
	{
		auto Delta = git_diff_get_delta(Diff, I);
		switch (Delta->status)
		{

		case GIT_DELTA_ADDED:
		{
			std::cout << "A\t" << Delta->new_file.path << "\n";
			add(Delta);
			break;
		}

		case GIT_DELTA_MODIFIED:
		{
			std::cout << "M\t" << Delta->new_file.path << "\n";
			add(Delta);
			break;
		}

		case GIT_DELTA_DELETED:
		{
			std::cout << "D\t" << Delta->new_file.path << "\n";
			Archive.remove(Delta->new_file.path);
			break;
		}

		default: throw std::runtime_error{"Unsupported delta"};
		}
	}

	git_tree_entry * TreeEntry;
	check_lg2(git_tree_entry_bypath(&TreeEntry, DestTree, Modinfo.c_str()), "git_tree_entry_bypath");
	git_blob * Blob;
	check_lg2(git_blob_lookup(&Blob, Repo, git_tree_entry_id(TreeEntry)), "git_blob_lookup");
	auto && BlobGuard = makeScopeGuard([&] { git_blob_free(Blob); });
	auto Size = git_blob_rawsize(Blob);
	auto Pointer = static_cast<char const *>(git_blob_rawcontent(Blob));
	PoolFileT File{Store};
	replaceVersion(Pointer, Size, CommitInfo.Version, [&](char const * Data, std::size_t Length)
	{
		File.write(Data, Length);
	});
	auto FileEntry = File.close();
	Archive.add(Modinfo, FileEntry);
	auto ArchiveEntry = Archive.save();

	VersionsT Versions{Store};
	Versions.load();
	std::string Tag;
	Tag += Prefix;
	Tag += ':';
	Tag += CommitInfo.Branch;
	Versions.add(Tag, ArchiveEntry);
	std::string Tag2;
	Tag2 += Prefix;
	Tag2 += ":git:";
	Tag2 += GitHash;
	Versions.add(Tag2, ArchiveEntry);
	Versions.save();

	LastGitT Last;
	Hex::decode(GitHash.c_str(), Last.Hex.data(), 20);
	Last.Digest = ArchiveEntry.Digest;
	LastGitT::save(Last, Store, Prefix);

	// Create zip if needed
	if (CommitInfo.MakeZip)
	{
		auto Path = Store.getBuildPath(Prefix, CommitInfo.Version);
		std::cout << "Generating zip file: " << Path << "\n";
		Archive.makeZip(Path);
		// Call upload.py?
	}
}

}

const std::string fixRoot(const std::string& root){
	if ((root == ".") || (root == "/")) {
		return "";
	}
	return root;
}


int main(int argc, char const * const * argv)
{
	umask(0002);

	if (argc != 7)
	{
		std::cerr << "Usage: " << argv[0] <<
			" <Git Path> <Mod Root> <Modinfo> <Store Path> <Git Hash> <Prefix>\n";
		return 1;
	}

	try
	{
		buildGit(
			argv[1],
			fixRoot(argv[2]),
			argv[3],
			argv[4],
			argv[5],
			argv[6]);
	}
	catch (std::exception const & Exception)
	{
		std::cerr << Exception.what() << "\n";
		return 1;
	}

	return 0;
}
