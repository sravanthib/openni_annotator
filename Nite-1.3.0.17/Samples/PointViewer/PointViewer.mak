SRC_FILES = \
	../PointViewer/main.cpp \
	../PointViewer/PointDrawer.cpp

INC_DIRS += ../PointViewer

EXE_NAME = Sample-PointViewer

DEFINES = USE_GLUT
USED_LIBS = glut

include ../NiteSampleMakefile

