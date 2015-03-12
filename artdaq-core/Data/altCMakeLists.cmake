set( Data_HEADERS
     Fragment.hh
     Fragments.hh
     PackageBuildInfo.hh
     QuickVec.hh
     RawEvent.hh
     dictionarycontrol.hh
     )

set( Data_detail_HEADERS
     detail/RawFragmentHeader.hh
     )

set( Data_SOURCES
     Fragment.cc
     RawEvent.cc
     )

add_library(artdaq-core_Data SHARED
     ${Data_HEADERS}
     ${Data_detail_HEADERS}
     ${Data_SOURCES}
     )

target_link_libraries( artdaq-core_Data 
     FNALCore::FNALCore
     art::art_Persistency_Provenance
     art::art_Utilities
     )

art_add_dictionary()

install(TARGETS
     artdaq-core_Data
     artdaq-core_Data_map
     artdaq-core_Data_dict
     EXPORT artdaq-coreLibraries
     RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
     LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
     ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
     COMPONENT Runtime 
     )

install(FILES ${Data_HEADERS} DESTINATION
     ${CMAKE_INSTALL_INCLUDEDIR}/artdaq-core/Data COMPONENT Development)

install(FILES ${Data_detail_HEADERS} DESTINATION
     ${CMAKE_INSTALL_INCLUDEDIR}/artdaq-core/Data/detail COMPONENT Development)


