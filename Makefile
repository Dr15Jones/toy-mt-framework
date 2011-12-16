CXX=g++
CPPFLAGS=-I/home/paterno/include -I$(BOOST_INC) -IDispatchProcessingDemo/DispatchFrameworkCore/ -IBusyWaitCalibration/
CXXFLAGS=-O3 -g -std=c++0x -fPIC
LDFLAGS=-L/home/paterno/lib -std=c++0x
LINKER=g++

all: BusyWaitCalibrate libDispatchFrameworkCore.so libDispatchTestModules.so DispatchDemo

BusyWaitCalibrate: BusyWaitCalibration/main.o BusyWaitCalibration/busy_wait_scale_factor.o BusyWaitCalibration/busyWait.o
	$(LINKER) $+ -o $@

DispatchDemo: libDispatchFrameworkCore.so libDispatchTestModules.so DispatchProcessingDemo/main.o
	$(LINKER) $+ $(LDFLAGS) -ldispatch -lkqueue -o $@

fc_objects := $(patsubst %.cpp,%.o,$(wildcard DispatchProcessingDemo/DispatchFrameworkCore/*.cpp))
tm_objects := $(patsubst %.cpp,%.o,$(wildcard DispatchProcessingDemo/DispatchTestModules/*.cpp))

libDispatchFrameworkCore.so: $(fc_objects)
	$(LINKER) $+ -shared -o $@

libDispatchTestModules.so: $(tm_objects) BusyWaitCalibration/busyWait.o BusyWaitCalibration/busy_wait_scale_factor.o
	$(LINKER) $+ -shared -o $@

clean:
	rm -f */*.o *.so
	rm -f BusyWaitCalibrate
	rm -f DispatchDemo
	rm -f $(fc_objects)
	rm -f $(tm_objects)