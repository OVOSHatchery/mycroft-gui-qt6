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
set(OVOS_GUI_COMMON_SRCS
    ${CMAKE_SOURCE_DIR}/import/guipage.cpp
    ${CMAKE_SOURCE_DIR}/import/guinamespace.cpp
    ${CMAKE_SOURCE_DIR}/import/guibusclient.cpp
    ${CMAKE_SOURCE_DIR}/import/namespacemodel.cpp
    ${CMAKE_SOURCE_DIR}/import/pagemodel.cpp
    ${CMAKE_SOURCE_DIR}/import/namespacedatamap.cpp
    ${CMAKE_SOURCE_DIR}/import/namespacedatamodel.cpp
    ${CMAKE_SOURCE_DIR}/import/filereader.cpp
    ${CMAKE_SOURCE_DIR}/import/globalsettings.cpp
    ${CMAKE_SOURCE_DIR}/import/mediaservice.cpp
    ${CMAKE_SOURCE_DIR}/import/mediaproviders/videoproviderservice.cpp
    ${CMAKE_SOURCE_DIR}/import/mediaproviders/audioproviderservice.cpp
    ${CMAKE_SOURCE_DIR}/import/mediaproviders/audiostreamdevice.cpp
    ${CMAKE_SOURCE_DIR}/import/mediaproviders/thirdparty/fftcalc.cpp
    ${CMAKE_SOURCE_DIR}/import/mediaproviders/thirdparty/fft.cpp
)

add_library(OvosGuiCommon STATIC ${OVOS_GUI_COMMON_SRCS})
target_include_directories(OvosGuiCommon PUBLIC ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_SOURCE_DIR}/import)

target_link_libraries(OvosGuiCommon PUBLIC
    Qt${QT_MAJOR_VERSION}::Core
    Qt${QT_MAJOR_VERSION}::Qml
    Qt${QT_MAJOR_VERSION}::Quick
    Qt${QT_MAJOR_VERSION}::Network
    Qt${QT_MAJOR_VERSION}::WebSockets
    Qt${QT_MAJOR_VERSION}::Multimedia
    Qt${QT_MAJOR_VERSION}::Gui
    KF6::Kirigami
    KF6::CoreAddons
)

# Allow position independent code for the static library
set_target_properties(OvosGuiCommon PROPERTIES POSITION_INDEPENDENT_CODE ON)


# System templates directory
if(NOT OVOS_SYSTEM_TEMPLATES_DIR)
    set(OVOS_SYSTEM_TEMPLATES_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/ovos-gui/system-templates")
endif()
target_compile_definitions(OvosGuiCommon PUBLIC OVOS_SYSTEM_TEMPLATES_DIR="${OVOS_SYSTEM_TEMPLATES_DIR}")
add_definitions(-DOVOS_SYSTEM_TEMPLATES_DIR="${OVOS_SYSTEM_TEMPLATES_DIR}")

# Compiler flags
add_compile_options(
    -Wall
    -Wextra
    -Werror=format
    -Wno-unused-parameter
)

# Qt6 specific settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Application, shell plugin, theme, and color schemes
include(KDEInstallDirs)
include(KDECMakeSettings)

add_subdirectory(application)
add_subdirectory(application/shell)
add_subdirectory(theme)
add_subdirectory(schemes)

# Tests
enable_testing()
add_subdirectory(autotests)

# System templates (SYSTEM: URI scheme resources)
install(DIRECTORY import/system-templates/
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/ovos-gui/system-templates
    FILES_MATCHING PATTERN "*.qml"
)

# Documentation
install(FILES
    README.md
    LICENSE
    DESTINATION share/doc/mycroft-gui-qt6
)
