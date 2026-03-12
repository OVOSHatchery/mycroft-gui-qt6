#include "ResetOperations.h"
#include <QDebug>

ResetOperations::ResetOperations(QObject *parent)
    : QObject(parent)
    , m_process(new QProcess(this))
{
    connect(m_process, &QProcess::started, this, &ResetOperations::resetOperationsStarted);
    connect(m_process, &QProcess::readyReadStandardOutput, this, &ResetOperations::onReadyReadStandardOutput);
    connect(m_process, &QProcess::readyReadStandardError, this, &ResetOperations::onReadyReadStandardError);
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &ResetOperations::onFinished);
}

ResetOperations::~ResetOperations()
{
    delete m_process;
}

void ResetOperations::runResetOperations(const QString &resetScriptPath)
{
    QStringList arguments;
    m_process->start(resetScriptPath, arguments);

    if (!m_process->waitForStarted()) {
        qWarning() << "Failed to start reset script";
    }

    Q_EMIT resetOperationsStarted();
}

void ResetOperations::runRestartDevice()
{
    QStringList arguments;
    arguments << QStringLiteral("-f");
    m_process->start(QStringLiteral("systemctl reboot"), arguments);

    if (!m_process->waitForStarted()) {
        qWarning() << "Failed to start reboot";
    }

    Q_EMIT resetOperationsStarted();
}

void ResetOperations::onReadyReadStandardOutput()
{
    const QString output = QString::fromUtf8(m_process->readAllStandardOutput());
    qDebug() << output;
}

void ResetOperations::onReadyReadStandardError()
{
    const QString output = QString::fromUtf8(m_process->readAllStandardError());
    qDebug() << output;
}

void ResetOperations::onFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitCode)
    Q_UNUSED(exitStatus)

    Q_EMIT resetOperationsFinished();
}