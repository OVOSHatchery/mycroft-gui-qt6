#pragma once

#include <QObject>
#include <QProcess>

class ResetOperations : public QObject
{
    Q_OBJECT

public:
    ResetOperations(QObject *parent = nullptr);
    ~ResetOperations();

    Q_INVOKABLE void runResetOperations(const QString &resetScriptPath);
    Q_INVOKABLE void runRestartDevice();
    void onReadyReadStandardOutput();
    void onReadyReadStandardError();
    void onFinished(int exitCode, QProcess::ExitStatus exitStatus);

Q_SIGNALS:
    void resetOperationsStarted();
    void resetOperationsFinished();

private:
    QProcess *m_process;
};