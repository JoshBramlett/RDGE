import errno
import os
import math
import subprocess

IMAGE_DIR = 'images'
SPRITESHEET_DIR = 'spritesheets'
TILESET_DIR = 'tilesets'
TILEMAP_DIR = 'tilemaps'

TEXTURE_PACKER_PATH = '/usr/local/bin/TexturePacker'
TILED_PATH = '/Users/jbramlett/Documents/GitHub/tiled/build/bin/Tiled.app/Contents/MacOS/Tiled'

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

# Tiled exporting has the behavior to update the paths of all dependencies
# in the file based on the relative paths of the source and destination.
# To keep these unchanged, we invoke the call from the source directory,
# and manually move the file to it's proper destination.
def invoke_tiled_export(command, source_file, dest_file):
    cwd = os.getcwd()
    os.chdir(os.path.dirname(source_file))

    cmd = TILED_PATH
    cmd += ' ' + command
    cmd += ' ' + os.path.basename(source_file)
    cmd += ' ' + os.path.basename(dest_file)

    code = subprocess.call(cmd, shell=True)
    if code is not 0:
        raise Exception('Tiled export failed. code=%d' % code)

    os.rename(os.path.basename(dest_file), dest_file)
    os.chdir(cwd)

# Verify expected formats
def verify_properties(parent):
    if 'properties' in parent:
        for prop in parent['properties']:
            if 'name' not in prop:
                raise Exception('Invalid property format. missing "name" property')
            elif 'type' not in prop:
                raise Exception('Invalid property format. missing "type" property')
            elif 'value' not in prop:
                raise Exception('Invalid property format. missing "value" property')

def rotate_point(p, angle):
    sin = math.sin(math.radians(angle))
    cos = math.cos(math.radians(angle))
    px = float(p['x'])
    py = float(p['y'])

    return { 'x': cos * px - sin * py, 'y': sin * px + cos * py }

# Translates Tiled objects to a clear universal format, removes all unused values,
# and throws for any unsupported configurations
def translate_object(obj):
    if 'gid' in obj:
        obj['obj_type'] = 'sprite'

    elif 'ellipse' in obj:
        if obj['width'] != obj['height']:
            raise Exception('width/height mismatch: ellipse only supports circles')

        obj['obj_type'] = 'circle'
        obj['radius'] = float(obj['width']) * 0.5

        # ellipse position is the top-left corner of the surrounding AABB
        # convert it to the centroid
        obj['x'] = obj['x'] + obj['radius']
        obj['y'] = obj['y'] + obj['radius']

        obj.pop('ellipse')
        obj.pop('rotation')
        obj.pop('width')
        obj.pop('height')

    elif 'polygon' in obj:
        if len(obj['polygon']) > 8:
            raise Exception('polygon vertices exceed max supported')

        obj['obj_type'] = 'polygon'
        obj['coords'] = []
        for coord in obj['polygon']:
            obj['coords'].append(rotate_point(coord, obj['rotation']))

        obj.pop('polygon')
        obj.pop('rotation')
        obj.pop('width')
        obj.pop('height')

    elif 'polyline' in obj:
        raise Exception('polyline not supported')

    elif 'point' in obj:
        obj['obj_type'] = 'point'

        obj.pop('point')
        obj.pop('rotation')
        obj.pop('width')
        obj.pop('height')

    elif 'text' in obj:
        raise Exception('text not supported')

    else: # rectangle
        obj['obj_type'] = 'polygon'
        p1 = { 'x': 0, 'y': 0 }
        p2 = { 'x': obj['width'], 'y': 0 }
        p3 = { 'x': 0, 'y': obj['height'] }
        p4 = { 'x': obj['width'], 'y': obj['height'] }
        obj['coords'] = [ rotate_point(p1, obj['rotation']),
                          rotate_point(p2, obj['rotation']),
                          rotate_point(p3, obj['rotation']),
                          rotate_point(p4, obj['rotation']) ]

        # rectangle position is the top-left corner
        # convert it to the bottom-left
        obj['y'] = obj['y'] + obj['height']

        obj.pop('rotation')
        obj.pop('width')
        obj.pop('height')

    verify_properties(obj)
