
#include "ReleaseNoter.h"
#include <vector>
#include <iostream>
#include "Utilities.h"

int main()
{
    std::cout << "Release Noter Git Test\n";

    ReleaseNoter noter(1);

    std::vector<CommitInfo> commitInfo;

    if (noter.FindGitRepository())
    {
        if (noter.ListCommit())
        {
            // noter.RemoveCommitByIndex({0, 1, 3, 4, 5, 7, 8});

            if (noter.CopyReleaseNoteToClipboard())
            {
                std::cout << "Copy to clipboard\n";
            }
        }
    }

    return 0;
}