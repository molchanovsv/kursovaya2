#include "DataLoader.h"
#include <fstream>
#include <iostream>
#include <QTextCodec>
#include <QDebug>

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
            std::cerr << "Не удалось открыть файл " << fname << std::endl;
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
            std::cerr << "Не удалось открыть файл " << filename << std::endl;
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

    bool validateStudentsFile(const std::string& filename, std::string& error) {
        int count = 0;
        Students_entry* test = loadStudents(1, count, filename);
        delete[] test;
        if (count == 0) {
            std::ifstream f(filename);
            if (!f.is_open()) {
                error = "Не удалось открыть файл студентов";
            } else {
                error = "Файл студентов пуст или имеет неверный формат";
            }
            return false;
        }
        return true;
    }

    bool validateConcertsFile(const std::string& filename, std::string& error) {
        auto data = loadConcertsData(filename);
        if (data.empty()) {
            std::ifstream f(filename);
            if (!f.is_open()) {
                error = "Не удалось открыть файл концертов";
            } else {
                error = "Файл концертов пуст или имеет неверный формат";
            }
            return false;
        }
        return true;
    }
}
