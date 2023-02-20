import PyReleaseNoter
from typing import List, Callable


class ReleaseNoter:
    def __init__(self, oldestDayPeriod=30):
        self.__cpp_noter_id = PyReleaseNoter.NewNoter(oldestDayPeriod)

    def set_copier_dll_path(self, dll_path):
        return PyReleaseNoter.SetCopierDllPath(self.__cpp_noter_id, dll_path)

    def find_git_repository(self, foundCallback: Callable = None) -> bool:
        return PyReleaseNoter.FindGitRepository(self.__cpp_noter_id, foundCallback)

    def list_commit(self) -> bool:
        return PyReleaseNoter.ListCommit(self.__cpp_noter_id)

    def remove_commit_by_index(self, to_remove: List) -> None:
        return PyReleaseNoter.RemoveCommitByIndex(self.__cpp_noter_id, to_remove)

    def get_current_list_commits(self) -> List:
        return PyReleaseNoter.GetCurrentListCommits(self.__cpp_noter_id)

    def copy_release_note_to_clipboard(self) -> bool:
        return PyReleaseNoter.CopyReleaseNoteToClipboard(self.__cpp_noter_id)
