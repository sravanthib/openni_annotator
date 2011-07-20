SRC_FILES = \
	../SceneAnalysis/main.cpp \
	../SceneAnalysis/SceneDrawer.cpp \
	../SceneAnalysis/kbhit.cpp \
	../SceneAnalysis/signal_catch.cpp

INC_DIRS += ../SceneAnalysis

EXE_NAME = Sample-SceneAnalysis

DEFINES = USE_GLUT
USED_LIBS = glut

include ../NiteSampleMakefile

