TARGET=gnatvCloth

QT+=gui opengl core

SOURCES+= src/main.cpp \
          src/Cloth.cpp \
          src/MassPoint.cpp \
          src/Triangle.cpp \
          src/NGLScene.cpp \
          src/NGLSceneMouseControls.cpp \
          src/WoolMaterial.cpp \
          src/AbstractClothMaterial.cpp

HEADERS+= include/Cloth.h \
          include/MassPoint.h \
          include/Triangle.h \
          include/NGLScene.h \
          include/WindowParams.h \
          include/AbstractClothMaterial.h \
          include/WoolMaterial.h

OTHER_FILES+= shaders/*.glsl

INCLUDEPATH+= include

cache()

# Following code written by Jon Macey
include($(HOME)/NGL/UseNGL.pri)
