#ifndef OVOSTHEME_H
#define OVOSTHEME_H

#include <Kirigami/PlatformTheme>
#include <Kirigami/KirigamiPluginFactory>

#include <QObject>
#include <QIcon>
#include <QColor>
#include <QFileSystemWatcher>

class OvosTheme : public Kirigami::PlatformTheme
{
    Q_OBJECT
    QPalette lightPalette;

public:
    explicit OvosTheme(QObject *parent = nullptr);

    Q_INVOKABLE QIcon iconFromTheme(const QString &name, const QColor &customColor = Qt::transparent) override;

    void syncColors();
    void syncWindow();
    void readConfig();
    void syncConfigChanges();
    void setupFileWatch();

protected:
    bool event(QEvent *event) override;

private:
    QColor m_primaryColor;
    QColor m_secondaryColor;
    QColor m_textColor;
    QString m_themeStyle;
    QPointer<QWindow> m_window;

    QFileSystemWatcher *m_fileWatcher;
};

#endif
