#ifndef KIRIGAMIPLASMAFACTORY_H
#define KIRIGAMIPLASMAFACTORY_H

#include <Kirigami/Platform/PlatformPluginFactory>
#include <QObject>
#include <QPluginLoader>

class OpenVoiceStyleFactory : public Kirigami::Platform::PlatformPluginFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.kirigami.PlatformPluginFactory" FILE "ovostheme.json")
    Q_INTERFACES(Kirigami::Platform::PlatformPluginFactory)

public:
    explicit OpenVoiceStyleFactory(QObject *parent = nullptr);
    ~OpenVoiceStyleFactory() override;

    Kirigami::Platform::PlatformTheme *createPlatformTheme(QObject *parent) override;
    Kirigami::Platform::Units *createUnits(QObject *parent) override;
};

#endif // KIRIGAMIPLASMAFACTORY_H
