IF (ALT_CMAKE)
INCLUDE(altCMakeLists.cmake)
ELSE()


cet_test(Fragment_t USE_BOOST_UNIT
  LIBRARIES artdaq-core_Data
  )

cet_test(RawEvent_t USE_BOOST_UNIT
  LIBRARIES artdaq-core_Data
  )

ENDIF()
