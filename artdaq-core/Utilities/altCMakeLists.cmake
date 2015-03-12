
set( Utilities_HEADERS
     SimpleLookupPolicy.h
     TimeUtils.h
     )

set( Utilities_SOURCES
     SimpleLookupPolicy.cc
     TimeUtils.cc
     )

add_library(artdaq-core_Utilities SHARED
     ${Utilities_HEADERS}
     ${Utilities_SOURCES}
     )

target_link_libraries(artdaq-core_Utilities
     FNALCore::FNALCore
     art::art_Persistency_Provenance
     art::art_Utilities
     )

install(TARGETS
     artdaq-core_Utilities
     EXPORT artdaq-coreLibraries
     RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
     LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
     ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
     COMPONENT Runtime 
     )

install(FILES ${Utilities_HEADERS} DESTINATION
     ${CMAKE_INSTALL_INCLUDEDIR}/artdaq-core/Utilities COMPONENT Development)



