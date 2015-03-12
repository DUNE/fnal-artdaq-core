IF (ALT_CMAKE)
INCLUDE(altCMakeLists.cmake)
ELSE()

art_make(
  LIB_LIBRARIES
  artdaq-core_Data
  ${ART_PERSISTENCY_COMMON}
  ${ART_UTILITIES}
  ${FHICLCPP}
  ${CETLIB}
  ${MF_MESSAGELOGGER}
  ${MF_UTILITIES}
  )

install_headers()
install_source()

ENDIF()
