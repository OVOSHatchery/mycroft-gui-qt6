project(mycroftimport)

set(mycroftimport_SRCS
    ovosplugin.cpp
    mycroftcontroller.cpp
    activeskillsmodel.cpp
    delegatesmodel.cpp
    abstractskillview.cpp
    abstractdelegate.cpp
    sessiondatamap.cpp
    sessiondatamodel.cpp
    globalsettings.cpp
    filereader.cpp
    audiorec.cpp
    mediaservice.cpp
    mediaproviders/videoproviderservice.cpp
    mediaproviders/audioproviderservice.cpp
    mediaproviders/audiostreamdevice.cpp
    mediaproviders/thirdparty/fftcalc.cpp
    mediaproviders/thirdparty/fft.cpp
    )

configure_file(controllerconfig.h.in ${CMAKE_CURRENT_BINARY_DIR}/controllerconfig.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

qt6_add_resources(mycroftimport_SRCS mycroft.qrc)

add_library(ovosplugin SHARED ${mycroftimport_SRCS} ${RESOURCES})

if (CMAKE_SYSTEM_NAME STREQUAL "Android")
    set(ovosplugin_EXTRA_LIBS
        Qt6::TextToSpeech
    )
endif()

target_link_libraries(ovosplugin
        PUBLIC
            Qt6::Core
            Qt6::Multimedia
            ${ovosplugin_EXTRA_LIBS}
        PRIVATE
            Qt6::Qml
            Qt6::Quick
            Qt6::Network
            Qt6::WebSockets
    )

install(TARGETS ovosplugin DESTINATION ${KDE_INSTALL_QMLDIR}/Mycroft)

install(FILES qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/Mycroft)
install(DIRECTORY qml/ DESTINATION ${KDE_INSTALL_QMLDIR}/Mycroft)
install(FILES plugins.qmltypes DESTINATION ${KDE_INSTALL_QMLDIR}/Mycroft)
