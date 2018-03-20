import getopt
import json
import os
import sys
from shutil import copyfile

from export_common import *

def usage():
    print ""
    print "Export a bmfont generated Bitmap Font"
    print ""
    print "Used with libgdx Heiro tool for generating bitmap fonts."
    print ""
    print "Data file will be written to <output_dir>/" + FONT_DIR
    print "Texture file will be written to <output_dir>/" + IMAGE_DIR
    print ""
    print "Usage:"
    print "export_font.py -f <font.fnt> -o <output_dir>"

def parse_bmfont_info(line, info_data):
    tokens = line.split()
    for token in tokens:
        if '=' in token:
            kv = token.split('=')
            if kv[0] == 'padding':
                padding_values = kv[1].split(',')
                info_data['padding'] = {}
                info_data['padding']['top'] = int(padding_values[0])
                info_data['padding']['right'] = int(padding_values[1])
                info_data['padding']['bottom'] = int(padding_values[2])
                info_data['padding']['left'] = int(padding_values[3])
            elif kv[0] == 'spacing':
                spacing_values = kv[1].split(',')
                info_data['spacing'] = {}
                info_data['spacing']['w'] = int(spacing_values[0])
                info_data['spacing']['h'] = int(spacing_values[0])
            elif kv[0] == 'face' or kv[0] == 'charset':
                info_data[kv[0]] = kv[1].replace('"', '')
            elif (kv[0] == 'bold' or kv[0] == 'italic' or
                 kv[0] == 'unicode' or kv[0] == 'smooth'):
                info_data[kv[0]] = bool(kv[1])
            else:
                info_data[kv[0]] = int(kv[1])

def parse_bmfont_common(line, common_data):
    tokens = line.split()
    for token in tokens:
        if '=' in token:
            kv = token.split('=')
            common_data[kv[0]] = int(kv[1])

def parse_bmfont_page(line, pages):
    page_data = {}
    tokens = line.split()
    for token in tokens:
        if '=' in token:
            kv = token.split('=')
            if kv[0] == 'id':
                page_data[kv[0]] = int(kv[1])
            elif kv[0] == 'file':
                page_data[kv[0]] = kv[1].replace('"', '')
    pages.append(page_data)

def parse_bmfont_char(line, chars):
    char_data = {}
    tokens = line.split()
    for token in tokens:
        if '=' in token:
            kv = token.split('=')
            char_data[kv[0]] = int(kv[1])
    chars.append(char_data)

def convert_bmfont_to_json(in_file):
    font_data = {}
    font_data['info'] = {}
    font_data['common'] = {}
    font_data['pages'] = []
    font_data['chars'] = []
    with open(in_file) as fp:
        lines = fp.read().split("\n")
        for line in lines:
            if line.startswith("info "):
                parse_bmfont_info(line, font_data['info'])
            if line.startswith("common "):
                parse_bmfont_common(line, font_data['common'])
            if line.startswith("page "):
                parse_bmfont_page(line, font_data['pages'])
            if line.startswith("char "):
                parse_bmfont_char(line, font_data['chars'])
    return font_data

def add_heiro_effects(in_file, font_data):
    # add all effect metadata to the font data file
    font_data['effects'] = []
    with open(in_file) as fp:
        lines = iter(fp.read().split("\n"))
        for line in lines:
            if line.startswith("effect"):
                effect = {}
                kv = line.split('=')
                key = kv[0].split('.')[-1].lower()
                if key == 'class':
                    effect['class'] = kv[1].split('.')[-1]
                    while True:
                        l = next(lines)
                        if '=' not in l:
                            break
                        kv = l.split('=')
                        key = kv[0].split('.')[-1].lower()
                        effect[key] = kv[1]
                    font_data['effects'].append(effect);

    # effect specific type conversions
    for effect in font_data['effects']:
        if effect['class'] == 'DistanceFieldEffect':
            effect['spread'] = float(effect['spread'])
            effect['scale'] = int(effect['scale'])

def process(in_file, out_dir):
    print('Exporting Font')
    print('    file: %s' % (in_file))
    print('    out:  %s' % (out_dir))

    if not os.path.isfile(in_file):
        raise Exception('Cannot find input file')

    if not os.path.isdir(out_dir):
        raise Exception('Cannot find output directory')

    in_dir = os.path.dirname(in_file)
    out_dir = os.path.abspath(out_dir)
    data_dir = os.path.join(out_dir, FONT_DIR)
    sheet_dir = os.path.join(out_dir, IMAGE_DIR)
    if not try_mkdir(data_dir) or not try_mkdir(sheet_dir):
        raise Exception('Unable to open/create child directories')

    # The directory of the in_file should contain 3 files:
    # a) fontname.heiro - Export configuration of the libgdx Heiro tool
    #
    # On Heiro export, the following is generated:
    # b) fontname.fnt   - BMFont formatted data file
    # c) fontname.png   - Texture of exported glyphs
    #
    # The data file exported is the bmfont file converted to json, plus the
    # effects data from the hiero config file.  Ideally the export directory
    # could only have the config file and perform the export via command line
    # but the hiero command line tools aren't well documented and I don't
    # feel like spending the time to dig through it (plus it adds a JDK
    # dependency).

    # 1) Convert bmfont format to json
    font_data = convert_bmfont_to_json(in_file)

    # 2) Add effects from the hiero export file
    hiero_file = os.path.splitext(in_file)[0] + '.hiero'
    if os.path.isfile(hiero_file):
        add_heiro_effects(hiero_file, font_data)

    # 3) Export sheets, and update the paths in the data file
    for page in font_data['pages']:
        in_sheet = os.path.join(in_dir, page['file'])
        copyfile(in_sheet, os.path.join(sheet_dir, page['file']))
        page['file'] = os.path.join('..', IMAGE_DIR, page['file'])

    # 4) Export data/sheet file
    base_name = os.path.splitext(os.path.basename(in_file))[0]
    export_data_file = os.path.join(data_dir, base_name + '.json')
    with open(export_data_file, 'w') as f:
        f.write(json.dumps(font_data, indent=2, ensure_ascii=False))

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
