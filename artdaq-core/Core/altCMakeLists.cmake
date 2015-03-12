
set(Core_HEADERS
     ConcurrentQueue.hh
     GlobalQueue.hh
     MonitoredQuantity.hh
     SimpleQueueReader.hh
     StatisticsCollection.hh
     )

set(Core_SOURCES
     GlobalQueue.cc
     MonitoredQuantity.cc
     SimpleQueueReader.cc
     StatisticsCollection.cc
     )

add_library(artdaq-core_Core SHARED
     ${Core_HEADERS}
     ${Core_SOURCES}
     )

target_link_libraries(artdaq-core_Core
     FNALCore::FNALCore
     )

install(TARGETS
     artdaq-core_Core
     EXPORT artdaq-coreLibraries
     RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
     LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
     ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
     COMPONENT Runtime 
     )

install(FILES ${Core_HEADERS} DESTINATION
     ${CMAKE_INSTALL_INCLUDEDIR}/artdaq-core/Core COMPONENT Development)

