#ifndef STUDENTS_ENTRY_H
#define STUDENTS_ENTRY_H

#include "FIO.h"
#include "Teacher.h"
#include <string>
#include <iostream>

struct Students_entry {
    FIO fio;
    std::string instrument;
    Teacher teacher;
};

std::ostream& operator<<(std::ostream& os, const Students_entry& entry);

#endif
