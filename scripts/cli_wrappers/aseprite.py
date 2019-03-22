import os
import subprocess
import getopt

from utils import rwops

class AsepriteCLI:
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
        self.__PATH = os.environ.get('RDGE_ASEPRITE_PATH')
        if self.__PATH is None:
            raise Exception('EnvVar \"RDGE_ASEPRITE_PATH\" is not set')


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
        in_file = rwops.get_file(in_file)
        out_path = rwops.get_dir(out_path)

        # Determine what the output files are, and check if the input file has a newer
        # modified date.  Skip the process entirely if possible.
        #
        # Note:  These options accept arguments that have dynamic formatting (which doesn't
        #        work for shit) that we don't support.  This'll fail if used.
        out_files = []
        for o in cli_options:
            opt, arg = o
            if (opt == '--sheet') or (opt == '--data') or (opt == '--save-as'):
                if '{' in arg:
                    raise Exception('Dynamic formatting not supported')
                out_files.append(os.path.join(out_path, arg))

        if len(out_files) == 0:
            raise Exception('Invalid options.  No output file(s) provided.')

        if not rwops.is_dirty(in_file, out_files):
            return False

        cmd = self.__generate_command(in_file, cli_options)
        self.__validate_command(cmd)
        p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, cwd=out_path)
        stdout = p.communicate()
        print stdout
        if p.returncode is not 0:
            raise Exception('Aseprite call failed.  code=%s' % os.strerror(code))

        return True
