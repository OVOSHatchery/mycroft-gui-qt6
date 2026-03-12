include(ECMAddTests)

find_package(Qt6 6.5 NO_MODULE REQUIRED Test)
set_package_properties(Qt6Test PROPERTIES PURPOSE "Required for autotests")

# Generate configuration header (same as main build)
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/../import/controllerconfig.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/controllerconfig.h
)

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/../import)

set(import_SRCS
    ${CMAKE_SOURCE_DIR}/import/abstractdelegate.cpp
    ${CMAKE_SOURCE_DIR}/import/mycroftcontroller.cpp
    ${CMAKE_SOURCE_DIR}/import/activeskillsmodel.cpp
    ${CMAKE_SOURCE_DIR}/import/delegatesmodel.cpp
    ${CMAKE_SOURCE_DIR}/import/sessiondatamap.cpp
    ${CMAKE_SOURCE_DIR}/import/sessiondatamodel.cpp
    ${CMAKE_SOURCE_DIR}/import/filereader.cpp
    ${CMAKE_SOURCE_DIR}/import/globalsettings.cpp
    ${CMAKE_SOURCE_DIR}/import/abstractskillview.cpp
   )

qt6_add_resources(import_SRCS ${CMAKE_SOURCE_DIR}/import/mycroft.qrc)

ecm_add_test(
  servertest.cpp
  ${import_SRCS}
  ${RESOURCES}

  TEST_NAME servertest

  LINK_LIBRARIES
    Qt6::Test
    Qt6::Qml
    Qt6::Quick
    Qt6::Network
    Qt6::WebSockets
    Qt6::Multimedia
)

ecm_add_test(
  modeltest.cpp
  ${import_SRCS}
  ${RESOURCES}

  TEST_NAME modeltest

  LINK_LIBRARIES
    Qt6::Test
    Qt6::Qml
    Qt6::Quick
    Qt6::Network
    Qt6::WebSockets
    Qt6::Multimedia
)

ecm_add_test(
  stresstest.cpp
  ${import_SRCS}
  ${RESOURCES}

  TEST_NAME stresstest

  LINK_LIBRARIES
    Qt6::Test
    Qt6::Qml
    Qt6::Quick
    Qt6::Network
    Qt6::WebSockets
    Qt6::Multimedia
)

ecm_add_test(
  message_routing_test.cpp

  TEST_NAME message_routing_test

  LINK_LIBRARIES
    Qt6::Test
)

ecm_add_test(
  qml_framework_components_test.cpp

  TEST_NAME qml_framework_components_test

  LINK_LIBRARIES
    Qt6::Test
    Qt6::Qml
)
