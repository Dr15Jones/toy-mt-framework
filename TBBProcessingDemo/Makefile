#Change this to point to your boost installation
BOOST_INCLUDES := -I/Users/cdj/src/cms/software/FWLiteVersions/osx106_amd64_gcc461/external/boost/1.47.0-cms3/include
#BOOST_INCLUDES := -I$(BOOST_INC)
UNAME:=$(shell uname -s)
#UNAME:=Linux
TBB_INCLUDES :=-I/Users/cdj/src/cms/threading/tbb/tbb40_20120408oss/include
TBB_LIB_PATH :=-L/Users/cdj/src/cms/threading/tbb/tbb40_20120408oss/build/macos_intel64_gcc_cc4.2.1_os10.7.4_release/
TBB_LIB := $(TBB_LIB_PATH) -ltbb
#TBB_LIB_PATH :=-L/Users/cdj/src/cms/threading/tbb/tbb40_20120408oss/build/macos_intel64_gcc_cc4.2.1_os10.7.4_debug/
#TBB_LIB := $(TBB_LIB_PATH) -ltbb_debug
ifeq ($(UNAME), Linux)
#change the following to to point to where you installed libdispatch and kqueue
TBB_INCLUDES := -I/home/paterno/include
TBB_LIB_PATH := -L/home/paterno/lib
TBB_LIB := $(TBB_LIB_PATH) -ltbb
endif
CXX=g++
CPPFLAGS=$(TBB_INCLUDES) $(BOOST_INCLUDES) -ITBBFrameworkCore/ -I../BusyWaitCalibration/
#CXXFLAGS=-O3 -g -std=c++0x -fPIC
CXXFLAGS=-g -std=c++0x -fPIC
LDFLAGS= -std=c++0x
LINKER=g++

all: libTBBFrameworkCore.so libTBBTestModules.so TBBDemo

TBBDemo: libTBBFrameworkCore.so libTBBTestModules.so main.o
	$(LINKER) $+ $(LDFLAGS) $(TBB_LIB) -o $@

fc_objects := $(patsubst %.cpp,%.o,$(wildcard TBBFrameworkCore/*.cpp))
tm_objects := $(patsubst %.cpp,%.o,$(wildcard TBBTestModules/*.cpp))

libTBBFrameworkCore.so: $(fc_objects)
	$(LINKER) $+ $(TBB_LIB) -shared -o $@

libTBBTestModules.so: $(tm_objects) ../BusyWaitCalibration/busyWait.o ../BusyWaitCalibration/busy_wait_scale_factor.o libTBBFrameworkCore.so
	$(LINKER) $+ -shared -o $@

clean:
	rm -f *.o */*.o *.so
	rm -f TBBDemo
	rm -f $(fc_objects)
	rm -f $(tm_objects)