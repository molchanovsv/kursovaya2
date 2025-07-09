#ifndef THEME_H
#define THEME_H

#include <QApplication>
#include <QString>

enum class Theme { Dark, Madagascar, Sonic, GojoSatoru, MarisaWriggle };

Theme themeFromString(const QString& name);
QString themeToString(Theme t);
void applyTheme(Theme t, QApplication& app);

#endif
