TARGET = CG2Application
CONFIG   += console
CONFIG   -= app_bundle
CONFIG   -= qt
OBJECTS_DIR= obj
DESTDIR = $$PWD
TEMPLATE = app


CONFIG += c++11

MAKEFILE = Makefile_qmake
MAKEFILE = Makefile_qmake.$${TARGET}
CONFIG(release, debug|release): DEFINES += NDEBUG

SOURCES +=\
    glad/src/glad.c \
    glfw_backend.cpp \
    cg2application.cpp \
    util.cpp\
    vd/vd.cpp \
    geometry.cpp \
    scene.cpp \
    camera.cpp \
    lights.cpp \
    object.cpp

HEADERS += \
    util.h \
    cg2application.h \
    gl_utils.h\
    vd/vd.h \
    vd/vd_internals.h \
    geometry.h \
    event_data.h \
    bindings_locations.h \
    scene.h \
    camera.h \
    lights.h \
    object.h

DISTFILES += \
    data/shaders/example.fs.glsl \
    data/shaders/example.vs.glsl \
    .gitignore

INCLUDEPATH  += $$PWD/glad/include\
		$$PWD/GLFW/include
unix{
    LIBS += -lglfw -lm -pthread -ldl -fopenmp
    QMAKE_CXXFLAGS += -fopenmp
}


win32{
    contains(QMAKE_COMPILER_DEFINES, __GNUC__) {
        message("experimental MinGW support");
        MINGW=1
    } else {
        MINGW=0
    }

    # select the architecture
    GLFW_LIB_DIR=$$PWD/GLFW
    contains(QMAKE_TARGET.arch,x86_64){
        # 64 Bit
        if ($$MINGW) {
            GLFW_LIB_DIR=$$GLFW_LIB_DIR/lib-mingw-w64
        } else {
            GLFW_LIB_DIR=$$GLFW_LIB_DIR/lib-vc-x86_64
        }
    } else {
        # 32 Bit
        equals(MINGW,1) {
            GLFW_LIB_DIR=$$GLFW_LIB_DIR/lib-mingw
        } else {
            GLFW_LIB_DIR=$$GLFW_LIB_DIR/lib-vc
        }
    }

    # select the VS version
    contains(QMAKE_COMPILER_DEFINES, _MSC_VER=1600) {
        # MSVS2010
        message("Generating for MSVS 2010")
        GLFW_LIB_DIR=$$GLFW_LIB_DIR/10.0
    }
    contains(QMAKE_COMPILER_DEFINES, _MSC_VER=1700) {
        # MSVS2012
        message("Generating for MSVS 2012")
        GLFW_LIB_DIR=$$GLFW_LIB_DIR/11.0
    }
    contains(QMAKE_COMPILER_DEFINES, _MSC_VER=1800) {
        # MSVS2013
        message("Generating for MSVS 2013")
        GLFW_LIB_DIR=$$GLFW_LIB_DIR/12.0
    }
    contains(QMAKE_COMPILER_DEFINES, _MSC_VER=1900) {
        # MSVS2015
        message("Generating for MSVS 2015")
        GLFW_LIB_DIR=$$GLFW_LIB_DIR/14.0
    }
    contains(QMAKE_COMPILER_DEFINES, $$find(QMAKE_COMPILER_DEFINES,_MSC_VER=191)) {
        # MSVS2017
        message("Generating for MSVS 2017")
        GLFW_LIB_DIR=$$GLFW_LIB_DIR/15.0
    }

    LIBS += -L$$GLFW_LIB_DIR

    # select the build mode
    CONFIG(release, debug|release){
        # release
        equals(MINGW,1) {
            LIBS+= -lglfw3
        } else {
            LIBS+= -lglfw3
            QMAKE_CFLAGS_RELEASE -= -MD
            QMAKE_CXXFLAGS_RELEASE -= -MD
            QMAKE_CXXFLAGS_RELEASE += -MT
            QMAKE_CFLAGS_RELEASE += -MT
        }
    }else{
        # debug
        equals(MINGW,1) {
            LIBS+= -lglfw3
        } else {
            LIBS+= -lglfw3d
            QMAKE_CFLAGS_DEBUG -= -MDd
            QMAKE_CXXFLAGS_DEBUG -= -MDd
            QMAKE_CXXFLAGS_DEBUG += -MTd
            QMAKE_CFLAGS_DEBUG += -MTd
        }
    }
    # GLFW dependencies
    LIBS+= \
    -lkernel32 \
    -luser32 \
    -lgdi32 \
    -lshell32

}
