import PyBuildUtilities as pyUtil
import shutil
import os
from subprocess import Popen

# Build utilities/VSFinder


def download_embeed_python():
    url = 'https://www.python.org/ftp/python/3.10.0/python-3.10.0-embed-amd64.zip'

    if os.path.isdir('python310'):
        shutil.rmtree('python310')

    if pyUtil.DownloadFile('python-embed.zip', url):
        shutil.unpack_archive('python-embed.zip', 'python-embed-dir')
        os.remove('python-embed.zip')
        os.mkdir('python310')
        shutil.move('python-embed-dir/python310.zip',
                    'python310/python310.zip')
        shutil.move('python-embed-dir/python310.dll',
                    'python310/python310.dll')
        shutil.move('python-embed-dir/vcruntime140.dll',
                    'python310/vcruntime140.dll')
        shutil.move('python-embed-dir/vcruntime140_1.dll',
                    'python310/vcruntime140_1.dll')

        shutil.unpack_archive('python310/python310.zip', 'python310/python310')

        os.remove('python310/python310.zip')

        shutil.rmtree('python-embed-dir')
    else:
        print('Download python embed failed')


download_embeed_python()
