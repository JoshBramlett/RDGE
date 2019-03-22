import os
import subprocess
import getopt

from utils import rwops

class TiledCLI:
    __SHORT_OPTIONS_ARGS = '?'
    __SHORT_OPTIONS = 'bvp'
    __LONG_OPTIONS = [ 'shell',
                        'batch',
                        'preview',
                        'sheet-pack',
                        'split-layers',
                        'split-tags',
                        'split-slices',
                        'all-layers',
                        'ignore-empty',
                        'trim',
                        'list-layers',
                        'list-tags',
                        'list-slices',
                        'oneframe',
                        'verbose',
                        'debug',
                        'help',
                        'version' ]
    __LONG_OPTIONS_ARGS = [ 'save-as=',
                            'palette=',
                            'scale=',
                            'dithering-algorithm=',
                            'dithering-matrix=',
                            'color-mode=',
                            'data=',
                            'format=',
                            'sheet=',
                            'sheet-width=',
                            'sheet-height=',
                            'layer=',
                            'import-layer=',
                            'ignore-layer=',
                            'frame-tag=',
                            'frame-range=',
                            'border-padding=',
                            'shape-padding=',
                            'inner-padding=',
                            'crop=',
                            'slice=',
                            'filename-format=',
                            'script=',
                            'script-param=' ]


    def __init__(self):
        self.__PATH = os.environ.get('RDGE_TILED_PATH')
        if self.__PATH is None:
            raise Exception('EnvVar \"RDGE_TILED_PATH\" is not set')


    def __format_option(self, opt, arg):
        arg = arg.strip()
        if ' ' in arg:
            arg = '\"' + arg + '\"'

        if opt.startswith('--'):
            long_opt = [o for o in self.__LONG_OPTIONS_ARGS if o.startswith(opt.lstrip('-'))]
            if long_opt:
                return opt + ' ' + arg
        else:
            try:
                i = opt.index(opt.lstrip('-'))
            except ValueError:
                pass
            else:
                return opt + ' ' + arg

        return opt


    def __generate_command(self, in_file, options):
        cmd = self.__PATH
        cmd += ' --batch'
        cmd += ' \"' + os.path.abspath(in_file) + '\"'
        for o in options:
            opt, arg = o
            cmd += ' ' + self.__format_option(opt, arg)

        return cmd


    def __validate_command(self, cmd):
        short_opts = self.__SHORT_OPTIONS_ARGS + ':' + self.__SHORT_OPTIONS
        long_opts = self.__LONG_OPTIONS_ARGS + self.__LONG_OPTIONS

        try:
            opts, args = getopt.getopt(cmd, short_opts, long_opts)
        except getopt.GetoptError as err:
            raise Exception('Command validation failed.  error=%s' % str(err))


    def execute(self, in_file, cli_options, out_path):
        print('Aseprite executing file: %s' % (in_file))
        in_file = rwops.get_file(in_file)

        # A hack to work-around aseprite CLI limitations:
        # We pre-format the output files to their explicit names b/c the 'filename-format'
        # option is inconsistent in it's meaning depending on how it's used.  Despite
        # the documentation the input file is not used when formatting, and when used with
        # the 'data' option the format will be applied to the json elements, not the
        # actual file name.
        out_files = []
        basename = os.path.splitext(os.path.basename(in_file))[0]
        for o in cli_options:
            opt, arg = o
            if (opt == '--sheet') or (opt == '--data') or (opt == '--save-as'):
                out_file = os.path.join(out_path, basename + '.' + arg)
                o[1] = out_file
                out_files.append(out_file)

        if len(out_files) == 0:
            raise Exception('Invalid options.  No output file(s) provided.')

        if not rwops.is_dirty(in_file, out_files):
            print('Execution skipped.')
            return False

        cmd = self.__generate_command(in_file, cli_options)
        self.__validate_command(cmd)
        code = subprocess.call(cmd, shell=True)
        if code is not 0:
            raise Exception('Aseprite call failed.  code=%s' % str(code))

        return True
