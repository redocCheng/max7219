from building import *

# get current directory
cwd     = GetCurrentDir()
# The set of source files associated with this SConscript file.
src     = Glob('src/*.c')
src     += Glob('port/*.c')

path    = [cwd + '/inc']

group = DefineGroup('Drivers', src, depend = ['PKG_USING_MAX7219'], CPPPATH = path)

Return('group')
