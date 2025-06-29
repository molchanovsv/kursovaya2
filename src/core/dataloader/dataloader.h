#ifndef DATALOADER_H
#define DATALOADER_H

#include "student_entry.h"
#include "concert_entry.h"
#include <vector>
#include <string>

namespace DataLoader {
    std::vector<Students_entry> loadStudents(int n, int& count, const std::string& filename);
    std::vector<Concerts_entry> loadConcertsData(const std::string& filename);
    bool validateStudentsFile(const std::string& filename, int& count, std::string& error);
    bool validateConcertsFile(const std::string& filename, std::string& error);
}

#endif
