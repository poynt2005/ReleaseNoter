#include "ReleaseNoter.h"
#include <ManagedCallBridge.h>
#include "Utilities.h"
#include <git2.h>
#include <fmt/format.h>
#include <algorithm>
#include <regex>

ReleaseNoter::ReleaseNoter()
    : ReleaseNoter(30)
{
}

ReleaseNoter::ReleaseNoter(const int _oldestDayPeriod)
    : oldestDayPeriod(_oldestDayPeriod)
{
    git_libgit2_init();
}

ReleaseNoter::~ReleaseNoter()
{
    if (repo != NULL)
    {
        git_repository_free(reinterpret_cast<git_repository *>(repo));
    }

    git_libgit2_shutdown();
}

bool ReleaseNoter::FindGitRepository()
{
    std::string gitFolder;
    return FindGitRepository(gitFolder);
}

bool ReleaseNoter::FindGitRepository(std::string &gitFolder)
{
    std::string gitRepoDir;

    bool isRepoDirFound = Utilities::findGitRepositoryFolder(gitRepoDir);

    if (!isRepoDirFound)
    {
        return false;
    }

    gitFolder = gitRepoDir;

    int openResult = git_repository_open(reinterpret_cast<git_repository **>(&repo), gitRepoDir.data());

    if (openResult != 0)
    {
        return false;
    }

    return true;
}

void ReleaseNoter::GetCurrentListCommits(std::vector<CommitInfo> &vec)
{
    vec.clear();

    vec.assign(commits.begin(), commits.end());
}

bool ReleaseNoter::ListCommit()
{
    commits.clear();

    if (repo == NULL)
    {
        return false;
    }

    int errorCode;

    git_object *head_commit;
    errorCode = git_revparse_single(&head_commit, reinterpret_cast<git_repository *>(repo), "HEAD^{commit}");

    if (errorCode != 0)
    {
        return false;
    }

    git_commit *commit = reinterpret_cast<git_commit *>(head_commit);
    git_oid *oid = const_cast<git_oid *>(git_commit_id(commit));

    git_revwalk *revwalk;

    errorCode = git_revwalk_new(&revwalk, reinterpret_cast<git_repository *>(repo));
    if (errorCode != 0)
    {
        return false;
    }

    errorCode = git_revwalk_sorting(revwalk, GIT_SORT_TIME);
    if (errorCode != 0)
    {
        return false;
    }

    errorCode = git_revwalk_push(revwalk, oid);
    if (errorCode != 0)
    {
        return false;
    }

    git_time_t start_time;
    time(&start_time);

    while (!git_revwalk_next(oid, revwalk))
    {
        CommitInfo commitInfo;
        const git_time_t commit_time = git_commit_time(commit);
        if ((difftime(start_time, commit_time) / 86400) > oldestDayPeriod)
        {
            break;
        }
        if (!git_commit_lookup(&commit, reinterpret_cast<git_repository *>(repo), oid))
        {

            std::vector<std::string> commitMessageSplit;
            Utilities::stringSplit(std::string(git_commit_message(commit)), "\\n", commitMessageSplit);

            const std::regex mantisRegexp("http:\\/\\/qa-server-2\\/mantisbt\\/view.php\\?id\\=[0-9]+");
            const std::regex asanaRegex("https:\\/\\/app.asana.com\\/0\\/[0-9]+\\/[0-9]+");
            const std::regex numberRegex("[0-9]+");

            int colonDelimeter = commitMessageSplit[0].find_first_of(":");

            std::string trimedStr;
            if (colonDelimeter != std::string::npos)
            {
                Utilities::stringTrim(commitMessageSplit[0].substr(colonDelimeter + 1), trimedStr);
            }
            else
            {
                Utilities::stringTrim(commitMessageSplit[0], trimedStr);
            }

            commitInfo.title = std::move(trimedStr);

            commitInfo.difftime = commit_time / 86400;

            const git_signature *author = git_commit_author(commit);

            commitInfo.author = std::string(author->name);
            commitInfo.authorMail = std::string(author->email);
            if (commitMessageSplit.size() < 2)
            {
                continue;
            }

            for (int i = 1; i < commitMessageSplit.size(); i++)
            {
                std::smatch m;
                if (std::regex_search(commitMessageSplit[i], m, mantisRegexp))
                {
                    if (commitInfo.mantisURL.empty() && commitInfo.mantisID.empty())
                    {
                        commitInfo.mantisURL = m.str(0);
                        std::vector<std::string> splitMantis;
                        Utilities::stringSplit(commitInfo.mantisURL, "\\/", splitMantis);

                        for (int j = splitMantis.size(); j >= 0; j--)
                        {
                            if (splitMantis[j].length() > 1e5)
                            {
                                continue;
                            }

                            std::smatch dm;

                            if (std::regex_search(splitMantis[j], dm, numberRegex))
                            {
                                commitInfo.mantisID = dm.str();
                                break;
                            }
                        }
                    }
                }
                else if (std::regex_search(commitMessageSplit[i], m, mantisRegexp))
                {
                    if (commitInfo.asanaURL.empty() && commitInfo.asanaID.empty())
                    {
                        commitInfo.asanaURL = m.str(0);
                        std::vector<std::string> splitAsana;
                        Utilities::stringSplit(commitInfo.asanaURL, "\\/", splitAsana);

                        for (int j = splitAsana.size(); j >= 0; j--)
                        {
                            if (splitAsana[j].length() > 1e5)
                            {
                                continue;
                            }

                            std::smatch dm;

                            if (std::regex_search(splitAsana[j], dm, numberRegex))
                            {
                                commitInfo.asanaID = dm.str();
                                break;
                            }
                        }
                    }
                }
            }
        }

        commits.emplace_back(std::move(commitInfo));
    }

    git_commit_free(commit);
    git_revwalk_free(revwalk);

    return true;
}

bool ReleaseNoter::CopyReleaseNoteToClipboard()
{
    std::string htmlString =
        "<div><li>";

    for (auto &&commit : commits)
    {
        auto mantisIssueHTML = commit.mantisURL.empty() ? "" : fmt::format("<a href=\"{}\"> #{} </a>", commit.mantisURL, commit.mantisID);
        auto asanaIssueHTML = commit.asanaURL.empty() ? "" : fmt::format("<a href=\"{}\"> δ{} </a>", commit.asanaURL, commit.asanaID);

        std::string commitHTML = fmt::format(
            "<ol><div>"
            "{}{}{}"
            " (@<a href=\"mailto:{}\">{}</a>) "
            "</div></ol>",
            commit.title, mantisIssueHTML, asanaIssueHTML, commit.authorMail, commit.author);

        htmlString += commitHTML;
    }

    htmlString += "</li></div>";

    bool isCopied;

    if (clipboardCopierDllPath.empty())
    {
        isCopied = static_cast<bool>(NativeClipboardCopier(htmlString.data()));
    }
    else
    {
        isCopied = static_cast<bool>(NativeClipboardCopierPath(htmlString.data(), clipboardCopierDllPath.data()));
    }

    return isCopied;
}

void ReleaseNoter::RemoveCommitByIndex(const std::vector<int> &toDeleteArrayIdxs)
{
    std::vector toDeleteArrayIdxsTmp(toDeleteArrayIdxs.begin(), toDeleteArrayIdxs.end());

    std::vector<CommitInfo> filteredCommits;

    int idx = 0;
    std::copy_if(commits.begin(), commits.end(), std::back_inserter(filteredCommits), [&](auto el) -> bool
                 {
                               if (toDeleteArrayIdxsTmp.size() != 0)
                               {
                                   std::vector<int>::iterator it = std::find(toDeleteArrayIdxsTmp.begin(), toDeleteArrayIdxsTmp.end(), idx);

                                   if(it != toDeleteArrayIdxsTmp.end()){
                                    toDeleteArrayIdxsTmp.erase(it);
                                    idx++;
                                    return false;
                                   }
                               }
                               idx++;
                               return true; });

    commits = std::move(filteredCommits);
}

void ReleaseNoter::SetClipboardCopierDllPath(const std::string &dllPath)
{
    clipboardCopierDllPath = std::move(dllPath);
}