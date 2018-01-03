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

def rotate_point(c, angle, p):
    sin = math.sin(math.radians(angle))
    cos = math.cos(math.radians(angle))

    cx = float(c['x'])
    cy = float(c['y'])
    px = float(p['x'])
    py = float(p['y'])
    return { 'x': cos * (px - cx) - sin * (py - cy) + cx,
             'y': sin * (px - cx) + cos * (py - cy) + cy }

# Translates Tiled objects to a clear universal format, removes all unused values,
# and throws for any unsupported configurations
def translate_object(obj):
    if 'gid' in obj:
        obj['obj_type'] = 'sprite'

    elif 'ellipse' in obj:
        if obj['width'] != obj['height']:
            raise Exception('width/height mismatch: ellipse only supports circles')

        obj['obj_type'] = 'ellipse'
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
            coord['x'] += obj['x']
            coord['y'] += obj['y']
            obj['coords'].append(rotate_point(obj, obj['rotation'], coord))

        obj.pop('polygon')
        obj.pop('rotation')
        obj.pop('x')
        obj.pop('y')
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
        p1 = { 'x': obj['x'], 'y': obj['y'] }
        p2 = { 'x': obj['x'] + obj['width'], 'y': obj['y'] }
        p3 = { 'x': obj['x'], 'y': obj['y'] + obj['height'] }
        p4 = { 'x': obj['x'] + obj['width'], 'y': obj['y'] + obj['height'] }
        obj['coords'] = [ rotate_point(obj, obj['rotation'], p1),
                          rotate_point(obj, obj['rotation'], p2),
                          rotate_point(obj, obj['rotation'], p3),
                          rotate_point(obj, obj['rotation'], p4) ]

        obj.pop('rotation')
        obj.pop('x')
        obj.pop('y')
        obj.pop('width')
        obj.pop('height')

    translate_properties(obj)
