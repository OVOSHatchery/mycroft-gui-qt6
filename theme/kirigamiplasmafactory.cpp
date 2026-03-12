#include "kirigamiplasmafactory.h"
#include "ovostheme.h"

OpenVoiceStyleFactory::OpenVoiceStyleFactory(QObject *parent)
    : Kirigami::KirigamiPluginFactory(parent)
{
}

OpenVoiceStyleFactory::~OpenVoiceStyleFactory() = default;

Kirigami::PlatformTheme *OpenVoiceStyleFactory::createPlatformTheme(QObject *parent)
{
    return new OvosTheme(parent);
}
