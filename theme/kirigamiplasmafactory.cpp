#include "kirigamiplasmafactory.h"
#include "ovostheme.h"

OpenVoiceStyleFactory::OpenVoiceStyleFactory(QObject *parent)
    : Kirigami::Platform::PlatformPluginFactory(parent)
{
}

OpenVoiceStyleFactory::~OpenVoiceStyleFactory() = default;

Kirigami::Platform::PlatformTheme *OpenVoiceStyleFactory::createPlatformTheme(QObject *parent)
{
    return new OvosTheme(parent);
}

Kirigami::Platform::Units *OpenVoiceStyleFactory::createUnits(QObject *parent)
{
    return nullptr; // Fallback to default units
}
