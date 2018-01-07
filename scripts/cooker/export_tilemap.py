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

# get the min and max gid values in a list.
# raw gid is extracted - the bits for flipping are removed
def gid_min_max(data):
    if not data:
        return None, None

    FLIP_BITS = 0x80000000 | 0x40000000 | 0x20000000

    maximum = 0
    minimum = sys.maxint

    for item in data:
        gid = int(item)
        if gid == 0:
            continue
        gid &= ~FLIP_BITS
        if gid > maximum:
            maximum = gid
        if gid < minimum:
            minimum = gid

    return minimum, maximum

# ensure that the min/max gid values from a layer map to a single tileset
def layer_tileset_mapping(tilesets, min_gid, max_gid):
    if min_gid is None or max_gid is None:
        return None, None

    last_index = len(tilesets) - 1
    for index, tileset in enumerate(tilesets):
        first_gid = tileset['firstgid']
        if index == last_index:
            if min_gid >= first_gid:
                return index, first_gid
        else:
            last_gid = tilesets[index + 1]['firstgid']
            if min_gid >= first_gid and max_gid < last_gid:
                return index, first_gid

    raise Exception('Layer maps to multiple tilesets')

# remove the firstgid offset from the values in the list.
def remove_gid_offset(data, first_gid):
    if not data:
        return

    for index, gid in enumerate(data):
        if gid == 0:
            data[index] = -1
        else:
            data[index] -= first_gid

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

    with open(data_file) as json_data:
        j = json.load(json_data)

    if 'type' not in j or j['type'] != "map":
        raise Exception('Invalid tilemap file format')

    translate_properties(j)

    # 2) Normalize and validate layer data
    for layer in j['layers']:
        translate_properties(layer)

        # Tiled x/y values are unused
        layer.pop('x')
        layer.pop('y')

        # Both an objectgroup and tilelayer may contain a list of gid's that map
        # to a tile id of any tileset.  Changes include:
        #
        # - Ensure that a layer doesn't reference multiple tilesets
        #   - Retreive the min/max gid values from a layer
        #   - Using the min/max gid values, retrieve the tileset index and firstgid
        #
        # - The new layer definition will contain:
        #   - A tileset index
        #   - A data array with the firstgid offset removed from all entries
        if layer['type'] == 'objectgroup':
            data = []
            for obj in layer['objects']:
                translate_object(obj)
                if 'gid' in obj:
                    data.append(obj['gid'])
            min_gid, max_gid = gid_min_max(data)
            ts_index, first_gid = layer_tileset_mapping(j['tilesets'], min_gid, max_gid)
            if ts_index is not None:
                layer['tileset_id'] = ts_index
                for obj in layer['objects']:
                    if 'gid' in obj:
                        obj['gid'] -= first_gid
        elif layer['type'] == 'tilelayer':
            if 'data' in layer and layer['data']:
                min_gid, max_gid = gid_min_max(layer['data'])
                ts_index, first_gid = layer_tileset_mapping(j['tilesets'], min_gid, max_gid)
                layer['tileset_id'] = ts_index
                remove_gid_offset(layer['data'], first_gid)
            elif 'chunks' in layer and layer['chunks']:
                agg_max = 0
                agg_min = sys.maxint
                for chunk in layer['chunks']:
                    min_gid, max_gid = gid_min_max(chunk['data'])
                    if min_gid is not None and min_gid < agg_min:
                        agg_min = min_gid
                    if max_gid is not None and max_gid > agg_max:
                        agg_max = max_gid
                ts_index, first_gid = layer_tileset_mapping(j['tilesets'], agg_min, agg_max)
                layer['tileset_id'] = ts_index
                for chunk in layer['chunks']:
                    remove_gid_offset(chunk['data'], first_gid)

    # 3) Update tileset reference paths
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
