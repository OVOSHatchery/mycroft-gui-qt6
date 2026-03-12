# OVOSPlugin QML module — provides Configuration and PlacesModel singletons
set(openvoiceshell_SRCS openvoiceshellplugin.cpp configuration.cpp placesmodel.cpp)

add_library(openvoiceshellplugin SHARED ${openvoiceshell_SRCS})

target_link_libraries(openvoiceshellplugin
    Qt6::Core
    Qt6::Quick
    Qt6::Qml
    Qt6::DBus
    KF6::GuiAddons
    KF6::ConfigWidgets
)

install(TARGETS openvoiceshellplugin DESTINATION ${KDE_INSTALL_QMLDIR}/OVOSPlugin)
install(FILES qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/OVOSPlugin)
install(FILES plugins.qmltypes DESTINATION ${KDE_INSTALL_QMLDIR}/OVOSPlugin)
