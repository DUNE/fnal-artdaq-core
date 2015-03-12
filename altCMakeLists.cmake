cmake_minimum_required (VERSION 2.8.12)

project(artdaq-core)
set(artdaq-core_VERSION 1.04.07)
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)
include(CheckCXXCompilerFlag)
set(BASE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/BuildProducts")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${BASE_OUTPUT_DIRECTORY}/${CMAKE_INSTALL_BINDIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${BASE_OUTPUT_DIRECTORY}/${CMAKE_INSTALL_LIBDIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${BASE_OUTPUT_DIRECTORY}/${CMAKE_INSTALL_LIBDIR}")

list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_CURRENT_LIST_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y -O3 -g -DNDEBUG -fno-omit-frame-pointer")


find_package(FNALCore 0.1.0 REQUIRED)
find_package(art 1.11.3 REQUIRED)
include(artTools)
find_package(Boost 1.55.0 REQUIRED)
find_package(CLHEP 2.2.0.3 REQUIRED)
find_package(ROOT 5.34.20 REQUIRED)
set(ROOTCINT ${ROOT_rootcint_CMD})


include_directories ( ${CMAKE_SOURCE_DIR} )
include_directories(${FNALCore_INCLUDE_DIRS})
include_directories(${art_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIRS})

add_subdirectory(artdaq-core)

configure_package_config_file(
  artdaq-coreConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/artdaq-coreConfig.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/artdaq-core-${artdaq-core_VERSION}
    PATH_VARS
    CMAKE_INSTALL_INCLUDEDIR
    CMAKE_INSTALL_LIBDIR
  )

write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/artdaq-coreConfigVersion.cmake
  VERSION ${artdaq-core_VERSION}
  COMPATIBILITY AnyNewerVersion
  )

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/artdaq-coreConfig.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/artdaq-coreConfigVersion.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/artdaq-core-${artdaq-core_VERSION}
  COMPONENT Development
  )

install(EXPORT artdaq-coreLibraries
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/artdaq-core-${artdaq-core_VERSION}
  NAMESPACE artdaq-core::
  COMPONENT Development
  )

