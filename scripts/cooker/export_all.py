import getopt
import json
import os
import sys

import export_animation
import export_font
import export_objectsheet
import export_tilemap
import export_tileset

def usage():
    print ""
    print "Export all resources"
    print ""
    print "Usage:"
    print "export_all.py -f <config.json>"

def is_modified_after(f, t):
    if os.path.isfile(f):
        return (os.path.getmtime(f) > t)
    else:
        raise IOError('File does not exist: %s' % f)

def main():
    try:
        opts, args = getopt.getopt(sys.argv[1:], "hf:v", ["help", "file="])
    except getopt.GetoptError as err:
        print str(err)
        usage()
        sys.exit(2)

    config_file = ""
    for opt, arg in opts:
        if opt in ("-h", "--help"):
            usage()
            sys.exit()
        elif opt in ("-f", "--file"):
            config_file = arg.strip()

    if config_file == "":
        print "Missing required fields"
        usage()
        sys.exit(2)

    if not os.path.isfile(config_file):
        print "Cannot find config file"
        sys.exit(2)

    skip_asset_packer = True
    try:
        with open(config_file) as json_data:
            j = json.load(json_data)

        cooker = j['cooker']
        export_dir = cooker['export_dir']
        if not os.path.isdir(export_dir):
            raise Exception('Cannot find export directory')

        pack_file = j["packer"]["data_file"]
        pack_file_modified = 0
        if os.path.isfile(pack_file):
            pack_file_modified = os.path.getmtime(pack_file)

        config_dir = os.path.dirname(config_file)
        for f in cooker['animations']:
            animation_file = os.path.join(config_dir, f)
            if is_modified_after(animation_file, pack_file_modified):
                export_animation.process(animation_file, export_dir)
                skip_asset_packer = False
            else:
                print('Skipped cooking: %s' % (f))

        for f in cooker['fonts']:
            font_file = os.path.join(config_dir, f)
            if is_modified_after(font_file, pack_file_modified):
                export_font.process(font_file, export_dir)
                skip_asset_packer = False
            else:
                print('Skipped cooking: %s' % (f))

        for f in cooker['objectsheets']:
            objectsheet_file = os.path.join(config_dir, f)
            if is_modified_after(objectsheet_file, pack_file_modified):
                export_objectsheet.process(objectsheet_file, export_dir)
                skip_asset_packer = False
            else:
                print('Skipped cooking: %s' % (f))

        for f in cooker['tilemaps']:
            tilemap_file = os.path.join(config_dir, f)
            if is_modified_after(tilemap_file, pack_file_modified):
                export_tilemap.process(tilemap_file, export_dir)
                skip_asset_packer = False
            else:
                print('Skipped cooking: %s' % (f))

        for f in cooker['tilesets']:
            tileset_file = os.path.join(config_dir, f)
            if is_modified_after(tileset_file, pack_file_modified):
                export_tileset.process(tileset_file, export_dir)
                skip_asset_packer = False
            else:
                print('Skipped cooking: %s' % (f))

    except ValueError as err:
        print('ValueError: %s' % str(err))
        sys.exit(2)
    except IOError as err:
        print('IOError: %s' % str(err))
        sys.exit(2)
    except Exception as err:
        print('Rumtime exception: %s' % str(err))
        sys.exit(2)

    if skip_asset_packer:
        print "All files skipped."
        sys.exit(1)

if __name__ == "__main__":
    main()
