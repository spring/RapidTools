#include "Rapid/Hex.hpp"
#include "Rapid/Last.hpp"
#include "Rapid/PoolArchive.hpp"
#include "Rapid/PoolFile.hpp"
#include "Rapid/Store.hpp"
#include "Rapid/Svn.hpp"
#include "Rapid/Versions.hpp"

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>

namespace {

using namespace Rapid;

// Replace all instances of $VERSION in a string by calling a functor with substrings
template<typename FunctorT>
void replaceVersion(std::string const & String, std::string const & Replacement, FunctorT Functor)
{
	std::size_t Pos = 0;

	while (true)
	{
		auto NewPos = String.find("$VERSION", Pos);
		if (NewPos == std::string::npos) break;
		auto Size = NewPos - Pos;
		if (Size == 0) continue;
		Functor(String.data() + Pos, Size);
		Functor(Replacement.data(), Replacement.size());
		Pos = NewPos + 8;
	}

	auto Size = String.size() - Pos;
	if (Size == 0) return;
	Functor(String.data() + Pos, String.size() - Pos);
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

	if (Log.size() >= StableString.size() &&
		std::equal(StableString.begin(), StableString.end(), Log.begin()))
	{
		return {"stable", "stable-" + RevisionString, true};
	}
	else if (Log.size() >= VersionString.size() &&
		std::equal(VersionString.begin(), VersionString.end(), Log.begin()))
	{
		auto EndPos = Log.find('}', 9);
		if (EndPos != std::string::npos) return {"stable", Log.substr(8, EndPos - 8), true};
	}

	return {"test", "test-" + RevisionString, false};
}


//helper function, removes prefix from string if exists
static std::string removePrefix(const std::string& str, const std::string& prefix) {
	assert(!str.empty());
	if (!prefix.empty() && (str.find(prefix) == 0)) { //FIXME: ugly hack, this should be used in all functions
		return str.substr(prefix.length());
	}
	return str;
}

void buildSvn(
	const std::string& SvnUrl,
	const std::string& ArchiveRoot, //relative path to the "root" of the ssd (where modinfo.lua is stored)
	const std::string& ModinfoFilename, //filename of modinfo.lua (could be mapinfo.lua)
	const std::string& StorePath,
	const std::string& RevisionString,
	const std::string& Prefix)
{
	SvnT Svn;

	// Absolute url of the archive's root
	const std::string AbsArchiveRoot = SvnUrl + '/' + ArchiveRoot;
	const std::string AbsModinfoPath = AbsArchiveRoot + '/' + ModinfoFilename;

	// Parse commit message for the type of commit
	svn_revnum_t RevisionNum = std::stoi(RevisionString);
	CommitInfoT CommitInfo;
	bool HasLog = false;
	Svn.log(SvnUrl, ArchiveRoot, RevisionNum, [&](svn_log_entry_t const * Entry)
	{
		char const * Author;
		char const * Date;
		char const * Message;
		svn_compat_log_revprops_out(&Author, &Date, &Message, Entry->revprops);
		CommitInfo = extractVersion(Message, RevisionString);
		HasLog = true;
	});
	if (!HasLog) throw std::runtime_error{"No log"};

	// Initialize the store
	StoreT Store{StorePath};
	Store.init();

	// Load the last proccessed revision
	auto Last = LastT::load(Store, Prefix, AbsModinfoPath);
	PoolArchiveT Archive{Store};
	if (Last.RevisionNum != 0) {
		std::cout << "Performing incremental an update from " << Last.RevisionNum << " to " << RevisionNum << "\n";
		Archive.load(Last.Digest);
	} else {
		std::cout << "Unable to perform incremental an update\n";
	}

	// Helper function used for adds/modifications
	auto add = [&](svn_client_diff_summarize_t const * Diff)
	{
		PoolFileT File{Store};
		const std::string Path = AbsArchiveRoot + "/" + removePrefix(Diff->path, ArchiveRoot + '/');
		Svn.cat(Path, RevisionNum, [&](char const * Data, apr_size_t Length)
		{
			File.write(Data, Length);
		});
		auto FileEntry = File.close();
		const std::string relpath = removePrefix(Diff->path, ArchiveRoot + '/');
		Archive.add( relpath , FileEntry);
	};

	// Ask svn for a diff from the last proccessed version (or 0 if none exists)
	Svn.summarize(AbsArchiveRoot, Last.RevisionNum, AbsArchiveRoot, RevisionNum,
		[&](svn_client_diff_summarize_t const * Diff)
	{
		if (Diff->summarize_kind == svn_client_diff_summarize_kind_added && Diff->node_kind == svn_node_file) {
			std::cout << "A\t" << Diff->path << "\n";
			add(Diff);
		} else if ( Diff->summarize_kind == svn_client_diff_summarize_kind_modified && Diff->node_kind == svn_node_file) {
			std::cout << "M\t" << Diff->path << "\n";
			add(Diff);
		} else if ( Diff->summarize_kind == svn_client_diff_summarize_kind_deleted && Diff->node_kind == svn_node_file) {
			std::cout << "D\t" << Diff->path << "\n";
			Archive.remove(Diff->path);
		} else if ( Diff->summarize_kind == svn_client_diff_summarize_kind_deleted && Diff->node_kind == svn_node_dir) { //directory deleted, do nothing
			std::cout << "D\t" << Diff->path << "\n";
//			Archive.removePrefix(Diff->path);
		} else if ( Diff->summarize_kind == svn_client_diff_summarize_kind_normal && Diff->node_kind == svn_node_file) {
			if (!Diff->prop_changed)
			{
				std::cout << "P\t" << Diff->path << "\n";
				add(Diff);
			}
		}
	});


	// Add updated modinfo.lua
	PoolFileT File{Store};
	auto Buffer = Svn.readFile(AbsModinfoPath, RevisionNum);
	replaceVersion(Buffer, CommitInfo.Version, [&](char const * Data, std::size_t Length)
	{
		File.write(Data, Length);
	});
	auto FileEntry = File.close();
	Archive.add("modinfo.lua", FileEntry);

	// Save archive and tag it
	auto ArchiveEntry = Archive.save();
	VersionsT Versions{Store};
	Versions.load();
	const std::string Tag = Prefix + ':' + CommitInfo.Branch;
	Versions.add(Tag, ArchiveEntry);
	const std::string Tag2 = Prefix + ":revision:" + RevisionString;
	Versions.add(Tag2, ArchiveEntry);
	Versions.save();

	// Update last proccesed revision
	Last.Digest = ArchiveEntry.Digest;
	Last.RevisionNum = RevisionNum;
	LastT::save(Last, Store, Prefix, AbsModinfoPath);

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

int main(int argc, char const * const * argv, char const * const * env)
{
	apr_app_initialize(&argc, &argv, &env);
	atexit(apr_terminate);
	umask(0002);

	if (argc != 7)
	{
		std::cerr << "Usage: " << argv[0] <<
			" <SVN URL> <Mods Root> <Archive Root> <filename of modinfo.lua> <Revision Number> <Rapid Prefix>\n";
		return 1;
	}

	try
	{
		buildSvn(
			argv[1],
			argv[2],
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
