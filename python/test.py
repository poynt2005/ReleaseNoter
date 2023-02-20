from ReleaseNoter import ReleaseNoter
from pprint import pprint
noter = ReleaseNoter(1)

if noter.find_git_repository(lambda x: print('Found git repository in %s' % x) if x else print('Git repository not found')):
    print('Git Repository found')
    if noter.list_commit():
        print('Commits listed')

        if noter.copy_release_note_to_clipboard():
            print('Copied to clipboard')
