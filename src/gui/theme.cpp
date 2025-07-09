#include "theme.h"
#include <QStyleFactory>

Theme themeFromString(const QString& name) {
    QString n = name.toLower();
    if (n == "light") return Theme::Light;
    if (n == "madagascar") return Theme::Madagascar;
    return Theme::Dark;
}

QString themeToString(Theme t) {
    switch (t) {
    case Theme::Light: return "light";
    case Theme::Madagascar: return "madagascar";
    default: return "dark";
    }
}

void applyTheme(Theme t, QApplication& app) {
    app.setStyle(QStyleFactory::create("Fusion"));
    QPalette p;
    switch (t) {
    case Theme::Dark:
        p.setColor(QPalette::Window, QColor(53,53,53));
        p.setColor(QPalette::WindowText, Qt::white);
        p.setColor(QPalette::Base, QColor(25,25,25));
        p.setColor(QPalette::AlternateBase, QColor(53,53,53));
        p.setColor(QPalette::ToolTipBase, Qt::white);
        p.setColor(QPalette::ToolTipText, Qt::white);
        p.setColor(QPalette::Text, Qt::white);
        p.setColor(QPalette::Button, QColor(53,53,53));
        p.setColor(QPalette::ButtonText, Qt::white);
        p.setColor(QPalette::BrightText, Qt::red);
        p.setColor(QPalette::Link, QColor(42, 130, 218));
        p.setColor(QPalette::Highlight, QColor(42,130,218));
        p.setColor(QPalette::HighlightedText, Qt::black);
        break;
    case Theme::Madagascar:
        p.setColor(QPalette::Window, QColor(244, 164, 96));
        p.setColor(QPalette::WindowText, Qt::black);
        p.setColor(QPalette::Base, QColor(255, 228, 181));
        p.setColor(QPalette::AlternateBase, QColor(244, 164, 96));
        p.setColor(QPalette::Text, Qt::black);
        p.setColor(QPalette::Button, QColor(250, 128, 114));
        p.setColor(QPalette::ButtonText, Qt::black);
        p.setColor(QPalette::Highlight, QColor(60, 179, 113));
        p.setColor(QPalette::HighlightedText, Qt::black);
        break;
    case Theme::Light:
    default:
        p = app.style()->standardPalette();
        break;
    }
    app.setPalette(p);
}

