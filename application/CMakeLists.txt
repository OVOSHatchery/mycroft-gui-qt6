PROJECT(mycroftguiapp)
include(ECMSetupVersion)

if (CMAKE_SYSTEM_NAME STREQUAL "Android")
    add_subdirectory("android")

    if ($ENV{BUILD_NUMBER})
        PROJECT(mycroftguiapp VERSION "0.$ENV{BUILD_NUMBER}")
    else()
        PROJECT(mycroftguiapp VERSION "0.91")
    endif()

    SET(mycroft_gui_app_SRC
        main.cpp
        appsettings.cpp
        speechintent.cpp
        keyfilter.cpp
        shell/plugins/EnvironmentSummary.cpp
        shell/plugins/ResetOperations.cpp
    )
else()
    PROJECT(mycroftguiapp VERSION "1.0.1")
    SET(mycroft_gui_app_SRC
        main.cpp
        appsettings.cpp
        shell/plugins/EnvironmentSummary.cpp
        shell/plugins/ResetOperations.cpp
    )
endif()

ecm_setup_version(PROJECT VERSION_HEADER version.h)

set(mycroft_gui_app_EXTRA_LIBS
    Qt${QT_MAJOR_VERSION}::Widgets
    KF6::DBusAddons
)

qt_add_resources(mycroft_gui_app_SRC
    qml.qrc
    shell-qml.qrc
)

add_executable(mycroft-gui-app ${mycroft_gui_app_SRC})
target_link_libraries(mycroft-gui-app Qt${QT_MAJOR_VERSION}::Core Qt${QT_MAJOR_VERSION}::Quick Qt${QT_MAJOR_VERSION}::WebView ${mycroft_gui_app_EXTRA_LIBS})


install(TARGETS mycroft-gui-app ${INSTALL_TARGETS_DEFAULT_ARGS})
install(PROGRAMS ai.mycroft.gui-app.desktop DESTINATION  ${KDE_INSTALL_APPDIR})
install(PROGRAMS ovos-shell.desktop DESTINATION  ${KDE_INSTALL_APPDIR})

if (CMAKE_SYSTEM_NAME STREQUAL "Android")
    find_package(KF6Kirigami2 REQUIRED)
    kirigami_package_breeze_icons(ICONS help-hint configure media-playback-pause media-playback-start media-seek-forward media-seek-backward audio-input-microphone help-about)
endif()
