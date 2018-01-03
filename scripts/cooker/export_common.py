import errno
import os
import math

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

# Translates Tiled dual dictionary implementation to a single array
#
# "properties":
# {
#   "cust_prop_float":3.14
#   "cust_prop_int":3
# },
# "propertytypes":
# {
#   "cust_prop_float":"float"
#   "cust_prop_int":"int"
# }
#
# to
#
# "properties": { [
#   "name": "cust_prop_float",
#   "value": 3.14,
#   "type": "float"
#  ], [
#   "name": "cust_prop_int",
#   "value": 3,
#   "type": "int"
#  ] }
def translate_properties(parent):
    props = parent.pop('properties', None)
    types = parent.pop('propertytypes', None)

    if props is not None:
        parent['properties'] = []
        for key, value in props.iteritems():
            if key not in types:
                raise Exception('Property/PropertyType mismatch')
            parent['properties'].append({ 'name': key, 'value': value, 'type': types[key] })

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
        obj['radius'] = obj['width']

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

        obj.pop('rotation')
        obj.pop('width')
        obj.pop('height')

    translate_properties(obj)
