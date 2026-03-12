set(OpenVoiceStyle_SRCS
    ovostheme.cpp
    kirigamiplasmafactory.cpp
)

add_library(OpenVoiceStyle SHARED ${OpenVoiceStyle_SRCS})
set_target_properties(OpenVoiceStyle PROPERTIES PREFIX "")

install(TARGETS OpenVoiceStyle DESTINATION ${KDE_INSTALL_PLUGINDIR}/kf6/kirigami)
install(FILES ovostheme.json DESTINATION ${KDE_INSTALL_PLUGINDIR}/kf6/kirigami)

target_link_libraries(OpenVoiceStyle
    PUBLIC
        Qt6::Core
        KF6::Kirigami
    PRIVATE
        Qt6::DBus
        Qt6::Qml
        Qt6::Quick
        KF6::GuiAddons
        KF6::ConfigWidgets
)

install(FILES Theme.qml DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/kirigami/styles/OpenVoiceStyle)
