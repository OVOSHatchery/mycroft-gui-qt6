include(ECMAddTests)

find_package(Qt6 6.5 NO_MODULE REQUIRED Test)
set_package_properties(Qt6Test PROPERTIES PURPOSE "Required for autotests")

# Generate configuration header (same as main build)
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/../import/controllerconfig.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/controllerconfig.h
)

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/../import)

# System templates directory (needed by guinamespace.cpp)
if(NOT OVOS_SYSTEM_TEMPLATES_DIR)
    set(OVOS_SYSTEM_TEMPLATES_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/ovos-gui/system-templates")
endif()
add_definitions(-DOVOS_SYSTEM_TEMPLATES_DIR="${OVOS_SYSTEM_TEMPLATES_DIR}")

ecm_add_test(
  servertest.cpp

  TEST_NAME servertest

  LINK_LIBRARIES
    OvosGuiCommon
    Qt6::Test
    Qt6::Qml
    Qt6::Quick
    Qt6::Network
    Qt6::WebSockets
    Qt6::Multimedia
)

ecm_add_test(
  modeltest.cpp

  TEST_NAME modeltest

  LINK_LIBRARIES
    OvosGuiCommon
    Qt6::Test
    Qt6::Qml
    Qt6::Quick
    Qt6::Network
    Qt6::WebSockets
    Qt6::Multimedia
)

ecm_add_test(
  stresstest.cpp

  TEST_NAME stresstest

  LINK_LIBRARIES
    OvosGuiCommon
    Qt6::Test
    Qt6::Qml
    Qt6::Quick
    Qt6::Network
    Qt6::WebSockets
    Qt6::Multimedia
)

set(demotest_SRCS
  demotest.cpp
  ${CMAKE_SOURCE_DIR}/application/appsettings.cpp
  ${CMAKE_SOURCE_DIR}/application/shell/plugins/EnvironmentSummary.cpp
  ${CMAKE_SOURCE_DIR}/application/shell/plugins/ResetOperations.cpp
)

qt6_add_resources(demotest_SRCS
    ${CMAKE_SOURCE_DIR}/application/qml.qrc
    ${CMAKE_SOURCE_DIR}/application/shell-qml.qrc
    ${CMAKE_SOURCE_DIR}/import/ovos.qrc
)

add_executable(demotest ${demotest_SRCS})

target_link_libraries(demotest
    OvosGuiCommon
    Qt6::Core
    Qt6::Quick
    Qt6::Qml
    Qt6::Widgets
    Qt6::Multimedia
    Qt6::Network
    Qt6::WebSockets
    Qt6::Gui
    KF6::Kirigami
    KF6::CoreAddons
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
