all : BusyWaitCalibrate DispatchDemo OpenMPDemo SingleThreadedDemo

BusyWaitCalibrate :
	cd BusyWaitCalibration ; ${MAKE} all ; cp BusyWaitCalibrate ..

DispatchDemo :
	cd DispatchProcessingDemo ; ${MAKE} all ; cp DispatchDemo ..

OpenMPDemo :
	cd OpenMPProcessingDemo ; ${MAKE} all ; cp OpenMPDemo ..

SingleThreadedDemo :
	cd SingleThreadedProcessingDemo ; ${MAKE} all ; cp SingleThreadedDemo ..

clean : BusyWaitCalibrate_clean DispatchDemo_clean OpenMPDemo_clean SingleThreadedDemo_clean

BusyWaitCalibrate_clean :
	rm -f BusyWaitCalibrate
	cd BusyWaitCalibration ; ${MAKE} clean

DispatchDemo_clean :
	rm -f DispatchDemo
	cd DispatchProcessingDemo ; ${MAKE} clean

OpenMPDemo_clean :
	rm -f OpenMPDemo
	cd OpenMPProcessingDemo ; ${MAKE} clean

SingleThreadedDemo_clean :
	rm -f SingleThreadedDemo
	cd SingleThreadedProcessingDemo ; ${MAKE} clean

