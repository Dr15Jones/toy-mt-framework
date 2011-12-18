#Change this to point to your boost installation
BOOST_INCLUDES := -I/Users/cdj/src/cms/software/FWLiteVersions/osx106_amd64_gcc461/external/boost/1.47.0-cms3/include
#BOOST_INCLUDES := -I$(BOOST_INC)
UNAME:=(shell uname -s)
DISPATCH_INCLUDES :=
DISPATCH_LIB :=
DISPATCH_LIB_PATH :=
ifeq ($(UNAME), Linux)
#change the following to to point to where you installed libdispatch and kqueue
DISPATCH_INCLUDES :=-I/home/paterno/include
DISPATCH_LIB_PATH :=-L/home/paterno/lib
DISPATCH_LIB := $(DISPATCH_LIB_PATH) -ldispatch -lkqueue
endif
CXX=g++
CPPFLAGS=$(DISPATCH_INCLUDES) $(BOOST_INCLUDES) -IDispatchProcessingDemo/DispatchFrameworkCore/ -IBusyWaitCalibration/
CXXFLAGS=-O3 -g -std=c++0x -fPIC
LDFLAGS= -std=c++0x
LINKER=g++

all: BusyWaitCalibrate libDispatchFrameworkCore.so libDispatchTestModules.so DispatchDemo

BusyWaitCalibrate: BusyWaitCalibration/main.o BusyWaitCalibration/busy_wait_scale_factor.o BusyWaitCalibration/busyWait.o
	$(LINKER) $+ -o $@

DispatchDemo: libDispatchFrameworkCore.so libDispatchTestModules.so DispatchProcessingDemo/main.o
	$(LINKER) $+ $(LDFLAGS) $(DISPATCH_LIB) -o $@

fc_objects := $(patsubst %.cpp,%.o,$(wildcard DispatchProcessingDemo/DispatchFrameworkCore/*.cpp))
tm_objects := $(patsubst %.cpp,%.o,$(wildcard DispatchProcessingDemo/DispatchTestModules/*.cpp))

libDispatchFrameworkCore.so: $(fc_objects)
	$(LINKER) $+ $(DISPATCH_LIB) -shared -o $@

libDispatchTestModules.so: $(tm_objects) BusyWaitCalibration/busyWait.o BusyWaitCalibration/busy_wait_scale_factor.o libDispatchFrameworkCore.so
	$(LINKER) $+ -shared -o $@

clean:
	rm -f */*.o *.so
	rm -f BusyWaitCalibrate
	rm -f DispatchDemo
	rm -f $(fc_objects)
	rm -f $(tm_objects)