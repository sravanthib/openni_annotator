SRC_FILES = \
	../Boxes/main.cpp \
	../Boxes/kbhit.cpp \
	../Boxes/signal_catch.cpp

INC_DIRS += ../Boxes

USED_LIBS = glut
DEFINES = USE_GLUT



EXE_NAME = Sample-Boxes
include ../NiteSampleMakefile

