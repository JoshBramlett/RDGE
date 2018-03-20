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
        print "Cannot file config file"
        sys.exit(2)

    try:
        with open(config_file) as json_data:
            j = json.load(json_data)

        cooker = j['cooker']
        export_dir = cooker['export_dir']
        if not os.path.isdir(export_dir):
            raise Exception('Cannot find export directory')

        config_dir = os.path.dirname(config_file)
        for f in cooker['animations']:
            export_animation.process(os.path.join(config_dir, f), export_dir)

        for f in cooker['fonts']:
            export_font.process(os.path.join(config_dir, f), export_dir)

        for f in cooker['objectsheets']:
            export_objectsheet.process(os.path.join(config_dir, f), export_dir)

        for f in cooker['tilemaps']:
            export_tilemap.process(os.path.join(config_dir, f), export_dir)

        for f in cooker['tilesets']:
            export_tileset.process(os.path.join(config_dir, f), export_dir)
    except ValueError as err:
        print('ValueError: %s' % str(err))
        sys.exit(2)
    except IOError as err:
        print('IOError: %s' % str(err))
        sys.exit(2)
    except Exception as err:
        print('Rumtime exception: %s' % str(err))
        sys.exit(2)

if __name__ == "__main__":
    main()
