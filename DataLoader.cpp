#include "DataLoader.h"
#include <fstream>
#include <iostream>
#include <QTextCodec>

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
        std::ifstream input(filename);
        if (!input.is_open()) {
            std::cerr << "Не удалось открыть файл " << filename << std::endl;
            count = 0;
            return new Students_entry[n];
        }
        Students_entry* records = new Students_entry[n];
        count = 0;

        std::string line;
        while (!input.eof() && count < n) {
            Students_entry record;
            std::string tmp;

            input >> tmp; record.fio.surname = decodeCp1251(tmp);
            input >> tmp; record.fio.name = decodeCp1251(tmp);
            input >> tmp; record.fio.patronymic = decodeCp1251(tmp);
            if (input.eof()) break;

            input >> tmp; record.instrument = decodeCp1251(tmp);

            input >> tmp; record.teacher.surname = decodeCp1251(tmp);
            input >> tmp; record.teacher.initials = decodeCp1251(tmp);

            records[count++] = record;
        }

        return records;
    }

    std::vector<Concerts_entry> loadConcertsData(const std::string& filename) {
        std::vector<Concerts_entry> entries;
        std::ifstream file(filename);

        if (!file.is_open()) {
            std::cerr << "Не удалось открыть файл " << filename << std::endl;
            return entries;
        }

        Concerts_entry entry;
        std::string line;

        while (file >> line) {
            entry.fio.surname = decodeCp1251(line);
            if (!(file >> line)) break; entry.fio.name = decodeCp1251(line);
            if (!(file >> line)) break; entry.fio.patronymic = decodeCp1251(line);
            // Чтение пьесы (в кавычках)
            file >> std::ws; // Пропустить пробелы
            std::getline(file, line, '"'); // Пропустить всё до первой кавычки
            std::getline(file, line, '"'); // Прочитать пьесу до закрывающей кавычки
            entry.play = decodeCp1251(line);

            // Чтение зала (2 слова)
            file >> std::ws;
            std::string hallPart1, hallPart2;
            file >> hallPart1 >> hallPart2;
            entry.hall = decodeCp1251(hallPart1) + " " + decodeCp1251(hallPart2);

            // Чтение даты
            file >> line;
            entry.date = decodeCp1251(line);

            entries.push_back(entry);
        }

        return entries;
    }
}
