art_add_module( artdaq-core_ArtModules_ArtdaqCoreBuildInfo_module ArtdaqCoreBuildInfo_module.cc )

art_add_module( artdaq-core_ArtModules_PrintBuildInfo_module PrintBuildInfo_module.cc )

install(TARGETS
     artdaq-core_ArtModules_ArtdaqCoreBuildInfo_module
     artdaq-core_ArtModules_PrintBuildInfo_module
     EXPORT artdaq-coreLibraries
     RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
     LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
     ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
     COMPONENT Runtime 
     )

install(FILES BuildInfo_module.hh DESTINATION
     ${CMAKE_INSTALL_INCLUDEDIR}/artdaq-core/ArtModules COMPONENT Development)


