#ifndef DEMOUTILS_H
#define DEMOUTILS_H

#include <QJsonObject>
#include <QJsonArray>
#include <QStringList>
#include <QString>

namespace DemoUtils {

static const QString kDemoNamespace = QStringLiteral("ovos.gui.namespace.demo");

static inline QJsonObject demoNamespaceData(const QString &templateName)
{
    if (templateName == "HomeScreen" || templateName == "Idle") return QJsonObject{
        {QStringLiteral("time_string"), QStringLiteral("10:45")},
        {QStringLiteral("date_string"), QStringLiteral("FRIDAY, MARCH 13")},
        {QStringLiteral("wallpaper_path"), QStringLiteral("https://images.unsplash.com/photo-1470770841072-f978cf4d019e?auto=format&fit=crop&w=1200&q=80")}};
    if (templateName == "Loading") return QJsonObject{{QStringLiteral("label"), QStringLiteral("Loading demo...")}};
    if (templateName == "Status") return QJsonObject{{QStringLiteral("label"), QStringLiteral("Demo status")}, {QStringLiteral("success"), true}};
    if (templateName == "Error") return QJsonObject{{QStringLiteral("label"), QStringLiteral("Demo error")}, {QStringLiteral("detail"), QStringLiteral("This is a test error message")}};
    if (templateName == "Clock") return QJsonObject{
        {QStringLiteral("weekday_string"),  QStringLiteral("THURSDAY")},
        {QStringLiteral("daymonth_string"), QStringLiteral("13 MARCH")},
        {QStringLiteral("year_string"),     QStringLiteral("2026")}};
    if (templateName == "Face") return QJsonObject{{QStringLiteral("sleeping"), false}};
    if (templateName == "Text") return QJsonObject{{QStringLiteral("title"), QStringLiteral("Demo Text")}, {QStringLiteral("text"), QStringLiteral("This is a demonstration of the text template.")}};
    if (templateName == "Image") return QJsonObject{{QStringLiteral("title"), QStringLiteral("Demo Image")}, {QStringLiteral("image"), QStringLiteral("https://picsum.photos/800/600")}, {QStringLiteral("caption"), QStringLiteral("Random image")}};
    if (templateName == "AnimatedImage") return QJsonObject{{QStringLiteral("title"), QStringLiteral("Demo Animation")}, {QStringLiteral("image"), QStringLiteral("https://media.giphy.com/media/JIX9t2j0ZTN9S/giphy.gif")}, {QStringLiteral("caption"), QStringLiteral("Animated demo")}};
    if (templateName == "Html") return QJsonObject{{QStringLiteral("html"), QStringLiteral("<h1>Hello OVOS</h1><p>This is a <b>HTML</b> template demo.</p>")}};
    if (templateName == "Url") return QJsonObject{{QStringLiteral("url"), QStringLiteral("https://openvoiceos.org")}};
    if (templateName == "Map") return QJsonObject{{QStringLiteral("latitude"), 48.8566}, {QStringLiteral("longitude"), 2.3522}, {QStringLiteral("zoom"), 12}, {QStringLiteral("label"), QStringLiteral("Paris, France")}};
    if (templateName == "Weather") return QJsonObject{
        {QStringLiteral("weatherCode"),          2},
        {QStringLiteral("currentTemperature"),   QStringLiteral("22°C")},
        {QStringLiteral("highTemperature"),      QStringLiteral("25°C")},
        {QStringLiteral("lowTemperature"),       QStringLiteral("18°C")},
        {QStringLiteral("condition"),            QStringLiteral("Partly Cloudy")},
        {QStringLiteral("windSpeed"),            QStringLiteral("12 km/h")},
        {QStringLiteral("humidity"),             QStringLiteral("58%")},
        {QStringLiteral("chanceOfPrecipitation"),QStringLiteral("10%")},
        {QStringLiteral("weatherLocation"),      QStringLiteral("Demo City")},
        {QStringLiteral("icon"),                 QStringLiteral("https://raw.githubusercontent.com/OpenVoiceOS/ovos-skill-weather/dev/ui/images/partial_clouds_day.png")},
        {QStringLiteral("sunrise"),              QStringLiteral("06:42")},
        {QStringLiteral("sunset"),               QStringLiteral("19:55")}};
    if (templateName == "Timer") return QJsonObject{
        {QStringLiteral("seconds"),  125},
        {QStringLiteral("duration"), 300},
        {QStringLiteral("label"),    QStringLiteral("Demo Timer")},
        {QStringLiteral("countdown"), true}};
    if (templateName == "Alarm") return QJsonObject{
        {QStringLiteral("alarmName"),      QStringLiteral("Morning Alarm")},
        {QStringLiteral("alarmTime"),      QStringLiteral("07:30")},
        {QStringLiteral("alarmAmPm"),      QStringLiteral("AM")},
        {QStringLiteral("alarmExpired"),   false},
        {QStringLiteral("alarmRepeat"),    true},
        {QStringLiteral("alarmRepeatStr"), QStringLiteral("Every weekday")},
        {QStringLiteral("alarmIndex"),     0}};
    if (templateName == "AudioPlayer") return QJsonObject{{QStringLiteral("title"), QStringLiteral("Demo Song")}, {QStringLiteral("artist"), QStringLiteral("OVOS")}, {QStringLiteral("album"), QStringLiteral("Templates")}, {QStringLiteral("playing"), true}, {QStringLiteral("duration"), 180000}, {QStringLiteral("position"), 45000}, {QStringLiteral("thumbnail"), QStringLiteral("https://picsum.photos/200")}};
    if (templateName == "MediaPlayer") return QJsonObject{{QStringLiteral("title"), QStringLiteral("Demo Media")}, {QStringLiteral("artist"), QStringLiteral("OVOS")}, {QStringLiteral("media_type"), QStringLiteral("audio")}, {QStringLiteral("playing"), true}, {QStringLiteral("media_url"), QStringLiteral("https://www.soundhelix.com/examples/mp3/SoundHelix-Song-1.mp3")}};
    if (templateName == "VideoPlayer") return QJsonObject{{QStringLiteral("video_url"), QStringLiteral("https://raw.githubusercontent.com/OpenVoiceOS/ovos-media/dev/test/test_media/test_video.mp4")}, {QStringLiteral("playing"), true}};
    if (templateName == "OCPNowPlaying") return QJsonObject{{QStringLiteral("title"), QStringLiteral("OCP Track")}, {QStringLiteral("artist"), QStringLiteral("OCP Artist")}, {QStringLiteral("image"), QStringLiteral("https://picsum.photos/400")}, {QStringLiteral("playing"), true}, {QStringLiteral("media_type"), QStringLiteral("audio")}, {QStringLiteral("uri"), QStringLiteral("https://cdn.pixabay.com/audio/2022/03/15/audio_783932411c.mp3")}};

    return QJsonObject{};
}

static inline QStringList allTemplates()
{
    return {
        "HomeScreen", "Idle", "Loading", "Status", "Error", "Face", "Clock",
        "Text", "Image", "AnimatedImage", "Html", "Url", "Map", "Weather",
        "Timer", "Alarm", "AudioPlayer", "MediaPlayer", "VideoPlayer", "OCPNowPlaying"
    };
}

} // namespace DemoUtils

#endif
