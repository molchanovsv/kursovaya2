#ifndef STUDENTS_ENTRY_H
#define STUDENTS_ENTRY_H

#include "fio.h"
#include "teacher.h"
#include <string>

struct Students_entry {
    FIO fio;
    std::string instrument;
    Teacher teacher;
};


#endif
