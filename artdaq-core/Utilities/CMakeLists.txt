IF (ALT_CMAKE)
INCLUDE(altCMakeLists.cmake)
ELSE()

art_make(
  LIB_LIBRARIES
  ${FHICLCPP}
  ${CETLIB}
  ${Boost_DATE_TIME_LIBRARY}
)

install_headers()
install_source()

ENDIF()
