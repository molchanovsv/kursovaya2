#ifndef DATALOADER_H
#define DATALOADER_H

#include "Students_entry.h"
#include "Concerts_entry.h"
#include <vector>
#include <string>

namespace DataLoader {
    Students_entry* loadStudents(int n, int& count, const std::string& filename);
    std::vector<Concerts_entry> loadConcertsData(const std::string& filename);
}

#endif
