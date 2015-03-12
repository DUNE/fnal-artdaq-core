# project name
project(artdaq-core_BuildInfo)

# the utcDateTime variable is used by the GetPackageBuildInfo code
# to report the build datetime
execute_process( COMMAND "date" "-u" "+%d-%b-%Y %H:%M:%S %Z" OUTPUT_VARIABLE rawUTCDate )
string( STRIP ${rawUTCDate} utcDateTime )

configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/GetPackageBuildInfo.cc.in
  ${CMAKE_CURRENT_BINARY_DIR}/GetPackageBuildInfo.cc @ONLY )


add_library(artdaq-core_BuildInfo SHARED
     ${CMAKE_CURRENT_BINARY_DIR}/GetPackageBuildInfo.cc
     )

install(TARGETS
     artdaq-core_BuildInfo
     EXPORT artdaq-coreLibraries
     RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
     LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
     ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
     COMPONENT Runtime 
     )

install(FILES GetPackageBuildInfo.hh DESTINATION
     ${CMAKE_INSTALL_INCLUDEDIR}/artdaq-core/BuildInfo COMPONENT Development)


