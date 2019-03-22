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
    print "Data file will be written to <output_dir>/" + TILEMAP_DIR
    print ""
    print "Usage:"
    print "export_tilemap.py -f <tilemap.tmx> -o <output_dir>"

# get the min and max gid values in a list.
# only the raw gid is compared - flipping bits are first extracted
def get_gid_range(data):
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

# get the tileset index from a gid range.
# ensures the min/max gid values from a layer reference a single tileset
def get_tileset(tilesets, min_gid, max_gid):
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
# performed after confirmation all cells within a layer use the same
# tileset, this makes the gid values locally index said tileset.
#
# note: gid of zero signifies no mapping, therefore valid tile
#       indicies start at 1
def remove_gid_offset(data, first_gid):
    if not data:
        return

    for index, gid in enumerate(data):
        if gid != 0:
            data[index] -= (first_gid - 1)

# for infinite maps there is no clear way to decipher the global grid, as the
# chunk data is sparse.  Making all layers relative to a global grid is easier
# when culling the rendered cells to the camera bounds.
def create_global_grid(tilemap):
    left = sys.maxint
    top = sys.maxint
    right = 0
    bottom = 0
    chunk_width = 0
    chunk_height = 0
    if 'infinite' in tilemap and tilemap['infinite'] == True:
        if 'layers' not in tilemap:
            raise Exception('Tilemap has no layers')
        for layer in tilemap['layers']:
            if layer['type'] == 'tilelayer':
                if 'chunks' not in layer:
                    continue
                if layer['startx'] < left:
                    left = layer['startx']
                    if (left + layer['width']) > right:
                        right = left + layer['width']
                if layer['starty'] < top:
                    top = layer['starty']
                    if (top + layer['height']) > bottom:
                        bottom = top + layer['height']
                for chunk in layer['chunks']:
                    if chunk_width == 0 and chunk_height == 0:
                        chunk_width = chunk['width']
                        chunk_height = chunk['height']
                    elif chunk_width != chunk['width'] or chunk_height != chunk['height']:
                        raise Exception('Chunk sizes differ')
                    chunk.pop('width')
                    chunk.pop('height')
    else:
        left = 0
        top = 0
        right = tilemap['width']
        bottom = tilemap['height']
        chunk_width = right
        chunk_height = bottom

    grid = {}
    grid['renderorder'] = tilemap['renderorder']
    grid['x'] = left
    grid['y'] = top
    grid['width'] = right - left
    grid['height'] = bottom - top
    grid['cells'] = { 'width': tilemap['tilewidth'], 'height': tilemap['tileheight'] }
    grid['chunks'] = { 'width': chunk_width, 'height': chunk_height }
    tilemap['grid'] = grid

    tilemap.pop('renderorder')
    tilemap.pop('tilewidth')
    tilemap.pop('tileheight')
    tilemap.pop('width')
    tilemap.pop('height')

def process(in_file, out_dir):
    print('Exporting Tilemap')
    print('    file: %s' % (in_file))
    print('    out:  %s' % (out_dir))

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

    # 1) Convert, normalize and verify data file
    invoke_tiled_export('--export-map json', in_file, data_file)

    with open(data_file) as json_data:
        tilemap = json.load(json_data)

    if 'type' not in tilemap or tilemap['type'] != "map":
        raise Exception('Invalid tilemap file format')

    verify_properties(tilemap)

    # Remove unused
    tilemap.pop('tiledversion')
    tilemap.pop('nextobjectid')

    # 2) Create a global grid for all tile layers
    #
    # Infinite maps have no clear way to decipher the global grid since the
    # chunk data is sparse.  Making all tile layers relative to a global grid
    # is easier when rendering, specifically when culling the rendered region
    # to the camera bounds.  Changes include:
    #
    # - Coalesce all tile layer regions to a single global region
    # - Ensure all chunks are the same size
    # - Clean up all repetitive/unused data
    #
    # - The new grid definition will contain:
    #   - The tile layer render order
    #   - A bounding box of the global region
    #   - cell size (in pixels)
    #   - chunk size (in cells)
    create_global_grid(tilemap)

    # 3) Normalize and validate layer data
    #
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
    for layer in tilemap['layers']:
        verify_properties(layer)

        # Remove unused
        layer.pop('x')
        layer.pop('y')

        if layer['type'] == 'objectgroup':
            data = []
            for obj in layer['objects']:
                translate_object(obj)
                if 'gid' in obj:
                    data.append(obj['gid'])
            min_gid, max_gid = get_gid_range(data)
            ts_index, first_gid = get_tileset(tilemap['tilesets'], min_gid, max_gid)
            if ts_index is not None:
                layer['tileset_index'] = ts_index
                for obj in layer['objects']:
                    if 'gid' in obj:
                        obj['gid'] -= (first_gid - 1)
        elif layer['type'] == 'tilelayer':
            if 'data' in layer and layer['data']:
                min_gid, max_gid = get_gid_range(layer['data'])
                ts_index, first_gid = get_tileset(tilemap['tilesets'], min_gid, max_gid)
                if ts_index is not None:
                    layer['tileset_index'] = ts_index
                    remove_gid_offset(layer['data'], first_gid)
            elif 'chunks' in layer and layer['chunks']:
                agg_max = 0
                agg_min = sys.maxint
                for chunk in layer['chunks']:
                    min_gid, max_gid = get_gid_range(chunk['data'])
                    if min_gid is not None and min_gid < agg_min:
                        agg_min = min_gid
                    if max_gid is not None and max_gid > agg_max:
                        agg_max = max_gid

                ts_index, first_gid = get_tileset(tilemap['tilesets'], agg_min, agg_max)
                if ts_index is not None:
                    layer['tileset_index'] = ts_index
                    for chunk in layer['chunks']:
                        remove_gid_offset(chunk['data'], first_gid)

    # 4) Update tileset reference paths
    for tileset in tilemap['tilesets']:
        if 'tilesets' in tileset['source']:
            source_file = os.path.splitext(os.path.basename(tileset['source']))[0] + '.json'
            tileset['source'] = os.path.join('..', TILESET_DIR, source_file)
        elif 'objects' in tileset['source']:
            source_file = os.path.splitext(os.path.basename(tileset['source']))[0] + '.json'
            tileset['source'] = os.path.join('..', SPRITESHEET_DIR, source_file)

    # 5) Add external referenced object types
    if 'properties' in tilemap:
        for prop in tilemap['properties']:
            if prop['name'] == 'object_types':
                if prop['type'] != 'file':
                    raise Exception('object_types property is not a file')

                tilemap_dir = os.path.dirname(in_file)
                obj_types_file = os.path.join(tilemap_dir, prop['value'])
                with open(obj_types_file) as json_data:
                    obj_types = json.load(json_data)

                tilemap['object_types'] = obj_types
                break

    with open(data_file, 'w') as f:
        f.write(json.dumps(tilemap, indent=2, ensure_ascii=False))

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
