# Set minimum CMake version (required for CMake 3.0 or later)
cmake_minimum_required(VERSION 3.16.0)

option(BUILD_REMOTE_TTS "Build remote TTS support" OFF)
option(BUILD_PLASMA_MOBILE "Build remote TTS support" OFF)

set(QT_MIN_VERSION "6.4.0")
set(KF_MIN_VERSION "5.240.0")

IF(POLICY CMP0048)
    CMAKE_POLICY(SET CMP0048 NEW)
ENDIF(POLICY CMP0048)

PROJECT(mycroft-gui)
PROJECT(mycroft-gui VERSION "1.0.1")

# Use Extra CMake Modules (ECM) for common functionality.
# See http://api.kde.org/ecm/manual/ecm.7.html
# and http://api.kde.org/ecm/manual/ecm-kde-modules.7.html
IF(POLICY CMP0063)
    CMAKE_POLICY(SET CMP0063 NEW)
ENDIF(POLICY CMP0063)

find_package(ECM 1.6.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(ECMInstallIcons)
set(CMAKE_AUTOMOC ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -Werror")

#this is used for mycroft spawning. It's horrible and should not be needed, but mycroft packaging is questionable at best
#in any sane world mycroft would install something in $PATH
#the Android case is a temporary hack, adjust it to where and android app can find
if(CMAKE_SYSTEM_NAME STREQUAL "Android")
    set(MYCROFT_CORE_DIR ":")
else(MYCROFT_CORE_DIR "$HOME/mycroft-core")
    set(MYCROFT_CORE_DIR "$HOME/mycroft-core")
endif()

#configure_file(config-mycroft.h.in ${CMAKE_CURRENT_BINARY_DIR}/config-mycroft.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

find_package(Qt${QT_MAJOR_VERSION} ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    Quick
    Core
    Qml
    Network
    WebSockets
    WebView
    Multimedia
)

if (QT_MAJOR_VERSION STREQUAL "6")
    find_package(Qt${QT_MAJOR_VERSION} ${QT_MIN_VERSION} REQUIRED COMPONENTS WebEngineQuick)
else ()
    find_package(Qt${QT_MAJOR_VERSION} ${QT_MIN_VERSION} REQUIRED COMPONENTS WebEngine)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Android")
    find_package(Qt${QT_MAJOR_VERSION} ${QT_MIN_VERSION} REQUIRED COMPONENTS
        AndroidExtras
        QuickControls2
        TextToSpeech
        Svg
    )
    find_package(OpenSSL REQUIRED)
endif()

find_package(KF6 ${KF_MIN_VERSION} REQUIRED COMPONENTS I18n)

if(NOT CMAKE_SYSTEM_NAME STREQUAL "Android")
    find_package(Qt${QT_MAJOR_VERSION} ${QT_MIN_VERSION} REQUIRED COMPONENTS Widgets)
    find_package(KF6 ${KF_MIN_VERSION} REQUIRED COMPONENTS Kirigami2 DBusAddons)
    # find_package(KF5Plasma ${KF5_MIN_VERSION} REQUIRED)
    # find_package(KF5KIO ${KF5_MIN_VERSION} REQUIRED)
endif()

add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x060F00)

add_definitions(-DQT_NO_URL_CAST_FROM_STRING -DQT_USE_QSTRINGBUILDER -DQT_NO_CAST_TO_ASCII -DQT_NO_CAST_FROM_ASCII)

add_subdirectory(application)
add_subdirectory(icons)
add_subdirectory(import)

if (BUILD_TESTING AND NOT CMAKE_SYSTEM_NAME STREQUAL "Android")
    #add_subdirectory(autotests)
endif()
