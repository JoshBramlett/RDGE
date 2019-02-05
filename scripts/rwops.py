__all__ = ["try_mkdir","try_delete_file","is_dirty"]

import os

# Attempt to create directory if it doesn't exist
def try_mkdir(path):
    try:
        os.makedirs(path)
    except OSError as err:
        if err.errno != errno.EEXIST:
            return False
    return True

def try_delete_file(path):
    print('Deleting file %s' % path)

    if not os.path.isfile(path):
        raise Exception('Invalid parameter')

    i = 0
    max_iterations = 10
    while True:
        try:
            os.remove(path)
            return
        except:
            print('Failed to delete file.  Attempt %d/%d' % (i + 1), max_iterations)
            time.sleep(1)
            i += 1
            if i >= max_iterations:
                raise Exception('Unable to delete file')

# Check if a file has a modified date more recent than any other corresponding file.
# If any file to check against does not exist the function will return True.
#
# Note: 'compare_paths' uses a list b/c ofttimes a script can generate multiple files
#       of output.  (e.g. exporting sheet and data files)
def is_dirty(path, compare_paths):
    if not os.path.isfile(path):
        raise IOError('File does not exist: %s' % f)

    ts = os.path.getmtime(path)
    for p in compare_paths:
        if (not os.path.isfile(p)) or (ts > os.path.getmtime(p)):
            return True

    return False
