Import('RTT_ROOT')
Import('rtconfig')
from building import *

cwd     = GetCurrentDir()
src     = Glob('button.c') 

CPPPATH = [cwd]

group = DefineGroup('button_drive', src, depend = ['PKG_USING_BUTTON'], CPPPATH = CPPPATH)

Return('group')
