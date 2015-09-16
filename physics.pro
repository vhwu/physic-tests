QT += core gui opengl

TARGET = warmup
TEMPLATE = app
QMAKE_CXXFLAGS += -std=c++11

# If you add your own folders, add them to INCLUDEPATH and DEPENDPATH, e.g.
# INCLUDEPATH += folder1 folder2
# DEPENDPATH += folder1 folder2

INCLUDEPATH += src
DEPENDPATH += src

win32: LIBS += -lglu32
unix:!macx{
    LIBS += -lGLU
}
macox:{
    LIBS += -framework GLUT
    LIBS += -framework OpenGL
    QMAKE_CXXFLAGS += -Wno-c++11-extentions
}

SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/view.cpp \
    src/game/player.cpp \
    src/engine/common/application.cpp \
    src/engine/common/camera.cpp \
    src/engine/common/graphic.cpp \
    src/engine/common/screen.cpp \
    src/engine/common/entity.cpp \
    src/engine/common/world.cpp \
    src/game/gamescreen.cpp \
    src/game/menuscreen.cpp \
    src/game/myapplication.cpp \
    src/engine/common/shape.cpp \
    src/engine/common/obj.cpp \
    src/engine/geometric/triangle.cpp \
    src/engine/geometric/collisioninfo.cpp \
    src/engine/common/ellipsoid.cpp \
    src/engine/verlet/link.cpp \
    src/engine/common/manager.cpp \
    src/engine/geometric/geometricmanager.cpp \
    src/engine/verlet/verletmanager.cpp \
    src/engine/verlet/verlet.cpp \
    src/engine/verlet/rope.cpp \
    src/engine/verlet/verletcube.cpp \
    src/engine/common/raytracer.cpp \
    src/engine/verlet/cloth.cpp \
    src/game/verlettest1.cpp \
    src/engine/common/collectiblemanager.cpp \
    src/engine/common/collectible.cpp \
    src/game/token.cpp \
    src/engine/verlet/translationconstraint.cpp \
    src/engine/verlet/constraint.cpp \
    src/engine/verlet/constraintmanager.cpp \
    src/engine/verlet/rotationconstraint.cpp \
    src/game/verlettest2.cpp \
    src/engine/verlet/verletconstraint.cpp \
    src/game/verletlevel.cpp \
    src/engine/verlet/angleconstraint.cpp \
    src/game/verlettest3.cpp \
    src/game/verlettest4.cpp \
    src/engine/verlet/setupcircle.cpp

HEADERS += \
    src/mainwindow.h \
    src/vector.h \
    src/view.h \
    src/game/player.h \
    src/engine/common/application.h \
    src/engine/common/camera.h \
    src/engine/common/graphic.h \
    src/engine/common/screen.h \
    src/engine/common/entity.h \
    src/engine/common/world.h \
    src/game/gamescreen.h \
    src/game/menuscreen.h \
    src/game/myapplication.h \
    src/engine/common/shape.h \
    src/engine/common/obj.h \
    src/engine/geometric/triangle.h \
    src/engine/geometric/collisioninfo.h \
    src/engine/verlet/link.h \
    src/engine/common/manager.h \
    src/engine/geometric/geometricmanager.h \
    src/engine/verlet/verletmanager.h \
    src/engine/verlet/verlet.h \
    src/engine/verlet/rope.h \
    src/engine/verlet/verletcube.h \
    src/engine/common/raytracer.h \
    src/engine/common/collision.h \
    src/engine/common/ellipsoid.h \
    src/engine/verlet/cloth.h \
    src/game/verlettest1.h \
    src/engine/common/collectiblemanager.h \
    src/engine/common/collectible.h \
    src/game/token.h \
    src/engine/verlet/translationconstraint.h \
    src/engine/verlet/constraint.h \
    src/engine/verlet/constraintmanager.h \
    src/engine/verlet/rotationconstraint.h \
    src/engine/common/axis.h \
    src/game/verlettest2.h \
    src/engine/verlet/verletconstraint.h \
    src/game/verletlevel.h \
    src/engine/verlet/angleconstraint.h \
    src/game/verlettest3.h \
    src/game/verlettest4.h \
    src/engine/verlet/setupcircle.h


FORMS += src/mainwindow.ui

RESOURCES += \
    res/res.qrc

DISTFILES += \
    res/mountain_maya.obj
