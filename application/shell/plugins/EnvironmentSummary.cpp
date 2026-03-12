#include "EnvironmentSummary.h"

EnvironmentSummary::EnvironmentSummary(QObject *parent)
    : QObject(parent)
{
}

EnvironmentSummary::~EnvironmentSummary()
{
}

QString EnvironmentSummary::readVariable(const QString &name)
{
    return QString::fromLocal8Bit(qgetenv(name.toLocal8Bit()));
}