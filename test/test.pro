TARGET=test
SOURCES+= main.cpp \
          ../gnatvCloth/src/Cloth.cpp \
          ../gnatvCloth/src/MassPoint.cpp \
          ../gnatvCloth/src/Triangle.cpp

LIBS+= -lgtest
INCLUDEPATH+= ../gnatvCloth/include

# Following code written by Jon Macey
include($$(HOME)/NGL/UseNGL.pri)
