#pragma once

#include <QObject>

// Simple QObject class to read environment variables from QML
class EnvironmentSummary : public QObject
{
    Q_OBJECT

public:
    EnvironmentSummary(QObject *parent = nullptr);
    ~EnvironmentSummary();

    Q_INVOKABLE QString readVariable(const QString &name);
};