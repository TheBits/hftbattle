#!/usr/bin/env python

import urllib
import sys
import zipfile

def progress_bar(count, block_size, total_size):
    percent = count * block_size * 100 / total_size
    sys.stdout.write('\r%2d%%' % percent)
    sys.stdout.flush()


def download(url, filename):
    print 'Downloading %s from %s:' % (filename, url)
    urllib.urlretrieve(url, filename, reporthook=progress_bar)
    print '\nDone!'

def unarchive(filename):
    print 'Unarchiving %s:' % filename
    zf = zipfile.ZipFile(filename, allowZip64=True)
    total_size = sum((file.file_size for file in zf.infolist()))
    cur_size = 0
    for file in zf.infolist():
        cur_size += file.file_size
        percent = cur_size * 100 / total_size
        sys.stdout.write("\r%2d%%" % percent)
        sys.stdout.flush()
        zf.extract(file)
    print '\nDone!'

url = 'https://www.dropbox.com/s/s1uxgs3vj3tgvr7/data.zip?dl=1'
filename = 'data.zip'

download(url, filename)
unarchive(filename)

