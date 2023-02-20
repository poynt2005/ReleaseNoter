#ifndef __RELEASE_NOTER_H__
#define __RELEASE_NOTER_H__

#include <string>
#include <vector>

#ifdef BUILDNOTERAPI
#define EXPORTNOTERAPI __declspec(dllexport)
#else
#define EXPORTNOTERAPI __declspec(dllimport)
#endif

using CommitInfo = struct _commitInfo
{
    std::string asanaURL;
    std::string asanaID;
    std::string mantisURL;
    std::string mantisID;
    std::string title;
    double difftime;
    std::string author;
    std::string authorMail;
};

class EXPORTNOTERAPI ReleaseNoter
{
public:
    ReleaseNoter();
    ReleaseNoter(const int);
    ~ReleaseNoter();

    bool FindGitRepository();
    bool FindGitRepository(std::string &gitFolder);
    bool ListCommit();
    bool CopyReleaseNoteToClipboard();

    void SetClipboardCopierDllPath(const std::string &);
    void RemoveCommitByIndex(const std::vector<int> &);
    void GetCurrentListCommits(std::vector<CommitInfo> &);

private:
    std::string clipboardCopierDllPath;
    std::vector<CommitInfo> commits;
    int oldestDayPeriod;
    std::string gitFolder;

    /** prevent this header file inclue git2 header file*/
    void *repo = NULL;
};

#endif