#include "validators.h"

bool wordValid(const QString& word)
{
    static const QRegularExpression pat(QStringLiteral("^[А-ЯЁ][а-яё]+$"));
    return pat.match(word).hasMatch();
}

bool wordValid(const std::string& word)
{
    return wordValid(QString::fromStdString(word));
}

bool fioValid(const QString& surname, const QString& name, const QString& patronymic)
{
    return wordValid(surname) && wordValid(name) && wordValid(patronymic);
}

bool fioValid(const FIO& f)
{
    return fioValid(QString::fromStdString(f.surname),
                    QString::fromStdString(f.name),
                    QString::fromStdString(f.patronymic));
}

bool instrumentValid(const QString& instrument)
{
    static const QRegularExpression pat(QStringLiteral("^[А-Яа-яЁё]+( [А-Яа-яЁё]+)*$"));
    return pat.match(instrument).hasMatch();
}

bool instrumentValid(const std::string& instrument)
{
    return instrumentValid(QString::fromStdString(instrument));
}

bool teacherValid(const QString& surname, const QString& initials)
{
    static const QRegularExpression sPat(QStringLiteral("^[А-ЯЁ][а-яё]+$"));
    static const QRegularExpression iPat(QStringLiteral("^[А-ЯЁ]\\.[А-ЯЁ]\\.$"));
    return sPat.match(surname).hasMatch() && iPat.match(initials).hasMatch();
}

bool teacherValid(const std::string& surname, const std::string& initials)
{
    return teacherValid(QString::fromStdString(surname), QString::fromStdString(initials));
}

bool hallValid(const QString& hall)
{
    static const QRegularExpression pat(QStringLiteral("^[А-Яа-яЁё]+ зал$"));
    return pat.match(hall).hasMatch();
}

bool hallValid(const std::string& hall)
{
    return hallValid(QString::fromStdString(hall));
}

bool dateValid(const QString& date)
{
    static const QRegularExpression pat(QStringLiteral("^(0[1-9]|[12][0-9]|3[01])\\.(0[1-9]|1[0-2])\\.[0-9]{4}$"));
    return pat.match(date).hasMatch();
}

bool dateValid(const std::string& date)
{
    return dateValid(QString::fromStdString(date));
}
