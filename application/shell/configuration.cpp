#include "configuration.h"
#include <KConfigGroup>
#include <KSharedConfig>
#include <KUser>
#include <QColor>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

Configuration &Configuration::self()
{
    static Configuration c;
    return c;
}

QColor Configuration::primaryColor()
{
    static KSharedConfigPtr config = KSharedConfig::openConfig(QLatin1String("OvosTheme"));
    static KConfigGroup grp(config, QLatin1String("ColorScheme"));

    if (grp.isValid()) {
        return grp.readEntry(QLatin1String("primaryColor"), "#313131");
    }

    return QColor::fromHsl(0,0,19);
}

void Configuration::setPrimaryColor(QColor &mPrimaryColor)
{
    static KSharedConfigPtr config = KSharedConfig::openConfig(QLatin1String("OvosTheme"));
    static KConfigGroup grp(config, QLatin1String("ColorScheme"));

    if (primaryColor() == mPrimaryColor)
        return;

    grp.writeEntry(QLatin1String("primaryColor"), mPrimaryColor.name(QColor::HexArgb));
    grp.sync();

    static KConfigGroup grpTwo(config, QLatin1String("SelectedScheme"));
    grpTwo.writeEntry(QLatin1String("name"), "custom");
    grpTwo.writeEntry(QLatin1String("path"), "custom");
    grpTwo.sync();

    m_selectedSchemeName = QLatin1String("custom");
    m_selectedSchemePath = QLatin1String("custom");
    emit primaryColorChanged();
}

QColor Configuration::secondaryColor()
{
    static KSharedConfigPtr config = KSharedConfig::openConfig(QLatin1String("OvosTheme"));
    static KConfigGroup grp(config, QLatin1String("ColorScheme"));

    if (grp.isValid()) {
        return grp.readEntry(QLatin1String("secondaryColor"), "#F70D1A");
    }

    return QColor::fromHsl(357,94,51);
}

void Configuration::setSecondaryColor(QColor &mSecondaryColor)
{
    static KSharedConfigPtr config = KSharedConfig::openConfig(QLatin1String("OvosTheme"));
    static KConfigGroup grp(config, QLatin1String("ColorScheme"));

    if (secondaryColor() == mSecondaryColor)
        return;

    grp.writeEntry(QLatin1String("secondaryColor"), mSecondaryColor.name(QColor::HexArgb));
    grp.sync();

    static KConfigGroup grpTwo(config, QLatin1String("SelectedScheme"));
    grpTwo.writeEntry(QLatin1String("name"), "custom");
    grpTwo.writeEntry(QLatin1String("path"), "custom");
    grpTwo.sync();

    m_selectedSchemeName = QLatin1String("custom");
    m_selectedSchemePath = QLatin1String("custom");
    emit secondaryColorChanged();
}

QColor Configuration::textColor()
{
    static KSharedConfigPtr config = KSharedConfig::openConfig(QLatin1String("OvosTheme"));
    static KConfigGroup grp(config, QLatin1String("ColorScheme"));

    if (grp.isValid()) {
        return grp.readEntry(QLatin1String("textColor"), "#F1F1F1");
    }

    return QColor::fromHsl(0,0,95);
}

void Configuration::setTextColor(QColor &mTextColor)
{
    static KSharedConfigPtr config = KSharedConfig::openConfig(QLatin1String("OvosTheme"));
    static KConfigGroup grp(config, QLatin1String("ColorScheme"));

    if (textColor() == mTextColor)
        return;

    grp.writeEntry(QLatin1String("textColor"), mTextColor.name(QColor::HexArgb));
    grp.sync();

    static KConfigGroup grpTwo(config, QLatin1String("SelectedScheme"));
    grpTwo.writeEntry(QLatin1String("name"), "custom");
    grpTwo.writeEntry(QLatin1String("path"), "custom");
    grpTwo.sync();

    m_selectedSchemeName = QLatin1String("custom");
    m_selectedSchemePath = QLatin1String("custom");
    emit textColorChanged();
}

QString Configuration::themeStyle()
{
    static KSharedConfigPtr config = KSharedConfig::openConfig(QLatin1String("OvosTheme"));
    static KConfigGroup grp(config, QLatin1String("ColorScheme"));

    if (grp.isValid()) {
        return grp.readEntry(QLatin1String("themeStyle"), QStringLiteral("dark"));
    }

    return QStringLiteral("dark");
}

void Configuration::setThemeStyle(QString &mThemeStyle)
{
    static KSharedConfigPtr config = KSharedConfig::openConfig(QLatin1String("OvosTheme"));
    static KConfigGroup grp(config, QLatin1String("ColorScheme"));

    if (themeStyle() == mThemeStyle)
        return;

    grp.writeEntry(QLatin1String("themeStyle"), mThemeStyle);
    grp.sync();

    static KConfigGroup grpTwo(config, QLatin1String("SelectedScheme"));
    grpTwo.writeEntry(QLatin1String("name"), "custom");
    grpTwo.writeEntry(QLatin1String("path"), "custom");
    grpTwo.sync();

    m_selectedSchemeName = QLatin1String("custom");
    m_selectedSchemePath = QLatin1String("custom");
    emit themeStyleChanged();
}

void Configuration::setupSchemeWatcher(){
    m_schemeWatcher.addPath(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QLatin1String("/OVOS/ColorSchemes"));
    m_schemeWatcher.addPath(QLatin1String("/usr/share/OVOS/ColorSchemes"));
    m_schemeWatcher.addPath(QLatin1String("/usr/local/share/OVOS/ColorSchemes"));
    connect(&m_schemeWatcher, &QFileSystemWatcher::directoryChanged, this, &Configuration::updateSchemeList);
}

void Configuration::updateSchemeList(){
    m_schemeList.clear();

    for(int i=0; i<m_jsonArray.count(); i++) {
        m_jsonArray.removeAt(0);
    }

    QDir dir(QLatin1String("/usr/local/share/OVOS/ColorSchemes"));
    QDir dir2(QLatin1String("/usr/share/OVOS/ColorSchemes"));
    QDir dir3(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QLatin1String("/OVOS/ColorSchemes"));

    if (dir.exists() && dir.entryList(QStringList() << QLatin1String("*.json"), QDir::Files).count() > 0) {
        fetchFromFolder(dir);
    }
    if (dir2.exists() && dir2.entryList(QStringList() << QLatin1String("*.json"), QDir::Files).count() > 0) {
        fetchFromFolder(dir2);
    }
    if (dir3.exists() && dir3.entryList(QStringList() << QLatin1String("*.json"), QDir::Files).count() > 0) {
        fetchFromFolder(dir3);
    }

    m_finalObject = QJsonObject({
        {QLatin1String("schemes"), m_jsonArray}
    });
    m_schemeList.insert(m_finalObject.toVariantMap());
    updateSelectedScheme();
    emit schemeListChanged();
}

void Configuration::fetchFromFolder(QDir &dir)
{
    qDebug() << "Fetching from folder: " << dir.absolutePath();
    QStringList filters;
    filters << QLatin1String("*.json");
    QFileInfoList fileList = dir.entryInfoList(filters, QDir::Files);
    qDebug() << "Found " << fileList.count() << " files";

    for (int i = 0; i < fileList.count(); i++) {
        QFile file(fileList.at(i).absoluteFilePath());
        if (file.open(QIODevice::ReadOnly)) {
            QJsonParseError error;
            QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &error);
            if (error.error == QJsonParseError::NoError) {
                QJsonObject obj = doc.object();
                obj.insert(QLatin1String("path"), fileList.at(i).absoluteFilePath());

                if (!m_jsonArray.contains(obj)) {
                    m_jsonArray.append(obj);
                }
            }
        }
    }
}

QVariantMap Configuration::getSchemeList()
{
    self().updateSchemeList();
    return m_schemeList;
}

QString Configuration::getSelectedSchemeName()
{
    return m_selectedSchemeName;
}

void Configuration::updateSelectedScheme()
{
    static KSharedConfigPtr config = KSharedConfig::openConfig(QLatin1String("OvosTheme"));
    static KConfigGroup grp(config, QLatin1String("SelectedScheme"));

    if (grp.isValid()) {
        m_selectedSchemeName = grp.readEntry(QLatin1String("name"), "default");
        m_selectedSchemePath = grp.readEntry(QLatin1String("path"), "default");
    }
}

void Configuration::setSelectedSchemeName(QString &mSelectedSchemeName)
{
    if (mSelectedSchemeName == m_selectedSchemeName)
        return;

    m_selectedSchemeName = mSelectedSchemeName;
    emit selectedSchemeNameChanged();
}

QString Configuration::getSelectedSchemePath()
{
    return m_selectedSchemePath;
}

void Configuration::setSelectedSchemePath(QString &mSelectedSchemePath)
{
    if (mSelectedSchemePath == m_selectedSchemePath)
        return;

    m_selectedSchemePath = mSelectedSchemePath;
    emit selectedSchemePathChanged();
}

bool Configuration::isSchemeValid(){
    QFile inFile(m_selectedSchemePath);
    inFile.open(QIODevice::ReadOnly|QIODevice::Text);
    QByteArray data = inFile.readAll();
    inFile.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject obj = doc.object();
    if (obj.contains(QLatin1String("primaryColor")) && obj.contains(QLatin1String("secondaryColor")) && obj.contains(QLatin1String("textColor")))
        return true;
    
    return false;
}

QVariantMap Configuration::getScheme(QString &schemePath)
{
    QFile inFile(schemePath);
    inFile.open(QIODevice::ReadOnly|QIODevice::Text);
    QByteArray data = inFile.readAll();
    inFile.close();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject obj = doc.object();
    return obj.toVariantMap();
}

void Configuration::setScheme(QString schemeName, QString schemePath, QString schemeStyle)
{
    QFile inFile(schemePath);
    inFile.open(QIODevice::ReadOnly|QIODevice::Text);
    QByteArray data = inFile.readAll();
    inFile.close();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject obj = doc.object();
    QString name = obj.value(QLatin1String("name")).toString();
    QString primaryColor = obj.value(QLatin1String("primaryColor")).toString();
    QString secondaryColor = obj.value(QLatin1String("secondaryColor")).toString();
    QString textColor = obj.value(QLatin1String("textColor")).toString();
    QString themeStyle = schemeStyle;

    static KSharedConfigPtr config = KSharedConfig::openConfig(QLatin1String("OvosTheme"));
    static KConfigGroup grp(config, QLatin1String("ColorScheme"));

    grp.writeEntry(QLatin1String("name"), name);
    grp.writeEntry(QLatin1String("primaryColor"), primaryColor);
    grp.writeEntry(QLatin1String("secondaryColor"), secondaryColor);
    grp.writeEntry(QLatin1String("textColor"), textColor);
    grp.writeEntry(QLatin1String("themeStyle"), themeStyle);
    grp.sync();

    static KConfigGroup grpTwo(config, QLatin1String("SelectedScheme"));
    grpTwo.writeEntry(QLatin1String("name"), name);
    grpTwo.writeEntry(QLatin1String("path"), schemePath);
    grpTwo.sync();

    setSelectedSchemeName(name);
    setSelectedSchemePath(schemePath);
    emit schemeChanged();
}
