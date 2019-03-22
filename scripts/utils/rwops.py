__all__ = ["pure_basename","try_mkdir","try_delete_file","is_dirty","get_dir","get_file"]

import os
import errno

# Get the basename from a path without an extension
def pure_basename(path):
    return os.path.splitext(os.path.basename(path))[0]

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
# True is returned if any files do not exist
#
# Note: 'compare_paths' uses a list b/c ofttimes a script can generate multiple files
#       of output.  (e.g. exporting sheet and data files)
def is_dirty(path, compare_paths):
    if (not os.path.isfile(path)) or (len(compare_paths) == 0):
        return True

    ts = os.path.getmtime(path)
    for p in compare_paths:
        if (not os.path.isfile(p)) or (ts > os.path.getmtime(p)):
            return True

    return False

# Get the path for an existing directory (throws if non-existant)
def get_dir(path, *paths):
    full_path = os.path.join(path, *paths)
    if not os.path.isdir(full_path):
        raise IOError('Directory does not exist: %s' % full_path)
    return os.path.normpath(full_path)

# Get the path for an existing file (throws if non-existant)
def get_file(path, *paths):
    full_path = os.path.join(path, *paths)
    if not os.path.isfile(full_path):
        raise IOError('File does not exist: %s' % full_path)
    return os.path.normpath(full_path)
