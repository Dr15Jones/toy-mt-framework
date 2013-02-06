#Change this to point to your boost installation
#BOOST_INCLUDES := -I/Users/cdj/src/cms/software/FWLiteVersions/osx106_amd64_gcc461/external/boost/1.47.0-cms3/include
#BOOST_INCLUDES := -I$(BOOST_INC)
BOOST_INCLUDES := -I/Users/cdj/src/cms/software/FWLiteVersions/osx108_amd64_gcc470/external/boost/1.50.0/include
BOOST_LIB_PATH := -L/Users/cdj/src/cms/software/FWLiteVersions/osx108_amd64_gcc470/external/boost/1.50.0/lib
UNAME:=(shell uname -s)
ifeq ($(UNAME), Linux)
endif
CXX=g++
CPPFLAGS=$(BOOST_INCLUDES) -ISingleThreadedFrameworkCore/ -I../BusyWaitCalibration/
CXXFLAGS=-O3 -g -std=c++0x -fPIC
LDFLAGS= -std=c++0x
LINKER=g++

all: libSingleThreadedFrameworkCore.so libSingleThreadedTestModules.so SingleThreadedDemo

SingleThreadedDemo: libSingleThreadedFrameworkCore.so libSingleThreadedTestModules.so main.o
	$(LINKER) $+ $(LDFLAGS) $(DISPATCH_LIB) -o $@

fc_objects := $(patsubst %.cpp,%.o,$(wildcard SingleThreadedFrameworkCore/*.cpp))
tm_objects := $(patsubst %.cpp,%.o,$(wildcard SingleThreadedTestModules/*.cpp))

libSingleThreadedFrameworkCore.so: $(fc_objects)
	$(LINKER) $+ -shared -o $@

libSingleThreadedTestModules.so: $(tm_objects) ../BusyWaitCalibration/busyWait.o ../BusyWaitCalibration/busy_wait_scale_factor.o libSingleThreadedFrameworkCore.so
	$(LINKER) $+ -shared -o $@

clean:
	rm -f *.o */*.o *.so
	rm -f SingleThreadedDemo
	rm -f $(fc_objects)
	rm -f $(tm_objects)

