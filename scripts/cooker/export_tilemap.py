import getopt
import json
import os
import sys
import subprocess

from export_common import *

def usage():
    print ""
    print "Export a Tiled Map Editor tilemap"
    print ""
    print "Accepts maps in TMX format, and exports in json."
    print "No verification is done on the referenced tilesets.  It will determine"
    print "if the tileset is a natural tileset or objectsheet and adjust the"
    print "path accordingly"
    print ""
    print "Data file will be written to <output_dir>/tilemap"
    print ""
    print "Usage:"
    print "export_tilemap.py -f <tilemap.tmx> -o <output_dir>"

def process(in_file, out_dir):
    if not os.path.isfile(in_file):
        raise Exception('Cannot find input file')

    if not os.path.isdir(out_dir):
        raise Exception('Cannot find output directory')

    out_dir = os.path.abspath(out_dir)
    data_dir = os.path.join(out_dir, TILEMAP_DIR)
    if not try_mkdir(data_dir):
        raise Exception('Unable to open/create child directories')

    base_name = os.path.splitext(os.path.basename(in_file))[0]
    data_file = os.path.join(data_dir, base_name + '.json')

    # 1) Convert and export data file
    cmd = TILED_PATH
    cmd += ' --export-map json'
    cmd += ' ' + in_file
    cmd += ' ' + data_file

    code = subprocess.call(cmd, shell=True)
    if code is not 0:
        raise Exception('Tiled export call failed.  code=%s' % str(code))

    # 2) Update tileset reference paths
    with open(data_file) as json_data:
        j = json.load(json_data)

    if 'type' not in j or j['type'] != "map":
        raise Exception('Invalid tilemap file format')

    translate_properties(j)

    for layer in j['layers']:
        translate_properties(layer)
        if layer['type'] == 'objectgroup':
            for obj in layer['objects']:
                translate_object(obj)

    for tileset in j['tilesets']:
        if 'tilesets' in tileset['source']:
            source_file = os.path.basename(tileset['source'])
            tileset['source'] = os.path.join('..', TILESET_DIR, source_file)
        elif 'objects' in tileset['source']:
            source_file = os.path.basename(tileset['source'])
            tileset['source'] = os.path.join('..', SPRITESHEET_DIR, source_file)

    with open(data_file, 'w') as f:
        f.write(json.dumps(j, indent=2, ensure_ascii=False))

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
    except IOError as err:
        print('Failed to copy sheet file: %s' % str(err))
        sys.exit(2)
    except Exception as err:
        print('Rumtime exception: %s' % str(err))
        sys.exit(2)

if __name__ == "__main__":
    main()
