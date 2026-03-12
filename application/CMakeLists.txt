PROJECT(mycroftguiapp VERSION "1.0.1")
include(ECMSetupVersion)
ecm_setup_version(PROJECT VERSION_HEADER version.h)

SET(mycroft_gui_app_SRC
    main.cpp
    appsettings.cpp
    shell/plugins/EnvironmentSummary.cpp
    shell/plugins/ResetOperations.cpp
)

set(mycroft_gui_app_EXTRA_LIBS
    Qt${QT_MAJOR_VERSION}::Widgets
    Qt${QT_MAJOR_VERSION}::Multimedia
    Qt${QT_MAJOR_VERSION}::Network
    Qt${QT_MAJOR_VERSION}::WebSockets
    KF6::DBusAddons
    ovosplugin
)

# WebView is optional
if(Qt6_WebView_FOUND)
    list(APPEND mycroft_gui_app_EXTRA_LIBS Qt${QT_MAJOR_VERSION}::WebView)
endif()

qt_add_resources(mycroft_gui_app_SRC
    qml.qrc
    shell-qml.qrc
)

add_executable(mycroft-gui-app ${mycroft_gui_app_SRC})
target_link_libraries(mycroft-gui-app Qt${QT_MAJOR_VERSION}::Core Qt${QT_MAJOR_VERSION}::Quick ${mycroft_gui_app_EXTRA_LIBS})

# Link to the import plugin library and include paths
target_include_directories(mycroft-gui-app PRIVATE ../import ${CMAKE_CURRENT_BINARY_DIR}/../)


install(TARGETS mycroft-gui-app ${INSTALL_TARGETS_DEFAULT_ARGS})
install(PROGRAMS ai.mycroft.gui-app.desktop DESTINATION  ${KDE_INSTALL_APPDIR})
install(PROGRAMS ovos-shell.desktop DESTINATION  ${KDE_INSTALL_APPDIR})
