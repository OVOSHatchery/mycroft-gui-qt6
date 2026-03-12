cmake_minimum_required(VERSION 3.24)

project(mycroft-gui-qt6 LANGUAGES CXX)

# Qt6 requires C++17 minimum
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt6
find_package(Qt6 6.5 REQUIRED COMPONENTS
    Core
    Qml
    Quick
    Network
    WebSockets
    Multimedia
    Gui
    Widgets
    DBus
)

# WebView is optional
find_package(Qt6 QUIET COMPONENTS WebView)

find_package(ECM 6.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

# KDE Frameworks 6
find_package(KF6 6.0 REQUIRED COMPONENTS
    Kirigami
    CoreAddons
    GuiAddons
    Config
    ConfigWidgets
    IconThemes
    DBusAddons
)

# Minimum versions
set(KF6_MIN_VERSION "6.0.0")
set(Qt6_MIN_VERSION "6.5.0")

# Optional KDE/Plasma components
find_package(KF6Plasma QUIET)
find_package(KF6KIO QUIET)
find_package(KF6DBusAddons QUIET)

# Generate configuration header
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/import/controllerconfig.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/controllerconfig.h
)

# Source files
set(mycroft_gui_SRCS
    import/abstractdelegate.cpp
    import/abstractskillview.cpp
    import/mycroftcontroller.cpp
    import/activeskillsmodel.cpp
    import/delegatesmodel.cpp
    import/sessiondatamap.cpp
    import/sessiondatamodel.cpp
    import/filereader.cpp
    import/globalsettings.cpp
)

# QML resources
qt6_add_resources(mycroft_gui_SRCS import/mycroft.qrc)

# Main library
add_library(mycroft-gui-qt6 SHARED ${mycroft_gui_SRCS})

# Include directory for generated headers
target_include_directories(mycroft-gui-qt6 PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(mycroft-gui-qt6
    PUBLIC
        Qt6::Core
        Qt6::Qml
        Qt6::Quick
        Qt6::Network
        Qt6::WebSockets
        Qt6::Multimedia
        Qt6::Gui
    PRIVATE
        KF6::Kirigami
        KF6::CoreAddons
)

if(KF6Plasma_FOUND)
    target_link_libraries(mycroft-gui-qt6 PRIVATE KF6::Plasma)
endif()

if(KF6KIO_FOUND)
    target_link_libraries(mycroft-gui-qt6 PRIVATE KF6::KIOWidgets)
endif()

# Compiler flags
target_compile_options(mycroft-gui-qt6 PRIVATE
    -Wall
    -Wextra
    -Werror=format
    -Wno-unused-parameter
)

# Qt6 specific settings
set_target_properties(mycroft-gui-qt6 PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# Application, shell plugin, theme, and color schemes
include(KDEInstallDirs)
include(KDECMakeSettings)
add_subdirectory(application)
# Shell plugin requires additional KDE components - skip for now
#add_subdirectory(application/shell)
# Theme has KF6 compatibility issues - skip for now
#add_subdirectory(theme)
add_subdirectory(schemes)

# Tests
enable_testing()
add_subdirectory(autotests)

# Installation
install(TARGETS mycroft-gui-app
    RUNTIME DESTINATION bin
)

install(TARGETS mycroft-gui-qt6
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# QML plugin
install(DIRECTORY import/
    DESTINATION lib/qt6/qml/Mycroft
)

# Documentation
install(FILES
    README.md
    LICENSE
    DESTINATION share/doc/mycroft-gui-qt6
)
