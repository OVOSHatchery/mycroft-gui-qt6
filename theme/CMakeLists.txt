set(OpenVoiceStyle_SRCS
    ovostheme.cpp
    kirigamiplasmafactory.cpp
)

kcoreaddons_add_plugin(OpenVoiceStyle SOURCES ${OpenVoiceStyle_SRCS} INSTALL_NAMESPACE "kf6/kirigami" JSON ovostheme.json)

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
