# SCons buildfile for skeloton
import os
from os.path import expanduser

home = expanduser("~")
MINIATDIR = home + "/miniat/out"

# this makes it so that id doesn't matter if your #includes have 'SDL' in front of them or not
FLAGS = ['-D_REENTRANT',
         '-std=c++11',
         '-I/usr/include/SDL2/',
         '-I' + MINIATDIR + '/vm/inc',    # miniat include directory
		 '-I' + MINIATDIR + '/vm/inc/miniat'
        ]

LIBDIRS = [
		   '-L' + MINIATDIR + '/lib'        # miniat library directory
          ]
		
# set environment variable flags for compiling
env = Environment(CC = 'g++',
                  CCFLAGS = FLAGS,
				  LIBPATH = LIBDIRS,
                  LIBS = ['SDL2', 'SDL2_image', 'SDL2_ttf', 'SDL2_mixer', 'libminiat'])
                  
# compile the program
env.Program(['main.cpp', 'Sprite.cpp', 'peripherals.cpp', 'ports.cpp'])

# copy dll files

