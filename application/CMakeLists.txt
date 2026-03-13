PROJECT(ovosguiapp VERSION "1.0.1")
include(ECMSetupVersion)
ecm_setup_version(PROJECT VERSION_HEADER version.h)

SET(ovos_gui_app_SRC
    main.cpp
    appsettings.cpp
    shell/plugins/EnvironmentSummary.cpp
    shell/plugins/ResetOperations.cpp
)

set(ovos_gui_app_EXTRA_LIBS
    Qt${QT_MAJOR_VERSION}::Widgets
    Qt${QT_MAJOR_VERSION}::Multimedia
    Qt${QT_MAJOR_VERSION}::Network
    Qt${QT_MAJOR_VERSION}::WebSockets
    KF6::DBusAddons
    ovosplugin
)

# WebView is optional
if(Qt6_WebView_FOUND)
    list(APPEND ovos_gui_app_EXTRA_LIBS Qt${QT_MAJOR_VERSION}::WebView)
endif()

qt_add_resources(ovos_gui_app_SRC
    qml.qrc
    shell-qml.qrc
)

add_executable(ovos-gui-app ${ovos_gui_app_SRC})
target_link_libraries(ovos-gui-app Qt${QT_MAJOR_VERSION}::Core Qt${QT_MAJOR_VERSION}::Quick ${ovos_gui_app_EXTRA_LIBS})

# Link to the import plugin library and include paths
target_include_directories(ovos-gui-app PRIVATE ../import ${CMAKE_CURRENT_BINARY_DIR}/../)


install(TARGETS ovos-gui-app ${INSTALL_TARGETS_DEFAULT_ARGS})
install(PROGRAMS ovos-gui-app.desktop DESTINATION  ${KDE_INSTALL_APPDIR})
install(PROGRAMS ovos-shell.desktop DESTINATION  ${KDE_INSTALL_APPDIR})
