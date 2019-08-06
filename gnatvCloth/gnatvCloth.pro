TARGET=gnatvCloth

QT+=gui opengl core

SOURCES+= src/main.cpp \
          src/Cloth.cpp \
          src/MassPoint.cpp \
          src/Triangle.cpp \
          src/NGLScene.cpp \
          src/NGLSceneMouseControls.cpp

HEADERS+= include/Cloth.h \
          include/MassPoint.h \
          include/Triangle.h \
          include/NGLScene.h \
          include/WindowParams.h \
          include/Materials.h

INCLUDEPATH+= include

cache()

# Following code written by Jon Macey
include($(HOME)/NGL/UseNGL.pri)
