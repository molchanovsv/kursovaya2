#include "theme.h"
#include <QStyleFactory>
#include <QPalette>
#include <QColor>
#include <QStyle>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QHash>
#include <QFont>

namespace {
QFont& defaultFont()
{
    static QFont f;
    return f;
}
QJsonObject loadThemes()
{
    QFile f(":/themes.json");
    if (!f.open(QIODevice::ReadOnly))
        return {};
    QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
    return doc.object();
}

QJsonObject& themes()
{
    static QJsonObject obj = loadThemes();
    return obj;
}

QPalette::ColorRole roleFromString(const QString& name)
{
    static const QHash<QString, QPalette::ColorRole> map = {
        {"window", QPalette::Window},
        {"windowtext", QPalette::WindowText},
        {"base", QPalette::Base},
        {"alternatebase", QPalette::AlternateBase},
        {"tooltipbase", QPalette::ToolTipBase},
        {"tooltiptext", QPalette::ToolTipText},
        {"text", QPalette::Text},
        {"button", QPalette::Button},
        {"buttontext", QPalette::ButtonText},
        {"brighttext", QPalette::BrightText},
        {"link", QPalette::Link},
        {"highlight", QPalette::Highlight},
        {"highlightedtext", QPalette::HighlightedText}
    };
    return map.value(name.toLower(), QPalette::Window);
}
}

Theme themeFromString(const QString& name) {
    QString n = name.toLower();
    if (n == "madagascar") return Theme::Madagascar;
    if (n == "sonic") return Theme::Sonic;
    if (n == "gojo" || n == "gojosatoru") return Theme::GojoSatoru;
    if (n == "sans") return Theme::Sans;

    return Theme::Dark;
}

QString themeToString(Theme t) {
    switch (t) {
    case Theme::Madagascar: return "madagascar";
    case Theme::Sonic: return "sonic";
    case Theme::GojoSatoru: return "gojo";
    case Theme::Sans: return "sans";
    default: return "dark";
    }
}

void applyTheme(Theme t, QApplication& app) {
    app.setStyle(QStyleFactory::create("Fusion"));
    QPalette p = app.style()->standardPalette();

    if (defaultFont().family().isEmpty())
        defaultFont() = app.font();

    QJsonObject themeObj = themes().value(themeToString(t)).toObject();
    for (auto it = themeObj.constBegin(); it != themeObj.constEnd(); ++it) {
        if (it.key().toLower() == "font")
            continue;
        QPalette::ColorRole role = roleFromString(it.key());
        QColor color(it.value().toString());
        if (color.isValid())
            p.setColor(role, color);
    }
    app.setPalette(p);
    if (t == Theme::Sans) {
        app.setFont(QFont("Comic Sans MS"));
    } else {
        QString fontName = themeObj.value("font").toString();
        if (!fontName.isEmpty())
            app.setFont(QFont(fontName));
        else
            app.setFont(defaultFont());
    }
}

