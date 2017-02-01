#############################################################################
# Makefile for building: SR_srv_semaforow
# Generated by qmake (2.01a) (Qt 4.8.7) on: ?r. lut 1 19:39:26 2017
# Project:  SR_srv_semaforow.pro
# Template: app
# Command: /usr/lib/qt4/bin/qmake -spec /usr/share/qt4/mkspecs/linux-g++ CONFIG+=debug -o Makefile SR_srv_semaforow.pro
#############################################################################

####### Compiler, tools and options

CC            = gcc
CXX           = g++
DEFINES       = 
CFLAGS        = -pipe -g -Wall -W $(DEFINES)
CXXFLAGS      = -pipe -g -Wall -W $(DEFINES)
INCPATH       = -I/usr/share/qt4/mkspecs/linux-g++ -I. -I.
LINK          = g++
LFLAGS        = -Wl,-O1,--sort-common,--as-needed,-z,relro
LIBS          = $(SUBLIBS)   -lboost_program_options -lboost_system -lboost_thread -lcppnetlib-uri -lcppnetlib-server-parsers -lcppnetlib-client-connections -ljsoncpp -lpthread 
AR            = ar cqs
RANLIB        = 
QMAKE         = /usr/lib/qt4/bin/qmake
TAR           = tar -cf
COMPRESS      = gzip -9f
COPY          = cp -f
SED           = sed
COPY_FILE     = $(COPY)
COPY_DIR      = $(COPY) -r
STRIP         = strip
INSTALL_FILE  = install -m 644 -p
INSTALL_DIR   = $(COPY_DIR)
INSTALL_PROGRAM = install -m 755 -p
DEL_FILE      = rm -f
SYMLINK       = ln -f -s
DEL_DIR       = rmdir
MOVE          = mv -f
CHK_DIR_EXISTS= test -d
MKDIR         = mkdir -p

####### Output directory

OBJECTS_DIR   = ./

####### Files

SOURCES       = client.cpp \
		main.cpp \
		helpers.cpp \
		semaphore.cpp \
		abstractserver.cpp \
		server.cpp \
		clientserver.cpp \
		clientconsole.cpp \
		servicable.cpp \
		serverserver.cpp 
OBJECTS       = client.o \
		main.o \
		helpers.o \
		semaphore.o \
		abstractserver.o \
		server.o \
		clientserver.o \
		clientconsole.o \
		servicable.o \
		serverserver.o
DIST          = /usr/share/qt4/mkspecs/common/unix.conf \
		/usr/share/qt4/mkspecs/common/linux.conf \
		/usr/share/qt4/mkspecs/common/gcc-base.conf \
		/usr/share/qt4/mkspecs/common/gcc-base-unix.conf \
		/usr/share/qt4/mkspecs/common/g++-base.conf \
		/usr/share/qt4/mkspecs/common/g++-unix.conf \
		/usr/share/qt4/mkspecs/qconfig.pri \
		/usr/share/qt4/mkspecs/modules/qt_webkit.pri \
		/usr/share/qt4/mkspecs/features/qt_functions.prf \
		/usr/share/qt4/mkspecs/features/qt_config.prf \
		/usr/share/qt4/mkspecs/features/exclusive_builds.prf \
		/usr/share/qt4/mkspecs/features/default_pre.prf \
		/usr/share/qt4/mkspecs/features/debug.prf \
		/usr/share/qt4/mkspecs/features/default_post.prf \
		/usr/share/qt4/mkspecs/features/shared.prf \
		/usr/share/qt4/mkspecs/features/unix/gdb_dwarf_index.prf \
		/usr/share/qt4/mkspecs/features/warn_on.prf \
		/usr/share/qt4/mkspecs/features/resources.prf \
		/usr/share/qt4/mkspecs/features/uic.prf \
		/usr/share/qt4/mkspecs/features/yacc.prf \
		/usr/share/qt4/mkspecs/features/lex.prf \
		/usr/share/qt4/mkspecs/features/include_source_dir.prf \
		SR_srv_semaforow.pro
QMAKE_TARGET  = SR_srv_semaforow
DESTDIR       = 
TARGET        = SR_srv_semaforow

first: all
####### Implicit rules

.SUFFIXES: .o .c .cpp .cc .cxx .C

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.C.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o "$@" "$<"

####### Build rules

all: Makefile $(TARGET)

$(TARGET):  $(OBJECTS)  
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJCOMP) $(LIBS)
	{ test -n "$(DESTDIR)" && DESTDIR="$(DESTDIR)" || DESTDIR=.; } && test $$(gdb --version | sed -e 's,[^0-9][^0-9]*\([0-9]\)\.\([0-9]\).*,\1\2,;q') -gt 72 && gdb --nx --batch --quiet -ex 'set confirm off' -ex "save gdb-index $$DESTDIR" -ex quit '$(TARGET)' && test -f $(TARGET).gdb-index && objcopy --add-section '.gdb_index=$(TARGET).gdb-index' --set-section-flags '.gdb_index=readonly' '$(TARGET)' '$(TARGET)' && rm -f $(TARGET).gdb-index || true

Makefile: SR_srv_semaforow.pro  /usr/share/qt4/mkspecs/linux-g++/qmake.conf /usr/share/qt4/mkspecs/common/unix.conf \
		/usr/share/qt4/mkspecs/common/linux.conf \
		/usr/share/qt4/mkspecs/common/gcc-base.conf \
		/usr/share/qt4/mkspecs/common/gcc-base-unix.conf \
		/usr/share/qt4/mkspecs/common/g++-base.conf \
		/usr/share/qt4/mkspecs/common/g++-unix.conf \
		/usr/share/qt4/mkspecs/qconfig.pri \
		/usr/share/qt4/mkspecs/modules/qt_webkit.pri \
		/usr/share/qt4/mkspecs/features/qt_functions.prf \
		/usr/share/qt4/mkspecs/features/qt_config.prf \
		/usr/share/qt4/mkspecs/features/exclusive_builds.prf \
		/usr/share/qt4/mkspecs/features/default_pre.prf \
		/usr/share/qt4/mkspecs/features/debug.prf \
		/usr/share/qt4/mkspecs/features/default_post.prf \
		/usr/share/qt4/mkspecs/features/shared.prf \
		/usr/share/qt4/mkspecs/features/unix/gdb_dwarf_index.prf \
		/usr/share/qt4/mkspecs/features/warn_on.prf \
		/usr/share/qt4/mkspecs/features/resources.prf \
		/usr/share/qt4/mkspecs/features/uic.prf \
		/usr/share/qt4/mkspecs/features/yacc.prf \
		/usr/share/qt4/mkspecs/features/lex.prf \
		/usr/share/qt4/mkspecs/features/include_source_dir.prf
	$(QMAKE) -spec /usr/share/qt4/mkspecs/linux-g++ CONFIG+=debug -o Makefile SR_srv_semaforow.pro
/usr/share/qt4/mkspecs/common/unix.conf:
/usr/share/qt4/mkspecs/common/linux.conf:
/usr/share/qt4/mkspecs/common/gcc-base.conf:
/usr/share/qt4/mkspecs/common/gcc-base-unix.conf:
/usr/share/qt4/mkspecs/common/g++-base.conf:
/usr/share/qt4/mkspecs/common/g++-unix.conf:
/usr/share/qt4/mkspecs/qconfig.pri:
/usr/share/qt4/mkspecs/modules/qt_webkit.pri:
/usr/share/qt4/mkspecs/features/qt_functions.prf:
/usr/share/qt4/mkspecs/features/qt_config.prf:
/usr/share/qt4/mkspecs/features/exclusive_builds.prf:
/usr/share/qt4/mkspecs/features/default_pre.prf:
/usr/share/qt4/mkspecs/features/debug.prf:
/usr/share/qt4/mkspecs/features/default_post.prf:
/usr/share/qt4/mkspecs/features/shared.prf:
/usr/share/qt4/mkspecs/features/unix/gdb_dwarf_index.prf:
/usr/share/qt4/mkspecs/features/warn_on.prf:
/usr/share/qt4/mkspecs/features/resources.prf:
/usr/share/qt4/mkspecs/features/uic.prf:
/usr/share/qt4/mkspecs/features/yacc.prf:
/usr/share/qt4/mkspecs/features/lex.prf:
/usr/share/qt4/mkspecs/features/include_source_dir.prf:
qmake:  FORCE
	@$(QMAKE) -spec /usr/share/qt4/mkspecs/linux-g++ CONFIG+=debug -o Makefile SR_srv_semaforow.pro

dist: 
	@$(CHK_DIR_EXISTS) .tmp/SR_srv_semaforow1.0.0 || $(MKDIR) .tmp/SR_srv_semaforow1.0.0 
	$(COPY_FILE) --parents $(SOURCES) $(DIST) .tmp/SR_srv_semaforow1.0.0/ && (cd `dirname .tmp/SR_srv_semaforow1.0.0` && $(TAR) SR_srv_semaforow1.0.0.tar SR_srv_semaforow1.0.0 && $(COMPRESS) SR_srv_semaforow1.0.0.tar) && $(MOVE) `dirname .tmp/SR_srv_semaforow1.0.0`/SR_srv_semaforow1.0.0.tar.gz . && $(DEL_FILE) -r .tmp/SR_srv_semaforow1.0.0


clean:compiler_clean 
	-$(DEL_FILE) $(OBJECTS)
	-$(DEL_FILE) *~ core *.core


####### Sub-libraries

distclean: clean
	-$(DEL_FILE) $(TARGET) 
	-$(DEL_FILE) Makefile


check: first

compiler_rcc_make_all:
compiler_rcc_clean:
compiler_uic_make_all:
compiler_uic_clean:
compiler_image_collection_make_all: qmake_image_collection.cpp
compiler_image_collection_clean:
	-$(DEL_FILE) qmake_image_collection.cpp
compiler_yacc_decl_make_all:
compiler_yacc_decl_clean:
compiler_yacc_impl_make_all:
compiler_yacc_impl_clean:
compiler_lex_make_all:
compiler_lex_clean:
compiler_clean: 

####### Compile

client.o: client.cpp client.hpp \
		clientserver.hpp \
		abstractserver.hpp \
		semaphore.hpp \
		clientconsole.hpp \
		servicable.hpp \
		helpers.hpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o client.o client.cpp

main.o: main.cpp client.hpp \
		clientserver.hpp \
		abstractserver.hpp \
		semaphore.hpp \
		clientconsole.hpp \
		servicable.hpp \
		server.hpp \
		serverserver.hpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o main.o main.cpp

helpers.o: helpers.cpp abstractserver.hpp \
		semaphore.hpp \
		helpers.hpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o helpers.o helpers.cpp

semaphore.o: semaphore.cpp semaphore.hpp \
		helpers.hpp \
		abstractserver.hpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o semaphore.o semaphore.cpp

abstractserver.o: abstractserver.cpp abstractserver.hpp \
		semaphore.hpp \
		helpers.hpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o abstractserver.o abstractserver.cpp

server.o: server.cpp server.hpp \
		servicable.hpp \
		serverserver.hpp \
		abstractserver.hpp \
		semaphore.hpp \
		helpers.hpp \
		client.hpp \
		clientserver.hpp \
		clientconsole.hpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o server.o server.cpp

clientserver.o: clientserver.cpp clientserver.hpp \
		abstractserver.hpp \
		semaphore.hpp \
		helpers.hpp \
		client.hpp \
		clientconsole.hpp \
		servicable.hpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o clientserver.o clientserver.cpp

clientconsole.o: clientconsole.cpp clientconsole.hpp \
		servicable.hpp \
		client.hpp \
		clientserver.hpp \
		abstractserver.hpp \
		semaphore.hpp \
		helpers.hpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o clientconsole.o clientconsole.cpp

servicable.o: servicable.cpp servicable.hpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o servicable.o servicable.cpp

serverserver.o: serverserver.cpp server.hpp \
		servicable.hpp \
		serverserver.hpp \
		abstractserver.hpp \
		semaphore.hpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o serverserver.o serverserver.cpp

####### Install

install:   FORCE

uninstall:   FORCE

FORCE:

