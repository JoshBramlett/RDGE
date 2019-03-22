import getopt
import json
import os
import subprocess
import sys

from cli_wrappers import aseprite
# from cookers import animation_chef
# from cookers import font_chef
# from cookers import objectsheet_chef
# from cookers import objecttype_chef
# from cookers import tilemap_chef
# from cookers import tileset_chef
# from cookers import ui_chef
from utils import rwops

g_verbose = False
def print_verbose(message):
    global g_verbose
    if g_verbose == True:
        print(message)

def print_usage(message=None, exit_code=0):
    if message:
        print("ERROR: {0}".format(message))
    print ""
    print "Executes automated functionality for different steps in the asset pipeline."
    print "All execution is defined via config file, which includes support for the following:"
    print ""
    print "  Image export [-e, --export]"
    print "      Takes the image editing source files and generates the appropriate raw"
    print "      image and data files.  Exported data files can represent definitions for"
    print "      animations, splicing, etc."
    print "  Cooker [-c, --cook]"
    print "      Takes the raw asset files and prepares them for asset packing.  The asset"
    print "      packer has a rigid set of constraints, so the cooker will combine data"
    print "      files, generate spritesheets, etc."
    print "  Asset Packer [-p, --pack]"
    print "      Combines all asset files into a single binary.  This requires the"
    print "      asset_packer executable to have been previously built. "
    print ""
    print "One of the above tasks must be specified, or alternatively they all can be ran"
    print "by specifying [-a, --all]."
    print ""
    print "Usage:"
    print 'asset_pipeline.py [-hvecpa] [config_path]'
    sys.exit(exit_code)

def execute_image_exporter(j_config):
    print('Starting image exporter...')
    j_image_exporter = j_config['image_exporter']
    j_items = j_image_exporter['items']
    j_export = j_image_exporter['export']
    export_path = rwops.get_dir(j_export['path'])

    print_verbose('ImageExporter config:')
    print_verbose('  export_path: {0}'.format(export_path))
    print_verbose('  items: {0}'.format(len(j_items)))

    ase_cli = aseprite.AsepriteCLI()
    total_processed = 0
    total_files = 0

    for j_item in j_items:
        chef = j_item['chef']
        name = j_item['name']
        import_path = j_item['import_path']
        files = j_item['files']

        # conflate command line options
        options = j_item['options']
        common_options = j_export['common_chef_options'][chef]
        options.extend(common_options)

        # Generate the path where the command will be executed from.  The
        # item_export_path is common to all files in the item and has the format
        #   {export.path}/{item[].chef}/{item[].name}
        #
        # The split basename of each file will be later added to the above path
        item_export_path = os.path.join(export_path, chef, name)

        print('Processing item: {0}::{1}'.format(chef, name))
        print_verbose('  import_path: {0}'.format(import_path))
        print_verbose('  item_export_path: {0}'.format(item_export_path))
        print_verbose('  files: {0}'.format(len(files)))

        file_index = 0
        num_processed = 0
        for f in files:
            file_index += 1
            import_file = os.path.join(import_path, f)
            print_verbose("[{0}] File: {1}".format(file_index, import_file))

            bn = rwops.pure_basename(f)
            file_export_path = os.path.join(item_export_path, bn)
            rwops.try_mkdir(file_export_path)

            if ase_cli.execute(import_file, options, file_export_path):
                num_processed += 1
            else:
                print_verbose('File export skipped')

        total_files += len(files)
        total_processed += num_processed
        print_verbose('Completed item: {0}::{1}'.format(chef, name))
        print_verbose('  Processed {0} of {1} files.'.format(num_processed, len(files)))

    print('Image exporter complete')
    print('  Skipped:   {0}'.format(total_files - total_processed))
    print('  Processed: {0}'.format(total_processed))

def execute_cooker(j_config):
    print('Starting cooker...')
    j_cooker = j_config['cooker']
    j_import = j_cooker['import']
    j_export = j_cooker['export']

    j_chefs = j_import['chefs']
    import_path = rwops.get_dir(j_import['path'])
    export_path = rwops.get_dir(j_export['path'])

    print_verbose('Cooker config:')
    print_verbose('  import_path: {0}'.format(import_path))
    print_verbose('  export_path: {0}'.format(export_path))
    print_verbose('  chefs: {0}'.format(len(j_chefs)))

    pack_file = j_config["packer"]["data_file"]
    total_processed = 0
    total_files = 0

    for j_chef in j_chefs:
        chef_name = j_chef['name']
        files = j_chef['files']

        print('Processing chef: %s' % (chef_name))
        print_verbose('  files: {0}'.format(len(files)))

        file_index = 0
        num_processed = 0
        for f in files:
            file_index += 1
            import_file = os.path.join(import_path, f)
            print_verbose("[{0}] File: {1}".format(file_index, import_file))

            if not rwops.is_dirty(pack_file, [import_file]):
                print('Skipped cooking: %s' % (f))
                continue

            num_processed += 1
            # if chef_name == 'animations':
                # animation_chef.cook(import_file, export_path)
            # elif chef_name == 'fonts':
                # font_chef.cook(import_file, export_path)
            # elif chef_name == 'objectsheets':
                # objectsheet_chef.cook(import_file, export_path)
            # elif chef_name == 'objecttypes':
                # objecttype_chef.cook(import_file, export_path)
            # elif chef_name == 'tilemaps':
                # tilemap_chef.cook(import_file, export_path)
            # elif chef_name == 'tilesets':
                # tileset_chef.cook(import_file, export_path)
            # elif chef_name == 'ui':
                # ui_chef.cook(import_file, export_path)
            # else:
                # raise Exception('Subdirectory unsupported: %s' % (chef_name))

        total_files += len(files)
        total_processed += num_processed
        print_verbose('Completed chef: {0}'.format(chef_name))
        print_verbose('  Processed {0} of {1} files.'.format(num_processed, len(files)))

    print('Cooker complete')
    print('  Skipped:   {0}'.format(total_files - total_processed))
    print('  Processed: {0}'.format(total_processed))

    return (total_processed > 0)

def execute_asset_packer(j_config, config_path):
    # TODO remove
    print('Avoid calling asset packer...')
    return

    print('Starting packer...')
    j_packer = j_config['packer']

    cmd = j_packer['executable']
    cmd += ' ' + config_path
    cmd += ' --silent'      # Suppress yes/no confirmation

    code = subprocess.call(cmd, shell=True)
    if code is not 0:
        raise Exception('asset_packer call failed.  code=%s' % str(code))

def main():
    try:
        opts, args = getopt.getopt(sys.argv[1:], "hecpa:v", ["help", "export", "cook", "pack", "all"])
    except getopt.GetoptError as err:
        print_usage(err, 2)

    run_image_exporter = False
    run_cooker = False
    run_asset_packer = False
    for opt, arg in opts:
        if opt == "-v":
            global g_verbose
            g_verbose = True
        elif opt in ("-h", "--help"):
            print_usage()
        elif opt in ("-e", "--export"):
            run_image_exporter = True
        elif opt in ("-c", "--cook"):
            run_cooker = True
        elif opt in ("-p", "--pack"):
            run_asset_packer = True
        elif opt in ("-a", "--all"):
            run_image_exporter = True
            run_cooker = True
            run_asset_packer = True
        else:
            print_usage('Unsupported command line syntax', 2)

    if not len(args) == 1:
        print_usage('Config file not provided', 2)

    has_task = (run_image_exporter == True) or (run_cooker == True) or (run_asset_packer == True)
    if not has_task:
        print_usage('No task provided', 2)

    try:
        print_verbose('VERBOSE=TRUE')
        config_path = rwops.get_file(args[0])
        with open(config_path) as json_data:
            j_config = json.load(json_data)

        if run_image_exporter == True:
            execute_image_exporter(j_config)
            print('\n-----------------------------')

        if run_cooker == True:
            # Give cooker a chance to suppress packing if it's not neccessary
            packing_needed = execute_cooker(j_config)
            if run_asset_packer:
                run_asset_packer = packing_needed
            print('\n-----------------------------')

        if run_asset_packer == True:
            execute_asset_packer(j_config, config_path)
            print('\n-----------------------------')

        print("Asset pipline request complete")

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
