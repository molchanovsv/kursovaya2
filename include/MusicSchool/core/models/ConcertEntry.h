#ifndef CONCERTS_ENTRY_H
#define CONCERTS_ENTRY_H

#include "FIO.h"
#include <string>

struct Concerts_entry {
    FIO fio;
    std::string play;
    std::string hall;
    std::string date;
};

#endif
