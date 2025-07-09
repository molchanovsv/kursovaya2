#ifndef CONCERTS_ENTRY_H
#define CONCERTS_ENTRY_H

#include "fio.h"
#include <string>

struct Concerts_entry {
    FIO fio;
    std::string instrument;
    std::string play;
    std::string hall;
    std::string date;

    bool operator==(const Concerts_entry& other) const {
        return fio == other.fio && instrument == other.instrument &&
               play == other.play && hall == other.hall && date == other.date;
    }
};

#endif
