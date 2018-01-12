import getopt
import json
import os
import sys
import subprocess

from export_common import *

def usage():
    print ""
    print "Builds a spritesheet from a Tiled Map Editor image collection tileset"
    print ""
    print "Used with tilesets that are created from a collection of images, this script"
    print "will create a spritesheet from the collection.  Tile collision data is stored"
    print "in the sheet's data file to the associated frame."
    print ""
    print "Any optional properties for the TexturePacker processing can be added"
    print "by providing a tps file in the same directory (and same base name) as"
    print "the tileset."
    print ""
    print "Data file will be written to <output_dir>/spritesheets"
    print "Texture file will be written to <output_dir>/images"
    print ""
    print "Note: Requires TexturePacker command line tools."
    print ""
    print "Usage:"
    print "export_objectsheet.py -f <tileset.json> -o <output_dir>"

tile_defs = []
def parse_tileset(in_file):
    with open(in_file) as json_data:
        j = json.load(json_data)

    if 'type' not in j or j['type'] != "tileset":
        raise Exception('Invalid tileset file format')

    path = os.path.dirname(in_file)
    for tile in j['tiles']:
        tile_def = {}
        tile_def['index'] = int(tile)
        tile_def['image'] = os.path.join(path, j['tiles'][tile]['image'])
        tile_def['objects'] = []

        objects = j['tiles'][tile]['objectgroup']['objects']
        for obj in objects:
            translate_object(obj)
            tile_def['objects'].append(obj)

        global tile_defs
        tile_defs.insert(int(tile), tile_def)

def merge_data(data_file):
    with open(data_file) as json_data:
        j = json.load(json_data)

    j['meta']['sheet_type'] = 'objectsheet'
    for d in tile_defs:
        name = os.path.splitext(os.path.basename(d['image']))[0]
        for frame in j['frames']:
            if frame['filename'] == name:
                frame['index'] = d['index']
                frame['objects'] = d['objects']

    with open(data_file, 'w') as f:
        f.write(json.dumps(j, indent=2, ensure_ascii=False))

def process(in_file, out_dir):
    if not os.path.isfile(in_file):
        raise Exception('Cannot find input file')

    if not os.path.isdir(out_dir):
        raise Exception('Cannot find output directory')

    # 1) parse tileset to get tile collision objects
    parse_tileset(in_file)

    out_dir = os.path.abspath(out_dir)
    sheet_dir = os.path.join(out_dir, IMAGE_DIR)
    data_dir = os.path.join(out_dir, SPRITESHEET_DIR)
    if not try_mkdir(sheet_dir) or not try_mkdir(data_dir):
        raise Exception('Unable to open/create child directories')

    base_name = os.path.splitext(os.path.basename(in_file))[0]
    sheet_file = os.path.join(sheet_dir, base_name + '.png')
    data_file = os.path.join(data_dir, base_name + '.json')

    cmd = TEXTURE_PACKER_PATH
    cmd += ' --force-publish'                # force write even if unchanged
    cmd += ' --sheet ' + sheet_file          # texture file location/filename

    # data file
    cmd += ' --format json-array'                             # data file format
    cmd += ' --data ' + data_file                             # data file location/filename
    cmd += ' --texturepath ' + os.path.join('..', IMAGE_DIR)  # texture file path

    # data file: frame naming convention
    cmd += ' --trim-sprite-names'            # remove file extension from name

    # misc
    cmd += ' --algorithm MaxRects'           # best algorithm for rectangle packing
    cmd += ' --trim-mode Trim'               # maintain size, remove surrounding transparency
    cmd += ' --extrude 0'                    # don't allow extension of border pixels
    cmd += ' --padding 2'                    # 2 pixel padding between sprites
    # cmd += ' --enable-rotation'
    # cmd += ' --disable-rotation'

    # include all files parsed from tileset
    for d in tile_defs:
        cmd += ' ' + d['image']

    # include already existing tps file if existing
    tps_file = os.path.splitext(in_file)[0] + '.tps'
    if os.path.isfile(tps_file):
        cmd += ' ' + tps_file

    # 2) generate spritesheet and data file
    code = subprocess.call(cmd, shell=True)
    if code is not 0:
        raise Exception('TexturePacker call failed.  code=%s' % str(code))

    # 3) merge generated spritesheet data file with collision objects
    merge_data(data_file)

def main():
    try:
        opts, args = getopt.getopt(sys.argv[1:], "hf:o:v", ["help", "file=", "output="])
    except getopt.GetoptError as err:
        print str(err)
        usage()
        sys.exit(2)

    in_file = ""
    out_dir = ""
    for opt, arg in opts:
        if opt in ("-h", "--help"):
            usage()
            sys.exit()
        elif opt in ("-f", "--file"):
            in_file = arg.strip()
        elif opt in ("-o", "--output"):
            out_dir = arg.strip()

    if in_file == "" or out_dir == "":
        print "Missing required fields"
        usage()
        sys.exit(2)

    try:
        process(in_file, out_dir)
    except ValueError as err:
        print('ValueError: %s' % str(err))
        sys.exit(2)
    except Exception as err:
        print('Rumtime exception: %s' % str(err))
        sys.exit(2)

if __name__ == "__main__":
    main()