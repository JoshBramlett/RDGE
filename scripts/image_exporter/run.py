import getopt
import json
import os
import sys

from cli_wrappers import aseprite

def process(config_file):
    skip_asset_packer = True
    try:
        with open(config_file) as json_data:
            j = json.load(json_data)

        exporter = j['image_exporter']
        export_path = exporter['export_path']
        if not os.path.isdir(export_path):
            raise Exception('Cannot find export directory')

        num_processed = 0
        num_files = 0

        ase_cli = aseprite.AsepriteCLI()
        subdirectories = exporter['subdirectories']
        for subdir in subdirectories:
            name = subdir['name']
            import_path = subdir['import_path']
            options = subdir['options']
            files = subdir['files']
            subdir_export_path = os.path.join(export_path, name)

            print('Processing subdirectory: %s' % (name))
            for f in files:
                num_files += 1
                import_file = os.path.join(import_path, f)
                if ase_cli.execute(import_file, options, subdir_export_path):
                    num_processed += 1

        print('\n-----------------------------')
        print('Execution complete')
        print('  Skipped:   {0}'.format(num_files - num_processed))
        print('  Processed: {0}'.format(num_processed))

    except ValueError as err:
        print('ValueError: %s' % str(err))
        sys.exit(1)
    except IOError as err:
        print('IOError: %s' % str(err))
        sys.exit(1)
    except Exception as err:
        print('Rumtime exception: %s' % str(err))
        sys.exit(1)

def print_usage(message=None, exit_code=0):
    if message:
        print("ERROR: {0}".format(message))
    print ""
    print "Export all image resources"
    print ""
    print "Usage:"
    print 'run.py [-hv] [file]'
    sys.exit(exit_code)

def main():
    try:
        opts, args = getopt.getopt(sys.argv[1:], "h:v", ["help"])
    except getopt.GetoptError as err:
        print_usage(err, 2)

    verbose = False
    for opt, arg in opts:
        if opt == "-v":
            verbose = True
        if opt in ("-h", "--help"):
            print_usage()
        else:
            print_usage('Unsupported command line syntax', 2)

    if not len(args) == 1:
        print_usage('Unsupported command line syntax', 2)

    config_file = args[0]
    if not os.path.isfile(config_file):
        print_usage('Cannot find config file', 2)

    process(config_file)


if __name__ == "__main__":
    main()
