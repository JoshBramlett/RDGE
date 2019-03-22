import getopt
import json
import os
import sys
from shutil import copyfile

from export_common import *

def usage():
    print ""
    print "Export a Tiled Map Editor single image tileset"
    print ""
    print "Used with tilesets that are created from a single image."
    print "Script normalizes the data file and writes to the export directory tree."
    print ""
    print "Data file will be written to <output_dir>/tilesets"
    print "Texture file will be written to <output_dir>/images"
    print ""
    print "Note: Any tile collision data will be discarded."
    print ""
    print "Usage:"
    print "export_tileset.py -f <tileset.tsx> -o <output_dir>"

def process(in_file, out_dir):
    print('Exporting Tileset')
    print('    file: %s' % (in_file))
    print('    out:  %s' % (out_dir))

    if not os.path.isfile(in_file):
        raise Exception('Cannot find input file')

    if not os.path.isdir(out_dir):
        raise Exception('Cannot find output directory')

    out_dir = os.path.abspath(out_dir)
    data_dir = os.path.join(out_dir, TILESET_DIR)
    sheet_dir = os.path.join(out_dir, IMAGE_DIR)
    if not try_mkdir(data_dir) or not try_mkdir(sheet_dir):
        raise Exception('Unable to open/create child directories')

    base_name = os.path.splitext(os.path.basename(in_file))[0]
    sheet_file = os.path.join(sheet_dir, base_name + '.png')
    data_file = os.path.join(data_dir, base_name + '.json')

    # 1) Convert, normalize and verify data file
    invoke_tiled_export('--export-tileset json', in_file, data_file)

    with open(data_file) as json_data:
        tileset = json.load(json_data)

    if 'type' not in tileset or tileset['type'] != "tileset":
        raise Exception('Invalid tileset file format')

    original_sheet_file = os.path.join(os.path.dirname(in_file), tileset['image'])
    if not os.path.isfile(original_sheet_file):
        raise Exception('Cannot read original sheet.  file=%s' % original_sheet_file)

    tileset['image'] = os.path.join('..', IMAGE_DIR, os.path.basename(sheet_file))
    verify_properties(tileset)

    with open(data_file, 'w') as f:
        f.write(json.dumps(tileset, indent=2, ensure_ascii=False))

    # 2) Export sheet file
    copyfile(original_sheet_file, sheet_file)

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
