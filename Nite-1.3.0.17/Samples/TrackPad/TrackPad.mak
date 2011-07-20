SRC_FILES = \
	../TrackPad/main.cpp \
	../TrackPad/kbhit.cpp \
	../TrackPad/signal_catch.cpp

INC_DIRS += ../TrackPad

USED_LIBS = glut
DEFINES = USE_GLUT



EXE_NAME = Sample-TrackPad
include ../NiteSampleMakefile

