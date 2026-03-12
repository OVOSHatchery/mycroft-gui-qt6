#ifndef KIRIGAMIPLASMAFACTORY_H
#define KIRIGAMIPLASMAFACTORY_H

#include <Kirigami/KirigamiPluginFactory>
#include <QObject>
#include <QPluginLoader>

class OpenVoiceStyleFactory : public Kirigami::KirigamiPluginFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.Kirigami.KirigamiPluginFactory" FILE "ovostheme.json")
    Q_INTERFACES(Kirigami::KirigamiPluginFactory)

public:
    explicit OpenVoiceStyleFactory(QObject *parent = nullptr);
    ~OpenVoiceStyleFactory() override;

    Kirigami::PlatformTheme *createPlatformTheme(QObject *parent) override;
};

#endif // KIRIGAMIPLASMAFACTORY_H
