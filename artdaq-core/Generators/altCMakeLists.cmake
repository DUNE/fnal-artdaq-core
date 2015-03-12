set(Generators_HEADERS
     FragmentGenerator.hh
     GeneratorMacros.hh
     makeFragmentGenerator.hh
     )

set(Generators_SOURCES
     makeFragmentGenerator.cc
     )

add_library(artdaq-core_Generators SHARED
     ${Generators_HEADERS}
     ${Generators_SOURCES}
     )

target_link_libraries(artdaq-core_Generators
     artdaq-core_Data
     FNALCore::FNALCore
     art::art_Persistency_Provenance
     art::art_Utilities
     )

install(TARGETS
     artdaq-core_Generators
     EXPORT artdaq-coreLibraries
     RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
     LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
     ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
     COMPONENT Runtime 
     )

install(FILES ${Generators_HEADERS} DESTINATION
     ${CMAKE_INSTALL_INCLUDEDIR}/artdaq-core/Generators COMPONENT Development)


