#ifndef VALIDATORS_H
#define VALIDATORS_H

#include <QString>
#include <QRegularExpression>
#include <string>
#include "fio.h"

bool wordValid(const QString& word);
bool wordValid(const std::string& word);

bool fioValid(const QString& surname, const QString& name, const QString& patronymic);
bool fioValid(const FIO& f);

bool instrumentValid(const QString& instrument);
bool instrumentValid(const std::string& instrument);

bool teacherValid(const QString& surname, const QString& initials);
bool teacherValid(const std::string& surname, const std::string& initials);

bool hallValid(const QString& hall);
bool hallValid(const std::string& hall);

bool dateValid(const QString& date);
bool dateValid(const std::string& date);

#endif // VALIDATORS_H
