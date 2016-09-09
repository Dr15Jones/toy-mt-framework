add_executable(BusyWaitCalibrate main.cpp busy_wait_scale_factor.cpp busyWait.cpp)
install(
  TARGETS BusyWaitCalibrate 
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  COMPONENT Runtime
)
