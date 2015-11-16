#Change this to point to your boost installation
#BOOST_INCLUDES := -I/Users/cdj/src/cms/software/FWLiteVersions/osx106_amd64_gcc461/external/boost/1.47.0-cms3/include
#BOOST_INCLUDES := -I/Users/cdj/src/cms/software/FWLiteVersions/osx108_amd64_gcc470/external/boost/1.50.0/include
BOOST_INCLUDES := -I/Users/cdj/src/artCourse/products/boost/v1_57_0/Darwin64bit+14-e7-prof/include
#BOOST_INCLUDES := -I$(BOOST_INC)
CPPUNIT_INCLUDES := -I/Users/cdj/src/cms/threading/external/include
CPPUNIT_LIB_PATH := -L/Users/cdj/src/cms/threading/external/lib
CPPUNIT_LIB := $(CPPUNIT_LIB_PATH) -lcppunit
CXXOTHERFLAGS :=
UNAME:=$(shell uname -s)
#UNAME:=Linux
#TBB_INCLUDES :=-I/Users/cdj/src/cms/threading/tbb/tbb40_20120408oss/include
#TBB_LIB_PATH :=-L/Users/cdj/src/cms/threading/tbb/tbb40_20120408oss/build/macos_intel64_gcc_cc4.2.1_os10.7.4_release/
TBB_INCLUDES := -I/Users/cdj/src/artCourse/products/tbb/v4_3_5/Darwin64bit+14-e7-prof/include
TBB_LIB_PATH := -L/Users/cdj/src/artCourse/products/tbb/v4_3_5/Darwin64bit+14-e7-prof/lib/
#TBB_LIB_PATH :=-L/Users/cdj/src/cms/threading/tbb/tbb40_20120408oss/build/macos_intel64_gcc_cc4.2.1_os10.7.4_debug/
#TBB_LIB := $(TBB_LIB_PATH) -ltbb_debug
ifeq ($(UNAME), Linux)
BOOST_INCLUDES := -I/cvmfs/cms.cern.ch/slc6_amd64_gcc493/external/boost/1.57.0-kpegke/include
CPPUNIT_INCLUDES := -I/cvmfs/cms.cern.ch/slc6_amd64_gcc493/external/cppunit/1.12.1/include
CPPUNIT_LIB_PATH := -L/cvmfs/cms.cern.ch/slc6_amd64_gcc493/external/cppunit/1.12.1/lib
CPPUNIT_LIB := $(CPPUNIT_LIB_PATH) -lcppunit
#change the following to to point to where you installed tbb
TBB_INCLUDES := -I/cvmfs/cms.cern.ch/slc6_amd64_gcc493/external/tbb/43_20150316oss/include
#TBB_LIB_PATH := -L/uscms_data/d2/cdj/build/multicore/tbb/tbb40_20120408oss//build/linux_intel64_gcc_cc4.7.0_libc2.5_kernel2.6.18_debug
TBB_LIB_PATH := -L/cvmfs/cms.cern.ch/slc6_amd64_gcc493/external/tbb/43_20150316oss/lib
CXXOTHERFLAGS := -D__USE_XOPEN2K8
endif
TBB_LIB := $(TBB_LIB_PATH) -ltbb
CXX=g++
CPPFLAGS=$(TBB_INCLUDES) $(BOOST_INCLUDES) $(CPPUNIT_INCLUDES) -ITBBFrameworkCore/ -I../BusyWaitCalibration/
#CXXFLAGS=-O3 -g -std=c++14 -fPIC -pthread $(CXXOTHERFLAGS)
CXXFLAGS=-g -std=c++14 -fPIC -pthread $(CXXOTHERFLAGS)
LDFLAGS= -std=c++14 -pthread
LINKER=g++

all: libTBBFrameworkCore.so libTBBTestModules.so TBBDemo

TBBDemo: libTBBFrameworkCore.so libTBBTestModules.so main.o
	$(LINKER) $+ $(LDFLAGS) $(TBB_LIB) -o $@

fc_objects := $(patsubst %.cpp,%.o,$(wildcard TBBFrameworkCore/*.cpp))
tm_objects := $(patsubst %.cpp,%.o,$(wildcard TBBTestModules/*.cpp))
unittest_objects := $(patsubst %.cpp,%.o,$(wildcard TBBUnitTests/*.cpp))

libTBBFrameworkCore.so: $(fc_objects)
	$(LINKER) $+ $(TBB_LIB) -shared -o $@

libTBBTestModules.so: $(tm_objects) ../BusyWaitCalibration/busyWait.o ../BusyWaitCalibration/busy_wait_scale_factor.o libTBBFrameworkCore.so
	$(LINKER) $+ -shared -o $@

TBBRunUnitTests: $(unittest_objects) libTBBFrameworkCore.so
	$(LINKER) $+ $(LDFLAGS) $(TBB_LIB) $(CPPUNIT_LIB) -o $@

clean:
	rm -f *.o */*.o *.so
	rm -f TBBDemo
	rm -f $(fc_objects)
	rm -f $(tm_objects)