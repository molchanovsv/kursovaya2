#ifndef STUDENTS_ENTRY_H
#define STUDENTS_ENTRY_H

#include "fio.h"
#include "teacher.h"
#include <string>

struct Students_entry {
    FIO fio;
    std::string instrument;
    Teacher teacher;

    bool operator==(const Students_entry& other) const {
        return fio == other.fio && instrument == other.instrument &&
               teacher.surname == other.teacher.surname &&
               teacher.initials == other.teacher.initials;
    }
};


#endif
