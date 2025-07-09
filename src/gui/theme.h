#ifndef THEME_H
#define THEME_H

#include <QApplication>
#include <QString>

enum class Theme { Dark, Light, Madagascar };

Theme themeFromString(const QString& name);
QString themeToString(Theme t);
void applyTheme(Theme t, QApplication& app);

#endif
