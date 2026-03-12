#ifndef AUDIOREC_H
#define AUDIOREC_H

#include <QObject>
#include <QFile>
#include <QtWebSockets>
#include "mycroftcontroller.h"
#include "controllerconfig.h"

#include <QAudioSource>

class AudioRec : public QObject
{
    Q_OBJECT
public:
    explicit AudioRec(QObject *parent = nullptr);

public Q_SLOTS:
    void recordTStart();
    void recordTStop();
    void readStream();
    void returnStream();
    void captureDataFromDevice();

Q_SIGNALS:
    void recordTStatus(const QString &recStatus);
    void micAudioLevelChanged(const qreal &micLevel);

private:
    OVOSController *m_controller;
    QFile destinationFile;
    QByteArray m_audStream;
    qint16 m_audStream_size;
    QAudioSource *audio;
    QIODevice *device;
};

#endif // AUDIOREC_H