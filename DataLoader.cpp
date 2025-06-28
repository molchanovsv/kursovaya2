#include "DataLoader.h"
#include <fstream>
#include <iostream>

namespace DataLoader {
    Students_entry* loadStudents(int n, int& count) {
        std::ifstream input("students.txt");
        Students_entry* records = new Students_entry[n];
        count = 0;

        std::string line;
        while (!input.eof() && count < n) {
            Students_entry record;

            input >> record.fio.surname >> record.fio.name >> record.fio.patronymic;
            if (input.eof()) break;

            input >> record.instrument;

            input >> record.teacher.surname;
            std::string initials;
            input >> initials;
            record.teacher.initials = initials;

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

        while (file >> entry.fio.surname >> entry.fio.name >> entry.fio.patronymic) {
            // Чтение пьесы (в кавычках)
            file >> std::ws; // Пропустить пробелы
            std::getline(file, entry.play, '"'); // Пропустить всё до первой кавычки
            std::getline(file, entry.play, '"'); // Прочитать пьесу до закрывающей кавычки

            // Чтение зала (2 слова)
            file >> std::ws;
            std::string hallPart1, hallPart2;
            file >> hallPart1 >> hallPart2;
            entry.hall = hallPart1 + " " + hallPart2;

            // Чтение даты
            file >> entry.date;

            entries.push_back(entry);
        }

        return entries;
    }
}