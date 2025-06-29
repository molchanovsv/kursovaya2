#include "dataloader.h"
#include <fstream>
#include "fio.h"
#include <QTextCodec>
#include <QDebug>
#include "validators.h"

namespace {
    std::string decodeCp1251(const std::string& str) {
        static QTextCodec* codec = QTextCodec::codecForName("Windows-1251");
        if (!codec) {
            // Fallback: return the string unchanged to avoid crashes
            return str;
        }
        return codec->toUnicode(str.c_str()).toStdString();
    }

}

namespace DataLoader {
    Students_entry* loadStudents(int n, int& count, const std::string& filename) {
        std::string fname = filename.empty() ? "students.txt" : filename;
        qDebug() << "Loading students from" << QString::fromStdString(fname);
        std::ifstream input(fname);
        if (!input.is_open()) {
            qWarning() << "Не удалось открыть файл" << QString::fromStdString(fname);
            count = 0;
            return new Students_entry[n];
        }
        Students_entry* records = new Students_entry[n];
        count = 0;

        std::string tmp;
        while (count < n && (input >> tmp)) {
            Students_entry record;
            record.fio.surname = decodeCp1251(tmp);

            if (!(input >> tmp))
                break;
            record.fio.name = decodeCp1251(tmp);

            if (!(input >> tmp))
                break;
            record.fio.patronymic = decodeCp1251(tmp);

            if (!(input >> tmp))
                break;
            record.instrument = decodeCp1251(tmp);

            if (!(input >> tmp))
                break;
            record.teacher.surname = decodeCp1251(tmp);

            if (!(input >> tmp))
                break;
            record.teacher.initials = decodeCp1251(tmp);

            records[count++] = record;
        }

        qDebug() << "Loaded" << count << "students";
        return records;
    }

    std::vector<Concerts_entry> loadConcertsData(const std::string& filename) {
        qDebug() << "Loading concerts from" << QString::fromStdString(filename);
        std::vector<Concerts_entry> entries;
        std::ifstream file(filename);

        if (!file.is_open()) {
            qWarning() << "Не удалось открыть файл" << QString::fromStdString(filename);
            return entries;
        }

        Concerts_entry entry;
        std::string line;

        while (true) {
            if (!(file >> line))
                break;
            entry.fio.surname = decodeCp1251(line);

            if (!(file >> line))
                break;
            entry.fio.name = decodeCp1251(line);

            if (!(file >> line))
                break;
            entry.fio.patronymic = decodeCp1251(line);

            file >> std::ws;
            if (!std::getline(file, line, '"'))
                break; // skip to quote
            if (!std::getline(file, line, '"'))
                break; // read quoted text
            entry.play = decodeCp1251(line);

            file >> std::ws;
            std::string hallPart1, hallPart2;
            if (!(file >> hallPart1 >> hallPart2))
                break;
            entry.hall = decodeCp1251(hallPart1) + " " + decodeCp1251(hallPart2);

            if (!(file >> line))
                break;
            entry.date = decodeCp1251(line);

            entries.push_back(entry);
        }

        qDebug() << "Loaded" << entries.size() << "concerts";
        return entries;
    }

    bool validateStudentsFile(const std::string& filename, int& count, std::string& error) {
        std::ifstream in(filename);
        if (!in.is_open()) {
            error = "Не удалось открыть файл студентов";
            return false;
        }

        std::string tmp;
        count = 0;
        while (in >> tmp) {
            Students_entry rec;
            rec.fio.surname = decodeCp1251(tmp);
            if (!(in >> tmp)) { error = "Файл студентов имеет неверный формат"; return false; }
            rec.fio.name = decodeCp1251(tmp);
            if (!(in >> tmp)) { error = "Файл студентов имеет неверный формат"; return false; }
            rec.fio.patronymic = decodeCp1251(tmp);
            if (!(in >> tmp)) { error = "Файл студентов имеет неверный формат"; return false; }
            rec.instrument = decodeCp1251(tmp);
            if (!(in >> tmp)) { error = "Файл студентов имеет неверный формат"; return false; }
            rec.teacher.surname = decodeCp1251(tmp);
            if (!(in >> tmp)) { error = "Файл студентов имеет неверный формат"; return false; }
            rec.teacher.initials = decodeCp1251(tmp);

            if (!fioValid(rec.fio) || !instrumentValid(rec.instrument) ||
                !teacherValid(rec.teacher.surname, rec.teacher.initials)) {
                error = "Некорректные данные в файле студентов";
                return false;
            }
            ++count;
        }

        if (count == 0) {
            error = "Файл студентов пуст или имеет неверный формат";
            return false;
        }
        return true;
    }

    bool validateConcertsFile(const std::string& filename, std::string& error) {
        std::ifstream in(filename);
        if (!in.is_open()) {
            error = "Не удалось открыть файл концертов";
            return false;
        }

        std::string word;
        int count = 0;
        while (in >> word) {
            Concerts_entry e;
            e.fio.surname = decodeCp1251(word);
            if (!(in >> word)) { error = "Файл концертов имеет неверный формат"; return false; }
            e.fio.name = decodeCp1251(word);
            if (!(in >> word)) { error = "Файл концертов имеет неверный формат"; return false; }
            e.fio.patronymic = decodeCp1251(word);

            in >> std::ws;
            if (!std::getline(in, word, '"')) { error = "Файл концертов имеет неверный формат"; return false; }
            if (!std::getline(in, word, '"')) { error = "Файл концертов имеет неверный формат"; return false; }
            e.play = decodeCp1251(word);

            in >> std::ws;
            std::string hall1, hall2;
            if (!(in >> hall1 >> hall2)) { error = "Файл концертов имеет неверный формат"; return false; }
            e.hall = decodeCp1251(hall1) + " " + decodeCp1251(hall2);

            if (!(in >> word)) { error = "Файл концертов имеет неверный формат"; return false; }
            e.date = decodeCp1251(word);

            if (!fioValid(e.fio) || !hallValid(e.hall) || !dateValid(e.date)) {
                error = "Некорректные данные в файле концертов";
                return false;
            }
            ++count;
        }

        if (count == 0) {
            error = "Файл концертов пуст или имеет неверный формат";
            return false;
        }
        return true;
    }
}
