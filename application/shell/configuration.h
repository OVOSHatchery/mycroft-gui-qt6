#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QObject>
#include <QColor>
#include <QVariant>
#include <QDir>
#include <QJsonArray>
#include <QJsonObject>
#include <QFileSystemWatcher>

class Q_DECL_EXPORT Configuration : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QColor primaryColor READ primaryColor WRITE setPrimaryColor NOTIFY primaryColorChanged)
    Q_PROPERTY(QColor secondaryColor READ secondaryColor WRITE setSecondaryColor NOTIFY secondaryColorChanged)
    Q_PROPERTY(QColor textColor READ textColor WRITE setTextColor NOTIFY textColorChanged)
    Q_PROPERTY(QString themeStyle READ themeStyle WRITE setThemeStyle NOTIFY themeStyleChanged)

public:
    static Configuration &self();

public Q_SLOTS:
    QColor primaryColor();
    void setPrimaryColor(QColor &mPrimaryColor);

    QColor secondaryColor();
    void setSecondaryColor(QColor &mSecondaryColor);

    QColor textColor();
    void setTextColor(QColor &mTextColor);

    QString themeStyle();
    void setThemeStyle(QString &mThemeStyle);

    QString getSelectedSchemeName();
    void setSelectedSchemeName(QString &schemeName);

    QString getSelectedSchemePath();
    void setSelectedSchemePath(QString &schemePath);

    QVariantMap getSchemeList();
    QVariantMap getScheme(QString &schemePath);

    bool isSchemeValid();
    void updateSchemeList();
    void fetchFromFolder(QDir &dir);

    void setupSchemeWatcher();

    void setScheme(QString schemeName, QString schemePath, QString schemeStyle);

    void updateSelectedScheme();

Q_SIGNALS:
    void primaryColorChanged();
    void secondaryColorChanged();
    void textColorChanged();
    void themeStyleChanged();
    
    void selectedSchemeNameChanged();
    void selectedSchemePathChanged();
    void schemeListChanged();

    void schemeChanged();

private:
    QString m_selectedSchemeName;
    QString m_selectedSchemePath;
    QVariantMap m_schemeList;
    QFileSystemWatcher m_schemeWatcher;
    QJsonArray m_jsonArray;
    QJsonObject m_finalObject;
};

#endif // CONFIGURATION_H
