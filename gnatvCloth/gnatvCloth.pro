TARGET=gnatvCloth

QT+=gui opengl core

SOURCES+= src/main.cpp \
          src/Cloth.cpp \
          src/MassPoint.cpp \
          src/Triangle.cpp \
          src/NGLScene.cpp \
          src/NGLSceneMouseControls.cpp \
          src/MainWindow.cpp \
          src/ClothInterface.cpp

HEADERS+= include/Cloth.h \
          include/MassPoint.h \
          include/Triangle.h \
          include/NGLScene.h \
          include/WindowParams.h \
          include/Materials.h \
          include/MainWindow.h \
          include/ClothInterface.h \
          include/FixPtTestDefaults.h

FORMS+= ui/MainWindow.ui

INCLUDEPATH+= include

cache()

# Following code written by Jon Macey
include($(HOME)/NGL/UseNGL.pri)
