import getopt
import json
import os
import sys
import subprocess

from export_common import *

def usage():
    print ""
    print "Builds a spritesheet from animation frames"
    print ""
    print "Creates a spritesheet from a collection of animation frames."
    print "Assumes a TexturePacker tps file and a json file defining the animations"
    print "are available.  Animation data is written to the generated data file."
    print ""
    print "Data file will be written to <output_dir>/spritesheets"
    print "Texture file will be written to <output_dir>/images"
    print ""
    print "Note: Requires TexturePacker command line tools."
    print ""
    print "Usage:"
    print "export_animation.py -f <animation.tps> -o <output_dir>"

# ./aseprite -b ~/Documents/assets/ase_chrono/autumn_front.aseprite --filename-format '{path}/{tag}_{title}_{tagframe00}.{extension}' --save-as ~/Documents/assets/ase_chrono/autumn/front.png

# ./aseprite -b ~/Documents/assets/ase_chrono/autumn_front.aseprite --data ~/Documents/assets/ase_chrono/autumn/autumn_front.json --list-layers --list-tags --format json-array --filename-format 'front_{tag}_{tagframe00}'

    # cmd = ASEPRITE_PATH
    # cmd += ' -b'                # force write even if unchanged
    # cmd += aseprite_file        # texture file location/filename

    # filename-format
    # cmd += ' --filename-format'
    # cmd += ' {path}/{tag}_{title}_{tagframe00}.{extension}'

    # save-as
    # cmd += ' --save-as'
    # cmd += ' '


    # cmd += ' --data ' + data_file                             # data file location/filename
    # cmd += ' --texturepath ' + os.path.join('..', IMAGE_DIR)  # texture file path

def merge_data(data_file, anim_file):
    with open(data_file) as json_data:
        j_data = json.load(json_data)
    with open(anim_file) as json_anim:
        j_anim = json.load(json_anim)

    # Very inefficient way to verify animations reference valid frames
    for a in j_anim['animations']:
        for af in a['frames']:
            found = False
            for df in j_data['frames']:
                if af['name'] == df['filename']:
                    found = True
                    break
            if found == False:
                raise Exception('Frame not found: ' + af['name'])

    j_data['meta']['sheet_type'] = 'spritesheet'
    j_data.update(j_anim)

    with open(data_file, 'w') as f:
        f.write(json.dumps(j_data, indent=2, ensure_ascii=False))

def process(tps_file, out_dir):
    print('Exporting Animation')
    print('    file: %s' % (tps_file))
    print('    out:  %s' % (out_dir))

    if not os.path.isfile(tps_file):
        raise Exception('Cannot find tps file. file=%s' % tps_file)

    anim_file = os.path.splitext(tps_file)[0] + '.json'
    if not os.path.isfile(anim_file):
        raise Exception('Cannot find animation file. file=%s' % anim_file)

    if not os.path.isdir(out_dir):
        raise Exception('Cannot find output directory')

    out_dir = os.path.abspath(out_dir)
    sheet_dir = os.path.join(out_dir, IMAGE_DIR)
    data_dir = os.path.join(out_dir, SPRITESHEET_DIR)
    if not try_mkdir(sheet_dir) or not try_mkdir(data_dir):
        raise Exception('Unable to open/create child directories')

    base_name = os.path.splitext(os.path.basename(tps_file))[0]
    sheet_file = os.path.join(sheet_dir, base_name + '.png')
    data_file = os.path.join(data_dir, base_name + '.json')

    cmd = TEXTURE_PACKER_PATH
    cmd += ' --force-publish'        # force write even if unchanged
    cmd += ' --sheet ' + sheet_file  # texture file location/filename

    # data file
    cmd += ' --format json-array'                             # data file format
    cmd += ' --data ' + data_file                             # data file location/filename
    cmd += ' --texturepath ' + os.path.join('..', IMAGE_DIR)  # texture file path

    # data file: frame naming convention
    cmd += ' --trim-sprite-names'    # remove file extension from names
    cmd += ' --prepend-folder-name'  # add directory structure to the names
    cmd += ' --replace [\/]=_'       # replace path separators in names with underscores

    # misc
    cmd += ' --algorithm MaxRects'   # best algorithm for rectangle packing
    cmd += ' --trim-mode Trim'       # maintain size, remove surrounding transparency
    cmd += ' --extrude 0'            # don't allow extension of border pixels
    cmd += ' --padding 2'            # 2 pixel padding between sprites
    # cmd += ' --enable-rotation'
    # cmd += ' --disable-rotation'

    cmd += ' ' + tps_file

    # 1) generate spritesheet and data file
    code = subprocess.call(cmd, shell=True)
    if code is not 0:
        raise Exception('TexturePacker call failed.  code=%s' % str(code))

    # 2) merge generated spritesheet data file with animation data
    merge_data(data_file, anim_file)

def main():
    try:
        opts, args = getopt.getopt(sys.argv[1:], "hf:o:v", ["help", "file=", "output="])
    except getopt.GetoptError as err:
        print str(err)
        usage()
        sys.exit(2)

    tps_file = ""
    out_dir = ""
    for opt, arg in opts:
        if opt in ("-h", "--help"):
            usage()
            sys.exit()
        elif opt in ("-f", "--file"):
            tps_file = arg.strip()
        elif opt in ("-o", "--output"):
            out_dir = arg.strip()

    if tps_file == "" or out_dir == "":
        print "Missing required fields"
        usage()
        sys.exit(2)

    try:
        process(tps_file, out_dir)
    except ValueError as err:
        print('ValueError: %s' % str(err))
        sys.exit(2)
    except Exception as err:
        print('Rumtime exception: %s' % str(err))
        sys.exit(2)

if __name__ == "__main__":
    main()
