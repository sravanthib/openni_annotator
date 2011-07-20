SRC_FILES = \
	../CircleControl/main.cpp \
	../CircleControl/kbhit.cpp \
	../CircleControl/signal_catch.cpp

INC_DIRS += ../CircleControl

DEFINES = USE_GLUT
USED_LIBS = glut



EXE_NAME = Sample-CircleControl
include ../NiteSampleMakefile

