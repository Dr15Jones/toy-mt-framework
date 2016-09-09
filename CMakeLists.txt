project(TOY_MT_FRAMEWORK)
cmake_minimum_required(VERSION 3.0)
include(GNUInstallDirs)
set(BASE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/BuildProducts")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${BASE_OUTPUT_DIRECTORY}/${CMAKE_INSTALL_BINDIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${BASE_OUTPUT_DIRECTORY}/${CMAKE_INSTALL_LIBDIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${BASE_OUTPUT_DIRECTORY}/${CMAKE_INSTALL_LIBDIR}")

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules" ${CMAKE_MODULE_PATH} )
find_package(OpenMP)
find_package(TBB)
find_package(CppUnit)
set(Boost_NO_BOOST_CMAKE ON)
find_package(Boost)
include_directories(${Boost_INCLUDE_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

add_subdirectory(BusyWaitCalibration)
# nothing ever sets DISPATCH_FOUND (!)
if(DISPATCH_FOUND)
    add_subdirectory(DispatchProcessingDemo)
endif()
add_subdirectory(SingleThreadedProcessingDemo)
if(TBB_FOUND)
    add_subdirectory(TBBProcessingDemo)
endif()
if (OPENMP_FOUND)
    add_subdirectory(OpenMPProcessingDemo)
endif()

set(readme_files
README
README.md
)

set(cms-data-files
cms-data/changebusywait
cms-data/changenevts
cms-data/createConfig_ave_times.py
cms-data/createConfig.py
cms-data/reco_hipileup_5_2_0_busywait_perfectIO.config
cms-data/reco_hipileup_5_2_0_sleeping_perfectIO.config
cms-data/reco_minbias_busywait.config
cms-data/reco_minbias_sleeping_1000.config
cms-data/reco_minbias_sleeping_betweenmodules.config
cms-data/reco_minbias_sleeping.config
cms-data/reco_minbias_sleeping_perfectIO.config
cms-data/reco_multijet_sleeping_1000.config
cms-data/reco_multijet_sleeping.config
cms-data/transform_times.py
)


install(FILES ${readme_files} 
  DESTINATION ${CMAKE_INSTALL_PREFIX}
  COMPONENT Runtime
)

install(FILES ${cms-data-files}
  DESTINATION ${CMAKE_INSTALL_PREFIX}/cms-data
  COMPONENT Runtime
)


