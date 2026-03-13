project(ovosimport)

set(ovosimport_SRCS
    ovosplugin.cpp
    ovoscontroller.cpp
    activeskillsmodel.cpp
    delegatesmodel.cpp
    abstractskillview.cpp
    abstractdelegate.cpp
    sessiondatamap.cpp
    sessiondatamodel.cpp
    globalsettings.cpp
    filereader.cpp
    mediaservice.cpp
    mediaproviders/videoproviderservice.cpp
    mediaproviders/audioproviderservice.cpp
    mediaproviders/audiostreamdevice.cpp
    mediaproviders/thirdparty/fftcalc.cpp
    mediaproviders/thirdparty/fft.cpp
    )

if(NOT OVOS_SYSTEM_TEMPLATES_DIR)
    set(OVOS_SYSTEM_TEMPLATES_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/ovos-gui/system-templates")
endif()
add_definitions(-DOVOS_SYSTEM_TEMPLATES_DIR="${OVOS_SYSTEM_TEMPLATES_DIR}")

configure_file(controllerconfig.h.in ${CMAKE_CURRENT_BINARY_DIR}/controllerconfig.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

qt6_add_resources(ovosimport_SRCS ovos.qrc)

add_library(ovosplugin SHARED ${ovosimport_SRCS} ${RESOURCES})

target_link_libraries(ovosplugin
        PUBLIC
            Qt6::Core
            Qt6::Multimedia
        PRIVATE
            Qt6::Qml
            Qt6::Quick
            Qt6::Network
            Qt6::WebSockets
    )

install(TARGETS ovosplugin DESTINATION ${KDE_INSTALL_QMLDIR}/OVOS)

install(FILES qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/OVOS)
install(DIRECTORY qml/ DESTINATION ${KDE_INSTALL_QMLDIR}/OVOS)
# plugins.qmltypes removed — was stale Qt5-era generated file
